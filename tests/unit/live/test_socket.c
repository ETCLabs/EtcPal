/******************************************************************************
 * Copyright 2022 ETC Inc.
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
 * This file is a part of EtcPal. For more information, go to:
 * https://github.com/ETCLabs/EtcPal
 ******************************************************************************/

// These are defined before the includes to enable ETCPAL_MAX_CONTROL_SIZE_PKTINFO support on Mac & Linux.
#if defined(__linux__) || defined(__APPLE__)
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif  // _GNU_SOURCE

#ifndef __APPLE_USE_RFC_3542
#define __APPLE_USE_RFC_3542
#endif  // __APPLE_USE_RFC_3542
#endif  // defined(__linux__) || defined(__APPLE__)

#if defined(_WIN32) || defined(__APPLE__) || defined(__linux__) || defined(__unix__) || defined(_POSIX_VERSION)
#define TEST_SOCKET_FULL_OS_AVAILABLE 1
#else
#define TEST_SOCKET_FULL_OS_AVAILABLE 0
#endif

#include "etcpal/socket.h"
#include "unity_fixture.h"

#include "etcpal/netint.h"
#include <stddef.h>

// For getaddrinfo
#if 0
static const char* test_hostname = "www.google.com";
static const char* test_service = "http";
#endif

static const char* const kTestGaiIpStr   = "10.101.1.1";
static const uint32_t    kTestGaiIp      = 0x0a650101;
static const char* const kTestGaiPortStr = "8080";
static const uint16_t    kTestGaiPort    = 8080;

TEST_GROUP(etcpal_socket);

TEST_SETUP(etcpal_socket)
{
  etcpal_init(ETCPAL_FEATURE_SOCKETS);
}

TEST_TEAR_DOWN(etcpal_socket)
{
  etcpal_deinit(ETCPAL_FEATURE_SOCKETS);
}

TEST(etcpal_socket, bind_works_as_expected)
{
  etcpal_socket_t sock = ETCPAL_SOCKET_INVALID;

  TEST_ASSERT_EQUAL(kEtcPalErrOk, etcpal_socket(ETCPAL_AF_INET, ETCPAL_SOCK_DGRAM, &sock));
  TEST_ASSERT_NOT_EQUAL(sock, ETCPAL_SOCKET_INVALID);

  // Make sure we can bind to the wildcard address and port
  EtcPalSockAddr bind_addr;
  etcpal_ip_set_wildcard(kEtcPalIpTypeV4, &bind_addr.ip);
  bind_addr.port = 0;
  TEST_ASSERT_EQUAL(kEtcPalErrOk, etcpal_bind(sock, &bind_addr));

  // Shouldn't be able to bind to a closed socket.
  TEST_ASSERT_EQUAL(kEtcPalErrOk, etcpal_close(sock));
  TEST_ASSERT_NOT_EQUAL(kEtcPalErrOk, etcpal_bind(sock, &bind_addr));
}

TEST(etcpal_socket, sockopts)
{
  // TODO, need getsockopt() implemented for this
}

TEST(etcpal_socket, blocking_state_is_consistent)
{
  etcpal_socket_t sock = ETCPAL_SOCKET_INVALID;

  TEST_ASSERT_EQUAL(kEtcPalErrOk, etcpal_socket(ETCPAL_AF_INET, ETCPAL_SOCK_STREAM, &sock));
  TEST_ASSERT_NOT_EQUAL(sock, ETCPAL_SOCKET_INVALID);

  // Set the socket to non-blocking, make sure it reads as non-blocking
  TEST_ASSERT_EQUAL(kEtcPalErrOk, etcpal_setblocking(sock, false));
  bool           is_blocking = true;
  etcpal_error_t gb_result   = etcpal_getblocking(sock, &is_blocking);

  // Special case - this function isn't implemented on all platforms, so we abort this test
  // prematurely if that's the case.
  if (gb_result == kEtcPalErrNotImpl)
  {
    etcpal_close(sock);
    TEST_IGNORE_MESSAGE("etcpal_getblocking() not implemented on this platform. Skipping the remainder of the test.");
  }

  TEST_ASSERT_EQUAL(kEtcPalErrOk, gb_result);
  TEST_ASSERT_FALSE(is_blocking);

  // Set the socket back to blocking, make sure it reads as blocking
  TEST_ASSERT_EQUAL(kEtcPalErrOk, etcpal_setblocking(sock, true));
  TEST_ASSERT_EQUAL(kEtcPalErrOk, etcpal_getblocking(sock, &is_blocking));
  TEST_ASSERT_TRUE(is_blocking);

  TEST_ASSERT_EQUAL(kEtcPalErrOk, etcpal_close(sock));
}

