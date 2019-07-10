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

// This file tests the lwpa_poll_* functions in lwpa_socket.

#include <array>
#include "lwpa/socket.h"
#include "lwpa/netint.h"
#include "gtest/gtest.h"

#if LWPA_SOCKET_MAX_POLL_SIZE <= 0 || LWPA_SOCKET_MAX_POLL_SIZE > 1024
// Limit the bulk socket test to a reasonable number
#define BULK_POLL_TEST_NUM_SOCKETS 512
#else
#define BULK_POLL_TEST_NUM_SOCKETS LWPA_SOCKET_MAX_POLL_SIZE
#endif

class SocketPollTest : public ::testing::Test
{
protected:
  SocketPollTest()
  {
    lwpa_init(LWPA_FEATURE_SOCKETS | LWPA_FEATURE_NETINTS);
    lwpa_netint_get_default_interface(kLwpaIpTypeV4, &default_netint_);
  }
  ~SocketPollTest() { lwpa_deinit(LWPA_FEATURE_SOCKETS | LWPA_FEATURE_NETINTS); }

  LwpaNetintInfo default_netint_;

  static const char* SEND_MSG;
  static const size_t SEND_MSG_LEN;

  void SetUp() override { ASSERT_EQ(kLwpaErrOk, lwpa_poll_context_init(&context_)); }
  void TearDown() override { lwpa_poll_context_deinit(&context_); }

  LwpaPollContext context_{};
};

const char* SocketPollTest::SEND_MSG = "testtesttest";
const size_t SocketPollTest::SEND_MSG_LEN = 12;

// Test to make sure various invalid calls to lwpa_poll_* functions fail properly.
TEST_F(SocketPollTest, invalid_calls)
{
  // Wait should fail with a meaningful error code when no sockets have been added
  LwpaPollEvent event;
  ASSERT_EQ(kLwpaErrNoSockets, lwpa_poll_wait(&context_, &event, 100));

  lwpa_socket_t sock;
  ASSERT_EQ(kLwpaErrOk, lwpa_socket(LWPA_AF_INET, LWPA_DGRAM, &sock));
  ASSERT_NE(sock, LWPA_SOCKET_INVALID);

  // Deinit and make sure add of a valid socket fails
  lwpa_poll_context_deinit(&context_);
  EXPECT_NE(kLwpaErrOk, lwpa_poll_add_socket(&context_, sock, LWPA_POLL_IN, nullptr));

  // Initialize the context and add invalid sockets or invalid events
  ASSERT_EQ(kLwpaErrOk, lwpa_poll_context_init(&context_));

  // Add invalid socket
  EXPECT_NE(kLwpaErrOk, lwpa_poll_add_socket(&context_, LWPA_SOCKET_INVALID, LWPA_POLL_IN, nullptr));

  // Add socket with invalid events
  EXPECT_NE(kLwpaErrOk, lwpa_poll_add_socket(&context_, sock, 0, nullptr));
  EXPECT_NE(kLwpaErrOk, lwpa_poll_add_socket(&context_, sock, LWPA_POLL_ERR, nullptr));

  // Try to modify a socket that has not been added
  EXPECT_NE(kLwpaErrOk, lwpa_poll_modify_socket(&context_, sock, LWPA_POLL_IN, nullptr));

  // Add the socket and try to modify it with invalid calls
  ASSERT_EQ(kLwpaErrOk, lwpa_poll_add_socket(&context_, sock, LWPA_POLL_IN, nullptr));
  EXPECT_NE(kLwpaErrOk, lwpa_poll_modify_socket(&context_, sock, 0, nullptr));              // Invalid events
  EXPECT_NE(kLwpaErrOk, lwpa_poll_modify_socket(&context_, sock, LWPA_POLL_ERR, nullptr));  // Invalid events

  // Deinit and make sure we cannot modify
  lwpa_poll_context_deinit(&context_);
  EXPECT_NE(kLwpaErrOk, lwpa_poll_modify_socket(&context_, sock, LWPA_POLL_OUT, nullptr));
}

