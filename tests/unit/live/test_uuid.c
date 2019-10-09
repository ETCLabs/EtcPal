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

#include "etcpal/uuid.h"
#include "unity_fixture.h"

#include <stddef.h>
#include <stdio.h>

// Disable sprintf() warning on Windows/MSVC
#ifdef _MSC_VER
#pragma warning(disable : 4996)
#endif

#define NUM_V1_UUID_GENERATIONS 10000
#define NUM_V4_UUID_GENERATIONS 10000

TEST_GROUP(etcpal_uuid);

TEST_SETUP(etcpal_uuid)
{
}

TEST_TEAR_DOWN(etcpal_uuid)
{
}

TEST(etcpal_uuid, invalid_calls_fail)
{
  EtcPalUuid uuid;

  TEST_ASSERT_UNLESS(etcpal_string_to_uuid("cbeaf15d-8197-4ace-903a-8e943c388fc6", NULL));
  TEST_ASSERT_UNLESS(etcpal_string_to_uuid(NULL, &uuid));
  TEST_ASSERT_UNLESS(etcpal_string_to_uuid(NULL, NULL));

  char str_buf[ETCPAL_UUID_STRING_BYTES];
  TEST_ASSERT_UNLESS(etcpal_uuid_to_string(&uuid, NULL));
  TEST_ASSERT_UNLESS(etcpal_uuid_to_string(NULL, str_buf));
  TEST_ASSERT_UNLESS(etcpal_uuid_to_string(NULL, NULL));

  TEST_ASSERT_NOT_EQUAL(kEtcPalErrOk, etcpal_generate_v1_uuid(NULL));
  TEST_ASSERT_NOT_EQUAL(kEtcPalErrOk, etcpal_generate_v4_uuid(NULL));
  TEST_ASSERT_NOT_EQUAL(kEtcPalErrOk, etcpal_generate_os_preferred_uuid(NULL));

  const uint8_t mac[6] = {0, 1, 2, 3, 4, 5};
  TEST_ASSERT_NOT_EQUAL(kEtcPalErrOk, etcpal_generate_v3_uuid("Test Device", mac, 0, NULL));
  TEST_ASSERT_NOT_EQUAL(kEtcPalErrOk, etcpal_generate_v3_uuid("Test Device", NULL, 0, &uuid));
  TEST_ASSERT_NOT_EQUAL(kEtcPalErrOk, etcpal_generate_v3_uuid(NULL, mac, 0, &uuid));
  TEST_ASSERT_NOT_EQUAL(kEtcPalErrOk, etcpal_generate_v3_uuid(NULL, NULL, 0, NULL));
}

TEST(etcpal_uuid, uuid_is_null_works)
{
  EtcPalUuid uuid = {{0}};
  TEST_ASSERT(ETCPAL_UUID_IS_NULL(&uuid));
  uuid = kEtcPalNullUuid;
  TEST_ASSERT(ETCPAL_UUID_IS_NULL(&uuid));
  for (uint8_t i = 0; i < ETCPAL_UUID_BYTES; ++i)
    uuid.data[i] = i;
  TEST_ASSERT_UNLESS(ETCPAL_UUID_IS_NULL(&uuid));
}

TEST(etcpal_uuid, uuid_compare_works)
{
  const EtcPalUuid uuid1 = {{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16}};
  const EtcPalUuid uuid1_dup = {{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16}};
  const EtcPalUuid uuid2 = {{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 17}};

  TEST_ASSERT_EQUAL(0, ETCPAL_UUID_CMP(&uuid1, &uuid1_dup));
  TEST_ASSERT_GREATER_THAN(0, ETCPAL_UUID_CMP(&uuid2, &uuid1));
  TEST_ASSERT_LESS_THAN(0, ETCPAL_UUID_CMP(&uuid1, &uuid2));
}

TEST(etcpal_uuid, uuid_to_string_conversion_works)
{
  char str_buf[ETCPAL_UUID_STRING_BYTES];
  EtcPalUuid uuid = {{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16}};

  etcpal_uuid_to_string(&uuid, str_buf);
  TEST_ASSERT_EQUAL_STRING(str_buf, "01020304-0506-0708-090a-0b0c0d0e0f10");

  uuid = kEtcPalNullUuid;
  etcpal_uuid_to_string(&uuid, str_buf);
  TEST_ASSERT_EQUAL_STRING(str_buf, "00000000-0000-0000-0000-000000000000");
}

