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
#include "etcpal/inet.h"
#include "unity_fixture.h"

#include <string.h>
#include <limits.h>

TEST_GROUP(lwpa_inet);

TEST_SETUP(lwpa_inet)
{
}

TEST_TEAR_DOWN(lwpa_inet)
{
}

// Test the LwpaIpAddr struct and macros
TEST(lwpa_inet, ipaddr_macros_work)
{
  LwpaIpAddr test_addr;

  LWPA_IP_SET_V4_ADDRESS(&test_addr, 0x01020304);
  TEST_ASSERT(LWPA_IP_IS_V4(&test_addr));
  TEST_ASSERT_EQUAL_UINT32(LWPA_IP_V4_ADDRESS(&test_addr), 0x01020304u);

  uint8_t v6_data[LWPA_IPV6_BYTES] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
  LWPA_IP_SET_V6_ADDRESS(&test_addr, v6_data);
  TEST_ASSERT(LWPA_IP_IS_V6(&test_addr));
  TEST_ASSERT_EQUAL_UINT8_ARRAY(LWPA_IP_V6_ADDRESS(&test_addr), v6_data, LWPA_IPV6_BYTES);

  LWPA_IP_SET_INVALID(&test_addr);
  TEST_ASSERT(LWPA_IP_IS_INVALID(&test_addr));
}

// Test the lwpa_ip_is_loopback() function
TEST(lwpa_inet, ip_is_loopback_works)
{
  LwpaIpAddr test_addr;

  // An invalid IP is not loopback
  LWPA_IP_SET_INVALID(&test_addr);
  TEST_ASSERT_UNLESS(lwpa_ip_is_loopback(&test_addr));

  // Test the edges of the IPv4 loopback range
  LWPA_IP_SET_V4_ADDRESS(&test_addr, 0x7effffff);
  TEST_ASSERT_UNLESS(lwpa_ip_is_loopback(&test_addr));

  LWPA_IP_SET_V4_ADDRESS(&test_addr, 0x7f000000);
  TEST_ASSERT(lwpa_ip_is_loopback(&test_addr));

  LWPA_IP_SET_V4_ADDRESS(&test_addr, 0x7fffffff);
  TEST_ASSERT(lwpa_ip_is_loopback(&test_addr));

  LWPA_IP_SET_V4_ADDRESS(&test_addr, 0x80000000);
  TEST_ASSERT_UNLESS(lwpa_ip_is_loopback(&test_addr));

  // Test the IPv6 loopback address
  uint8_t v6_data[LWPA_IPV6_BYTES] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1};
  LWPA_IP_SET_V6_ADDRESS(&test_addr, v6_data);
  TEST_ASSERT(lwpa_ip_is_loopback(&test_addr));

  v6_data[15] = 0;
  LWPA_IP_SET_V6_ADDRESS(&test_addr, v6_data);
  TEST_ASSERT_UNLESS(lwpa_ip_is_loopback(&test_addr));
}

