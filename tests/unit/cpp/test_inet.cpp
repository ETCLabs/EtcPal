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

#include "etcpal/inet.h"
#include "unity_fixture.h"

#include <array>

TEST_GROUP(etcpal_cpp_inet);

static EtcPalIpAddr ipv4_1;
static EtcPalIpAddr ipv4_2;
static EtcPalIpAddr ipv6_1;
static EtcPalIpAddr ipv6_2;

static EtcPalSockAddr sock_v4_port_1;
static EtcPalSockAddr sock_v4_port_2;
static EtcPalSockAddr sock_v6_port_1;

TEST_SETUP(etcpal_cpp_inet)
{
  ETCPAL_IP_SET_V4_ADDRESS(&ipv4_1, 0x00000001);
  ETCPAL_IP_SET_V4_ADDRESS(&ipv4_2, 0x00000002);
  std::array<uint8_t, 16> v6_data{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1};
  ETCPAL_IP_SET_V6_ADDRESS(&ipv6_1, v6_data.data());
  v6_data[15] = 2;
  ETCPAL_IP_SET_V6_ADDRESS(&ipv6_2, v6_data.data());

  ETCPAL_IP_SET_V4_ADDRESS(&sock_v4_port_1.ip, 0x00000001);
  sock_v4_port_1.port = 8888;
  ETCPAL_IP_SET_V4_ADDRESS(&sock_v4_port_2.ip, 0x00000001);
  sock_v4_port_2.port = 8889;
  ETCPAL_IP_SET_V6_ADDRESS(&sock_v6_port_1.ip, v6_data.data());
  sock_v6_port_1.port = 8888;
}

TEST_TEAR_DOWN(etcpal_cpp_inet)
{
}

TEST(etcpal_cpp_inet, c_ipaddr_equality_operators_work)
{
  // Items should equal themselves
  TEST_ASSERT_TRUE(ipv4_1 == ipv4_1);
  TEST_ASSERT_TRUE(ipv4_2 == ipv4_2);
  TEST_ASSERT_TRUE(ipv6_1 == ipv6_1);
  TEST_ASSERT_TRUE(ipv6_2 == ipv6_2);
  TEST_ASSERT_FALSE(ipv4_1 != ipv4_1);
  TEST_ASSERT_FALSE(ipv6_1 != ipv6_1);
  TEST_ASSERT_FALSE(ipv4_2 != ipv4_2);
  TEST_ASSERT_FALSE(ipv6_2 != ipv6_2);

  // IPv4 addresses should not equal IPv6 addresses
  TEST_ASSERT_FALSE(ipv4_1 == ipv6_1);
  TEST_ASSERT_FALSE(ipv6_1 == ipv4_1);
  TEST_ASSERT_TRUE(ipv4_1 != ipv6_1);
  TEST_ASSERT_TRUE(ipv6_1 != ipv4_1);

  // IPv4 and IPv6 addresses with different address values should not equal each other
  TEST_ASSERT_FALSE(ipv4_1 == ipv4_2);
  TEST_ASSERT_FALSE(ipv6_1 == ipv6_2);
  TEST_ASSERT_TRUE(ipv4_1 != ipv4_2);
  TEST_ASSERT_TRUE(ipv6_1 != ipv6_2);
}

TEST(etcpal_cpp_inet, c_ipaddr_comparison_operators_work)
{
  // Items should not be < or > themselves and should be <= and >=
  TEST_ASSERT_FALSE(ipv4_1 < ipv4_1);
  TEST_ASSERT_FALSE(ipv4_1 > ipv4_1);
  TEST_ASSERT_TRUE(ipv4_1 <= ipv4_1);
  TEST_ASSERT_TRUE(ipv4_1 >= ipv4_1);

  // Items of the same IP protocol and different addresses should compare correctly
  TEST_ASSERT_TRUE(ipv4_1 < ipv4_2);
  TEST_ASSERT_FALSE(ipv4_1 > ipv4_2);
  TEST_ASSERT_TRUE(ipv4_1 <= ipv4_2);
  TEST_ASSERT_FALSE(ipv4_1 >= ipv4_2);
  TEST_ASSERT_TRUE(ipv6_1 < ipv6_2);
  TEST_ASSERT_FALSE(ipv6_1 > ipv6_2);
  TEST_ASSERT_TRUE(ipv6_1 <= ipv6_2);
  TEST_ASSERT_FALSE(ipv6_1 >= ipv6_2);

  // All IPv4 addresses should compare less than all IPv6 addresses
  TEST_ASSERT_TRUE(ipv4_1 < ipv6_1);
  TEST_ASSERT_FALSE(ipv4_1 > ipv6_1);
  TEST_ASSERT_TRUE(ipv4_1 <= ipv6_1);
  TEST_ASSERT_FALSE(ipv4_1 >= ipv6_1);
  TEST_ASSERT_TRUE(ipv4_2 < ipv6_1);
  TEST_ASSERT_FALSE(ipv4_2 > ipv6_1);
  TEST_ASSERT_TRUE(ipv4_2 <= ipv6_1);
  TEST_ASSERT_FALSE(ipv4_2 >= ipv6_1);
  TEST_ASSERT_TRUE(ipv4_1 < ipv6_2);
  TEST_ASSERT_FALSE(ipv4_1 > ipv6_2);
  TEST_ASSERT_TRUE(ipv4_1 <= ipv6_2);
  TEST_ASSERT_FALSE(ipv4_1 >= ipv6_2);
  TEST_ASSERT_TRUE(ipv4_2 < ipv6_2);
  TEST_ASSERT_FALSE(ipv4_2 > ipv6_2);
  TEST_ASSERT_TRUE(ipv4_2 <= ipv6_2);
  TEST_ASSERT_FALSE(ipv4_2 >= ipv6_2);
}

