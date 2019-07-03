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
#include "lwpa/netint.h"
#include "gtest/gtest.h"
#include <cstddef>
#include <thread>

class SocketTest : public ::testing::Test
{
public:
  static const size_t NUM_TEST_PACKETS = 1000;
  static const char* SEND_MSG;
  static const size_t SEND_MSG_LEN;
  static const uint32_t TEST_MCAST_ADDR = 0xec02054d;  // 236.2.5.77
  lwpa_socket_t send_sock = LWPA_SOCKET_INVALID;
  LwpaSockaddr send_addr_1;

protected:
  SocketTest()
  {
    lwpa_init(LWPA_FEATURE_SOCKETS | LWPA_FEATURE_NETINTS);
    lwpa_netint_get_default_interface(kLwpaIpTypeV4, &default_netint_);
  }
  ~SocketTest() { lwpa_deinit(LWPA_FEATURE_SOCKETS | LWPA_FEATURE_NETINTS); }

  LwpaNetintInfo default_netint_;

  // For inet_xtox
  char str[LWPA_INET6_ADDRSTRLEN];
  const char* test_ip4_1 = "0.0.0.0";
  const char* test_ip4_2 = "255.255.255.255";
  const char* test_ip4_fail = "256.256.256.256";
  const char* test_ip6_1 = "::";
  const uint8_t test_ip6_1_bin[LWPA_IPV6_BYTES] = {0};
  const char* test_ip6_2 = "::1";
  const uint8_t test_ip6_2_bin[LWPA_IPV6_BYTES] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1};
  const char* test_ip6_3 = "ffff:FFFF:ffff:FFFF:ffff:FFFF:ffff:FFFF";
  const uint8_t test_ip6_3_bin[LWPA_IPV6_BYTES] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                                                   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
  const char* test_ip6_fail = "abcd::ef01::2345";

  // For getaddrinfo
  const char* test_hostname = "www.google.com";
  const char* test_service = "http";

  const char* test_gai_ip_str = "10.101.1.1";
  const uint32_t test_gai_ip = 0x0a650101;
  const char* test_gai_port_str = "8080";
  const uint16_t test_gai_port = 8080;
};

const char* SocketTest::SEND_MSG = "testtesttest";
const size_t SocketTest::SEND_MSG_LEN = 12;

TEST_F(SocketTest, inet_xtox)
{
  LwpaIpAddr addr;

  // Test lwpa_inet_pton()
  ASSERT_EQ(kLwpaErrOk, lwpa_inet_pton(kLwpaIpTypeV4, test_ip4_1, &addr));
  ASSERT_EQ(LWPA_IP_V4_ADDRESS(&addr), 0u);
  ASSERT_EQ(kLwpaErrOk, lwpa_inet_pton(kLwpaIpTypeV4, test_ip4_2, &addr));
  ASSERT_EQ(LWPA_IP_V4_ADDRESS(&addr), 0xffffffffu);
  ASSERT_NE(kLwpaErrOk, lwpa_inet_pton(kLwpaIpTypeV4, test_ip4_fail, &addr));
  ASSERT_EQ(kLwpaErrOk, lwpa_inet_pton(kLwpaIpTypeV6, test_ip6_1, &addr));
  ASSERT_EQ(0, memcmp(LWPA_IP_V6_ADDRESS(&addr), test_ip6_1_bin, LWPA_IPV6_BYTES));
  ASSERT_EQ(kLwpaErrOk, lwpa_inet_pton(kLwpaIpTypeV6, test_ip6_2, &addr));
  ASSERT_EQ(0, memcmp(LWPA_IP_V6_ADDRESS(&addr), test_ip6_2_bin, LWPA_IPV6_BYTES));
  ASSERT_EQ(kLwpaErrOk, lwpa_inet_pton(kLwpaIpTypeV6, test_ip6_3, &addr));
  ASSERT_EQ(0, memcmp(LWPA_IP_V6_ADDRESS(&addr), test_ip6_3_bin, LWPA_IPV6_BYTES));
  ASSERT_NE(kLwpaErrOk, lwpa_inet_pton(kLwpaIpTypeV6, test_ip6_fail, &addr));

  // Test lwpa_inet_ntop()
  LWPA_IP_SET_V4_ADDRESS(&addr, 0);
  ASSERT_EQ(kLwpaErrOk, lwpa_inet_ntop(&addr, str, LWPA_INET_ADDRSTRLEN));
  ASSERT_EQ(0, strcmp(str, test_ip4_1));
  LWPA_IP_SET_V4_ADDRESS(&addr, 0xffffffff);
  ASSERT_EQ(kLwpaErrOk, lwpa_inet_ntop(&addr, str, LWPA_INET_ADDRSTRLEN));
  ASSERT_EQ(0, strcmp(str, test_ip4_2));
  LWPA_IP_SET_V6_ADDRESS(&addr, test_ip6_1_bin);
  ASSERT_EQ(kLwpaErrOk, lwpa_inet_ntop(&addr, str, LWPA_INET6_ADDRSTRLEN));
  ASSERT_EQ(0, strcmp(str, test_ip6_1));
  LWPA_IP_SET_V6_ADDRESS(&addr, test_ip6_2_bin);
  ASSERT_EQ(kLwpaErrOk, lwpa_inet_ntop(&addr, str, LWPA_INET6_ADDRSTRLEN));
  ASSERT_EQ(0, strcmp(str, test_ip6_2));
  LWPA_IP_SET_V6_ADDRESS(&addr, test_ip6_3_bin);
  ASSERT_EQ(kLwpaErrOk, lwpa_inet_ntop(&addr, str, LWPA_INET6_ADDRSTRLEN));
  ASSERT_TRUE((0 == strcmp(str, "ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff")) ||
              (0 == strcmp(str, "FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF")));
}

