/******************************************************************************
 * Copyright 2020 ETC Inc.
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

#include "etcpal/event.h"

#include "etcpal/common.h"
#include "etcpal/timer.h"
#include "unity_fixture.h"

static etcpal_event_t event;

TEST_GROUP(etcpal_event);

TEST_SETUP(etcpal_event)
{
  TEST_ASSERT_TRUE(etcpal_event_create(&event));
}

TEST_TEAR_DOWN(etcpal_event)
{
  etcpal_event_destroy(&event);
}

TEST(etcpal_event, wait_and_set_works)
{
  etcpal_event_set_bits(&event, 0x2);
  TEST_ASSERT_EQUAL(etcpal_event_wait(&event, 0x2, 0), 0x2);
}

TEST(etcpal_event, wait_returns_immediately_if_bits_not_clear)
{
  etcpal_event_set_bits(&event, 0x2);
  TEST_ASSERT_EQUAL(etcpal_event_wait(&event, 0x2, 0), 0x2);
  TEST_ASSERT_EQUAL(etcpal_event_wait(&event, 0x2, 0), 0x2);
}

TEST(etcpal_event, timed_wait_works)
{
#if ETCPAL_EVENT_HAS_TIMED_WAIT
  EtcPalTimer timer;
  etcpal_timer_start(&timer, 100);

  TEST_ASSERT_EQUAL(etcpal_event_timed_wait(&event, 0x1, 0, 10), 0);

  // An unfortunately necessary heuristic - we assert that at least half the specified time has
  // gone by, to account for OS slop.
  TEST_ASSERT_GREATER_THAN_UINT32(5, etcpal_timer_elapsed(&timer));

  etcpal_event_set_bits(&event, 0x1);
  TEST_ASSERT_EQUAL(etcpal_event_timed_wait(&event, 0x1, 0, 10), 0x1);
#else
  // On this platform, timed_wait() return immediately if timeout_ms is 0, and behave like wait()
  // if timeout_ms != 0.
  TEST_ASSERT_EQUAL(etcpal_event_timed_wait(&event, 0x1, 0, 0), 0);
  etcpal_event_set_bits(&event, 0x1);
  TEST_ASSERT_EQUAL(etcpal_event_timed_wait(&event, 0x1, 0, 10), 0x1);
#endif
}

TEST(etcpal_event, wait_for_all_works)
{
  etcpal_event_set_bits(&event, 0x1);
  TEST_ASSERT_EQUAL(etcpal_event_timed_wait(&event, 0x3, ETCPAL_EVENT_WAIT_FOR_ALL, 0), 0);
  etcpal_event_set_bits(&event, 0x2);
  TEST_ASSERT_EQUAL(etcpal_event_timed_wait(&event, 0x3, ETCPAL_EVENT_WAIT_FOR_ALL, 0), 0x3);
  // The bits should not have auto-cleared without passing the flag
  TEST_ASSERT_EQUAL(etcpal_event_get_bits(&event), 0x3);
}

TEST(etcpal_event, clear_on_exit_works)
{
  etcpal_event_set_bits(&event, 0x3);
  TEST_ASSERT_EQUAL(etcpal_event_wait(&event, 0x3, 0), 0x3);
  // The bits should not have auto-cleared without passing the flag
  TEST_ASSERT_EQUAL(etcpal_event_get_bits(&event), 0x3);
  TEST_ASSERT_EQUAL(etcpal_event_wait(&event, 0x3, ETCPAL_EVENT_CLEAR_ON_EXIT), 0x3);
  // Now they should be cleared
  TEST_ASSERT_EQUAL(etcpal_event_get_bits(&event), 0x0);
}

TEST(etcpal_event, get_bits_works)
{
  etcpal_event_set_bits(&event, 0x3);
  TEST_ASSERT_EQUAL(etcpal_event_get_bits(&event), 0x3);
}

TEST(etcpal_event, clear_bits_works)
{
  etcpal_event_set_bits(&event, 0x3);
  etcpal_event_clear_bits(&event, 0x1);
  TEST_ASSERT_EQUAL(etcpal_event_get_bits(&event), 0x2);
}

TEST_GROUP_RUNNER(etcpal_event)
{
  etcpal_init(ETCPAL_FEATURE_TIMERS);
  RUN_TEST_CASE(etcpal_event, wait_and_set_works);
  RUN_TEST_CASE(etcpal_event, wait_returns_immediately_if_bits_not_clear);
  RUN_TEST_CASE(etcpal_event, timed_wait_works);
  RUN_TEST_CASE(etcpal_event, wait_for_all_works);
  RUN_TEST_CASE(etcpal_event, clear_on_exit_works);
  RUN_TEST_CASE(etcpal_event, get_bits_works);
  RUN_TEST_CASE(etcpal_event, clear_bits_works);
  etcpal_deinit(ETCPAL_FEATURE_TIMERS);
}