TEST(etcpal_uuid, string_to_uuid_conversion_works)
{
  EtcPalUuid uuid;
  const char* good_str = "08090a0b-0C0D-0e0f-1011-121314151617";
  const EtcPalUuid good_str_uuid = {{8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23}};
  // clang-format off
  const char* bad_strings[] = {
    "08090a0b-0c0d-0e0f-1011-1213141516", // Short by one character
    "08090a0b-0c0d-0e0f-1011-12131415161g", // Bad character at the end
    "g8090a0b-0c0d-0e0f-1011-121314151617", // Bad character at the beginning
    "This isn't a UUID", // Short random string
    "This isn't a UUID but it is a very long string" // Long random string
  };
  // clang-format on

  TEST_ASSERT(etcpal_string_to_uuid(good_str, &uuid));
  TEST_ASSERT_EQUAL(0, ETCPAL_UUID_CMP(&uuid, &good_str_uuid));

  uuid = kEtcPalNullUuid;

  for (size_t i = 0; i < (sizeof(bad_strings) / sizeof(const char*)); ++i)
  {
    char msg_buf[100];
    sprintf(msg_buf, "Failed on input: %s", bad_strings[i]);
    TEST_ASSERT_UNLESS_MESSAGE(etcpal_string_to_uuid(bad_strings[i], &uuid), msg_buf);
  }
}

TEST(etcpal_uuid, generates_correct_v1_uuids)
{
  // Generate a bunch of V1 UUIDs. They should all be unique from each other and have the proper
  // version and variant information. We will cheat a little and just make sure that each one is
  // unique from the last one generated.
  EtcPalUuid last_uuid = kEtcPalNullUuid;

  for (int i = 0; i < NUM_V1_UUID_GENERATIONS; ++i)
  {
    EtcPalUuid uuid;
    char error_msg[100];
    sprintf(error_msg, "This failure occurred on UUID attempt %d of %d", i + 1, NUM_V1_UUID_GENERATIONS);

    etcpal_error_t generate_result = etcpal_generate_v1_uuid(&uuid);

    // Special case - this function isn't implemented on all platforms, so we abort this test
    // prematurely if that's the case.
    if (generate_result == kEtcPalErrNotImpl)
    {
      TEST_PASS_MESSAGE(
          "etcpal_generate_v1_uuid() not implemented on this platform. Skipping the remainder of the test.");
    }

    TEST_ASSERT_EQUAL_MESSAGE(kEtcPalErrOk, generate_result, error_msg);

    // We should always have Variant 1, Version 1.
    TEST_ASSERT_EQUAL_UINT8_MESSAGE((uuid.data[6] & 0xf0u), 0x10u, error_msg);
    TEST_ASSERT_EQUAL_UINT8_MESSAGE((uuid.data[8] & 0xc0u), 0x80u, error_msg);

    // Should be unique from the last one generated.
    TEST_ASSERT_NOT_EQUAL_MESSAGE(0, ETCPAL_UUID_CMP(&uuid, &last_uuid), error_msg);

    last_uuid = uuid;
  }
}

