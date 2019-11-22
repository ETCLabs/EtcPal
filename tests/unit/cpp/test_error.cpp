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

#include "etcpal/cpp/error.h"
#include "etcpal/common.h"
#include "unity_fixture.h"

extern "C" {

TEST_GROUP(etcpal_cpp_error);

TEST_SETUP(etcpal_cpp_error)
{
}

TEST_TEAR_DOWN(etcpal_cpp_error)
{
}

TEST(etcpal_cpp_error, constructors_function_correctly)
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

TEST(etcpal_cpp_error, static_ok_works)
{
  auto result = etcpal::Result::Ok();
  TEST_ASSERT_TRUE(result);
  TEST_ASSERT_EQUAL(result.code(), kEtcPalErrOk);

  etcpal::Result result_2(kEtcPalErrOk);
  TEST_ASSERT(result_2 == result);
}

TEST(etcpal_cpp_error, assignment_operators)
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

TEST(etcpal_cpp_error, equality_operators)
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

TEST(etcpal_cpp_error, strings_exist_for_all_codes)
{
  for (int i = 0; i > 0 - ETCPAL_NUM_ERROR_CODES; --i)
  {
    etcpal::Result result(static_cast<etcpal_error_t>(i));
    TEST_ASSERT_UNLESS_MESSAGE(result.ToString().empty(),
                               std::string("Failed on iteration " + std::to_string(i)).c_str());
    TEST_ASSERT_NOT_NULL_MESSAGE(result.ToCString(), std::string("Failed on iteration " + std::to_string(i)).c_str());
  }
}

TEST_GROUP_RUNNER(etcpal_cpp_error)
{
  RUN_TEST_CASE(etcpal_cpp_error, constructors_function_correctly);
  RUN_TEST_CASE(etcpal_cpp_error, static_ok_works);
  RUN_TEST_CASE(etcpal_cpp_error, assignment_operators);
  RUN_TEST_CASE(etcpal_cpp_error, equality_operators);
  RUN_TEST_CASE(etcpal_cpp_error, strings_exist_for_all_codes);
}
}