// Test the user_data passing functionality.
TEST_F(SocketPollTest, user_data)
{
  lwpa_socket_t sock_1, sock_2;
  void* user_data_1 = reinterpret_cast<void*>(1);
  void* user_data_2 = reinterpret_cast<void*>(2);

  // Create two UDP sockets and poll for writability, make sure our user data gets passed back to us
  // intact.

  ASSERT_EQ(kLwpaErrOk, lwpa_socket(LWPA_AF_INET, LWPA_DGRAM, &sock_1));
  ASSERT_EQ(kLwpaErrOk, lwpa_socket(LWPA_AF_INET, LWPA_DGRAM, &sock_2));

  ASSERT_EQ(kLwpaErrOk, lwpa_poll_add_socket(&context_, sock_1, LWPA_POLL_OUT, user_data_1));

  LwpaPollEvent event;
  ASSERT_EQ(kLwpaErrOk, lwpa_poll_wait(&context_, &event, 100));
  ASSERT_EQ(event.socket, sock_1);
  ASSERT_EQ(event.user_data, user_data_1);

  lwpa_poll_remove_socket(&context_, sock_1);
  ASSERT_EQ(kLwpaErrOk, lwpa_poll_add_socket(&context_, sock_2, LWPA_POLL_OUT, user_data_2));

  ASSERT_EQ(kLwpaErrOk, lwpa_poll_wait(&context_, &event, 100));
  ASSERT_EQ(event.socket, sock_2);
  ASSERT_EQ(event.user_data, user_data_2);

  // Modify and make sure we get the updated user data
  ASSERT_EQ(kLwpaErrOk, lwpa_poll_modify_socket(&context_, sock_2, LWPA_POLL_OUT, user_data_1));
  ASSERT_EQ(kLwpaErrOk, lwpa_poll_wait(&context_, &event, 100));
  ASSERT_EQ(event.socket, sock_2);
  ASSERT_EQ(event.user_data, user_data_1);

  ASSERT_EQ(kLwpaErrOk, lwpa_close(sock_1));
  ASSERT_EQ(kLwpaErrOk, lwpa_close(sock_2));
}

#define POLL_MODIFY_TEST_PORT_BASE 8000

// Test the lwpa_poll_modify_socket() functionality, using UDP sockets for simplicity
TEST_F(SocketPollTest, modify)
{
  lwpa_socket_t sock = LWPA_SOCKET_INVALID;

  ASSERT_EQ(kLwpaErrOk, lwpa_socket(LWPA_AF_INET, LWPA_DGRAM, &sock));
  ASSERT_NE(sock, LWPA_SOCKET_INVALID);

  // Bind the socket to the wildcard address and a specific port.
  LwpaSockaddr bind_addr;
  lwpa_ip_set_wildcard(kLwpaIpTypeV4, &bind_addr.ip);
  bind_addr.port = POLL_MODIFY_TEST_PORT_BASE;
  ASSERT_EQ(kLwpaErrOk, lwpa_bind(sock, &bind_addr));

  // Add it for output polling first
  ASSERT_EQ(kLwpaErrOk, lwpa_poll_add_socket(&context_, sock, LWPA_POLL_OUT, nullptr));

  // Socket should be ready right away
  LwpaPollEvent event;
  EXPECT_EQ(kLwpaErrOk, lwpa_poll_wait(&context_, &event, 100));
  EXPECT_EQ(event.socket, sock);
  EXPECT_EQ(event.events, LWPA_POLL_OUT);
  EXPECT_EQ(event.err, kLwpaErrOk);

  // Modify it to do input polling
  ASSERT_EQ(kLwpaErrOk, lwpa_poll_modify_socket(&context_, sock, LWPA_POLL_IN, nullptr));

  // Should time out now
  EXPECT_EQ(kLwpaErrTimedOut, lwpa_poll_wait(&context_, &event, 100));

  // Send data to socket
  LwpaSockaddr send_addr;
  send_addr.ip = default_netint_.addr;
  send_addr.port = POLL_MODIFY_TEST_PORT_BASE;
  lwpa_sendto(sock, SEND_MSG, SEND_MSG_LEN, 0, &send_addr);

  // Should get the poll in event
  EXPECT_EQ(kLwpaErrOk, lwpa_poll_wait(&context_, &event, 100));
  EXPECT_EQ(event.socket, sock);
  EXPECT_EQ(event.events, LWPA_POLL_IN);
  EXPECT_EQ(event.err, kLwpaErrOk);

  ASSERT_EQ(kLwpaErrOk, lwpa_close(sock));
}