// Test to make sure various invalid calls to etcpal_poll_* functions fail properly.
TEST(etcpal_socket, poll_invalid_calls_fail)
{
  EtcPalPollContext context = ETCPAL_POLL_CONTEXT_INIT;
  TEST_ASSERT_EQUAL(kEtcPalErrOk, etcpal_poll_context_init(&context));

  // Wait should fail with a meaningful error code when no sockets have been added
  EtcPalPollEvent event;
  TEST_ASSERT_EQUAL(kEtcPalErrNoSockets, etcpal_poll_wait(&context, &event, 100));

  etcpal_socket_t sock = ETCPAL_SOCKET_INVALID;
  TEST_ASSERT_EQUAL(kEtcPalErrOk, etcpal_socket(ETCPAL_AF_INET, ETCPAL_SOCK_DGRAM, &sock));
  TEST_ASSERT_NOT_EQUAL(sock, ETCPAL_SOCKET_INVALID);

  // Deinit and make sure add of a valid socket fails
  etcpal_poll_context_deinit(&context);
  TEST_ASSERT_NOT_EQUAL(kEtcPalErrOk, etcpal_poll_add_socket(&context, sock, ETCPAL_POLL_IN, NULL));

  // Initialize the context and add invalid sockets or invalid events
  TEST_ASSERT_EQUAL(kEtcPalErrOk, etcpal_poll_context_init(&context));

  // Add invalid socket
  TEST_ASSERT_NOT_EQUAL(kEtcPalErrOk, etcpal_poll_add_socket(&context, ETCPAL_SOCKET_INVALID, ETCPAL_POLL_IN, NULL));

  // Add socket with invalid events
  TEST_ASSERT_NOT_EQUAL(kEtcPalErrOk, etcpal_poll_add_socket(&context, sock, 0, NULL));
  TEST_ASSERT_NOT_EQUAL(kEtcPalErrOk, etcpal_poll_add_socket(&context, sock, ETCPAL_POLL_ERR, NULL));

  // Try to modify a socket that has not been added
  TEST_ASSERT_NOT_EQUAL(kEtcPalErrOk, etcpal_poll_modify_socket(&context, sock, ETCPAL_POLL_IN, NULL));

  // Add the socket and try to modify it with invalid calls
  TEST_ASSERT_EQUAL(kEtcPalErrOk, etcpal_poll_add_socket(&context, sock, ETCPAL_POLL_IN, NULL));
  TEST_ASSERT_NOT_EQUAL(kEtcPalErrOk, etcpal_poll_modify_socket(&context, sock, 0, NULL));  // Invalid events
  TEST_ASSERT_NOT_EQUAL(kEtcPalErrOk,
                        etcpal_poll_modify_socket(&context, sock, ETCPAL_POLL_ERR, NULL));  // Invalid events

  // Deinit and make sure we cannot modify
  etcpal_poll_context_deinit(&context);
  TEST_ASSERT_NOT_EQUAL(kEtcPalErrOk, etcpal_poll_modify_socket(&context, sock, ETCPAL_POLL_OUT, NULL));

  TEST_ASSERT_EQUAL(kEtcPalErrOk, etcpal_close(sock));
}

// Test that we can add and remove sockets using etcpal_poll_add_socket() and
// etcpal_poll_remove_socket() with predictable behavior resulting
TEST(etcpal_socket, poll_add_remove_socket_works)
{
  EtcPalPollContext context = ETCPAL_POLL_CONTEXT_INIT;
  TEST_ASSERT_EQUAL(kEtcPalErrOk, etcpal_poll_context_init(&context));

  etcpal_socket_t sock = ETCPAL_SOCKET_INVALID;
  TEST_ASSERT_EQUAL(kEtcPalErrOk, etcpal_socket(ETCPAL_AF_INET, ETCPAL_SOCK_DGRAM, &sock));
  TEST_ASSERT_NOT_EQUAL(sock, ETCPAL_SOCKET_INVALID);

  TEST_ASSERT_EQUAL(kEtcPalErrOk, etcpal_poll_add_socket(&context, sock, ETCPAL_POLL_IN, NULL));
  etcpal_poll_remove_socket(&context, sock);
  TEST_ASSERT_EQUAL(kEtcPalErrOk, etcpal_poll_add_socket(&context, sock, ETCPAL_POLL_OUT, NULL));
  etcpal_poll_remove_socket(&context, sock);

  etcpal_poll_context_deinit(&context);
  TEST_ASSERT_EQUAL(kEtcPalErrOk, etcpal_close(sock));
}

