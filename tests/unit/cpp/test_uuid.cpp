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

#include "etcpal/cpp/uuid.h"
#include "unity_fixture.h"

extern "C" {
TEST_GROUP(etcpal_cpp_uuid);

TEST_SETUP(etcpal_cpp_uuid)
{
}

TEST_TEAR_DOWN(etcpal_cpp_uuid)
{
}

TEST(etcpal_cpp_uuid, default_constructor_works)
{
  etcpal::Uuid uuid;
  TEST_ASSERT_TRUE(uuid.IsNull());
}

#define UUID_INITIALIZER                                 \
  {                                                      \
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 \
  }

TEST(etcpal_cpp_uuid, copy_constructors_work)
{
  // Copy from an EtcPalUuid
  const EtcPalUuid c_uuid{UUID_INITIALIZER};
  const etcpal::Uuid uuid1(c_uuid);
  TEST_ASSERT(uuid1.get() == c_uuid);

  // Construct through an EtcPalUuid implicitly
  const etcpal::Uuid uuid2(UUID_INITIALIZER);
  TEST_ASSERT(uuid1.get() == uuid2.get());
  TEST_ASSERT(uuid1 == uuid2);

  // Use the default copy constructor
  const etcpal::Uuid uuid3(uuid2);
  TEST_ASSERT(uuid3.get() == uuid2.get());
  TEST_ASSERT(uuid3 == uuid2);
}

TEST(etcpal_cpp_uuid, assignment_operators_work)
{
  etcpal::Uuid uuid;
  EtcPalUuid c_uuid{UUID_INITIALIZER};

  // Assign to an EtcPalUuid
  uuid = c_uuid;
  TEST_ASSERT(uuid.get() == c_uuid);
  TEST_ASSERT_UNLESS(uuid.IsNull());

  // Test the default assignment operator
  const etcpal::Uuid null_uuid;
  uuid = null_uuid;
  TEST_ASSERT(uuid == null_uuid);
  TEST_ASSERT(uuid.get() == null_uuid.get());
  TEST_ASSERT(uuid.IsNull());
}

TEST(etcpal_cpp_uuid, to_string_works)
{
  const etcpal::Uuid uuid(UUID_INITIALIZER);
  TEST_ASSERT(uuid.ToString() == "00010203-0405-0607-0809-0a0b0c0d0e0f");

  const etcpal::Uuid null_uuid;
  TEST_ASSERT(null_uuid.ToString() == "00000000-0000-0000-0000-000000000000");
}

// We do more rigorous testing of the string conversion functions in the core C unit tests, so we
// will only test one bad string in this one.
TEST(etcpal_cpp_uuid, from_string_works)
{
  auto uuid = etcpal::Uuid::FromString("00010203-0405-0607-0809-0a0b0c0d0e0f");
  const etcpal::Uuid uuid_cmp(UUID_INITIALIZER);
  TEST_ASSERT(uuid == uuid_cmp);
  TEST_ASSERT_UNLESS(uuid.IsNull());

  uuid = etcpal::Uuid::FromString(std::string{"00010203-0405-0607-0809-0a0b0c0d0e0f"});
  TEST_ASSERT(uuid == uuid_cmp);
  TEST_ASSERT_UNLESS(uuid.IsNull());

  uuid = etcpal::Uuid::FromString("Bad string");
  TEST_ASSERT(uuid.IsNull());

  uuid = etcpal::Uuid::FromString(std::string{"Bad string"});
  TEST_ASSERT(uuid.IsNull());
}

TEST(etcpal_cpp_uuid, is_null_works)
{
  const etcpal::Uuid null_uuid({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0});
  TEST_ASSERT(null_uuid.IsNull());

  const etcpal::Uuid not_null_uuid({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1});
  TEST_ASSERT_UNLESS(not_null_uuid.IsNull());
}

// The pure C tests do a more rigorous test of this functionality. We will just generate one UUID
// and check it for each of these.

TEST(etcpal_cpp_uuid, generates_v1_correctly)
{
  // Only run this test if generate_v1_uuid() is implemented on this platform.
  EtcPalUuid test_uuid;
  if (etcpal_generate_v1_uuid(&test_uuid) == kEtcPalErrNotImpl)
    TEST_IGNORE_MESSAGE("etcpal::Uuid::V1() is not implemented on this platform.");

  const etcpal::Uuid v1 = etcpal::Uuid::V1();
  TEST_ASSERT_UNLESS(v1.IsNull());

  // Make sure the variant and version fields are correct. We should always have Variant 1, Version
  // 1.
  TEST_ASSERT_EQUAL_UINT8((v1.get().data[6] & 0xf0u), 0x10u);
  TEST_ASSERT_EQUAL_UINT8((v1.get().data[8] & 0xc0u), 0x80u);
}

TEST(etcpal_cpp_uuid, generates_v3_correctly)
{
  // TODO
}

TEST(etcpal_cpp_uuid, generates_v4_correctly)
{
  // Only run this test if generate_v4_uuid() is implemented on this platform.
  EtcPalUuid test_uuid;
  if (etcpal_generate_v4_uuid(&test_uuid) == kEtcPalErrNotImpl)
    TEST_IGNORE_MESSAGE("etcpal::Uuid::V4() is not implemented on this platform.");

  const etcpal::Uuid v4 = etcpal::Uuid::V4();
  TEST_ASSERT_UNLESS(v4.IsNull());

  // Make sure the variant and version fields are correct. We should always have Variant 1, Version
  // 4.
  TEST_ASSERT_EQUAL_UINT8((v4.get().data[6] & 0xf0u), 0x40u);
  TEST_ASSERT_EQUAL_UINT8((v4.get().data[8] & 0xc0u), 0x80u);
  TEST_ASSERT_EQUAL(v4.version(), etcpal::UuidVersion::V4);
}

TEST(etcpal_cpp_uuid, generates_v5_correctly)
{
  // TODO
}

TEST(etcpal_cpp_uuid, generates_os_preferred_correctly)
{
  // Only run this test if generate_os_preferred_uuid() is implemented on this platform.
  EtcPalUuid test_uuid;
  if (etcpal_generate_os_preferred_uuid(&test_uuid) == kEtcPalErrNotImpl)
    TEST_IGNORE_MESSAGE("etcpal::Uuid::OsPreferred() is not implemented on this platform.");

  const etcpal::Uuid os_preferred = etcpal::Uuid::OsPreferred();
  // There is really not much we can test here besides that it's not a null UUID
  TEST_ASSERT_UNLESS(os_preferred.IsNull());
}

TEST(etcpal_cpp_uuid, generates_device_correctly)
{
  const etcpal::MacAddr mac({0x00, 0xc0, 0x16, 0x01, 0x02, 0x03});

  const etcpal::Uuid dev_1 = etcpal::Uuid::Device("Test Device", mac, 20);
  const etcpal::Uuid dev_1_dup = etcpal::Uuid::Device("Test Device", mac, 20);
  const etcpal::Uuid dev_2 = etcpal::Uuid::Device("Test Device", mac, 21);

  TEST_ASSERT_UNLESS(dev_1.IsNull());
  TEST_ASSERT_UNLESS(dev_1_dup.IsNull());
  TEST_ASSERT_UNLESS(dev_2.IsNull());

  TEST_ASSERT(dev_1 == dev_1_dup);
  TEST_ASSERT(dev_1 != dev_2);
  TEST_ASSERT(dev_1_dup != dev_2);
}

TEST(etcpal_cpp_uuid, equality_operators_work)
{
  const etcpal::Uuid uuid_1({20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35});
  const etcpal::Uuid uuid_1_dup(uuid_1);
  const etcpal::Uuid uuid_2({0, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35});

  TEST_ASSERT(uuid_1 == uuid_1_dup);
  TEST_ASSERT(uuid_1_dup == uuid_1);
  TEST_ASSERT_UNLESS(uuid_1 != uuid_1_dup);
  TEST_ASSERT_UNLESS(uuid_1_dup != uuid_1);
  TEST_ASSERT(uuid_1 != uuid_2);
  TEST_ASSERT(uuid_2 != uuid_1);
  TEST_ASSERT_UNLESS(uuid_1 == uuid_2);
  TEST_ASSERT_UNLESS(uuid_2 == uuid_1);
}

TEST(etcpal_cpp_uuid, comparison_operators_work)
{
  // uuid_2 should be greater than uuid_1
  const etcpal::Uuid uuid_1({20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35});
  const etcpal::Uuid uuid_1_dup(uuid_1);
  const etcpal::Uuid uuid_2({20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 36});

  TEST_ASSERT_UNLESS(uuid_1 < uuid_1_dup);
  TEST_ASSERT_UNLESS(uuid_1 > uuid_1_dup);
  TEST_ASSERT(uuid_1 <= uuid_1_dup);
  TEST_ASSERT(uuid_1 >= uuid_1_dup);

  TEST_ASSERT(uuid_1 < uuid_2);
  TEST_ASSERT(uuid_2 > uuid_1);
  TEST_ASSERT_UNLESS(uuid_1 > uuid_2);
  TEST_ASSERT_UNLESS(uuid_2 < uuid_1);

  TEST_ASSERT(uuid_1 <= uuid_2);
  TEST_ASSERT(uuid_2 >= uuid_1);
  TEST_ASSERT_UNLESS(uuid_1 >= uuid_2);
  TEST_ASSERT_UNLESS(uuid_2 <= uuid_1);
}

// Test the operators that compare the C type EtcPalUuid with the C++ type etcpal::Uuid
TEST(etcpal_cpp_uuid, special_equality_operators_work)
{
  const etcpal::Uuid uuid_1({20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35});
  const EtcPalUuid c_uuid_1_dup = {20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35};
  const EtcPalUuid c_uuid_2 = {0, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35};

  TEST_ASSERT(uuid_1 == c_uuid_1_dup);
  TEST_ASSERT(c_uuid_1_dup == uuid_1);
  TEST_ASSERT_UNLESS(uuid_1 != c_uuid_1_dup);
  TEST_ASSERT_UNLESS(c_uuid_1_dup != uuid_1);
  TEST_ASSERT(uuid_1 != c_uuid_2);
  TEST_ASSERT(c_uuid_2 != uuid_1);
  TEST_ASSERT_UNLESS(uuid_1 == c_uuid_2);
  TEST_ASSERT_UNLESS(c_uuid_2 == uuid_1);
}

TEST_GROUP_RUNNER(etcpal_cpp_uuid)
{
  RUN_TEST_CASE(etcpal_cpp_uuid, default_constructor_works);
  RUN_TEST_CASE(etcpal_cpp_uuid, copy_constructors_work);
  RUN_TEST_CASE(etcpal_cpp_uuid, assignment_operators_work);
  RUN_TEST_CASE(etcpal_cpp_uuid, to_string_works);
  RUN_TEST_CASE(etcpal_cpp_uuid, from_string_works);
  RUN_TEST_CASE(etcpal_cpp_uuid, is_null_works);
  RUN_TEST_CASE(etcpal_cpp_uuid, generates_v1_correctly);
  RUN_TEST_CASE(etcpal_cpp_uuid, generates_v3_correctly);
  RUN_TEST_CASE(etcpal_cpp_uuid, generates_v4_correctly);
  RUN_TEST_CASE(etcpal_cpp_uuid, generates_v5_correctly);
  RUN_TEST_CASE(etcpal_cpp_uuid, generates_os_preferred_correctly);
  RUN_TEST_CASE(etcpal_cpp_uuid, generates_device_correctly);
  RUN_TEST_CASE(etcpal_cpp_uuid, equality_operators_work);
  RUN_TEST_CASE(etcpal_cpp_uuid, comparison_operators_work);
  RUN_TEST_CASE(etcpal_cpp_uuid, special_equality_operators_work);
}
}
