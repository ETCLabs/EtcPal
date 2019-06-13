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
#include "lwpa/inet.h"
#include "gtest/gtest.h"
#include <array>
#include <cstring>
#include <climits>

class InetTest : public ::testing::Test
{
};

// Test the LwpaIpAddr struct and macros
TEST_F(InetTest, addr_macros)
{
  LwpaIpAddr test_addr;

  LWPA_IP_SET_V4_ADDRESS(&test_addr, 0x01020304);
  EXPECT_TRUE(LWPA_IP_IS_V4(&test_addr));
  EXPECT_EQ(LWPA_IP_V4_ADDRESS(&test_addr), 0x01020304u);

  std::array<uint8_t, LWPA_IPV6_BYTES> v6_data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
  LWPA_IP_SET_V6_ADDRESS(&test_addr, v6_data.data());
  EXPECT_TRUE(LWPA_IP_IS_V6(&test_addr));
  EXPECT_EQ(0, memcmp(LWPA_IP_V6_ADDRESS(&test_addr), v6_data.data(), LWPA_IPV6_BYTES));

  LWPA_IP_SET_INVALID(&test_addr);
  EXPECT_TRUE(LWPA_IP_IS_INVALID(&test_addr));
}

// Test the lwpa_ip_is_loopback() function
TEST_F(InetTest, loopback)
{
  LwpaIpAddr test_addr;

  // An invalid IP is not loopback
  LWPA_IP_SET_INVALID(&test_addr);
  EXPECT_FALSE(lwpa_ip_is_loopback(&test_addr));

  // Test the edges of the IPv4 loopback range
  LWPA_IP_SET_V4_ADDRESS(&test_addr, 0x7effffff);
  EXPECT_FALSE(lwpa_ip_is_loopback(&test_addr));

  LWPA_IP_SET_V4_ADDRESS(&test_addr, 0x7f000000);
  EXPECT_TRUE(lwpa_ip_is_loopback(&test_addr));

  LWPA_IP_SET_V4_ADDRESS(&test_addr, 0x7fffffff);
  EXPECT_TRUE(lwpa_ip_is_loopback(&test_addr));

  LWPA_IP_SET_V4_ADDRESS(&test_addr, 0x80000000);
  EXPECT_FALSE(lwpa_ip_is_loopback(&test_addr));

  // Test the IPv6 loopback address
  std::array<uint8_t, LWPA_IPV6_BYTES> v6_data = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1};
  LWPA_IP_SET_V6_ADDRESS(&test_addr, v6_data.data());
  EXPECT_TRUE(lwpa_ip_is_loopback(&test_addr));

  v6_data[15] = 0;
  LWPA_IP_SET_V6_ADDRESS(&test_addr, v6_data.data());
  EXPECT_FALSE(lwpa_ip_is_loopback(&test_addr));
}

