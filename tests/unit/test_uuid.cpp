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
#include "lwpa/uuid.h"
#include "gtest/gtest.h"
#include <cstddef>
#include <algorithm>

class UuidTest : public ::testing::Test
{
public:
  static constexpr int kNumV1UuidGenerations = 10000;
  static constexpr int kNumV4UuidGenerations = 10000;
};

TEST_F(UuidTest, null)
{
  LwpaUuid uuid = {{0}};
  ASSERT_TRUE(lwpa_uuid_is_null(&uuid));
  uuid = LWPA_NULL_UUID;
  ASSERT_TRUE(lwpa_uuid_is_null(&uuid));
  for (size_t i = 0; i < LWPA_UUID_BYTES; ++i)
    uuid.data[i] = static_cast<uint8_t>(i);
  ASSERT_FALSE(lwpa_uuid_is_null(&uuid));
}

TEST_F(UuidTest, compare)
{
  LwpaUuid uuid1 = {{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16}};
  LwpaUuid uuid1_dup = {{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16}};
  LwpaUuid uuid2 = {{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 17}};

  ASSERT_EQ(0, lwpa_uuid_cmp(&uuid1, &uuid1_dup));
  ASSERT_LT(0, lwpa_uuid_cmp(&uuid2, &uuid1));
  ASSERT_GT(0, lwpa_uuid_cmp(&uuid1, &uuid2));

  // Test the C++ operators
  ASSERT_LT(uuid1, uuid2);
  ASSERT_EQ(uuid1, uuid1_dup);
}

TEST_F(UuidTest, string)
{
  const char good_str[] = "08090a0b-0C0D-0e0f-1011-121314151617";
  const char short_str[] = "08090a0b-0c0d-0e0f-1011-1213141516";
  const char bad_str[] = "This isn't a UUID";
  char str_buf[LWPA_UUID_STRING_BYTES];
  LwpaUuid uuid = {{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16}};

  lwpa_uuid_to_string(str_buf, &uuid);
  ASSERT_EQ(0, strcmp(str_buf, "01020304-0506-0708-090a-0b0c0d0e0f10"));
  uuid = LWPA_NULL_UUID;
  lwpa_uuid_to_string(str_buf, &uuid);
  ASSERT_EQ(0, strcmp(str_buf, "00000000-0000-0000-0000-000000000000"));
  ASSERT_TRUE(lwpa_string_to_uuid(&uuid, good_str, strlen(good_str)));
  LwpaUuid uuid_cmp = {{8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23}};
  ASSERT_EQ(0, lwpa_uuid_cmp(&uuid, &uuid_cmp));
  uuid = LWPA_NULL_UUID;
  ASSERT_FALSE(lwpa_string_to_uuid(&uuid, short_str, strlen(short_str)));
  ASSERT_FALSE(lwpa_string_to_uuid(&uuid, bad_str, strlen(bad_str)));
}

TEST_F(UuidTest, generate_v1)
{
  // Generate a bunch of V1 UUIDs. They should all be unique from each other and have the proper
  // version and variant information. We will cheat a little and just make sure that each one is
  // unique from the last one generated.
  LwpaUuid last_uuid = LWPA_NULL_UUID;

  for (int i = 0; i < kNumV1UuidGenerations; ++i)
  {
    LwpaUuid uuid;
    std::string failure_msg = "This failure occurred on UUID attempt " + std::to_string(i + 1) + " of " +
                              std::to_string(kNumV1UuidGenerations);

    ASSERT_EQ(LWPA_OK, lwpa_generate_v1_uuid(&uuid)) << failure_msg;

    // We should always have Variant 1, Version 1.
    ASSERT_EQ((uuid.data[6] & 0xf0u), 0x10u) << failure_msg;
    ASSERT_EQ((uuid.data[8] & 0xc0u), 0x80u) << failure_msg;

    // Should be unique from the last one generated.
    ASSERT_NE(0, lwpa_uuid_cmp(&uuid, &last_uuid)) << failure_msg;

    last_uuid = uuid;
  }
}

TEST_F(UuidTest, generate_v3)
{
  uint8_t mac1[6] = {0x00, 0xc0, 0x16, 0xff, 0xef, 0x12};
  uint8_t mac2[6] = {0x00, 0xc0, 0x16, 0xff, 0xef, 0x13};
  LwpaUuid uuid1, uuid2, uuid3, uuid4, uuid1_dup;

  // Version 3 UUIDs should be deterministic for the same combination of the three possible input
  // arguments. If any of the arguments is different, a different UUID should result.
  lwpa_generate_v3_uuid(&uuid1, "Test Device", mac1, 0);
  lwpa_generate_v3_uuid(&uuid2, "Test Device", mac1, 1);
  lwpa_generate_v3_uuid(&uuid3, "Tst Device", mac1, 0);
  lwpa_generate_v3_uuid(&uuid4, "Test Device", mac2, 0);
  lwpa_generate_v3_uuid(&uuid1_dup, "Test Device", mac1, 0);

  ASSERT_NE(0, lwpa_uuid_cmp(&uuid1, &uuid2));
  ASSERT_NE(0, lwpa_uuid_cmp(&uuid1, &uuid3));
  ASSERT_NE(0, lwpa_uuid_cmp(&uuid1, &uuid4));
  ASSERT_NE(0, lwpa_uuid_cmp(&uuid2, &uuid3));
  ASSERT_NE(0, lwpa_uuid_cmp(&uuid2, &uuid4));
  ASSERT_NE(0, lwpa_uuid_cmp(&uuid3, &uuid4));
  ASSERT_EQ(0, lwpa_uuid_cmp(&uuid1, &uuid1_dup));

  // Make sure the Variant Version bits are correct.
  // We should always have Variant 1, Version 3.
  std::vector<LwpaUuid> uuid_vect{uuid1, uuid2, uuid3, uuid4, uuid1_dup};
  std::for_each(uuid_vect.begin(), uuid_vect.end(), [](const LwpaUuid &uuid) {
    ASSERT_EQ((uuid.data[6] & 0xf0u), 0x30u);
    ASSERT_EQ((uuid.data[8] & 0xc0u), 0x80u);
  });
}

TEST_F(UuidTest, generate_v4)
{
  // Generate a bunch of V4 UUIDs. They should all be unique from each other and have the proper
  // version and variant information. We will cheat a little and just make sure that each one is
  // unique from the last one generated.
  LwpaUuid last_uuid = LWPA_NULL_UUID;

  for (int i = 0; i < kNumV4UuidGenerations; ++i)
  {
    LwpaUuid uuid;
    std::string failure_msg = "This failure occurred on UUID attempt " + std::to_string(i + 1) + " of " +
                              std::to_string(kNumV4UuidGenerations);

    ASSERT_EQ(LWPA_OK, lwpa_generate_v4_uuid(&uuid)) << failure_msg;

    // We should always have Variant 1, Version 4.
    ASSERT_EQ((uuid.data[6] & 0xf0u), 0x40u) << failure_msg;
    ASSERT_EQ((uuid.data[8] & 0xc0u), 0x80u) << failure_msg;

    // Should be unique from the last one generated.
    ASSERT_NE(0, lwpa_uuid_cmp(&uuid, &last_uuid)) << failure_msg;

    last_uuid = uuid;
  }
}