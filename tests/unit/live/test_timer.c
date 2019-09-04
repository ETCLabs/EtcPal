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
#include "lwpa/timer.h"
#include "unity_fixture.h"

// Interdependency is unavoidable here, we need a platform-neutral sleep
#include "lwpa/common.h"
#include "lwpa/thread.h"

TEST_GROUP(lwpa_timer);

TEST_SETUP(lwpa_timer)
{
  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_init(LWPA_FEATURE_TIMERS));
}

TEST_TEAR_DOWN(lwpa_timer)
{
  lwpa_deinit(LWPA_FEATURE_TIMERS);
}

TEST(lwpa_timer, getms_gets_increasing_values)
{
  uint32_t t1, t2;
  t1 = lwpa_getms();
  lwpa_thread_sleep(10);
  t2 = lwpa_getms();

  TEST_ASSERT_NOT_EQUAL(t1, 0u);
  TEST_ASSERT_NOT_EQUAL(t2, 0u);
  TEST_ASSERT_GREATER_OR_EQUAL_INT32(0, (int32_t)t2 - (int32_t)t1);
}

TEST(lwpa_timer, timers_report_expired_properly)
{
  LwpaTimer t1, t2;

  lwpa_timer_start(&t1, 0);
  lwpa_timer_start(&t2, 100);

  // A timer with a timeout of 0 should start expired.
  TEST_ASSERT(lwpa_timer_is_expired(&t1));

  // The nonzero timeout should not be expired yet.
  TEST_ASSERT_UNLESS(lwpa_timer_is_expired(&t2));

  lwpa_thread_sleep(110);

  // Now it should.
  TEST_ASSERT(lwpa_timer_is_expired(&t2));
  TEST_ASSERT_GREATER_OR_EQUAL_UINT32(100u, lwpa_timer_elapsed(&t2));

  // Test resetting the timer.
  lwpa_timer_reset(&t2);
  TEST_ASSERT_UNLESS(lwpa_timer_is_expired(&t2));

  // And test the timeout one more time.
  lwpa_thread_sleep(110);
  TEST_ASSERT(lwpa_timer_is_expired(&t2));
  TEST_ASSERT_GREATER_OR_EQUAL_UINT32(100u, lwpa_timer_elapsed(&t2));
}

TEST_GROUP_RUNNER(lwpa_timer)
{
  RUN_TEST_CASE(lwpa_timer, getms_gets_increasing_values);
  RUN_TEST_CASE(lwpa_timer, timers_report_expired_properly);
}