TEST(etcpal_socket, poll_user_data_works)
{
  etcpal_socket_t sock_1      = ETCPAL_SOCKET_INIT;
  etcpal_socket_t sock_2      = ETCPAL_SOCKET_INIT;
  void*           user_data_1 = (void*)1;
  void*           user_data_2 = (void*)2;

  EtcPalPollContext context = ETCPAL_POLL_CONTEXT_INIT;
  TEST_ASSERT_EQUAL(kEtcPalErrOk, etcpal_poll_context_init(&context));

  // Create two UDP sockets and poll for writability, make sure our user data gets passed back to us
  // intact.

  TEST_ASSERT_EQUAL(kEtcPalErrOk, etcpal_socket(ETCPAL_AF_INET, ETCPAL_SOCK_DGRAM, &sock_1));
  TEST_ASSERT_EQUAL(kEtcPalErrOk, etcpal_socket(ETCPAL_AF_INET, ETCPAL_SOCK_DGRAM, &sock_2));

  TEST_ASSERT_EQUAL(kEtcPalErrOk, etcpal_poll_add_socket(&context, sock_1, ETCPAL_POLL_OUT, user_data_1));

  EtcPalPollEvent event;
  TEST_ASSERT_EQUAL(kEtcPalErrOk, etcpal_poll_wait(&context, &event, 100));
  TEST_ASSERT_EQUAL(event.socket, sock_1);
  TEST_ASSERT_EQUAL_PTR(event.user_data, user_data_1);

  etcpal_poll_remove_socket(&context, sock_1);
  TEST_ASSERT_EQUAL(kEtcPalErrOk, etcpal_poll_add_socket(&context, sock_2, ETCPAL_POLL_OUT, user_data_2));

  TEST_ASSERT_EQUAL(kEtcPalErrOk, etcpal_poll_wait(&context, &event, 100));
  TEST_ASSERT_EQUAL(event.socket, sock_2);
  TEST_ASSERT_EQUAL_PTR(event.user_data, user_data_2);

  // Modify and make sure we get the updated user data
  TEST_ASSERT_EQUAL(kEtcPalErrOk, etcpal_poll_modify_socket(&context, sock_2, ETCPAL_POLL_OUT, user_data_1));
  TEST_ASSERT_EQUAL(kEtcPalErrOk, etcpal_poll_wait(&context, &event, 100));
  TEST_ASSERT_EQUAL(event.socket, sock_2);
  TEST_ASSERT_EQUAL_PTR(event.user_data, user_data_1);

  TEST_ASSERT_EQUAL(kEtcPalErrOk, etcpal_close(sock_1));
  TEST_ASSERT_EQUAL(kEtcPalErrOk, etcpal_close(sock_2));
  etcpal_poll_context_deinit(&context);
}

#define POLL_MODIFY_TEST_PORT_BASE 8000

// Test the etcpal_poll_modify_socket() functionality, using UDP sockets for simplicity
TEST(etcpal_socket, poll_modify_socket_works)
{
  etcpal_socket_t sock = ETCPAL_SOCKET_INVALID;

  EtcPalPollContext context = ETCPAL_POLL_CONTEXT_INIT;
  TEST_ASSERT_EQUAL(kEtcPalErrOk, etcpal_poll_context_init(&context));

  TEST_ASSERT_EQUAL(kEtcPalErrOk, etcpal_socket(ETCPAL_AF_INET, ETCPAL_SOCK_DGRAM, &sock));
  TEST_ASSERT_NOT_EQUAL(sock, ETCPAL_SOCKET_INVALID);

  // Bind the socket to the wildcard address and a specific port.
  EtcPalSockAddr bind_addr;
  etcpal_ip_set_wildcard(kEtcPalIpTypeV4, &bind_addr.ip);
  bind_addr.port = POLL_MODIFY_TEST_PORT_BASE;
  TEST_ASSERT_EQUAL(kEtcPalErrOk, etcpal_bind(sock, &bind_addr));

  // Add it for output polling first
  TEST_ASSERT_EQUAL(kEtcPalErrOk, etcpal_poll_add_socket(&context, sock, ETCPAL_POLL_OUT, NULL));

  // Socket should be ready right away
  EtcPalPollEvent event;
  TEST_ASSERT_EQUAL(kEtcPalErrOk, etcpal_poll_wait(&context, &event, 100));
  TEST_ASSERT_EQUAL(event.socket, sock);
  TEST_ASSERT_EQUAL(event.events, ETCPAL_POLL_OUT);
  TEST_ASSERT_EQUAL(event.err, kEtcPalErrOk);

  // Modify it to do input polling
  TEST_ASSERT_EQUAL(kEtcPalErrOk, etcpal_poll_modify_socket(&context, sock, ETCPAL_POLL_IN, NULL));

  // Should time out now
  TEST_ASSERT_EQUAL(kEtcPalErrTimedOut, etcpal_poll_wait(&context, &event, 100));

  // Send data to socket
  EtcPalSockAddr send_addr;
  ETCPAL_IP_SET_V4_ADDRESS(&send_addr.ip, 0x7f000001);
  send_addr.port = POLL_MODIFY_TEST_PORT_BASE;
  etcpal_sendto(sock, (const uint8_t*)"test message", sizeof("test message"), 0, &send_addr);

  // Should get the poll in event
  TEST_ASSERT_EQUAL(kEtcPalErrOk, etcpal_poll_wait(&context, &event, 100));
  TEST_ASSERT_EQUAL(event.socket, sock);
  TEST_ASSERT_EQUAL(event.events, ETCPAL_POLL_IN);
  TEST_ASSERT_EQUAL(event.err, kEtcPalErrOk);

  TEST_ASSERT_EQUAL(kEtcPalErrOk, etcpal_close(sock));
  etcpal_poll_context_deinit(&context);
}

#define POLL_UDP_IN_TEST_PORT_BASE 9000