// Test to make sure lwpa_poll_* functions work properly with a large number of sockets.
// (Tests the maximum number defined by LWPA_SOCKET_MAX_POLL_SIZE if that number is well-defined and
// reasonable).
TEST_F(SocketPollTest, bulk_poll)
{
  ASSERT_EQ(kLwpaErrOk, lwpa_poll_context_init(&context_));

  std::array<lwpa_socket_t, BULK_POLL_TEST_NUM_SOCKETS + 1> socket_arr;
  std::array<uint16_t, BULK_POLL_TEST_NUM_SOCKETS> bind_ports;
  for (size_t i = 0; i < BULK_POLL_TEST_NUM_SOCKETS + 1; ++i)
  {
    ASSERT_EQ(kLwpaErrOk, lwpa_socket(LWPA_AF_INET, LWPA_DGRAM, &socket_arr[i])) << "Failed on iteration " << i;
    ASSERT_NE(socket_arr[i], LWPA_SOCKET_INVALID) << "Failed on iteration " << i;

    if (i < BULK_POLL_TEST_NUM_SOCKETS)
    {
      LwpaSockaddr bind_addr;
      lwpa_ip_set_wildcard(kLwpaIpTypeV4, &bind_addr.ip);
      bind_addr.port = 0;
      ASSERT_EQ(kLwpaErrOk, lwpa_bind(socket_arr[i], &bind_addr)) << "Failed on iteration " << i;
      ASSERT_EQ(kLwpaErrOk, lwpa_getsockname(socket_arr[i], &bind_addr)) << "Failed on iteration " << i;
      bind_ports[i] = bind_addr.port;

      ASSERT_EQ(kLwpaErrOk, lwpa_poll_add_socket(&context_, socket_arr[i], LWPA_POLL_IN, nullptr))
          << "Failed on iteration " << i;
    }
  }

  // The first socket over BULK_POLL_TEST_NUM_SOCKETS should fail
  // ASSERT_NE(kLwpaErrOk, lwpa_poll_add_socket(&context_, socket_arr[BULK_POLL_TEST_NUM_SOCKETS], LWPA_POLL_IN,
  // nullptr));

  lwpa_socket_t send_sock;
  ASSERT_EQ(kLwpaErrOk, lwpa_socket(LWPA_AF_INET, LWPA_DGRAM, &send_sock));

  LwpaSockaddr send_addr;
  LWPA_IP_SET_V4_ADDRESS(&send_addr.ip, 0x7f000001);
  for (size_t i = 0; i < BULK_POLL_TEST_NUM_SOCKETS; ++i)
  {
    // Send to each socket in turn and make sure poll works for it
    send_addr.port = bind_ports[i];
    lwpa_sendto(send_sock, SEND_MSG, SEND_MSG_LEN, 0, &send_addr);

    LwpaPollEvent event;
    ASSERT_EQ(kLwpaErrOk, lwpa_poll_wait(&context_, &event, 100)) << "Failed on iteration " << i;
    ASSERT_EQ(event.socket, socket_arr[i]) << "Failed on iteration " << i;
    ASSERT_EQ(event.events, LWPA_POLL_IN) << "Failed on iteration " << i;
    ASSERT_EQ(event.err, kLwpaErrOk) << "Failed on iteration " << i;

    std::array<uint8_t, SEND_MSG_LEN> recv_buf;
    ASSERT_GT(lwpa_recvfrom(socket_arr[i], recv_buf.data(), SEND_MSG_LEN, 0, nullptr), 0);
  }

  for (size_t i = 0; i < BULK_POLL_TEST_NUM_SOCKETS + 1; ++i)
  {
    EXPECT_EQ(kLwpaErrOk, lwpa_close(socket_arr[i])) << "Failed on iteration " << i;
  }
}

#define POLL_UDP_IN_TEST_PORT_BASE 9000

