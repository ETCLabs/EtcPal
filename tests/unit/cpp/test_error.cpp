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

#include <type_traits>
#include "etcpal/cpp/error.h"
#include "etcpal/common.h"
#include "unity_fixture.h"

TEST_GROUP(etcpal_result);

TEST_SETUP(etcpal_result)
{
}

TEST_TEAR_DOWN(etcpal_result)
{
}

TEST(etcpal_result, constructors_function_correctly)
{
  // Explicit constructor from "OK" error code
  etcpal::Result exp_ok(kEtcPalErrOk);
  TEST_ASSERT_TRUE(exp_ok);
  TEST_ASSERT_TRUE(exp_ok.IsOk());
  TEST_ASSERT_EQUAL(exp_ok.code(), kEtcPalErrOk);
  TEST_ASSERT_NOT_NULL(exp_ok.ToCString());
  TEST_ASSERT_FALSE(exp_ok.ToString().empty());

  // Explicit constructor from non-"OK" error code
  etcpal::Result exp_not_ok(kEtcPalErrSys);
  TEST_ASSERT_FALSE(exp_not_ok);
  TEST_ASSERT_FALSE(exp_not_ok.IsOk());
  TEST_ASSERT_EQUAL(exp_not_ok.code(), kEtcPalErrSys);
  TEST_ASSERT_NOT_NULL(exp_not_ok.ToCString());
  TEST_ASSERT_FALSE(exp_not_ok.ToString().empty());
}

TEST(etcpal_result, static_ok_works)
{
  auto result = etcpal::Result::Ok();
  TEST_ASSERT_TRUE(result);
  TEST_ASSERT_EQUAL(result.code(), kEtcPalErrOk);

  etcpal::Result result_2(kEtcPalErrOk);
  TEST_ASSERT(result_2 == result);
}

TEST(etcpal_result, assignment_operators)
{
  etcpal::Result result = kEtcPalErrOk;
  TEST_ASSERT_TRUE(result);
  TEST_ASSERT_EQUAL(result.code(), kEtcPalErrOk);

  result = kEtcPalErrSys;
  TEST_ASSERT_FALSE(result);
  TEST_ASSERT_EQUAL(result.code(), kEtcPalErrSys);

  auto result_2 = result;
  TEST_ASSERT_FALSE(result_2);
  TEST_ASSERT_EQUAL(result.code(), result_2.code());
  TEST_ASSERT_EQUAL_STRING(result.ToCString(), result_2.ToCString());
  TEST_ASSERT(result.ToString() == result_2.ToString());
}

TEST(etcpal_result, equality_operators)
{
  etcpal::Result result(kEtcPalErrOk);
  TEST_ASSERT(result == kEtcPalErrOk);
  TEST_ASSERT(kEtcPalErrOk == result);

  etcpal::Result result_2(kEtcPalErrOk);
  TEST_ASSERT(result_2 == result);
  TEST_ASSERT(result == result_2);

  etcpal::Result result_3(kEtcPalErrSys);
  TEST_ASSERT_UNLESS(result_3 == result);
  TEST_ASSERT_UNLESS(result == result_3);
}

TEST(etcpal_result, strings_exist_for_all_codes)
{
  for (int i = 0; i > 0 - ETCPAL_NUM_ERROR_CODES; --i)
  {
    etcpal::Result result(static_cast<etcpal_error_t>(i));
    TEST_ASSERT_UNLESS_MESSAGE(result.ToString().empty(),
                               std::string("Failed on iteration " + std::to_string(i)).c_str());
    TEST_ASSERT_NOT_NULL_MESSAGE(result.ToCString(), std::string("Failed on iteration " + std::to_string(i)).c_str());
  }
}

TEST_GROUP(etcpal_expected);

TEST_SETUP(etcpal_expected)
{
}

TEST_TEAR_DOWN(etcpal_expected)
{
}

class NoDefaultConstructor
{
public:
  NoDefaultConstructor() = delete;
  NoDefaultConstructor(int val) : val_(val) {}

private:
  int val_;
};

static_assert(!std::is_default_constructible<etcpal::Expected<NoDefaultConstructor>>::value,
              "Expected should not be default-constructible with a non-default-constructible class");
static_assert(std::is_default_constructible<etcpal::Expected<int>>::value,
              "Expected should be default-constructible with a default-constructible class");

TEST(etcpal_expected, default_constructor)
{
  etcpal::Expected<int> e;
  TEST_ASSERT_EQUAL_INT(e.value(), 0);
}

TEST(etcpal_expected, from_error)
{
  etcpal::Expected<int> e = kEtcPalErrSys;
  try
  {
    int val = e.value();
    TEST_FAIL_MESSAGE("This block should not be entered");
  }
  catch (etcpal::BadExpectedAccess e)
  {
    TEST_ASSERT_EQUAL_INT(e.code(), kEtcPalErrSys);
  }
}

TEST_GROUP_RUNNER(etcpal_cpp_error)
{
  RUN_TEST_CASE(etcpal_result, constructors_function_correctly);
  RUN_TEST_CASE(etcpal_result, static_ok_works);
  RUN_TEST_CASE(etcpal_result, assignment_operators);
  RUN_TEST_CASE(etcpal_result, equality_operators);
  RUN_TEST_CASE(etcpal_result, strings_exist_for_all_codes);
  RUN_TEST_CASE(etcpal_expected, default_constructor);
  RUN_TEST_CASE(etcpal_expected, from_error);
}
