/******************************************************************************
 * Copyright 2019 ETC Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *******************************************************************************
 * This file is a part of lwpa. For more information, go to:
 * https://github.com/ETCLabs/lwpa
 ******************************************************************************/
#include "lwpa/socket.h"
#include "unity_fixture.h"

#include "lwpa/netint.h"
#include "lwpa/thread.h"

// DEBUG REMOVEME
#include <stdio.h>
// END DEBUG

// Disable sprintf() warning on Windows/MSVC
#ifdef _MSC_VER
#pragma warning(disable : 4996)
#endif

#ifndef LWPA_BULK_POLL_TEST_NUM_SOCKETS
#if LWPA_SOCKET_MAX_POLL_SIZE <= 0 || LWPA_SOCKET_MAX_POLL_SIZE > 1024
// Limit the bulk socket test to a reasonable number
#define LWPA_BULK_POLL_TEST_NUM_SOCKETS 512
#else
#define LWPA_BULK_POLL_TEST_NUM_SOCKETS LWPA_SOCKET_MAX_POLL_SIZE
#endif
#endif

#define NUM_TEST_PACKETS 1000

static LwpaNetintInfo v4_netint;
bool run_ipv4_mcast_test;
#if LWPA_TEST_IPV6
static LwpaNetintInfo v6_netint;
bool run_ipv6_mcast_test;
#endif

static const char kSocketTestMessage[] = "testtesttest";
#define SOCKET_TEST_MESSAGE_LENGTH (sizeof(kSocketTestMessage))
static const uint32_t kTestMcastAddrIPv4 = 0xec02054d;  // 236.2.5.77
// ff02::7465:7374:7465:7374
static const uint8_t kTestMcastAddrIPv6[LWPA_IPV6_BYTES] = {0xff, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                            0x74, 0x65, 0x73, 0x74, 0x74, 0x65, 0x73, 0x74};

static lwpa_socket_t send_sock;
static LwpaSockaddr send_addr;

// Select the default interface if available, the very first non-loopback, non-link-local interface
// if not.
static void select_network_interface_v4()
{
  run_ipv4_mcast_test = true;
  if (kLwpaErrOk != lwpa_netint_get_default_interface(kLwpaIpTypeV4, &v4_netint))
  {
    const LwpaNetintInfo* arr = lwpa_netint_get_interfaces();
    if (arr)
    {
      for (const LwpaNetintInfo* netint = arr; netint < arr + lwpa_netint_get_num_interfaces(); ++netint)
      {
        if (LWPA_IP_IS_V4(&netint->addr) && !lwpa_ip_is_link_local(&netint->addr) &&
            !lwpa_ip_is_loopback(&netint->addr))
        {
          v4_netint = *netint;
          return;
        }
      }
      // We haven't found a network interface...
      UnityPrint(
          "WARNING: No IPv4 non-loopback, non-link-local network interfaces found. Disabling multicast IPv4 "
          "test...\n");
      run_ipv4_mcast_test = false;
    }
  }
}

#if LWPA_TEST_IPV6
// Select the default interface if available, the very first non-loopback interface if not.
static void select_network_interface_v6()
{
  run_ipv6_mcast_test = true;
  if (kLwpaErrOk != lwpa_netint_get_default_interface(kLwpaIpTypeV6, &v6_netint))
  {
    const LwpaNetintInfo* arr = lwpa_netint_get_interfaces();
    if (arr)
    {
      for (const LwpaNetintInfo* netint = arr; netint < arr + lwpa_netint_get_num_interfaces(); ++netint)
      {
        if (LWPA_IP_IS_V6(&netint->addr) && !lwpa_ip_is_loopback(&netint->addr))
        {
          v6_netint = *netint;
          return;
        }
      }
      // We haven't found a network interface...
      UnityPrint("WARNING: No IPv6 non-loopback network interfaces found. Disabling multicast IPv6 test...\n");
      run_ipv6_mcast_test = false;
    }
  }
}
#endif

TEST_GROUP(socket_integration);