// Test the etcpal_poll_* API, polling for readability on UDP sockets.
TEST(etcpal_socket, poll_for_readability_on_udp_sockets_works)
{
  etcpal_socket_t send_sock = ETCPAL_SOCKET_INVALID;
  etcpal_socket_t rcvsock1  = ETCPAL_SOCKET_INVALID;
  etcpal_socket_t rcvsock2  = ETCPAL_SOCKET_INVALID;

  EtcPalPollContext context = ETCPAL_POLL_CONTEXT_INIT;
  TEST_ASSERT_EQUAL(kEtcPalErrOk, etcpal_poll_context_init(&context));

  TEST_ASSERT_EQUAL(kEtcPalErrOk, etcpal_socket(ETCPAL_AF_INET, ETCPAL_SOCK_DGRAM, &rcvsock1));
  TEST_ASSERT_NOT_EQUAL(rcvsock1, ETCPAL_SOCKET_INVALID);

  TEST_ASSERT_EQUAL(kEtcPalErrOk, etcpal_socket(ETCPAL_AF_INET, ETCPAL_SOCK_DGRAM, &rcvsock2));
  TEST_ASSERT_NOT_EQUAL(rcvsock2, ETCPAL_SOCKET_INVALID);

  TEST_ASSERT_EQUAL(kEtcPalErrOk, etcpal_socket(ETCPAL_AF_INET, ETCPAL_SOCK_DGRAM, &send_sock));
  TEST_ASSERT_NOT_EQUAL(send_sock, ETCPAL_SOCKET_INVALID);

  // Bind socket 1 to the wildcard address and a specific port.
  EtcPalSockAddr bind_addr;
  etcpal_ip_set_wildcard(kEtcPalIpTypeV4, &bind_addr.ip);
  bind_addr.port = POLL_UDP_IN_TEST_PORT_BASE;
  TEST_ASSERT_EQUAL(kEtcPalErrOk, etcpal_bind(rcvsock1, &bind_addr));

  // Bind socket 2 to the wildcard address and a different port.
  bind_addr.port = POLL_UDP_IN_TEST_PORT_BASE + 1;
  TEST_ASSERT_EQUAL(kEtcPalErrOk, etcpal_bind(rcvsock2, &bind_addr));

  // Get the poll context set up
  TEST_ASSERT_EQUAL(kEtcPalErrOk, etcpal_poll_add_socket(&context, rcvsock1, ETCPAL_POLL_IN, NULL));
  TEST_ASSERT_EQUAL(kEtcPalErrOk, etcpal_poll_add_socket(&context, rcvsock2, ETCPAL_POLL_IN, NULL));

  // Test poll with nothing sending - should time out.
  EtcPalPollEvent event;
  TEST_ASSERT_EQUAL(kEtcPalErrTimedOut, etcpal_poll_wait(&context, &event, 100));

  EtcPalSockAddr send_addr;
  ETCPAL_IP_SET_V4_ADDRESS(&send_addr.ip, 0x7f000001);
  send_addr.port = POLL_UDP_IN_TEST_PORT_BASE;

#define POLL_UDP_IN_TEST_MESSAGE        (const uint8_t*)"test message"
#define POLL_UDP_IN_TEST_MESSAGE_LENGTH sizeof("test message")

  etcpal_sendto(send_sock, POLL_UDP_IN_TEST_MESSAGE, POLL_UDP_IN_TEST_MESSAGE_LENGTH, 0, &send_addr);
  send_addr.port = POLL_UDP_IN_TEST_PORT_BASE + 1;
  etcpal_sendto(send_sock, POLL_UDP_IN_TEST_MESSAGE, POLL_UDP_IN_TEST_MESSAGE_LENGTH, 0, &send_addr);

  // Poll once, make sure we get one of the sockets.
  TEST_ASSERT_EQUAL(kEtcPalErrOk, etcpal_poll_wait(&context, &event, 1000));
  TEST_ASSERT(event.socket == rcvsock1 || event.socket == rcvsock2);
  TEST_ASSERT_EQUAL(event.events, ETCPAL_POLL_IN);
  TEST_ASSERT_EQUAL(event.err, kEtcPalErrOk);

  // Receive data on the socket.
  uint8_t        recv_buf[POLL_UDP_IN_TEST_MESSAGE_LENGTH] = {0};
  EtcPalSockAddr from_addr;
  TEST_ASSERT_EQUAL(POLL_UDP_IN_TEST_MESSAGE_LENGTH,
                    (size_t)etcpal_recvfrom(event.socket, recv_buf, POLL_UDP_IN_TEST_MESSAGE_LENGTH, 0, &from_addr));
  TEST_ASSERT_EQUAL(etcpal_ip_cmp(&send_addr.ip, &from_addr.ip), 0);
  TEST_ASSERT_NOT_EQUAL(from_addr.port, POLL_UDP_IN_TEST_PORT_BASE);
  TEST_ASSERT_NOT_EQUAL(from_addr.port, POLL_UDP_IN_TEST_PORT_BASE + 1);
  TEST_ASSERT_EQUAL_MEMORY(recv_buf, POLL_UDP_IN_TEST_MESSAGE, POLL_UDP_IN_TEST_MESSAGE_LENGTH);

  etcpal_socket_t first_socket = event.socket;

  // Poll a second time for the other socket.
  TEST_ASSERT_EQUAL(kEtcPalErrOk, etcpal_poll_wait(&context, &event, 1000));
  if (first_socket == rcvsock1)
    TEST_ASSERT_EQUAL(event.socket, rcvsock2);
  else
    TEST_ASSERT_EQUAL(event.socket, rcvsock1);
  TEST_ASSERT_EQUAL(event.events, ETCPAL_POLL_IN);
  TEST_ASSERT_EQUAL(event.err, kEtcPalErrOk);

  TEST_ASSERT_EQUAL(kEtcPalErrOk, etcpal_close(rcvsock1));
  TEST_ASSERT_EQUAL(kEtcPalErrOk, etcpal_close(rcvsock2));
  TEST_ASSERT_EQUAL(kEtcPalErrOk, etcpal_close(send_sock));
  etcpal_poll_context_deinit(&context);
}