TEST(etcpal_cpp_inet, c_sockaddr_equality_operators_work)
{
  // Items should equal themselves
  TEST_ASSERT_TRUE(sock_v4_port_1 == sock_v4_port_1);
  TEST_ASSERT_TRUE(sock_v4_port_2 == sock_v4_port_2);
  TEST_ASSERT_TRUE(sock_v6_port_1 == sock_v6_port_1);
  TEST_ASSERT_FALSE(sock_v4_port_1 != sock_v4_port_1);
  TEST_ASSERT_FALSE(sock_v6_port_1 != sock_v6_port_1);
  TEST_ASSERT_FALSE(sock_v4_port_2 != sock_v4_port_2);

  // Sockaddrs with different ports should not equal each other
  TEST_ASSERT_FALSE(sock_v4_port_1 == sock_v4_port_2);
  TEST_ASSERT_TRUE(sock_v4_port_1 != sock_v4_port_2);

  // Sockaddrs with different address values should not equal each other
  TEST_ASSERT_FALSE(sock_v4_port_1 == sock_v6_port_1);
  TEST_ASSERT_FALSE(sock_v6_port_1 == sock_v4_port_1);
  TEST_ASSERT_TRUE(sock_v4_port_1 != sock_v6_port_1);
  TEST_ASSERT_TRUE(sock_v6_port_1 != sock_v4_port_1);
}

TEST(etcpal_cpp_inet, c_sockaddr_comparison_operators_work)
{
  // Items should not be < or > themselves and should be <= and >=
  TEST_ASSERT_FALSE(sock_v4_port_1 < sock_v4_port_1);
  TEST_ASSERT_FALSE(sock_v4_port_1 > sock_v4_port_1);
  TEST_ASSERT_TRUE(sock_v4_port_1 <= sock_v4_port_1);
  TEST_ASSERT_TRUE(sock_v4_port_1 >= sock_v4_port_1);

  // Items with the same address and different ports should compare correctly
  TEST_ASSERT_TRUE(sock_v4_port_1 < sock_v4_port_2);
  TEST_ASSERT_FALSE(sock_v4_port_1 > sock_v4_port_2);
  TEST_ASSERT_TRUE(sock_v4_port_1 <= sock_v4_port_2);
  TEST_ASSERT_FALSE(sock_v4_port_1 >= sock_v4_port_2);

  // Items with the same port and different addresses should compare correctly
  TEST_ASSERT_TRUE(sock_v4_port_1 < sock_v6_port_1);
  TEST_ASSERT_FALSE(sock_v4_port_1 > sock_v6_port_1);
  TEST_ASSERT_TRUE(sock_v4_port_1 <= sock_v6_port_1);
  TEST_ASSERT_FALSE(sock_v4_port_1 >= sock_v6_port_1);
}

TEST_GROUP_RUNNER(etcpal_cpp_inet)
{
  RUN_TEST_CASE(etcpal_cpp_inet, c_ipaddr_equality_operators_work);
  RUN_TEST_CASE(etcpal_cpp_inet, c_ipaddr_comparison_operators_work);
  RUN_TEST_CASE(etcpal_cpp_inet, c_sockaddr_equality_operators_work);
  RUN_TEST_CASE(etcpal_cpp_inet, c_sockaddr_comparison_operators_work);
}