TEST_SETUP(socket_integration)
{
}

TEST_TEAR_DOWN(socket_integration)
{
}

// For UDP testing we apply strength in numbers to get around the inherent unreliability of UDP.
// Packets are sent in bulk from another thread, and pass if we receive some of them.

static void send_thread(void* arg)
{
  (void)arg;

  for (size_t i = 0; i < NUM_TEST_PACKETS; ++i)
    lwpa_sendto(send_sock, (const uint8_t*)kSocketTestMessage, SOCKET_TEST_MESSAGE_LENGTH, 0, &send_addr);
}

#define UNICAST_UDP_PORT_BASE 6000

void unicast_udp_test(lwpa_iptype_t ip_type, lwpa_socket_t rcvsock1, lwpa_socket_t rcvsock2)
{
  int intval = 500;
  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_setsockopt(rcvsock1, LWPA_SOL_SOCKET, LWPA_SO_RCVTIMEO, &intval, sizeof(int)));
  intval = 1;
  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_setsockopt(rcvsock2, LWPA_SOL_SOCKET, LWPA_SO_RCVTIMEO, &intval, sizeof(int)));

  LwpaSockaddr bind_addr;
  lwpa_ip_set_wildcard(ip_type, &bind_addr.ip);
  bind_addr.port = UNICAST_UDP_PORT_BASE;
  // Bind socket 1 to the wildcard address and a specific port.
  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_bind(rcvsock1, &bind_addr));
  // Bind socket 2 to the wildcard address and a different port.
  bind_addr.port = UNICAST_UDP_PORT_BASE + 1;
  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_bind(rcvsock2, &bind_addr));

  send_addr.port = UNICAST_UDP_PORT_BASE;

  // Start the send thread
  LwpaThreadParams thread_params;
  LWPA_THREAD_SET_DEFAULT_PARAMS(&thread_params);
  lwpa_thread_t send_thr_handle;
  TEST_ASSERT(lwpa_thread_create(&send_thr_handle, &thread_params, send_thread, NULL));

  size_t num_packets_received = 0;
  for (size_t i = 0; i < NUM_TEST_PACKETS; ++i)
  {
    LwpaSockaddr from_addr;
    uint8_t buf[SOCKET_TEST_MESSAGE_LENGTH + 1];

    int res = lwpa_recvfrom(rcvsock1, buf, SOCKET_TEST_MESSAGE_LENGTH, 0, &from_addr);
    if (res == SOCKET_TEST_MESSAGE_LENGTH)
    {
      ++num_packets_received;
    }
    else
    {
      TEST_ASSERT_EQUAL(res, kLwpaErrTimedOut);
      break;
    }

    TEST_ASSERT(lwpa_ip_equal(&send_addr.ip, &from_addr.ip));
    TEST_ASSERT_NOT_EQUAL(from_addr.port, UNICAST_UDP_PORT_BASE);

    buf[SOCKET_TEST_MESSAGE_LENGTH] = '\0';
    TEST_ASSERT_EQUAL_STRING((char*)buf, kSocketTestMessage);
  }

  TEST_ASSERT_GREATER_THAN_UINT(0u, num_packets_received);

  // recvfrom should time out because this socket is bound to a different port and we set the
  // timeout option on this socket.
  LwpaSockaddr from_addr;
  uint8_t buf[SOCKET_TEST_MESSAGE_LENGTH + 1];
  TEST_ASSERT_LESS_OR_EQUAL_INT(0, lwpa_recvfrom(rcvsock2, buf, SOCKET_TEST_MESSAGE_LENGTH, 0, &from_addr));

  // Let the send thread end
  TEST_ASSERT_TRUE(lwpa_thread_join(&send_thr_handle));
}