// Test the etcpal_poll_* API, polling for writeablility on UDP sockets.
TEST(etcpal_socket, poll_for_writability_on_udp_sockets_works)
{
  etcpal_socket_t sock_1 = ETCPAL_SOCKET_INIT;
  etcpal_socket_t sock_2 = ETCPAL_SOCKET_INIT;

  EtcPalPollContext context = ETCPAL_POLL_CONTEXT_INIT;
  TEST_ASSERT_EQUAL(kEtcPalErrOk, etcpal_poll_context_init(&context));

  TEST_ASSERT_EQUAL(kEtcPalErrOk, etcpal_socket(ETCPAL_AF_INET, ETCPAL_SOCK_DGRAM, &sock_1));
  TEST_ASSERT_EQUAL(kEtcPalErrOk, etcpal_socket(ETCPAL_AF_INET, ETCPAL_SOCK_DGRAM, &sock_2));

  // The sockets should poll as ready for output right away. Not sure what else there is to test
  // here.
  TEST_ASSERT_EQUAL(kEtcPalErrOk, etcpal_poll_add_socket(&context, sock_1, ETCPAL_POLL_OUT, NULL));

  EtcPalPollEvent event;
  TEST_ASSERT_EQUAL(kEtcPalErrOk, etcpal_poll_wait(&context, &event, 100));
  TEST_ASSERT_EQUAL(event.socket, sock_1);
  TEST_ASSERT_EQUAL(event.events, ETCPAL_POLL_OUT);
  TEST_ASSERT_EQUAL(event.err, kEtcPalErrOk);

  etcpal_poll_remove_socket(&context, sock_1);
  TEST_ASSERT_EQUAL(kEtcPalErrOk, etcpal_poll_add_socket(&context, sock_2, ETCPAL_POLL_OUT, NULL));

  TEST_ASSERT_EQUAL(kEtcPalErrOk, etcpal_poll_wait(&context, &event, 100));
  TEST_ASSERT_EQUAL(event.socket, sock_2);
  TEST_ASSERT_EQUAL(event.events, ETCPAL_POLL_OUT);
  TEST_ASSERT_EQUAL(event.err, kEtcPalErrOk);

  TEST_ASSERT_EQUAL(kEtcPalErrOk, etcpal_close(sock_1));
  TEST_ASSERT_EQUAL(kEtcPalErrOk, etcpal_close(sock_2));
  etcpal_poll_context_deinit(&context);
}

// The lwIP select function requires the max socket fd + 1 as the first parameter. This test ensures an edge case is
// correctly handled where the max fd is removed.
TEST(etcpal_socket, poll_still_works_when_max_fd_removed)
{
  etcpal_socket_t sock_1 = ETCPAL_SOCKET_INIT;
  etcpal_socket_t sock_2 = ETCPAL_SOCKET_INIT;

  EtcPalPollContext context = ETCPAL_POLL_CONTEXT_INIT;
  TEST_ASSERT_EQUAL(kEtcPalErrOk, etcpal_poll_context_init(&context));

  TEST_ASSERT_EQUAL(kEtcPalErrOk, etcpal_socket(ETCPAL_AF_INET, ETCPAL_SOCK_DGRAM, &sock_1));
  TEST_ASSERT_EQUAL(kEtcPalErrOk, etcpal_socket(ETCPAL_AF_INET, ETCPAL_SOCK_DGRAM, &sock_2));

  TEST_ASSERT_EQUAL(kEtcPalErrOk, etcpal_poll_add_socket(&context, sock_1, ETCPAL_POLL_OUT, NULL));
  TEST_ASSERT_EQUAL(kEtcPalErrOk, etcpal_poll_add_socket(&context, sock_2, ETCPAL_POLL_OUT, NULL));

  etcpal_socket_t max_fd = (sock_1 > sock_2) ? sock_1 : sock_2;
  etcpal_socket_t min_fd = (sock_1 > sock_2) ? sock_2 : sock_1;

  etcpal_poll_remove_socket(&context, max_fd);

  EtcPalPollEvent event;
  TEST_ASSERT_EQUAL(kEtcPalErrOk, etcpal_poll_wait(&context, &event, 100));
  TEST_ASSERT_EQUAL(event.socket, min_fd);
  TEST_ASSERT_EQUAL(event.events, ETCPAL_POLL_OUT);
  TEST_ASSERT_EQUAL(event.err, kEtcPalErrOk);

  TEST_ASSERT_EQUAL(kEtcPalErrOk, etcpal_close(sock_1));
  TEST_ASSERT_EQUAL(kEtcPalErrOk, etcpal_close(sock_2));
  etcpal_poll_context_deinit(&context);
}

