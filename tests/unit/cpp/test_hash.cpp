/******************************************************************************
 * Copyright 2022 ETC Inc.
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

#include "etcpal/cpp/hash.h"

#include <functional>
#include <string>

extern "C" {
#include "unity_fixture.h"

TEST_GROUP(etcpal_cpp_hash);

TEST_SETUP(etcpal_cpp_hash)
{
}

TEST_TEAR_DOWN(etcpal_cpp_hash)
{
}

TEST(etcpal_cpp_hash, hash_combine_works)
{
  int         val1 = 1234;
  std::string val2("5678");

  size_t seed = std::hash<int>()(val1);
  etcpal::HashCombine(seed, val2);

  TEST_ASSERT_NOT_EQUAL(seed, 0u);
  TEST_ASSERT_NOT_EQUAL(seed, std::hash<int>()(val1));
  TEST_ASSERT_NOT_EQUAL(seed, std::hash<std::string>()(val2));
}

TEST_GROUP_RUNNER(etcpal_cpp_hash)
{
  RUN_TEST_CASE(etcpal_cpp_hash, hash_combine_works);
}
}