TEST_F(SocketTest, sockopts)
{
  // TODO, need getsockopt() implemented for this
}

static void send_thread(SocketTest* fixture)
{
  if (fixture)
  {
    const uint8_t* send_buf = (const uint8_t*)(SocketTest::SEND_MSG);
    for (size_t i = 0; i < SocketTest::NUM_TEST_PACKETS; ++i)
      lwpa_sendto(fixture->send_sock, send_buf, SocketTest::SEND_MSG_LEN, 0, &fixture->send_addr_1);
  }
}

#define UNICAST_UDP_PORT_BASE 6000

TEST_F(SocketTest, unicast_udp)
{
  lwpa_socket_t rcvsock1 = LWPA_SOCKET_INVALID;
  lwpa_socket_t rcvsock2 = LWPA_SOCKET_INVALID;
  lwpa_socket_t rcvsock3 = LWPA_SOCKET_INVALID;
  LwpaSockaddr bind_addr;

  // We'll just use the default interface for this test.

  ASSERT_EQ(kLwpaErrOk, lwpa_socket(LWPA_AF_INET, LWPA_DGRAM, &rcvsock1));
  ASSERT_NE(rcvsock1, LWPA_SOCKET_INVALID);

  ASSERT_EQ(kLwpaErrOk, lwpa_socket(LWPA_AF_INET, LWPA_DGRAM, &rcvsock2));
  ASSERT_NE(rcvsock2, LWPA_SOCKET_INVALID);

  int intval = 500;
  ASSERT_EQ(kLwpaErrOk, lwpa_setsockopt(rcvsock1, LWPA_SOL_SOCKET, LWPA_SO_RCVTIMEO, &intval, sizeof(int)));
  intval = 1;
  ASSERT_EQ(kLwpaErrOk, lwpa_setsockopt(rcvsock2, LWPA_SOL_SOCKET, LWPA_SO_RCVTIMEO, &intval, sizeof(int)));

  ASSERT_EQ(kLwpaErrOk, lwpa_socket(LWPA_AF_INET, LWPA_DGRAM, &rcvsock3));
  ASSERT_NE(rcvsock3, LWPA_SOCKET_INVALID);

  ASSERT_EQ(kLwpaErrOk, lwpa_socket(LWPA_AF_INET, LWPA_DGRAM, &send_sock));
  ASSERT_NE(send_sock, LWPA_SOCKET_INVALID);

  ASSERT_EQ(kLwpaErrOk, lwpa_close(rcvsock3));

  lwpa_ip_set_wildcard(kLwpaIpTypeV4, &bind_addr.ip);
  bind_addr.port = UNICAST_UDP_PORT_BASE;
  // Shouldn't be able to bind to a closed socket.
  ASSERT_NE(kLwpaErrOk, lwpa_bind(rcvsock3, &bind_addr));
  // Bind socket 1 to the wildcard address and a specific port.
  ASSERT_EQ(kLwpaErrOk, lwpa_bind(rcvsock1, &bind_addr));
  // Bind socket 2 to the wildcard address and a different port.
  bind_addr.port = UNICAST_UDP_PORT_BASE + 1;
  ASSERT_EQ(kLwpaErrOk, lwpa_bind(rcvsock2, &bind_addr));

  LWPA_IP_SET_V4_ADDRESS(&send_addr_1.ip, 0x7f000001u);
  send_addr_1.port = UNICAST_UDP_PORT_BASE;

  std::thread send_thr(send_thread, this);
  ASSERT_TRUE(send_thr.joinable());

  size_t num_packets_received = 0;
  for (size_t i = 0; i < NUM_TEST_PACKETS; ++i)
  {
    LwpaSockaddr from_addr;
    uint8_t buf[SEND_MSG_LEN + 1];

    int res = lwpa_recvfrom(rcvsock1, buf, SEND_MSG_LEN, 0, &from_addr);
    if (res == SEND_MSG_LEN)
    {
      ++num_packets_received;
    }
    else
    {
      EXPECT_EQ(res, kLwpaErrTimedOut);
      break;
    }

    EXPECT_TRUE(lwpa_ip_equal(&send_addr_1.ip, &from_addr.ip));
    EXPECT_NE(from_addr.port, UNICAST_UDP_PORT_BASE);

    buf[SEND_MSG_LEN] = '\0';
    EXPECT_EQ(0, strcmp((char*)buf, SEND_MSG));
  }

  EXPECT_GT(num_packets_received, 0u);

  // recvfrom should time out because this socket is bound to a different port and we set the
  // timeout option on this socket.
  LwpaSockaddr from_addr;
  uint8_t buf[SEND_MSG_LEN + 1];
  EXPECT_GE(0, lwpa_recvfrom(rcvsock2, buf, SEND_MSG_LEN, 0, &from_addr));

  // Let the send thread end
  send_thr.join();

  ASSERT_EQ(kLwpaErrOk, lwpa_close(rcvsock1));
  ASSERT_EQ(kLwpaErrOk, lwpa_close(rcvsock2));
  ASSERT_EQ(kLwpaErrOk, lwpa_close(send_sock));
}

