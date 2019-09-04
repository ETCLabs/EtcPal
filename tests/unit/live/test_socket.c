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
 * This file is a part of EtcPal. For more information, go to:
 * https://github.com/ETCLabs/EtcPal
 ******************************************************************************/
#include "etcpal/socket.h"
#include "unity_fixture.h"

#include "etcpal/netint.h"
#include <stddef.h>

// For getaddrinfo
#if 0
static const char* test_hostname = "www.google.com";
static const char* test_service = "http";
#endif

static const char* test_gai_ip_str = "10.101.1.1";
static const uint32_t test_gai_ip = 0x0a650101;
static const char* test_gai_port_str = "8080";
static const uint16_t test_gai_port = 8080;

TEST_GROUP(lwpa_socket);

TEST_SETUP(lwpa_socket)
{
  lwpa_init(LWPA_FEATURE_SOCKETS);
}

TEST_TEAR_DOWN(lwpa_socket)
{
  lwpa_deinit(LWPA_FEATURE_SOCKETS);
}

TEST(lwpa_socket, bind_works_as_expected)
{
  lwpa_socket_t sock = LWPA_SOCKET_INVALID;

  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_socket(LWPA_AF_INET, LWPA_DGRAM, &sock));
  TEST_ASSERT_NOT_EQUAL(sock, LWPA_SOCKET_INVALID);

  // Make sure we can bind to the wildcard address and port
  LwpaSockaddr bind_addr;
  lwpa_ip_set_wildcard(kLwpaIpTypeV4, &bind_addr.ip);
  bind_addr.port = 0;
  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_bind(sock, &bind_addr));

  // Shouldn't be able to bind to a closed socket.
  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_close(sock));
  TEST_ASSERT_NOT_EQUAL(kLwpaErrOk, lwpa_bind(sock, &bind_addr));
}

TEST(lwpa_socket, sockopts)
{
  // TODO, need getsockopt() implemented for this
}

TEST(lwpa_socket, blocking_state_is_consistent)
{
  lwpa_socket_t sock;

  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_socket(LWPA_AF_INET, LWPA_STREAM, &sock));
  TEST_ASSERT_NOT_EQUAL(sock, LWPA_SOCKET_INVALID);

  // Set the socket to non-blocking, make sure it reads as non-blocking
  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_setblocking(sock, false));
  bool is_blocking;
  lwpa_error_t gb_result = lwpa_getblocking(sock, &is_blocking);

  // Special case - this function isn't implemented on all platforms, so we abort this test
  // prematurely if that's the case.
  if (gb_result == kLwpaErrNotImpl)
  {
    lwpa_close(sock);
    TEST_PASS_MESSAGE("lwpa_getblocking() not implemented on this platform. Skipping the remainder of the test.");
  }

  TEST_ASSERT_EQUAL(kLwpaErrOk, gb_result);
  TEST_ASSERT_FALSE(is_blocking);

  // Set the socket back to blocking, make sure it reads as blocking
  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_setblocking(sock, true));
  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_getblocking(sock, &is_blocking));
  TEST_ASSERT_TRUE(is_blocking);

  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_close(sock));
}

// Test to make sure various invalid calls to lwpa_poll_* functions fail properly.
TEST(lwpa_socket, poll_invalid_calls_fail)
{
  LwpaPollContext context;
  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_poll_context_init(&context));

  // Wait should fail with a meaningful error code when no sockets have been added
  LwpaPollEvent event;
  TEST_ASSERT_EQUAL(kLwpaErrNoSockets, lwpa_poll_wait(&context, &event, 100));

  lwpa_socket_t sock;
  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_socket(LWPA_AF_INET, LWPA_DGRAM, &sock));
  TEST_ASSERT_NOT_EQUAL(sock, LWPA_SOCKET_INVALID);

  // Deinit and make sure add of a valid socket fails
  lwpa_poll_context_deinit(&context);
  TEST_ASSERT_NOT_EQUAL(kLwpaErrOk, lwpa_poll_add_socket(&context, sock, LWPA_POLL_IN, NULL));

  // Initialize the context and add invalid sockets or invalid events
  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_poll_context_init(&context));

  // Add invalid socket
  TEST_ASSERT_NOT_EQUAL(kLwpaErrOk, lwpa_poll_add_socket(&context, LWPA_SOCKET_INVALID, LWPA_POLL_IN, NULL));

  // Add socket with invalid events
  TEST_ASSERT_NOT_EQUAL(kLwpaErrOk, lwpa_poll_add_socket(&context, sock, 0, NULL));
  TEST_ASSERT_NOT_EQUAL(kLwpaErrOk, lwpa_poll_add_socket(&context, sock, LWPA_POLL_ERR, NULL));

  // Try to modify a socket that has not been added
  TEST_ASSERT_NOT_EQUAL(kLwpaErrOk, lwpa_poll_modify_socket(&context, sock, LWPA_POLL_IN, NULL));

  // Add the socket and try to modify it with invalid calls
  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_poll_add_socket(&context, sock, LWPA_POLL_IN, NULL));
  TEST_ASSERT_NOT_EQUAL(kLwpaErrOk, lwpa_poll_modify_socket(&context, sock, 0, NULL));              // Invalid events
  TEST_ASSERT_NOT_EQUAL(kLwpaErrOk, lwpa_poll_modify_socket(&context, sock, LWPA_POLL_ERR, NULL));  // Invalid events

  // Deinit and make sure we cannot modify
  lwpa_poll_context_deinit(&context);
  TEST_ASSERT_NOT_EQUAL(kLwpaErrOk, lwpa_poll_modify_socket(&context, sock, LWPA_POLL_OUT, NULL));

  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_close(sock));
}