TEST(etcpal_socket, getaddrinfo_works_as_expected)
{
  EtcPalAddrinfo ai_hints;
  EtcPalAddrinfo ai;

  memset(&ai_hints, 0, sizeof ai_hints);

  // We can't currently assume internet access for our tests.
#if 0
  ai_hints.ai_family = ETCPAL_AF_INET;
  TEST_ASSERT_EQUAL(kEtcPalErrOk, etcpal_getaddrinfo(test_hostname, test_service, &ai_hints, &ai));
  TEST_ASSERT(ETCPAL_IP_IS_V4(&ai.ai_addr.ip));
  etcpal_freeaddrinfo(&ai);
#endif

  ai_hints.ai_flags = ETCPAL_AI_NUMERICHOST;
  TEST_ASSERT_EQUAL(kEtcPalErrOk, etcpal_getaddrinfo(kTestGaiIpStr, kTestGaiPortStr, &ai_hints, &ai));
  TEST_ASSERT(ETCPAL_IP_IS_V4(&ai.ai_addr.ip));
  TEST_ASSERT_EQUAL_UINT32(ETCPAL_IP_V4_ADDRESS(&ai.ai_addr.ip), kTestGaiIp);
  TEST_ASSERT_EQUAL_UINT16(ai.ai_addr.port, kTestGaiPort);
  etcpal_freeaddrinfo(&ai);
}

#define RECVMSG_TEST_PORT_BASE      9000
#define RECVMSG_TEST_MESSAGE        (const uint8_t*)"test message"
#define RECVMSG_TEST_MESSAGE_LENGTH sizeof("test message")

static void recvmsg_test_setup(etcpal_socket_t* recv_sock)
{
  etcpal_socket_t send_sock = ETCPAL_SOCKET_INVALID;

  TEST_ASSERT_EQUAL(kEtcPalErrOk, etcpal_socket(ETCPAL_AF_INET, ETCPAL_SOCK_DGRAM, recv_sock));
  TEST_ASSERT_NOT_EQUAL(*recv_sock, ETCPAL_SOCKET_INVALID);

  TEST_ASSERT_EQUAL(kEtcPalErrOk, etcpal_socket(ETCPAL_AF_INET, ETCPAL_SOCK_DGRAM, &send_sock));
  TEST_ASSERT_NOT_EQUAL(send_sock, ETCPAL_SOCKET_INVALID);

  int intval = 1;
#ifdef __MQX__
  TEST_IGNORE_MESSAGE("ETCPAL_IP_PKTINFO not implemented on this platform.");
#else
  TEST_ASSERT_EQUAL(kEtcPalErrOk,
                    etcpal_setsockopt(*recv_sock, ETCPAL_IPPROTO_IP, ETCPAL_IP_PKTINFO, &intval, sizeof(int)));
#endif
  intval = 10;
  TEST_ASSERT_EQUAL(kEtcPalErrOk,
                    etcpal_setsockopt(*recv_sock, ETCPAL_SOL_SOCKET, ETCPAL_SO_RCVTIMEO, &intval, sizeof(int)));

  EtcPalSockAddr bind_addr;
  etcpal_ip_set_wildcard(kEtcPalIpTypeV4, &bind_addr.ip);
  bind_addr.port = RECVMSG_TEST_PORT_BASE;
  TEST_ASSERT_EQUAL(kEtcPalErrOk, etcpal_bind(*recv_sock, &bind_addr));

  EtcPalSockAddr send_addr;
  ETCPAL_IP_SET_V4_ADDRESS(&send_addr.ip, 0x7f000001);
  send_addr.port = RECVMSG_TEST_PORT_BASE;

  etcpal_sendto(send_sock, RECVMSG_TEST_MESSAGE, RECVMSG_TEST_MESSAGE_LENGTH, 0, &send_addr);

  etcpal_close(send_sock);
}

TEST(etcpal_socket, recvmsg_works)
{
  etcpal_socket_t recv_sock = ETCPAL_SOCKET_INIT;
  recvmsg_test_setup(&recv_sock);

  uint8_t buf[RECVMSG_TEST_MESSAGE_LENGTH + 1]     = {0};
  uint8_t control[ETCPAL_MAX_CONTROL_SIZE_PKTINFO] = {0};

  EtcPalMsgHdr msg = {{0}};
  msg.buf          = buf;
  msg.buflen       = RECVMSG_TEST_MESSAGE_LENGTH;
  msg.control      = control;
  msg.controllen   = ETCPAL_MAX_CONTROL_SIZE_PKTINFO;

  TEST_ASSERT_EQUAL(RECVMSG_TEST_MESSAGE_LENGTH, etcpal_recvmsg(recv_sock, &msg, 0));

  buf[RECVMSG_TEST_MESSAGE_LENGTH] = '\0';
  TEST_ASSERT_EQUAL_STRING((char*)buf, RECVMSG_TEST_MESSAGE);

  // Verify nothing remains on the input queue.
  etcpal_error_t result = etcpal_recvmsg(recv_sock, &msg, 0);
  TEST_ASSERT((result == kEtcPalErrTimedOut) || (result == kEtcPalErrWouldBlock));

  etcpal_close(recv_sock);
}