#define MULTICAST_UDP_PORT_BASE 7000

TEST_F(SocketTest, multicast_udp)
{
  lwpa_socket_t rcvsock1 = LWPA_SOCKET_INVALID;
  lwpa_socket_t rcvsock2 = LWPA_SOCKET_INVALID;
  LwpaSockaddr bind_addr;

  ASSERT_EQ(kLwpaErrOk, lwpa_socket(LWPA_AF_INET, LWPA_DGRAM, &rcvsock1));
  ASSERT_NE(rcvsock1, LWPA_SOCKET_INVALID);

  int intval = 500;
  ASSERT_EQ(kLwpaErrOk, lwpa_setsockopt(rcvsock1, LWPA_SOL_SOCKET, LWPA_SO_RCVTIMEO, &intval, sizeof(int)));
  intval = 1;
  ASSERT_EQ(kLwpaErrOk, lwpa_setsockopt(rcvsock1, LWPA_SOL_SOCKET, LWPA_SO_REUSEADDR, &intval, sizeof(int)));

  ASSERT_EQ(kLwpaErrOk, lwpa_socket(LWPA_AF_INET, LWPA_DGRAM, &rcvsock2));
  ASSERT_NE(rcvsock2, LWPA_SOCKET_INVALID);

  ASSERT_EQ(kLwpaErrOk, lwpa_setsockopt(rcvsock2, LWPA_SOL_SOCKET, LWPA_SO_RCVTIMEO, &intval, sizeof(int)));
  ASSERT_EQ(kLwpaErrOk, lwpa_setsockopt(rcvsock2, LWPA_SOL_SOCKET, LWPA_SO_REUSEADDR, &intval, sizeof(int)));

  ASSERT_EQ(kLwpaErrOk, lwpa_socket(LWPA_AF_INET, LWPA_DGRAM, &send_sock));
  ASSERT_NE(send_sock, LWPA_SOCKET_INVALID);

  ASSERT_EQ(kLwpaErrOk, lwpa_setsockopt(send_sock, LWPA_IPPROTO_IP, LWPA_IP_MULTICAST_LOOP, &intval, sizeof(int)));
  ASSERT_EQ(kLwpaErrOk, lwpa_setsockopt(send_sock, LWPA_IPPROTO_IP, LWPA_IP_MULTICAST_IF, &default_netint_.addr,
                                        sizeof default_netint_.addr));

  // Bind socket 1 to the wildcard address and a specific port.
  lwpa_ip_set_wildcard(kLwpaIpTypeV4, &bind_addr.ip);
  bind_addr.port = MULTICAST_UDP_PORT_BASE;
  ASSERT_EQ(kLwpaErrOk, lwpa_bind(rcvsock1, &bind_addr));

  // Bind socket 2 to the wildcard address and a different port.
  bind_addr.port = MULTICAST_UDP_PORT_BASE + 1;
  ASSERT_EQ(kLwpaErrOk, lwpa_bind(rcvsock2, &bind_addr));

  // Subscribe socket 1 to the multicast address.
  LwpaGroupReq greq;
  greq.ifindex = default_netint_.index;
  LWPA_IP_SET_V4_ADDRESS(&greq.group, TEST_MCAST_ADDR);
  ASSERT_EQ(kLwpaErrOk, lwpa_setsockopt(rcvsock1, LWPA_IPPROTO_IP, LWPA_MCAST_JOIN_GROUP, &greq, sizeof greq));

  // Subscribe socket 2 to the multicast address
  ASSERT_EQ(kLwpaErrOk, lwpa_setsockopt(rcvsock2, LWPA_IPPROTO_IP, LWPA_MCAST_JOIN_GROUP, &greq, sizeof greq));

  LWPA_IP_SET_V4_ADDRESS(&send_addr_1.ip, TEST_MCAST_ADDR);
  send_addr_1.port = MULTICAST_UDP_PORT_BASE;

  // Start the send thread.
  std::thread send_thr(send_thread, this);
  ASSERT_TRUE(send_thr.joinable());

  size_t num_packets_received = 0;
  for (size_t i = 0; i < NUM_TEST_PACKETS; ++i)
  {
    LwpaSockaddr from_addr;
    uint8_t buf[SEND_MSG_LEN + 1];

    int res = lwpa_recvfrom(rcvsock1, buf, SEND_MSG_LEN, 0, &from_addr);
    if (res == SEND_MSG_LEN)
    {
      ++num_packets_received;
    }
    else
    {
      EXPECT_EQ(res, kLwpaErrWouldBlock);
      break;
    }

    EXPECT_NE(from_addr.port, MULTICAST_UDP_PORT_BASE);

    buf[SEND_MSG_LEN] = '\0';
    EXPECT_EQ(0, strcmp((char*)buf, SEND_MSG));
  }
  EXPECT_GT(num_packets_received, 0u);

  LwpaSockaddr from_addr;
  uint8_t buf[SEND_MSG_LEN + 1];
  // recvfrom should time out because this socket is bound to a different port and we set the
  // timeout option on this socket.
  ASSERT_GE(0, lwpa_recvfrom(rcvsock2, buf, SEND_MSG_LEN, 0, &from_addr));

  // Let the send thread end
  send_thr.join();

  ASSERT_EQ(kLwpaErrOk, lwpa_close(rcvsock1));
  ASSERT_EQ(kLwpaErrOk, lwpa_close(rcvsock2));
  ASSERT_EQ(kLwpaErrOk, lwpa_close(send_sock));
}

TEST_F(SocketTest, getaddrinfo)
{
  LwpaAddrinfo ai_hints;
  LwpaAddrinfo ai;

  memset(&ai_hints, 0, sizeof ai_hints);
  ai_hints.ai_family = LWPA_AF_INET;
  ASSERT_EQ(kLwpaErrOk, lwpa_getaddrinfo(test_hostname, test_service, &ai_hints, &ai));
  ASSERT_TRUE(LWPA_IP_IS_V4(&ai.ai_addr.ip));
  lwpa_freeaddrinfo(&ai);

  ai_hints.ai_flags = LWPA_AI_NUMERICHOST;
  ASSERT_EQ(kLwpaErrOk, lwpa_getaddrinfo(test_gai_ip_str, test_gai_port_str, &ai_hints, &ai));
  ASSERT_TRUE(LWPA_IP_IS_V4(&ai.ai_addr.ip));
  ASSERT_EQ(LWPA_IP_V4_ADDRESS(&ai.ai_addr.ip), test_gai_ip);
  ASSERT_EQ(ai.ai_addr.port, test_gai_port);
}