TEST(lwpa_socket, poll_user_data_works)
{
  lwpa_socket_t sock_1, sock_2;
  void* user_data_1 = (void*)1;
  void* user_data_2 = (void*)2;

  LwpaPollContext context;
  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_poll_context_init(&context));

  // Create two UDP sockets and poll for writability, make sure our user data gets passed back to us
  // intact.

  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_socket(LWPA_AF_INET, LWPA_DGRAM, &sock_1));
  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_socket(LWPA_AF_INET, LWPA_DGRAM, &sock_2));

  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_poll_add_socket(&context, sock_1, LWPA_POLL_OUT, user_data_1));

  LwpaPollEvent event;
  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_poll_wait(&context, &event, 100));
  TEST_ASSERT_EQUAL(event.socket, sock_1);
  TEST_ASSERT_EQUAL(event.user_data, user_data_1);

  lwpa_poll_remove_socket(&context, sock_1);
  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_poll_add_socket(&context, sock_2, LWPA_POLL_OUT, user_data_2));

  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_poll_wait(&context, &event, 100));
  TEST_ASSERT_EQUAL(event.socket, sock_2);
  TEST_ASSERT_EQUAL(event.user_data, user_data_2);

  // Modify and make sure we get the updated user data
  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_poll_modify_socket(&context, sock_2, LWPA_POLL_OUT, user_data_1));
  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_poll_wait(&context, &event, 100));
  TEST_ASSERT_EQUAL(event.socket, sock_2);
  TEST_ASSERT_EQUAL(event.user_data, user_data_1);

  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_close(sock_1));
  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_close(sock_2));
  lwpa_poll_context_deinit(&context);
}

#define POLL_MODIFY_TEST_PORT_BASE 8000

// Test the lwpa_poll_modify_socket() functionality, using UDP sockets for simplicity
TEST(lwpa_socket, poll_modify_socket_works)
{
  lwpa_socket_t sock = LWPA_SOCKET_INVALID;

  LwpaPollContext context;
  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_poll_context_init(&context));

  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_socket(LWPA_AF_INET, LWPA_DGRAM, &sock));
  TEST_ASSERT_NOT_EQUAL(sock, LWPA_SOCKET_INVALID);

  // Bind the socket to the wildcard address and a specific port.
  LwpaSockaddr bind_addr;
  lwpa_ip_set_wildcard(kLwpaIpTypeV4, &bind_addr.ip);
  bind_addr.port = POLL_MODIFY_TEST_PORT_BASE;
  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_bind(sock, &bind_addr));

  // Add it for output polling first
  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_poll_add_socket(&context, sock, LWPA_POLL_OUT, NULL));

  // Socket should be ready right away
  LwpaPollEvent event;
  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_poll_wait(&context, &event, 100));
  TEST_ASSERT_EQUAL(event.socket, sock);
  TEST_ASSERT_EQUAL(event.events, LWPA_POLL_OUT);
  TEST_ASSERT_EQUAL(event.err, kLwpaErrOk);

  // Modify it to do input polling
  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_poll_modify_socket(&context, sock, LWPA_POLL_IN, NULL));

  // Should time out now
  TEST_ASSERT_EQUAL(kLwpaErrTimedOut, lwpa_poll_wait(&context, &event, 100));

  // Send data to socket
  LwpaSockaddr send_addr;
  LWPA_IP_SET_V4_ADDRESS(&send_addr.ip, 0x7f000001);
  send_addr.port = POLL_MODIFY_TEST_PORT_BASE;
  lwpa_sendto(sock, (const uint8_t*)"test message", sizeof("test message"), 0, &send_addr);

  // Should get the poll in event
  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_poll_wait(&context, &event, 100));
  TEST_ASSERT_EQUAL(event.socket, sock);
  TEST_ASSERT_EQUAL(event.events, LWPA_POLL_IN);
  TEST_ASSERT_EQUAL(event.err, kLwpaErrOk);

  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_close(sock));
  lwpa_poll_context_deinit(&context);
}

