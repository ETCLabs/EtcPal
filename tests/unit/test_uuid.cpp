/******************************************************************************
 * Copyright 2018 ETC Inc.
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

class UuidTest : public ::testing::Test
{
};

TEST_F(UuidTest, null)
{
  LwpaUuid uuid = {{0}};
  ASSERT_TRUE(uuid_isnull(&uuid));
  uuid = NULL_UUID;
  ASSERT_TRUE(uuid_isnull(&uuid));
  for (size_t i = 0; i < UUID_BYTES; ++i)
    uuid.data[i] = static_cast<uint8_t>(i);
  ASSERT_FALSE(uuid_isnull(&uuid));
}

TEST_F(UuidTest, compare)
{
  LwpaUuid uuid1 = {{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16}};
  LwpaUuid uuid1_dup = {{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16}};
  LwpaUuid uuid2 = {{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 17}};

  ASSERT_EQ(0, uuidcmp(&uuid1, &uuid1_dup));
  ASSERT_LT(0, uuidcmp(&uuid2, &uuid1));
  ASSERT_GT(0, uuidcmp(&uuid1, &uuid2));
}

TEST_F(UuidTest, string)
{
  const char good_str[] = "08090a0b-0C0D-0e0f-1011-121314151617";
  const char short_str[] = "08090a0b-0c0d-0e0f-1011-1213141516";
  const char bad_str[] = "This isn't a UUID";
  char str_buf[UUID_STRING_BYTES];
  LwpaUuid uuid = {{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16}};

  uuid_to_string(str_buf, &uuid);
  ASSERT_EQ(0, strcmp(str_buf, "01020304-0506-0708-090a-0b0c0d0e0f10"));
  uuid = NULL_UUID;
  uuid_to_string(str_buf, &uuid);
  ASSERT_EQ(0, strcmp(str_buf, "00000000-0000-0000-0000-000000000000"));
  ASSERT_TRUE(string_to_uuid(&uuid, good_str, strlen(good_str)));
  LwpaUuid uuid_cmp = {{8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23}};
  ASSERT_EQ(0, uuidcmp(&uuid, &uuid_cmp));
  uuid = NULL_UUID;
  ASSERT_FALSE(string_to_uuid(&uuid, short_str, strlen(short_str)));
  ASSERT_FALSE(string_to_uuid(&uuid, bad_str, strlen(bad_str)));
}

TEST_F(UuidTest, generate)
{
  uint8_t mac1[6] = {0x00, 0xc0, 0x16, 0xff, 0xef, 0x12};
  uint8_t mac2[6] = {0x00, 0xc0, 0x16, 0xff, 0xef, 0x13};
  LwpaUuid uuid1, uuid2, uuid3, uuid4, uuid1_dup, rdm1, rdm2, rdm1_dup;

  // Test the generate_uuid() function
  generate_uuid(&uuid1, "Test Device", mac1, 0);
  generate_uuid(&uuid2, "Test Device", mac1, 1);
  generate_uuid(&uuid3, "Tst Device", mac1, 0);
  generate_uuid(&uuid4, "Test Device", mac2, 0);
  generate_uuid(&uuid1_dup, "Test Device", mac1, 0);

  ASSERT_NE(0, uuidcmp(&uuid1, &uuid2));
  ASSERT_NE(0, uuidcmp(&uuid1, &uuid3));
  ASSERT_NE(0, uuidcmp(&uuid1, &uuid4));
  ASSERT_NE(0, uuidcmp(&uuid2, &uuid3));
  ASSERT_NE(0, uuidcmp(&uuid2, &uuid4));
  ASSERT_NE(0, uuidcmp(&uuid3, &uuid4));
  ASSERT_EQ(0, uuidcmp(&uuid1, &uuid1_dup));

  // Test the generate_rdm_uuid() function.
  // We'll reuse the MAC addresses as UIDs.
  generate_rdm_uuid(&rdm1, mac1);
  generate_rdm_uuid(&rdm2, mac2);
  generate_rdm_uuid(&rdm1_dup, mac1);
  ASSERT_NE(0, uuidcmp(&rdm1, &rdm2));
  ASSERT_EQ(0, uuidcmp(&rdm1, &rdm1_dup));
}