TEST(socket_integration, unicast_udp_ipv4)
{
  lwpa_socket_t rcvsock1 = LWPA_SOCKET_INVALID;
  lwpa_socket_t rcvsock2 = LWPA_SOCKET_INVALID;

  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_socket(LWPA_AF_INET, LWPA_DGRAM, &rcvsock1));
  TEST_ASSERT_NOT_EQUAL(rcvsock1, LWPA_SOCKET_INVALID);

  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_socket(LWPA_AF_INET, LWPA_DGRAM, &rcvsock2));
  TEST_ASSERT_NOT_EQUAL(rcvsock2, LWPA_SOCKET_INVALID);

  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_socket(LWPA_AF_INET, LWPA_DGRAM, &send_sock));
  TEST_ASSERT_NOT_EQUAL(send_sock, LWPA_SOCKET_INVALID);

  LWPA_IP_SET_V4_ADDRESS(&send_addr.ip, 0x7f000001u);

  unicast_udp_test(kLwpaIpTypeV4, rcvsock1, rcvsock2);

  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_close(rcvsock1));
  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_close(rcvsock2));
  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_close(send_sock));
}

TEST(socket_integration, unicast_udp_ipv6)
{
  lwpa_socket_t rcvsock1 = LWPA_SOCKET_INVALID;
  lwpa_socket_t rcvsock2 = LWPA_SOCKET_INVALID;

  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_socket(LWPA_AF_INET6, LWPA_DGRAM, &rcvsock1));
  TEST_ASSERT_NOT_EQUAL(rcvsock1, LWPA_SOCKET_INVALID);

  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_socket(LWPA_AF_INET6, LWPA_DGRAM, &rcvsock2));
  TEST_ASSERT_NOT_EQUAL(rcvsock2, LWPA_SOCKET_INVALID);

  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_socket(LWPA_AF_INET6, LWPA_DGRAM, &send_sock));
  TEST_ASSERT_NOT_EQUAL(send_sock, LWPA_SOCKET_INVALID);

  uint8_t v6_loopback[LWPA_IPV6_BYTES];
  memset(&v6_loopback, 0, LWPA_IPV6_BYTES);
  v6_loopback[15] = 1;
  LWPA_IP_SET_V6_ADDRESS(&send_addr.ip, &v6_loopback);

  unicast_udp_test(kLwpaIpTypeV6, rcvsock1, rcvsock2);

  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_close(rcvsock1));
  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_close(rcvsock2));
  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_close(send_sock));
}

#define MULTICAST_UDP_PORT_BASE 7000

void multicast_udp_test(lwpa_socket_t rcvsock1, lwpa_socket_t rcvsock2)
{
  int intval = 500;
  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_setsockopt(rcvsock1, LWPA_SOL_SOCKET, LWPA_SO_RCVTIMEO, &intval, sizeof(int)));
  intval = 1;
  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_setsockopt(rcvsock2, LWPA_SOL_SOCKET, LWPA_SO_RCVTIMEO, &intval, sizeof(int)));

  // Start the send thread
  LwpaThreadParams thread_params;
  LWPA_THREAD_SET_DEFAULT_PARAMS(&thread_params);
  lwpa_thread_t send_thr_handle;
  TEST_ASSERT(lwpa_thread_create(&send_thr_handle, &thread_params, send_thread, NULL));

  size_t num_packets_received = 0;
  for (size_t i = 0; i < NUM_TEST_PACKETS; ++i)
  {
    LwpaSockaddr from_addr;
    uint8_t buf[SOCKET_TEST_MESSAGE_LENGTH + 1];

    int res = lwpa_recvfrom(rcvsock1, buf, SOCKET_TEST_MESSAGE_LENGTH, 0, &from_addr);
    if (res == SOCKET_TEST_MESSAGE_LENGTH)
    {
      ++num_packets_received;
    }
    else
    {
      TEST_ASSERT_EQUAL(res, kLwpaErrWouldBlock);
      break;
    }

    TEST_ASSERT_NOT_EQUAL(from_addr.port, MULTICAST_UDP_PORT_BASE);

    buf[SOCKET_TEST_MESSAGE_LENGTH] = '\0';
    TEST_ASSERT_EQUAL_STRING((char*)buf, kSocketTestMessage);
  }
  TEST_ASSERT_GREATER_THAN(0u, num_packets_received);

  LwpaSockaddr from_addr;
  uint8_t buf[SOCKET_TEST_MESSAGE_LENGTH + 1];
  // recvfrom should time out because this socket is bound to a different port and we set the
  // timeout option on this socket.
  TEST_ASSERT_LESS_OR_EQUAL_INT(0, lwpa_recvfrom(rcvsock2, buf, SOCKET_TEST_MESSAGE_LENGTH, 0, &from_addr));

  // Let the send thread end
  TEST_ASSERT(lwpa_thread_join(&send_thr_handle));
}