#define POLL_UDP_IN_TEST_PORT_BASE 9000

// Test the lwpa_poll_* API, polling for readability on UDP sockets.
TEST(lwpa_socket, poll_for_readability_on_udp_sockets_works)
{
  lwpa_socket_t send_sock = LWPA_SOCKET_INVALID;
  lwpa_socket_t rcvsock1 = LWPA_SOCKET_INVALID;
  lwpa_socket_t rcvsock2 = LWPA_SOCKET_INVALID;

  LwpaPollContext context;
  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_poll_context_init(&context));

  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_socket(LWPA_AF_INET, LWPA_DGRAM, &rcvsock1));
  TEST_ASSERT_NOT_EQUAL(rcvsock1, LWPA_SOCKET_INVALID);

  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_socket(LWPA_AF_INET, LWPA_DGRAM, &rcvsock2));
  TEST_ASSERT_NOT_EQUAL(rcvsock2, LWPA_SOCKET_INVALID);

  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_socket(LWPA_AF_INET, LWPA_DGRAM, &send_sock));
  TEST_ASSERT_NOT_EQUAL(send_sock, LWPA_SOCKET_INVALID);

  // Bind socket 1 to the wildcard address and a specific port.
  LwpaSockaddr bind_addr;
  lwpa_ip_set_wildcard(kLwpaIpTypeV4, &bind_addr.ip);
  bind_addr.port = POLL_UDP_IN_TEST_PORT_BASE;
  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_bind(rcvsock1, &bind_addr));

  // Bind socket 2 to the wildcard address and a different port.
  bind_addr.port = POLL_UDP_IN_TEST_PORT_BASE + 1;
  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_bind(rcvsock2, &bind_addr));

  // Get the poll context set up
  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_poll_add_socket(&context, rcvsock1, LWPA_POLL_IN, NULL));
  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_poll_add_socket(&context, rcvsock2, LWPA_POLL_IN, NULL));

  // Test poll with nothing sending - should time out.
  LwpaPollEvent event;
  TEST_ASSERT_EQUAL(kLwpaErrTimedOut, lwpa_poll_wait(&context, &event, 100));

  LwpaSockaddr send_addr;
  LWPA_IP_SET_V4_ADDRESS(&send_addr.ip, 0x7f000001);
  send_addr.port = POLL_UDP_IN_TEST_PORT_BASE;

#define POLL_UDP_IN_TEST_MESSAGE (const uint8_t*)"test message"
#define POLL_UDP_IN_TEST_MESSAGE_LENGTH sizeof("test message")

  lwpa_sendto(send_sock, POLL_UDP_IN_TEST_MESSAGE, POLL_UDP_IN_TEST_MESSAGE_LENGTH, 0, &send_addr);
  send_addr.port = POLL_UDP_IN_TEST_PORT_BASE + 1;
  lwpa_sendto(send_sock, POLL_UDP_IN_TEST_MESSAGE, POLL_UDP_IN_TEST_MESSAGE_LENGTH, 0, &send_addr);

  // Poll once, make sure we get one of the sockets.
  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_poll_wait(&context, &event, 1000));
  TEST_ASSERT(event.socket == rcvsock1 || event.socket == rcvsock2);
  TEST_ASSERT_EQUAL(event.events, LWPA_POLL_IN);
  TEST_ASSERT_EQUAL(event.err, kLwpaErrOk);

  // Receive data on the socket.
  uint8_t recv_buf[POLL_UDP_IN_TEST_MESSAGE_LENGTH];
  LwpaSockaddr from_addr;
  TEST_ASSERT_EQUAL(POLL_UDP_IN_TEST_MESSAGE_LENGTH,
                    (size_t)lwpa_recvfrom(event.socket, recv_buf, POLL_UDP_IN_TEST_MESSAGE_LENGTH, 0, &from_addr));
  TEST_ASSERT(lwpa_ip_equal(&send_addr.ip, &from_addr.ip));
  TEST_ASSERT_NOT_EQUAL(from_addr.port, POLL_UDP_IN_TEST_PORT_BASE);
  TEST_ASSERT_NOT_EQUAL(from_addr.port, POLL_UDP_IN_TEST_PORT_BASE + 1);
  TEST_ASSERT_EQUAL_MEMORY(recv_buf, POLL_UDP_IN_TEST_MESSAGE, POLL_UDP_IN_TEST_MESSAGE_LENGTH);

  lwpa_socket_t first_socket = event.socket;

  // Poll a second time for the other socket.
  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_poll_wait(&context, &event, 1000));
  if (first_socket == rcvsock1)
    TEST_ASSERT_EQUAL(event.socket, rcvsock2);
  else
    TEST_ASSERT_EQUAL(event.socket, rcvsock1);
  TEST_ASSERT_EQUAL(event.events, LWPA_POLL_IN);
  TEST_ASSERT_EQUAL(event.err, kLwpaErrOk);

  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_close(rcvsock1));
  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_close(rcvsock2));
  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_close(send_sock));
  lwpa_poll_context_deinit(&context);
}