// Test the lwpa_poll_* API, polling for readability on UDP sockets.
TEST_F(SocketPollTest, udp_in)
{
  lwpa_socket_t send_sock = LWPA_SOCKET_INVALID;
  lwpa_socket_t rcvsock1 = LWPA_SOCKET_INVALID;
  lwpa_socket_t rcvsock2 = LWPA_SOCKET_INVALID;

  ASSERT_EQ(kLwpaErrOk, lwpa_socket(LWPA_AF_INET, LWPA_DGRAM, &rcvsock1));
  ASSERT_NE(rcvsock1, LWPA_SOCKET_INVALID);

  ASSERT_EQ(kLwpaErrOk, lwpa_socket(LWPA_AF_INET, LWPA_DGRAM, &rcvsock2));
  ASSERT_NE(rcvsock2, LWPA_SOCKET_INVALID);

  ASSERT_EQ(kLwpaErrOk, lwpa_socket(LWPA_AF_INET, LWPA_DGRAM, &send_sock));
  ASSERT_NE(send_sock, LWPA_SOCKET_INVALID);

  // Bind socket 1 to the wildcard address and a specific port.
  LwpaSockaddr bind_addr;
  lwpa_ip_set_wildcard(kLwpaIpTypeV4, &bind_addr.ip);
  bind_addr.port = POLL_UDP_IN_TEST_PORT_BASE;
  ASSERT_EQ(kLwpaErrOk, lwpa_bind(rcvsock1, &bind_addr));

  // Bind socket 2 to the wildcard address and a different port.
  bind_addr.port = POLL_UDP_IN_TEST_PORT_BASE + 1;
  ASSERT_EQ(kLwpaErrOk, lwpa_bind(rcvsock2, &bind_addr));

  // Get the poll context set up
  ASSERT_EQ(kLwpaErrOk, lwpa_poll_add_socket(&context_, rcvsock1, LWPA_POLL_IN, nullptr));
  ASSERT_EQ(kLwpaErrOk, lwpa_poll_add_socket(&context_, rcvsock2, LWPA_POLL_IN, nullptr));

  // Test poll with nothing sending - should time out.
  LwpaPollEvent event;
  ASSERT_EQ(kLwpaErrTimedOut, lwpa_poll_wait(&context_, &event, 100));

  LwpaSockaddr send_addr;
  send_addr.ip = default_netint_.addr;
  send_addr.port = POLL_UDP_IN_TEST_PORT_BASE;

  lwpa_sendto(send_sock, SEND_MSG, SEND_MSG_LEN, 0, &send_addr);
  send_addr.port = POLL_UDP_IN_TEST_PORT_BASE + 1;
  lwpa_sendto(send_sock, SEND_MSG, SEND_MSG_LEN, 0, &send_addr);

  // Poll once, make sure we get one of the sockets.
  ASSERT_EQ(kLwpaErrOk, lwpa_poll_wait(&context_, &event, 1000));
  ASSERT_TRUE(event.socket == rcvsock1 || event.socket == rcvsock2);
  ASSERT_EQ(event.events, LWPA_POLL_IN);
  ASSERT_EQ(event.err, kLwpaErrOk);

  // Receive data on the socket.
  std::array<uint8_t, SEND_MSG_LEN + 1> recv_buf;
  LwpaSockaddr from_addr;
  ASSERT_EQ(SEND_MSG_LEN, (size_t)lwpa_recvfrom(event.socket, recv_buf.data(), SEND_MSG_LEN, 0, &from_addr));
  ASSERT_TRUE(lwpa_ip_equal(&send_addr.ip, &from_addr.ip));
  ASSERT_NE(from_addr.port, POLL_UDP_IN_TEST_PORT_BASE);
  ASSERT_NE(from_addr.port, POLL_UDP_IN_TEST_PORT_BASE + 1);
  recv_buf[SEND_MSG_LEN] = '\0';
  ASSERT_EQ(0, strcmp((char*)recv_buf.data(), SEND_MSG));

  lwpa_socket_t first_socket = event.socket;

  // Poll a second time for the other socket.
  ASSERT_EQ(kLwpaErrOk, lwpa_poll_wait(&context_, &event, 1000));
  if (first_socket == rcvsock1)
    ASSERT_EQ(event.socket, rcvsock2);
  else
    ASSERT_EQ(event.socket, rcvsock1);
  ASSERT_EQ(event.events, LWPA_POLL_IN);
  ASSERT_EQ(event.err, kLwpaErrOk);

  ASSERT_EQ(kLwpaErrOk, lwpa_close(rcvsock1));
  ASSERT_EQ(kLwpaErrOk, lwpa_close(rcvsock2));
  ASSERT_EQ(kLwpaErrOk, lwpa_close(send_sock));
}

// Test the lwpa_poll_* API, polling for writeablility on UDP sockets.
TEST_F(SocketPollTest, udp_out)
{
  lwpa_socket_t sock_1, sock_2;

  ASSERT_EQ(kLwpaErrOk, lwpa_socket(LWPA_AF_INET, LWPA_DGRAM, &sock_1));
  ASSERT_EQ(kLwpaErrOk, lwpa_socket(LWPA_AF_INET, LWPA_DGRAM, &sock_2));

  // The sockets should poll as ready for output right away. Not sure what else there is to test
  // here.
  ASSERT_EQ(kLwpaErrOk, lwpa_poll_add_socket(&context_, sock_1, LWPA_POLL_OUT, nullptr));

  LwpaPollEvent event;
  ASSERT_EQ(kLwpaErrOk, lwpa_poll_wait(&context_, &event, 100));
  ASSERT_EQ(event.socket, sock_1);
  ASSERT_EQ(event.events, LWPA_POLL_OUT);
  ASSERT_EQ(event.err, kLwpaErrOk);

  lwpa_poll_remove_socket(&context_, sock_1);
  ASSERT_EQ(kLwpaErrOk, lwpa_poll_add_socket(&context_, sock_2, LWPA_POLL_OUT, nullptr));

  ASSERT_EQ(kLwpaErrOk, lwpa_poll_wait(&context_, &event, 100));
  ASSERT_EQ(event.socket, sock_2);
  ASSERT_EQ(event.events, LWPA_POLL_OUT);
  ASSERT_EQ(event.err, kLwpaErrOk);

  ASSERT_EQ(kLwpaErrOk, lwpa_close(sock_1));
  ASSERT_EQ(kLwpaErrOk, lwpa_close(sock_2));
}