TEST(socket_integration, multicast_udp_ipv4)
{
  LwpaSockaddr bind_addr;

  lwpa_socket_t rcvsock1 = LWPA_SOCKET_INVALID;
  lwpa_socket_t rcvsock2 = LWPA_SOCKET_INVALID;

  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_socket(LWPA_AF_INET, LWPA_DGRAM, &rcvsock1));
  TEST_ASSERT_NOT_EQUAL(rcvsock1, LWPA_SOCKET_INVALID);

  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_socket(LWPA_AF_INET, LWPA_DGRAM, &rcvsock2));
  TEST_ASSERT_NOT_EQUAL(rcvsock2, LWPA_SOCKET_INVALID);

  int intval = 1;
  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_setsockopt(rcvsock1, LWPA_SOL_SOCKET, LWPA_SO_REUSEADDR, &intval, sizeof(int)));
  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_setsockopt(rcvsock2, LWPA_SOL_SOCKET, LWPA_SO_REUSEADDR, &intval, sizeof(int)));

  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_socket(LWPA_AF_INET, LWPA_DGRAM, &send_sock));
  TEST_ASSERT_NOT_EQUAL(send_sock, LWPA_SOCKET_INVALID);

  TEST_ASSERT_EQUAL(kLwpaErrOk,
                    lwpa_setsockopt(send_sock, LWPA_IPPROTO_IP, LWPA_IP_MULTICAST_LOOP, &intval, sizeof(int)));
  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_setsockopt(send_sock, LWPA_IPPROTO_IP, LWPA_IP_MULTICAST_IF, &v4_netint.index,
                                                sizeof v4_netint.index));

  // Bind socket 1 to the wildcard address and a specific port.
  lwpa_ip_set_wildcard(kLwpaIpTypeV4, &bind_addr.ip);
  bind_addr.port = MULTICAST_UDP_PORT_BASE;
  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_bind(rcvsock1, &bind_addr));

  // Bind socket 2 to the wildcard address and a different port.
  bind_addr.port = MULTICAST_UDP_PORT_BASE + 1;
  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_bind(rcvsock2, &bind_addr));

  // Subscribe socket 1 to the multicast address.
  LwpaGroupReq greq;
  greq.ifindex = v4_netint.index;
  LWPA_IP_SET_V4_ADDRESS(&greq.group, kTestMcastAddrIPv4);
  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_setsockopt(rcvsock1, LWPA_IPPROTO_IP, LWPA_MCAST_JOIN_GROUP, &greq, sizeof greq));

  // Subscribe socket 2 to the multicast address
  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_setsockopt(rcvsock2, LWPA_IPPROTO_IP, LWPA_MCAST_JOIN_GROUP, &greq, sizeof greq));

  LWPA_IP_SET_V4_ADDRESS(&send_addr.ip, kTestMcastAddrIPv4);
  send_addr.port = MULTICAST_UDP_PORT_BASE;

  multicast_udp_test(rcvsock1, rcvsock2);

  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_close(rcvsock1));
  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_close(rcvsock2));
  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_close(send_sock));
}