TEST(etcpal_socket, recvmsg_trunc_flag_works)
{
  etcpal_socket_t recv_sock = ETCPAL_SOCKET_INIT;
  recvmsg_test_setup(&recv_sock);

  uint8_t buf[RECVMSG_TEST_MESSAGE_LENGTH]         = {0};
  uint8_t control[ETCPAL_MAX_CONTROL_SIZE_PKTINFO] = {0};

  EtcPalMsgHdr msg = {{0}};
  msg.buf          = buf;
  msg.buflen       = RECVMSG_TEST_MESSAGE_LENGTH - 1;  // Intentionally 1 byte short to trigger TRUNC flag.
  msg.control      = control;
  msg.controllen   = ETCPAL_MAX_CONTROL_SIZE_PKTINFO;

  // Windows, Mac, and Linux return the truncated length here, but lwIP returns the full length.
  TEST_ASSERT(etcpal_recvmsg(recv_sock, &msg, 0) >= (RECVMSG_TEST_MESSAGE_LENGTH - 1));
  TEST_ASSERT_EQUAL(ETCPAL_MSG_TRUNC, msg.flags);

  // Verify nothing remains on the input queue.
  etcpal_error_t result = etcpal_recvmsg(recv_sock, &msg, 0);
  TEST_ASSERT((result == kEtcPalErrTimedOut) || (result == kEtcPalErrWouldBlock));

  etcpal_close(recv_sock);
}

TEST(etcpal_socket, recvmsg_ctrunc_flag_works)
{
  etcpal_socket_t recv_sock = ETCPAL_SOCKET_INIT;
  recvmsg_test_setup(&recv_sock);

  uint8_t buf[RECVMSG_TEST_MESSAGE_LENGTH]         = {0};
  uint8_t control[ETCPAL_MAX_CONTROL_SIZE_PKTINFO] = {0};

  EtcPalMsgHdr msg = {{0}};
  msg.buf          = buf;
  msg.buflen       = RECVMSG_TEST_MESSAGE_LENGTH;
  msg.control      = control;
  msg.controllen   = 1u;  // Intentionally too short to trigger CTRUNC flag.

  TEST_ASSERT_EQUAL(RECVMSG_TEST_MESSAGE_LENGTH, etcpal_recvmsg(recv_sock, &msg, 0));
  TEST_ASSERT_EQUAL(ETCPAL_MSG_CTRUNC, msg.flags);

  // Verify nothing remains on the input queue.
  etcpal_error_t result = etcpal_recvmsg(recv_sock, &msg, 0);
  TEST_ASSERT((result == kEtcPalErrTimedOut) || (result == kEtcPalErrWouldBlock));

  etcpal_close(recv_sock);
}

TEST(etcpal_socket, recvmsg_peek_flag_works)
{
  etcpal_socket_t recv_sock = ETCPAL_SOCKET_INIT;
  recvmsg_test_setup(&recv_sock);

  uint8_t buf[RECVMSG_TEST_MESSAGE_LENGTH + 1]     = {0};
  uint8_t control[ETCPAL_MAX_CONTROL_SIZE_PKTINFO] = {0};

  EtcPalMsgHdr msg = {{0}};
  msg.buf          = buf;
  msg.buflen       = RECVMSG_TEST_MESSAGE_LENGTH;
  msg.control      = control;
  msg.controllen   = ETCPAL_MAX_CONTROL_SIZE_PKTINFO;

  TEST_ASSERT_EQUAL(RECVMSG_TEST_MESSAGE_LENGTH, etcpal_recvmsg(recv_sock, &msg, ETCPAL_MSG_PEEK));

  buf[RECVMSG_TEST_MESSAGE_LENGTH] = '\0';
  TEST_ASSERT_EQUAL_STRING((char*)buf, RECVMSG_TEST_MESSAGE);

  // Verify the same data remained in the input queue, and this time remove it.
  memset(buf, 0, RECVMSG_TEST_MESSAGE_LENGTH);

  TEST_ASSERT_EQUAL(RECVMSG_TEST_MESSAGE_LENGTH, etcpal_recvmsg(recv_sock, &msg, 0));

  buf[RECVMSG_TEST_MESSAGE_LENGTH] = '\0';
  TEST_ASSERT_EQUAL_STRING((char*)buf, RECVMSG_TEST_MESSAGE);

  // Verify nothing remains on the input queue.
  etcpal_error_t result = etcpal_recvmsg(recv_sock, &msg, 0);
  TEST_ASSERT((result == kEtcPalErrTimedOut) || (result == kEtcPalErrWouldBlock));

  etcpal_close(recv_sock);
}

