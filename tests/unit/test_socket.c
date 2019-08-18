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
#include <stddef.h>

// For getaddrinfo
static const char* test_hostname = "www.google.com";
static const char* test_service = "http";

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

TEST(lwpa_socket, getaddrinfo_works_as_expected)
{
  LwpaAddrinfo ai_hints;
  LwpaAddrinfo ai;

  memset(&ai_hints, 0, sizeof ai_hints);
  ai_hints.ai_family = LWPA_AF_INET;
  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_getaddrinfo(test_hostname, test_service, &ai_hints, &ai));
  TEST_ASSERT(LWPA_IP_IS_V4(&ai.ai_addr.ip));
  lwpa_freeaddrinfo(&ai);

  ai_hints.ai_flags = LWPA_AI_NUMERICHOST;
  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_getaddrinfo(test_gai_ip_str, test_gai_port_str, &ai_hints, &ai));
  TEST_ASSERT(LWPA_IP_IS_V4(&ai.ai_addr.ip));
  TEST_ASSERT_EQUAL_UINT32(LWPA_IP_V4_ADDRESS(&ai.ai_addr.ip), test_gai_ip);
  TEST_ASSERT_EQUAL_UINT16(ai.ai_addr.port, test_gai_port);
}

TEST_GROUP_RUNNER(lwpa_socket)
{
  RUN_TEST_CASE(lwpa_socket, bind_works_as_expected);
  RUN_TEST_CASE(lwpa_socket, sockopts);
  RUN_TEST_CASE(lwpa_socket, getaddrinfo_works_as_expected);
}

void run_all_tests(void)
{
  RUN_TEST_GROUP(lwpa_socket);
}