#if LWPA_TEST_IPV6
TEST(socket_integration, multicast_udp_ipv6)
{
  LwpaSockaddr bind_addr;

  lwpa_socket_t rcvsock1 = LWPA_SOCKET_INVALID;
  lwpa_socket_t rcvsock2 = LWPA_SOCKET_INVALID;

  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_socket(LWPA_AF_INET6, LWPA_DGRAM, &rcvsock1));
  TEST_ASSERT_NOT_EQUAL(rcvsock1, LWPA_SOCKET_INVALID);

  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_socket(LWPA_AF_INET6, LWPA_DGRAM, &rcvsock2));
  TEST_ASSERT_NOT_EQUAL(rcvsock2, LWPA_SOCKET_INVALID);

  int intval = 1;
  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_setsockopt(rcvsock1, LWPA_SOL_SOCKET, LWPA_SO_REUSEADDR, &intval, sizeof(int)));
  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_setsockopt(rcvsock2, LWPA_SOL_SOCKET, LWPA_SO_REUSEADDR, &intval, sizeof(int)));

  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_socket(LWPA_AF_INET6, LWPA_DGRAM, &send_sock));
  TEST_ASSERT_NOT_EQUAL(send_sock, LWPA_SOCKET_INVALID);

  TEST_ASSERT_EQUAL(kLwpaErrOk,
                    lwpa_setsockopt(send_sock, LWPA_IPPROTO_IPV6, LWPA_IP_MULTICAST_LOOP, &intval, sizeof(int)));
  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_setsockopt(send_sock, LWPA_IPPROTO_IPV6, LWPA_IP_MULTICAST_IF, &v6_netint.index,
                                                sizeof v6_netint.index));

  // Bind socket 1 to the wildcard address and a specific port.
  lwpa_ip_set_wildcard(kLwpaIpTypeV6, &bind_addr.ip);
  bind_addr.port = MULTICAST_UDP_PORT_BASE;
  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_bind(rcvsock1, &bind_addr));

  // Bind socket 2 to the wildcard address and a different port.
  bind_addr.port = MULTICAST_UDP_PORT_BASE + 1;
  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_bind(rcvsock2, &bind_addr));

  // Subscribe socket 1 to the multicast address.
  LwpaGroupReq greq;
  greq.ifindex = v6_netint.index;
  LWPA_IP_SET_V6_ADDRESS(&greq.group, kTestMcastAddrIPv6);
  TEST_ASSERT_EQUAL(kLwpaErrOk,
                    lwpa_setsockopt(rcvsock1, LWPA_IPPROTO_IPV6, LWPA_MCAST_JOIN_GROUP, &greq, sizeof greq));

  // Subscribe socket 2 to the multicast address
  TEST_ASSERT_EQUAL(kLwpaErrOk,
                    lwpa_setsockopt(rcvsock2, LWPA_IPPROTO_IPV6, LWPA_MCAST_JOIN_GROUP, &greq, sizeof greq));

  LWPA_IP_SET_V6_ADDRESS(&send_addr.ip, kTestMcastAddrIPv6);
  send_addr.port = MULTICAST_UDP_PORT_BASE;

  multicast_udp_test(rcvsock1, rcvsock2);

  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_close(rcvsock1));
  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_close(rcvsock2));
  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_close(send_sock));
}
#endif