TEST(etcpal_uuid, generates_correct_v3_uuids)
{
  const uint8_t mac1[6] = {0x00, 0xc0, 0x16, 0xff, 0xef, 0x12};
  const uint8_t mac2[6] = {0x00, 0xc0, 0x16, 0xff, 0xef, 0x13};
  EtcPalUuid uuid1, uuid2, uuid3, uuid4, uuid1_dup;

  // Version 3 UUIDs should be deterministic for the same combination of the three possible input
  // arguments. If any of the arguments is different, a different UUID should result.
  TEST_ASSERT_EQUAL(kEtcPalErrOk, etcpal_generate_v3_uuid("Test Device", mac1, 0, &uuid1));
  TEST_ASSERT_EQUAL(kEtcPalErrOk, etcpal_generate_v3_uuid("Test Device", mac1, 1, &uuid2));
  TEST_ASSERT_EQUAL(kEtcPalErrOk, etcpal_generate_v3_uuid("Tst Device", mac1, 0, &uuid3));
  TEST_ASSERT_EQUAL(kEtcPalErrOk, etcpal_generate_v3_uuid("Test Device", mac2, 0, &uuid4));
  TEST_ASSERT_EQUAL(kEtcPalErrOk, etcpal_generate_v3_uuid("Test Device", mac1, 0, &uuid1_dup));

  TEST_ASSERT_NOT_EQUAL(0, ETCPAL_UUID_CMP(&uuid1, &uuid2));
  TEST_ASSERT_NOT_EQUAL(0, ETCPAL_UUID_CMP(&uuid1, &uuid3));
  TEST_ASSERT_NOT_EQUAL(0, ETCPAL_UUID_CMP(&uuid1, &uuid4));
  TEST_ASSERT_NOT_EQUAL(0, ETCPAL_UUID_CMP(&uuid2, &uuid3));
  TEST_ASSERT_NOT_EQUAL(0, ETCPAL_UUID_CMP(&uuid2, &uuid4));
  TEST_ASSERT_NOT_EQUAL(0, ETCPAL_UUID_CMP(&uuid3, &uuid4));
  TEST_ASSERT_EQUAL(0, ETCPAL_UUID_CMP(&uuid1, &uuid1_dup));

  // Make sure the Variant Version bits are correct.
  // We should always have Variant 1, Version 3.
  TEST_ASSERT_EQUAL_UINT8((uuid1.data[6] & 0xf0u), 0x30u);
  TEST_ASSERT_EQUAL_UINT8((uuid1.data[8] & 0xc0u), 0x80u);
  TEST_ASSERT_EQUAL_UINT8((uuid2.data[6] & 0xf0u), 0x30u);
  TEST_ASSERT_EQUAL_UINT8((uuid2.data[8] & 0xc0u), 0x80u);
  TEST_ASSERT_EQUAL_UINT8((uuid3.data[6] & 0xf0u), 0x30u);
  TEST_ASSERT_EQUAL_UINT8((uuid3.data[8] & 0xc0u), 0x80u);
  TEST_ASSERT_EQUAL_UINT8((uuid4.data[6] & 0xf0u), 0x30u);
  TEST_ASSERT_EQUAL_UINT8((uuid4.data[8] & 0xc0u), 0x80u);
  TEST_ASSERT_EQUAL_UINT8((uuid1_dup.data[6] & 0xf0u), 0x30u);
  TEST_ASSERT_EQUAL_UINT8((uuid1_dup.data[8] & 0xc0u), 0x80u);
}

TEST(etcpal_uuid, generates_correct_v4_uuids)
{
  // Generate a bunch of V4 UUIDs. They should all be unique from each other and have the proper
  // version and variant information. We will cheat a little and just make sure that each one is
  // unique from the last one generated.
  EtcPalUuid last_uuid = kEtcPalNullUuid;

  for (int i = 0; i < NUM_V4_UUID_GENERATIONS; ++i)
  {
    EtcPalUuid uuid;
    char error_msg[100];
    sprintf(error_msg, "This failure occurred on UUID attempt %d of %d", i + 1, NUM_V4_UUID_GENERATIONS);

    etcpal_error_t generate_result = etcpal_generate_v4_uuid(&uuid);

    // Special case - this function isn't implemented on all platforms, so we abort this test
    // prematurely if that's the case.
    if (generate_result == kEtcPalErrNotImpl)
    {
      TEST_PASS_MESSAGE(
          "etcpal_generate_v4_uuid() not implemented on this platform. Skipping the remainder of the test.");
    }

    TEST_ASSERT_EQUAL_MESSAGE(kEtcPalErrOk, generate_result, error_msg);

    // We should always have Variant 1, Version 4.
    TEST_ASSERT_EQUAL_MESSAGE((uuid.data[6] & 0xf0u), 0x40u, error_msg);
    TEST_ASSERT_EQUAL_MESSAGE((uuid.data[8] & 0xc0u), 0x80u, error_msg);

    // Should be unique from the last one generated.
    TEST_ASSERT_NOT_EQUAL_MESSAGE(0, ETCPAL_UUID_CMP(&uuid, &last_uuid), error_msg);

    last_uuid = uuid;
  }
}

TEST_GROUP_RUNNER(etcpal_uuid)
{
  RUN_TEST_CASE(etcpal_uuid, invalid_calls_fail);
  RUN_TEST_CASE(etcpal_uuid, uuid_is_null_works);
  RUN_TEST_CASE(etcpal_uuid, uuid_compare_works);
  RUN_TEST_CASE(etcpal_uuid, uuid_to_string_conversion_works);
  RUN_TEST_CASE(etcpal_uuid, string_to_uuid_conversion_works);
  RUN_TEST_CASE(etcpal_uuid, generates_correct_v1_uuids);
  RUN_TEST_CASE(etcpal_uuid, generates_correct_v3_uuids);
  RUN_TEST_CASE(etcpal_uuid, generates_correct_v4_uuids);
}