// Test the lwpa_ip_is_multicast() function
TEST_F(InetTest, multicast)
{
  LwpaIpAddr test_addr;

  // An invalid IP is not multicast
  LWPA_IP_SET_INVALID(&test_addr);
  EXPECT_FALSE(lwpa_ip_is_multicast(&test_addr));

  // Test the edges of the IPv4 multicast range
  LWPA_IP_SET_V4_ADDRESS(&test_addr, 0xdfffffff);
  EXPECT_FALSE(lwpa_ip_is_multicast(&test_addr));

  LWPA_IP_SET_V4_ADDRESS(&test_addr, 0xe0000000);
  EXPECT_TRUE(lwpa_ip_is_multicast(&test_addr));

  LWPA_IP_SET_V4_ADDRESS(&test_addr, 0xefffffff);
  EXPECT_TRUE(lwpa_ip_is_multicast(&test_addr));

  LWPA_IP_SET_V4_ADDRESS(&test_addr, 0xf0000000);
  EXPECT_FALSE(lwpa_ip_is_multicast(&test_addr));

  LWPA_IP_SET_V4_ADDRESS(&test_addr, 0xffffffff);
  EXPECT_FALSE(lwpa_ip_is_multicast(&test_addr));

  // Test the edges of the IPv6 multicast range
  std::array<uint8_t, LWPA_IPV6_BYTES> v6_data = {0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                                                  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
  LWPA_IP_SET_V6_ADDRESS(&test_addr, v6_data.data());
  EXPECT_FALSE(lwpa_ip_is_multicast(&test_addr));

  v6_data = {0xff, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  LWPA_IP_SET_V6_ADDRESS(&test_addr, v6_data.data());
  EXPECT_TRUE(lwpa_ip_is_multicast(&test_addr));

  v6_data = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
  LWPA_IP_SET_V6_ADDRESS(&test_addr, v6_data.data());
  EXPECT_TRUE(lwpa_ip_is_multicast(&test_addr));
}

// Just tests reflexivity of the wildcard functions, we don't rely on the wildcard address to be
// any specific value.
TEST_F(InetTest, wildcard)
{
  LwpaIpAddr test_addr;

  lwpa_ip_set_wildcard(kLwpaIpTypeV4, &test_addr);
  EXPECT_TRUE(lwpa_ip_is_wildcard(&test_addr));

  lwpa_ip_set_wildcard(kLwpaIpTypeV6, &test_addr);
  EXPECT_TRUE(lwpa_ip_is_wildcard(&test_addr));
}

// Test the lwpa_ip_equal() and lwpa_ip_cmp() functions
TEST_F(InetTest, compare)
{
  LwpaIpAddr v4;
  LWPA_IP_SET_V4_ADDRESS(&v4, 0x01020304);

  // Copied address should compare equal
  LwpaIpAddr v4_copy = v4;
  EXPECT_TRUE(lwpa_ip_equal(&v4_copy, &v4));
  EXPECT_EQ(0, lwpa_ip_cmp(&v4_copy, &v4));

  LwpaIpAddr v6;
  std::array<uint8_t, LWPA_IPV6_BYTES> v6_data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
  LWPA_IP_SET_V6_ADDRESS(&v6, v6_data.data());

  // Copied address should compare equal
  LwpaIpAddr v6_copy = v6;
  EXPECT_TRUE(lwpa_ip_equal(&v6_copy, &v6));
  EXPECT_EQ(0, lwpa_ip_cmp(&v6_copy, &v6));

  // The v4 and v6 should not be equal, and per the comparison algorithm the v4 should be less than
  // the v6
  EXPECT_FALSE(lwpa_ip_equal(&v4, &v6));
  EXPECT_LT(lwpa_ip_cmp(&v4, &v6), 0);

  // Further test the lwpa_ip_cmp() function, with like address types
  LwpaIpAddr v4_less;
  LWPA_IP_SET_V4_ADDRESS(&v4_less, 0x01020303);

  LwpaIpAddr v6_less;
  auto v6_data_less = v6_data;
  v6_data_less[15] = 14;
  LWPA_IP_SET_V6_ADDRESS(&v6_less, v6_data_less.data());

  EXPECT_LT(lwpa_ip_cmp(&v4_less, &v4), 0);
  EXPECT_LT(lwpa_ip_cmp(&v6_less, &v6), 0);

  // Make sure there are no gotchas with 2's complement by setting high bits of addresses
  LWPA_IP_SET_V4_ADDRESS(&v4, 0xffffffff);
  LWPA_IP_SET_V4_ADDRESS(&v4_less, 0x0);

  EXPECT_LT(lwpa_ip_cmp(&v4_less, &v4), 0);
}

// Test the lwpa_ip_mask_length() function
TEST_F(InetTest, mask_length)
{
  LwpaIpAddr test_mask;

  // Test mask: Invalid or null IP
  EXPECT_EQ(0u, lwpa_ip_mask_length(nullptr));
  LWPA_IP_SET_INVALID(&test_mask);
  EXPECT_EQ(0u, lwpa_ip_mask_length(&test_mask));

  // Test mask: /0, V4
  LWPA_IP_SET_V4_ADDRESS(&test_mask, 0);
  EXPECT_EQ(0u, lwpa_ip_mask_length(&test_mask));

  // Test mask: /7, V4
  LWPA_IP_SET_V4_ADDRESS(&test_mask, 0xfe000000u);
  EXPECT_EQ(7u, lwpa_ip_mask_length(&test_mask));

  // Test mask: /25, V4
  LWPA_IP_SET_V4_ADDRESS(&test_mask, 0xffffff80u);
  EXPECT_EQ(25u, lwpa_ip_mask_length(&test_mask));

  // Test mask: /32, V4
  LWPA_IP_SET_V4_ADDRESS(&test_mask, 0xffffffffu);
  EXPECT_EQ(32u, lwpa_ip_mask_length(&test_mask));

  // Test mask: nonzero but with leading zeroes, V4
  LWPA_IP_SET_V4_ADDRESS(&test_mask, 0x7fffffff);
  EXPECT_EQ(0u, lwpa_ip_mask_length(&test_mask));

  // Test mask: /0, V6
  std::array<uint8_t, LWPA_IPV6_BYTES> test_v6_val{};
  LWPA_IP_SET_V6_ADDRESS(&test_mask, test_v6_val.data());
  EXPECT_EQ(0u, lwpa_ip_mask_length(&test_mask));

  // Test mask: /64, V6
  test_v6_val = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0, 0, 0, 0, 0, 0, 0, 0};
  LWPA_IP_SET_V6_ADDRESS(&test_mask, test_v6_val.data());
  EXPECT_EQ(64u, lwpa_ip_mask_length(&test_mask));

  // Test mask: /77, V6
  test_v6_val = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0, 0, 0, 0, 0, 0};
  LWPA_IP_SET_V6_ADDRESS(&test_mask, test_v6_val.data());
  EXPECT_EQ(77u, lwpa_ip_mask_length(&test_mask));

  // Test mask: /128, V6
  test_v6_val = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
  LWPA_IP_SET_V6_ADDRESS(&test_mask, test_v6_val.data());
  EXPECT_EQ(128u, lwpa_ip_mask_length(&test_mask));

  // Test mask: nonzero but with leading zeroes, V6
  test_v6_val = {0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
  LWPA_IP_SET_V6_ADDRESS(&test_mask, test_v6_val.data());
  EXPECT_EQ(0u, lwpa_ip_mask_length(&test_mask));
}

// Test the lwpa_ip_mask_from_length() function
TEST_F(InetTest, mask_from_length)
{
  // Test mask: /0, V4
  LwpaIpAddr mask_out = lwpa_ip_mask_from_length(kLwpaIpTypeV4, 0);
  EXPECT_TRUE(LWPA_IP_IS_V4(&mask_out));
  EXPECT_EQ(LWPA_IP_V4_ADDRESS(&mask_out), 0u);

  // Test mask: /7, V4
  mask_out = lwpa_ip_mask_from_length(kLwpaIpTypeV4, 7);
  EXPECT_TRUE(LWPA_IP_IS_V4(&mask_out));
  EXPECT_EQ(LWPA_IP_V4_ADDRESS(&mask_out), 0xfe000000u);

  // Test mask: /25, V4
  mask_out = lwpa_ip_mask_from_length(kLwpaIpTypeV4, 25);
  EXPECT_TRUE(LWPA_IP_IS_V4(&mask_out));
  EXPECT_EQ(LWPA_IP_V4_ADDRESS(&mask_out), 0xffffff80u);

  // Test mask: /32, V4
  mask_out = lwpa_ip_mask_from_length(kLwpaIpTypeV4, 32);
  EXPECT_TRUE(LWPA_IP_IS_V4(&mask_out));
  EXPECT_EQ(LWPA_IP_V4_ADDRESS(&mask_out), 0xffffffffu);

  // Test mask: Greater than /32, V4
  // Should truncate to 32
  mask_out = lwpa_ip_mask_from_length(kLwpaIpTypeV4, UINT_MAX);
  EXPECT_TRUE(LWPA_IP_IS_V4(&mask_out));
  EXPECT_EQ(LWPA_IP_V4_ADDRESS(&mask_out), 0xffffffffu);

  // Test mask: /0, V6
  std::array<uint8_t, LWPA_IPV6_BYTES> v6_compare_val{};
  mask_out = lwpa_ip_mask_from_length(kLwpaIpTypeV6, 0);
  EXPECT_TRUE(LWPA_IP_IS_V6(&mask_out));
  EXPECT_EQ(0, memcmp(LWPA_IP_V6_ADDRESS(&mask_out), v6_compare_val.data(), LWPA_IPV6_BYTES));

  // Test mask: /64, V6
  v6_compare_val = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0, 0, 0, 0, 0, 0, 0, 0};
  mask_out = lwpa_ip_mask_from_length(kLwpaIpTypeV6, 64);
  EXPECT_TRUE(LWPA_IP_IS_V6(&mask_out));
  EXPECT_EQ(0, memcmp(LWPA_IP_V6_ADDRESS(&mask_out), v6_compare_val.data(), LWPA_IPV6_BYTES));

  // Test mask: /77, V6
  v6_compare_val = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0, 0, 0, 0, 0, 0};
  mask_out = lwpa_ip_mask_from_length(kLwpaIpTypeV6, 77);
  EXPECT_TRUE(LWPA_IP_IS_V6(&mask_out));
  EXPECT_EQ(0, memcmp(LWPA_IP_V6_ADDRESS(&mask_out), v6_compare_val.data(), LWPA_IPV6_BYTES));

  // Test mask: /128, V6
  v6_compare_val = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
  mask_out = lwpa_ip_mask_from_length(kLwpaIpTypeV6, 128);
  EXPECT_TRUE(LWPA_IP_IS_V6(&mask_out));
  EXPECT_EQ(0, memcmp(LWPA_IP_V6_ADDRESS(&mask_out), v6_compare_val.data(), LWPA_IPV6_BYTES));

  // Test mask: Greater than /128, V6
  // Should truncate to 128
  mask_out = lwpa_ip_mask_from_length(kLwpaIpTypeV6, UINT_MAX);
  EXPECT_TRUE(LWPA_IP_IS_V6(&mask_out));
  EXPECT_EQ(0, memcmp(LWPA_IP_V6_ADDRESS(&mask_out), v6_compare_val.data(), LWPA_IPV6_BYTES));
}