TEST(etcpal_socket, recvmsg_trunc_peek_works)
{
  etcpal_socket_t recv_sock = ETCPAL_SOCKET_INIT;
  recvmsg_test_setup(&recv_sock);

  uint8_t buf[RECVMSG_TEST_MESSAGE_LENGTH + 1]     = {0};
  uint8_t control[ETCPAL_MAX_CONTROL_SIZE_PKTINFO] = {0};

  EtcPalMsgHdr msg = {{0}};
  msg.buf          = buf;
  msg.buflen       = 1u;  // Intentionally too short to trigger TRUNC flag.
  msg.control      = control;
  msg.controllen   = ETCPAL_MAX_CONTROL_SIZE_PKTINFO;

  // Windows, Mac, and Linux return the truncated length here, but lwIP returns the full length.
  TEST_ASSERT(etcpal_recvmsg(recv_sock, &msg, ETCPAL_MSG_PEEK) >= 1);
  TEST_ASSERT(msg.flags & ETCPAL_MSG_TRUNC);

  // Verify the full, non-truncatad data can still be received, removing it from the input queue.
  memset(buf, 0, RECVMSG_TEST_MESSAGE_LENGTH);
  msg.buflen = RECVMSG_TEST_MESSAGE_LENGTH;
  TEST_ASSERT_EQUAL(RECVMSG_TEST_MESSAGE_LENGTH, etcpal_recvmsg(recv_sock, &msg, 0));

  buf[RECVMSG_TEST_MESSAGE_LENGTH] = '\0';
  TEST_ASSERT_EQUAL_STRING((char*)buf, RECVMSG_TEST_MESSAGE);

  TEST_ASSERT_EQUAL(0, msg.flags);

  // Verify nothing remains on the input queue.
  etcpal_error_t result = etcpal_recvmsg(recv_sock, &msg, 0);
  TEST_ASSERT((result == kEtcPalErrTimedOut) || (result == kEtcPalErrWouldBlock));

  etcpal_close(recv_sock);
}

#if TEST_SOCKET_FULL_OS_AVAILABLE
TEST(etcpal_socket, so_sndbuf_works)
{
  etcpal_socket_t sock = ETCPAL_SOCKET_INVALID;

  TEST_ASSERT_EQUAL(kEtcPalErrOk, etcpal_socket(ETCPAL_AF_INET, ETCPAL_SOCK_DGRAM, &sock));
  TEST_ASSERT_NOT_EQUAL(sock, ETCPAL_SOCKET_INVALID);

  int set_sndbuf_val = 115000;
  TEST_ASSERT_EQUAL(kEtcPalErrOk,
                    etcpal_setsockopt(sock, ETCPAL_SOL_SOCKET, ETCPAL_SO_SNDBUF, &set_sndbuf_val, sizeof(int)));

  int    get_sndbuf_val  = 0;
  size_t get_sndbuf_size = sizeof(int);
  TEST_ASSERT_EQUAL(kEtcPalErrOk,
                    etcpal_getsockopt(sock, ETCPAL_SOL_SOCKET, ETCPAL_SO_SNDBUF, &get_sndbuf_val, &get_sndbuf_size));
  TEST_ASSERT_EQUAL(set_sndbuf_val, get_sndbuf_val);
  TEST_ASSERT_EQUAL(sizeof(int), get_sndbuf_size);

  etcpal_close(sock);
}
#endif  // TEST_SOCKET_FULL_OS_AVAILABLE

TEST_GROUP_RUNNER(etcpal_socket)
{
  RUN_TEST_CASE(etcpal_socket, bind_works_as_expected);
  RUN_TEST_CASE(etcpal_socket, sockopts);
  RUN_TEST_CASE(etcpal_socket, blocking_state_is_consistent);
  RUN_TEST_CASE(etcpal_socket, poll_invalid_calls_fail);
  RUN_TEST_CASE(etcpal_socket, poll_add_remove_socket_works);
  RUN_TEST_CASE(etcpal_socket, poll_user_data_works);
  RUN_TEST_CASE(etcpal_socket, poll_modify_socket_works);
  RUN_TEST_CASE(etcpal_socket, poll_for_readability_on_udp_sockets_works);
  RUN_TEST_CASE(etcpal_socket, poll_for_writability_on_udp_sockets_works);
  RUN_TEST_CASE(etcpal_socket, getaddrinfo_works_as_expected);
  RUN_TEST_CASE(etcpal_socket, recvmsg_works);
  RUN_TEST_CASE(etcpal_socket, recvmsg_trunc_flag_works);
  RUN_TEST_CASE(etcpal_socket, recvmsg_ctrunc_flag_works);
  RUN_TEST_CASE(etcpal_socket, recvmsg_peek_flag_works);
  RUN_TEST_CASE(etcpal_socket, recvmsg_trunc_peek_works);
#if TEST_SOCKET_FULL_OS_AVAILABLE
  RUN_TEST_CASE(etcpal_socket, so_sndbuf_works);
#endif  // TEST_SOCKET_FULL_OS_AVAILABLE
}
