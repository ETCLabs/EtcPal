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

class InetTest : public ::testing::Test
{
};

// Test the LwpaIpAddr struct and macros
TEST_F(InetTest, ip_addr)
{
  LwpaIpAddr v4;

  lwpaip_set_v4_address(&v4, 0x01020304);
  ASSERT_TRUE(lwpaip_is_v4(&v4));
  ASSERT_EQ(lwpaip_v4_address(&v4), 0x01020304u);

  // Copied address should compare equal
  LwpaIpAddr v4_copy = v4;
  ASSERT_TRUE(lwpaip_equal(&v4_copy, &v4));
  ASSERT_EQ(0, lwpaip_cmp(&v4_copy, &v4));

  LwpaIpAddr v6;

  std::array<uint8_t, LWPA_IPV6_BYTES> v6_data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
  lwpaip_set_v6_address(&v6, v6_data.data());
  ASSERT_TRUE(lwpaip_is_v6(&v6));
  ASSERT_EQ(0, memcmp(lwpaip_v6_address(&v6), v6_data.data(), LWPA_IPV6_BYTES));

  // Copied address should compare equal
  LwpaIpAddr v6_copy = v6;
  ASSERT_TRUE(lwpaip_equal(&v6_copy, &v6));
  ASSERT_EQ(0, lwpaip_cmp(&v6_copy, &v6));

  // The v4 and v6 should not be equal, and per the comparison algorithm the v4 should be less than
  // the v6
  ASSERT_FALSE(lwpaip_equal(&v4, &v6));
  ASSERT_LT(lwpaip_cmp(&v4, &v6), 0);

  // Further test the lwpaip_cmp() macro, with like address types
  LwpaIpAddr v4_less;
  lwpaip_set_v4_address(&v4_less, 0x01020303);

  LwpaIpAddr v6_less;
  auto v6_data_less = v6_data;
  v6_data_less[15] = 14;
  lwpaip_set_v6_address(&v6_less, v6_data_less.data());

  ASSERT_LT(lwpaip_cmp(&v4_less, &v4), 0);
  ASSERT_LT(lwpaip_cmp(&v6_less, &v6), 0);
}