// Test the lwpa_poll_* API, polling for writeablility on UDP sockets.
TEST(lwpa_socket, poll_for_writability_on_udp_sockets_works)
{
  lwpa_socket_t sock_1, sock_2;

  LwpaPollContext context;
  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_poll_context_init(&context));

  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_socket(LWPA_AF_INET, LWPA_DGRAM, &sock_1));
  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_socket(LWPA_AF_INET, LWPA_DGRAM, &sock_2));

  // The sockets should poll as ready for output right away. Not sure what else there is to test
  // here.
  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_poll_add_socket(&context, sock_1, LWPA_POLL_OUT, NULL));

  LwpaPollEvent event;
  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_poll_wait(&context, &event, 100));
  TEST_ASSERT_EQUAL(event.socket, sock_1);
  TEST_ASSERT_EQUAL(event.events, LWPA_POLL_OUT);
  TEST_ASSERT_EQUAL(event.err, kLwpaErrOk);

  lwpa_poll_remove_socket(&context, sock_1);
  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_poll_add_socket(&context, sock_2, LWPA_POLL_OUT, NULL));

  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_poll_wait(&context, &event, 100));
  TEST_ASSERT_EQUAL(event.socket, sock_2);
  TEST_ASSERT_EQUAL(event.events, LWPA_POLL_OUT);
  TEST_ASSERT_EQUAL(event.err, kLwpaErrOk);

  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_close(sock_1));
  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_close(sock_2));
  lwpa_poll_context_deinit(&context);
}

TEST(lwpa_socket, getaddrinfo_works_as_expected)
{
  LwpaAddrinfo ai_hints;
  LwpaAddrinfo ai;

  memset(&ai_hints, 0, sizeof ai_hints);

  // We can't currently assume internet access for our tests.
#if 0
  ai_hints.ai_family = LWPA_AF_INET;
  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_getaddrinfo(test_hostname, test_service, &ai_hints, &ai));
  TEST_ASSERT(LWPA_IP_IS_V4(&ai.ai_addr.ip));
  lwpa_freeaddrinfo(&ai);
#endif

  ai_hints.ai_flags = LWPA_AI_NUMERICHOST;
  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_getaddrinfo(test_gai_ip_str, test_gai_port_str, &ai_hints, &ai));
  TEST_ASSERT(LWPA_IP_IS_V4(&ai.ai_addr.ip));
  TEST_ASSERT_EQUAL_UINT32(LWPA_IP_V4_ADDRESS(&ai.ai_addr.ip), test_gai_ip);
  TEST_ASSERT_EQUAL_UINT16(ai.ai_addr.port, test_gai_port);
  lwpa_freeaddrinfo(&ai);
}

TEST_GROUP_RUNNER(lwpa_socket)
{
  RUN_TEST_CASE(lwpa_socket, bind_works_as_expected);
  RUN_TEST_CASE(lwpa_socket, sockopts);
  RUN_TEST_CASE(lwpa_socket, blocking_state_is_consistent);
  RUN_TEST_CASE(lwpa_socket, poll_invalid_calls_fail);
  RUN_TEST_CASE(lwpa_socket, poll_user_data_works);
  RUN_TEST_CASE(lwpa_socket, poll_modify_socket_works);
  RUN_TEST_CASE(lwpa_socket, poll_for_readability_on_udp_sockets_works);
  RUN_TEST_CASE(lwpa_socket, poll_for_writability_on_udp_sockets_works);
  RUN_TEST_CASE(lwpa_socket, getaddrinfo_works_as_expected);
}