// Test to make sure lwpa_poll_* functions work properly with a large number of sockets.
// (Tests the maximum number defined by LWPA_SOCKET_MAX_POLL_SIZE if that number is well-defined and
// reasonable).
TEST(socket_integration, bulk_poll)
{
  LwpaPollContext context;
  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_poll_context_init(&context));

  static lwpa_socket_t socket_arr[LWPA_BULK_POLL_TEST_NUM_SOCKETS];
  static uint16_t bind_ports[LWPA_BULK_POLL_TEST_NUM_SOCKETS];
  for (size_t i = 0; i < LWPA_BULK_POLL_TEST_NUM_SOCKETS; ++i)
  {
    char error_msg[50];
    sprintf(error_msg, "Failed on iteration %zu", i);

    TEST_ASSERT_EQUAL_MESSAGE(kLwpaErrOk, lwpa_socket(LWPA_AF_INET, LWPA_DGRAM, &socket_arr[i]), error_msg);
    TEST_ASSERT_NOT_EQUAL_MESSAGE(socket_arr[i], LWPA_SOCKET_INVALID, error_msg);

    LwpaSockaddr bind_addr;
    lwpa_ip_set_wildcard(kLwpaIpTypeV4, &bind_addr.ip);
    bind_addr.port = 0;
    TEST_ASSERT_EQUAL_MESSAGE(kLwpaErrOk, lwpa_bind(socket_arr[i], &bind_addr), error_msg);
    TEST_ASSERT_EQUAL_MESSAGE(kLwpaErrOk, lwpa_getsockname(socket_arr[i], &bind_addr), error_msg);
    bind_ports[i] = bind_addr.port;

    TEST_ASSERT_EQUAL_MESSAGE(kLwpaErrOk, lwpa_poll_add_socket(&context, socket_arr[i], LWPA_POLL_IN, NULL), error_msg);
  }

  lwpa_socket_t poll_send_sock;
  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_socket(LWPA_AF_INET, LWPA_DGRAM, &poll_send_sock));

  LwpaSockaddr poll_send_addr;
  LWPA_IP_SET_V4_ADDRESS(&poll_send_addr.ip, 0x7f000001);
  for (size_t i = 0; i < LWPA_BULK_POLL_TEST_NUM_SOCKETS; ++i)
  {
    char error_msg[50];
    sprintf(error_msg, "Failed on iteration %zu", i);

    // Send to each socket in turn and make sure poll works for it
    poll_send_addr.port = bind_ports[i];
    lwpa_sendto(poll_send_sock, kSocketTestMessage, SOCKET_TEST_MESSAGE_LENGTH, 0, &poll_send_addr);

    LwpaPollEvent event;
    TEST_ASSERT_EQUAL_MESSAGE(kLwpaErrOk, lwpa_poll_wait(&context, &event, 100), error_msg);
    TEST_ASSERT_EQUAL_MESSAGE(event.socket, socket_arr[i], error_msg);
    TEST_ASSERT_EQUAL_MESSAGE(event.events, LWPA_POLL_IN, error_msg);
    TEST_ASSERT_EQUAL_MESSAGE(event.err, kLwpaErrOk, error_msg);

    uint8_t recv_buf[SOCKET_TEST_MESSAGE_LENGTH];
    TEST_ASSERT_GREATER_THAN(0, lwpa_recvfrom(socket_arr[i], recv_buf, SOCKET_TEST_MESSAGE_LENGTH, 0, NULL));
  }

  for (size_t i = 0; i < LWPA_BULK_POLL_TEST_NUM_SOCKETS; ++i)
  {
    char error_msg[50];
    sprintf(error_msg, "Failed on iteration %zu", i);

    TEST_ASSERT_EQUAL_MESSAGE(kLwpaErrOk, lwpa_close(socket_arr[i]), error_msg);
  }
}

TEST_GROUP_RUNNER(socket_integration)
{
  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_init(LWPA_FEATURE_SOCKETS | LWPA_FEATURE_NETINTS));

  select_network_interface_v4();
#if LWPA_TEST_IPV6
  select_network_interface_v6();

  // DEBUG REMOVEME
  char addr_str[LWPA_INET6_ADDRSTRLEN];
  lwpa_inet_ntop(&v6_netint.addr, addr_str, LWPA_INET6_ADDRSTRLEN);
  printf("IPv6 netint address %d, index %u\n", addr_str, v6_netint.index);
  // END DEBUG
#endif

  RUN_TEST_CASE(socket_integration, unicast_udp_ipv4);
  if (run_ipv4_mcast_test)
    RUN_TEST_CASE(socket_integration, multicast_udp_ipv4);
#if LWPA_TEST_IPV6
  RUN_TEST_CASE(socket_integration, unicast_udp_ipv6);
  if (run_ipv6_mcast_test)
    RUN_TEST_CASE(socket_integration, multicast_udp_ipv6);
#endif

  RUN_TEST_CASE(socket_integration, bulk_poll);

  lwpa_deinit(LWPA_FEATURE_SOCKETS | LWPA_FEATURE_NETINTS);
}
