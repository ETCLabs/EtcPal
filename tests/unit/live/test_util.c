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

#include "etcpal/private/util.h"
#include "unity_fixture.h"

TEST_GROUP(etcpal_util);

TEST_SETUP(etcpal_util)
{
}

TEST_TEAR_DOWN(etcpal_util)
{
}

TEST(etcpal_util, alignment_padding_macro_works)
{
  // Test a few odd-numbered offsets and make sure proper padding is produced
  void* addr = (void*)0x80000001ull;

  size_t padding = GET_PADDING_FOR_ALIGNMENT(addr, unsigned long);
  TEST_ASSERT_NOT_EQUAL(padding, 0);
  TEST_ASSERT_EQUAL(((uintptr_t)addr + padding) % ETCPAL_ALIGNOF(unsigned long), 0);

  addr = (void*)0x80000003ull;

  padding = GET_PADDING_FOR_ALIGNMENT(addr, unsigned long);
  TEST_ASSERT_NOT_EQUAL(padding, 0);
  TEST_ASSERT_EQUAL(((uintptr_t)addr + padding) % ETCPAL_ALIGNOF(unsigned long), 0);

  // Make sure no padding is produced for aligned address
  addr    = (void*)0x80000000ull;
  padding = GET_PADDING_FOR_ALIGNMENT(addr, unsigned long);
  TEST_ASSERT_EQUAL(padding, 0);
}

TEST_GROUP_RUNNER(etcpal_util)
{
  RUN_TEST_CASE(etcpal_util, alignment_padding_macro_works);
}