// Test the lwpa_ip_is_multicast() function
TEST(lwpa_inet, ip_is_multicast_works)
{
  LwpaIpAddr test_addr;

  // An invalid IP is not multicast
  LWPA_IP_SET_INVALID(&test_addr);
  TEST_ASSERT_UNLESS(lwpa_ip_is_multicast(&test_addr));

  // Test the edges of the IPv4 multicast range
  LWPA_IP_SET_V4_ADDRESS(&test_addr, 0xdfffffff);
  TEST_ASSERT_UNLESS(lwpa_ip_is_multicast(&test_addr));

  LWPA_IP_SET_V4_ADDRESS(&test_addr, 0xe0000000);
  TEST_ASSERT(lwpa_ip_is_multicast(&test_addr));

  LWPA_IP_SET_V4_ADDRESS(&test_addr, 0xefffffff);
  TEST_ASSERT(lwpa_ip_is_multicast(&test_addr));

  LWPA_IP_SET_V4_ADDRESS(&test_addr, 0xf0000000);
  TEST_ASSERT_UNLESS(lwpa_ip_is_multicast(&test_addr));

  LWPA_IP_SET_V4_ADDRESS(&test_addr, 0xffffffff);
  TEST_ASSERT_UNLESS(lwpa_ip_is_multicast(&test_addr));

  // Test the edges of the IPv6 multicast range
  uint8_t v6_data[LWPA_IPV6_BYTES] = {0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                                      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
  LWPA_IP_SET_V6_ADDRESS(&test_addr, v6_data);
  TEST_ASSERT_UNLESS(lwpa_ip_is_multicast(&test_addr));

  memcpy(v6_data, (uint8_t[]){0xff, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, LWPA_IPV6_BYTES);
  LWPA_IP_SET_V6_ADDRESS(&test_addr, v6_data);
  TEST_ASSERT(lwpa_ip_is_multicast(&test_addr));

  memcpy(v6_data,
         (uint8_t[]){0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
         LWPA_IPV6_BYTES);
  LWPA_IP_SET_V6_ADDRESS(&test_addr, v6_data);
  TEST_ASSERT(lwpa_ip_is_multicast(&test_addr));
}

// Just tests reflexivity of the wildcard functions, we don't rely on the wildcard address to be
// any specific value.
TEST(lwpa_inet, ip_is_wildcard_works)
{
  LwpaIpAddr test_addr;

  lwpa_ip_set_wildcard(kLwpaIpTypeV4, &test_addr);
  TEST_ASSERT(lwpa_ip_is_wildcard(&test_addr));

  lwpa_ip_set_wildcard(kLwpaIpTypeV6, &test_addr);
  TEST_ASSERT(lwpa_ip_is_wildcard(&test_addr));
}

// Test the lwpa_ip_equal() and lwpa_ip_cmp() functions
TEST(lwpa_inet, ip_compare_functions_work)
{
  LwpaIpAddr v4;
  LWPA_IP_SET_V4_ADDRESS(&v4, 0x01020304);

  // Copied address should compare equal
  LwpaIpAddr v4_copy = v4;
  TEST_ASSERT(lwpa_ip_equal(&v4_copy, &v4));
  TEST_ASSERT_EQUAL_INT(0, lwpa_ip_cmp(&v4_copy, &v4));

  LwpaIpAddr v6;
  uint8_t v6_data[LWPA_IPV6_BYTES] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
  LWPA_IP_SET_V6_ADDRESS(&v6, v6_data);

  // Copied address should compare equal
  LwpaIpAddr v6_copy = v6;
  TEST_ASSERT(lwpa_ip_equal(&v6_copy, &v6));
  TEST_ASSERT_EQUAL_INT(0, lwpa_ip_cmp(&v6_copy, &v6));

  // The v4 and v6 should not be equal, and per the comparison algorithm the v4 should be less than
  // the v6
  TEST_ASSERT_UNLESS(lwpa_ip_equal(&v4, &v6));
  TEST_ASSERT_LESS_THAN_INT(0, lwpa_ip_cmp(&v4, &v6));

  // Further test the lwpa_ip_cmp() function, with like address types
  LwpaIpAddr v4_less;
  LWPA_IP_SET_V4_ADDRESS(&v4_less, 0x01020303);

  LwpaIpAddr v6_less;
  uint8_t v6_data_less[LWPA_IPV6_BYTES];
  memcpy(v6_data_less, v6_data, LWPA_IPV6_BYTES);
  v6_data_less[15] = 14;
  LWPA_IP_SET_V6_ADDRESS(&v6_less, v6_data_less);

  TEST_ASSERT_LESS_THAN_INT(0, lwpa_ip_cmp(&v4_less, &v4));
  TEST_ASSERT_LESS_THAN_INT(0, lwpa_ip_cmp(&v6_less, &v6));

  // Make sure there are no gotchas with 2's complement by setting high bits of addresses
  LWPA_IP_SET_V4_ADDRESS(&v4, 0xffffffff);
  LWPA_IP_SET_V4_ADDRESS(&v4_less, 0x0);

  TEST_ASSERT_LESS_THAN_INT(0, lwpa_ip_cmp(&v4_less, &v4));
}

// Test the lwpa_ip_mask_length() function
TEST(lwpa_inet, ip_mask_length_works)
{
  LwpaIpAddr test_mask;

  // Test mask: Invalid or null IP
  TEST_ASSERT_EQUAL_UINT(0u, lwpa_ip_mask_length(NULL));
  LWPA_IP_SET_INVALID(&test_mask);
  TEST_ASSERT_EQUAL_UINT(0u, lwpa_ip_mask_length(&test_mask));

  // Test mask: /0, V4
  LWPA_IP_SET_V4_ADDRESS(&test_mask, 0);
  TEST_ASSERT_EQUAL_UINT(0u, lwpa_ip_mask_length(&test_mask));

  // Test mask: /7, V4
  LWPA_IP_SET_V4_ADDRESS(&test_mask, 0xfe000000u);
  TEST_ASSERT_EQUAL_UINT(7u, lwpa_ip_mask_length(&test_mask));

  // Test mask: /25, V4
  LWPA_IP_SET_V4_ADDRESS(&test_mask, 0xffffff80u);
  TEST_ASSERT_EQUAL_UINT(25u, lwpa_ip_mask_length(&test_mask));

  // Test mask: /32, V4
  LWPA_IP_SET_V4_ADDRESS(&test_mask, 0xffffffffu);
  TEST_ASSERT_EQUAL_UINT(32u, lwpa_ip_mask_length(&test_mask));

  // Test mask: nonzero but with leading zeroes, V4
  LWPA_IP_SET_V4_ADDRESS(&test_mask, 0x7fffffff);
  TEST_ASSERT_EQUAL_UINT(0u, lwpa_ip_mask_length(&test_mask));

  // Test mask: /0, V6
  uint8_t test_v6_val[LWPA_IPV6_BYTES] = {0};
  LWPA_IP_SET_V6_ADDRESS(&test_mask, test_v6_val);
  TEST_ASSERT_EQUAL_UINT(0u, lwpa_ip_mask_length(&test_mask));

  // Test mask: /64, V6
  memcpy(test_v6_val, (uint8_t[]){0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0, 0, 0, 0, 0, 0, 0, 0},
         LWPA_IPV6_BYTES);
  LWPA_IP_SET_V6_ADDRESS(&test_mask, test_v6_val);
  TEST_ASSERT_EQUAL_UINT(64u, lwpa_ip_mask_length(&test_mask));

  // Test mask: /77, V6
  memcpy(test_v6_val, (uint8_t[]){0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0, 0, 0, 0, 0, 0},
         LWPA_IPV6_BYTES);
  LWPA_IP_SET_V6_ADDRESS(&test_mask, test_v6_val);
  TEST_ASSERT_EQUAL_UINT(77u, lwpa_ip_mask_length(&test_mask));

  // Test mask: /128, V6
  memcpy(test_v6_val,
         (uint8_t[]){0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
         LWPA_IPV6_BYTES);
  LWPA_IP_SET_V6_ADDRESS(&test_mask, test_v6_val);
  TEST_ASSERT_EQUAL_UINT(128u, lwpa_ip_mask_length(&test_mask));

  // Test mask: nonzero but with leading zeroes, V6
  memcpy(test_v6_val,
         (uint8_t[]){0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
         LWPA_IPV6_BYTES);
  LWPA_IP_SET_V6_ADDRESS(&test_mask, test_v6_val);
  TEST_ASSERT_EQUAL_UINT(0u, lwpa_ip_mask_length(&test_mask));
}

// Test the lwpa_ip_mask_from_length() function
TEST(lwpa_inet, ip_mask_from_length_works)
{
  // Test mask: /0, V4
  LwpaIpAddr mask_out = lwpa_ip_mask_from_length(kLwpaIpTypeV4, 0);
  TEST_ASSERT(LWPA_IP_IS_V4(&mask_out));
  TEST_ASSERT_EQUAL_UINT32(LWPA_IP_V4_ADDRESS(&mask_out), 0u);

  // Test mask: /7, V4
  mask_out = lwpa_ip_mask_from_length(kLwpaIpTypeV4, 7);
  TEST_ASSERT(LWPA_IP_IS_V4(&mask_out));
  TEST_ASSERT_EQUAL_UINT32(LWPA_IP_V4_ADDRESS(&mask_out), 0xfe000000u);

  // Test mask: /25, V4
  mask_out = lwpa_ip_mask_from_length(kLwpaIpTypeV4, 25);
  TEST_ASSERT(LWPA_IP_IS_V4(&mask_out));
  TEST_ASSERT_EQUAL_UINT32(LWPA_IP_V4_ADDRESS(&mask_out), 0xffffff80u);

  // Test mask: /32, V4
  mask_out = lwpa_ip_mask_from_length(kLwpaIpTypeV4, 32);
  TEST_ASSERT(LWPA_IP_IS_V4(&mask_out));
  TEST_ASSERT_EQUAL_UINT32(LWPA_IP_V4_ADDRESS(&mask_out), 0xffffffffu);

  // Test mask: Greater than /32, V4
  // Should truncate to 32
  mask_out = lwpa_ip_mask_from_length(kLwpaIpTypeV4, UINT_MAX);
  TEST_ASSERT(LWPA_IP_IS_V4(&mask_out));
  TEST_ASSERT_EQUAL_UINT32(LWPA_IP_V4_ADDRESS(&mask_out), 0xffffffffu);

  // Test mask: /0, V6
  uint8_t v6_compare_val[LWPA_IPV6_BYTES] = {0};
  mask_out = lwpa_ip_mask_from_length(kLwpaIpTypeV6, 0);
  TEST_ASSERT(LWPA_IP_IS_V6(&mask_out));
  TEST_ASSERT_EQUAL_UINT8_ARRAY(LWPA_IP_V6_ADDRESS(&mask_out), v6_compare_val, LWPA_IPV6_BYTES);

  // Test mask: /64, V6
  memcpy(v6_compare_val, (uint8_t[]){0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0, 0, 0, 0, 0, 0, 0, 0},
         LWPA_IPV6_BYTES);
  mask_out = lwpa_ip_mask_from_length(kLwpaIpTypeV6, 64);
  TEST_ASSERT(LWPA_IP_IS_V6(&mask_out));
  TEST_ASSERT_EQUAL_UINT8_ARRAY(LWPA_IP_V6_ADDRESS(&mask_out), v6_compare_val, LWPA_IPV6_BYTES);

  // Test mask: /77, V6
  memcpy(v6_compare_val, (uint8_t[]){0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0, 0, 0, 0, 0, 0},
         LWPA_IPV6_BYTES);
  mask_out = lwpa_ip_mask_from_length(kLwpaIpTypeV6, 77);
  TEST_ASSERT(LWPA_IP_IS_V6(&mask_out));
  TEST_ASSERT_EQUAL_UINT8_ARRAY(LWPA_IP_V6_ADDRESS(&mask_out), v6_compare_val, LWPA_IPV6_BYTES);

  // Test mask: /128, V6
  memcpy(v6_compare_val,
         (uint8_t[]){0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
         LWPA_IPV6_BYTES);
  mask_out = lwpa_ip_mask_from_length(kLwpaIpTypeV6, 128);
  TEST_ASSERT(LWPA_IP_IS_V6(&mask_out));
  TEST_ASSERT_EQUAL_UINT8_ARRAY(LWPA_IP_V6_ADDRESS(&mask_out), v6_compare_val, LWPA_IPV6_BYTES);

  // Test mask: Greater than /128, V6
  // Should truncate to 128
  mask_out = lwpa_ip_mask_from_length(kLwpaIpTypeV6, UINT_MAX);
  TEST_ASSERT(LWPA_IP_IS_V6(&mask_out));
  TEST_ASSERT_EQUAL_UINT8_ARRAY(LWPA_IP_V6_ADDRESS(&mask_out), v6_compare_val, LWPA_IPV6_BYTES);
}

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

TEST(lwpa_inet, inet_string_functions_work)
{
  LwpaIpAddr addr;

  // Test lwpa_inet_pton()
  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_inet_pton(kLwpaIpTypeV4, test_ip4_1, &addr));
  TEST_ASSERT_EQUAL(LWPA_IP_V4_ADDRESS(&addr), 0u);
  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_inet_pton(kLwpaIpTypeV4, test_ip4_2, &addr));
  TEST_ASSERT_EQUAL(LWPA_IP_V4_ADDRESS(&addr), 0xffffffffu);
  TEST_ASSERT_NOT_EQUAL(kLwpaErrOk, lwpa_inet_pton(kLwpaIpTypeV4, test_ip4_fail, &addr));
  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_inet_pton(kLwpaIpTypeV6, test_ip6_1, &addr));
  TEST_ASSERT_EQUAL(0, memcmp(LWPA_IP_V6_ADDRESS(&addr), test_ip6_1_bin, LWPA_IPV6_BYTES));
  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_inet_pton(kLwpaIpTypeV6, test_ip6_2, &addr));
  TEST_ASSERT_EQUAL(0, memcmp(LWPA_IP_V6_ADDRESS(&addr), test_ip6_2_bin, LWPA_IPV6_BYTES));
  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_inet_pton(kLwpaIpTypeV6, test_ip6_3, &addr));
  TEST_ASSERT_EQUAL(0, memcmp(LWPA_IP_V6_ADDRESS(&addr), test_ip6_3_bin, LWPA_IPV6_BYTES));
  TEST_ASSERT_NOT_EQUAL(kLwpaErrOk, lwpa_inet_pton(kLwpaIpTypeV6, test_ip6_fail, &addr));

  // Test lwpa_inet_ntop()
  LWPA_IP_SET_V4_ADDRESS(&addr, 0);
  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_inet_ntop(&addr, str, LWPA_INET_ADDRSTRLEN));
  TEST_ASSERT_EQUAL(0, strcmp(str, test_ip4_1));
  LWPA_IP_SET_V4_ADDRESS(&addr, 0xffffffff);
  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_inet_ntop(&addr, str, LWPA_INET_ADDRSTRLEN));
  TEST_ASSERT_EQUAL(0, strcmp(str, test_ip4_2));
  LWPA_IP_SET_V6_ADDRESS(&addr, test_ip6_1_bin);
  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_inet_ntop(&addr, str, LWPA_INET6_ADDRSTRLEN));
  TEST_ASSERT_EQUAL(0, strcmp(str, test_ip6_1));
  LWPA_IP_SET_V6_ADDRESS(&addr, test_ip6_2_bin);
  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_inet_ntop(&addr, str, LWPA_INET6_ADDRSTRLEN));
  TEST_ASSERT_EQUAL(0, strcmp(str, test_ip6_2));
  LWPA_IP_SET_V6_ADDRESS(&addr, test_ip6_3_bin);
  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_inet_ntop(&addr, str, LWPA_INET6_ADDRSTRLEN));
  TEST_ASSERT((0 == strcmp(str, "ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff")) ||
              (0 == strcmp(str, "FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF")));
}

TEST_GROUP_RUNNER(lwpa_inet)
{
  RUN_TEST_CASE(lwpa_inet, ipaddr_macros_work);
  RUN_TEST_CASE(lwpa_inet, ip_is_loopback_works);
  RUN_TEST_CASE(lwpa_inet, ip_is_multicast_works);
  RUN_TEST_CASE(lwpa_inet, ip_is_wildcard_works);
  RUN_TEST_CASE(lwpa_inet, ip_compare_functions_work);
  RUN_TEST_CASE(lwpa_inet, ip_mask_length_works);
  RUN_TEST_CASE(lwpa_inet, ip_mask_from_length_works);
  RUN_TEST_CASE(lwpa_inet, inet_string_functions_work);
}
