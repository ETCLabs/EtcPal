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

#include "etcpal/timer.h"
#include "unity_fixture.h"
#include "fff.h"

DEFINE_FFF_GLOBALS;

TEST_GROUP(etcpal_timer_wraparound);

TEST_SETUP(etcpal_timer_wraparound)
{
}

TEST_TEAR_DOWN(etcpal_timer_wraparound)
{
}

FAKE_VALUE_FUNC(uint32_t, etcpal_getms);

TEST(etcpal_timer_wraparound, wraparound_works_as_expected)
{
  LwpaTimer t1;

  // Test the wraparound case by forcing a wraparound value returned from etcpal_getms()
  etcpal_getms_fake.return_val = 0xfffffff0u;
  etcpal_timer_start(&t1, 0x20);

  // We've wrapped around but have not exceeded the interval yet
  etcpal_getms_fake.return_val = 0x0f;
  TEST_ASSERT_FALSE(etcpal_timer_is_expired(&t1));
  TEST_ASSERT_EQUAL_UINT32(etcpal_timer_elapsed(&t1), 0x1fu);

  etcpal_getms_fake.return_val = 0x11;
  TEST_ASSERT_TRUE(etcpal_timer_is_expired(&t1));
}

TEST_GROUP_RUNNER(etcpal_timer_wraparound)
{
  RUN_TEST_CASE(etcpal_timer_wraparound, wraparound_works_as_expected);
}

void run_all_tests(void)
{
  RUN_TEST_GROUP(etcpal_timer_wraparound);
}
