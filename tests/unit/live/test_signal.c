/******************************************************************************
 * Copyright 2021 ETC Inc.
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

#include "etcpal/signal.h"

#include "etcpal/common.h"
#include "etcpal/timer.h"
#include "unity_fixture.h"

TEST_GROUP(etcpal_signal);

TEST_SETUP(etcpal_signal)
{
}

TEST_TEAR_DOWN(etcpal_signal)
{
}

TEST(etcpal_signal, create_and_destroy_works)
{
  etcpal_signal_t signal;

  // Basic creation.
  TEST_ASSERT_TRUE(etcpal_signal_create(&signal));

  etcpal_signal_post(&signal);
  TEST_ASSERT_TRUE(etcpal_signal_wait(&signal));
  etcpal_signal_post(&signal);
  etcpal_signal_destroy(&signal);
}

TEST(etcpal_signal, try_wait_works)
{
  etcpal_signal_t signal;
  TEST_ASSERT_TRUE(etcpal_signal_create(&signal));

  // Signals shouldn't be created in the signaled state.
  TEST_ASSERT_FALSE(etcpal_signal_try_wait(&signal));

  etcpal_signal_post(&signal);
  TEST_ASSERT_TRUE(etcpal_signal_try_wait(&signal));

  etcpal_signal_destroy(&signal);
}

TEST(etcpal_signal, timed_wait_works)
{
  etcpal_signal_t signal;
  TEST_ASSERT_TRUE(etcpal_signal_create(&signal));

#if ETCPAL_SIGNAL_HAS_TIMED_WAIT
  EtcPalTimer timer;
  etcpal_timer_start(&timer, 100);

  TEST_ASSERT_FALSE(etcpal_signal_timed_wait(&signal, 10));

  // An unfortunately necessary heuristic - we assert that at least half the specified time has
  // gone by, to account for OS slop.
  TEST_ASSERT_GREATER_THAN_UINT32(5, etcpal_timer_elapsed(&timer));
#else
  // On this platform, timed_wait() should behave the same as try_wait() if timeout_ms is 0, and
  // wait() if timeout_ms != 0.
  TEST_ASSERT_FALSE(etcpal_signal_timed_wait(&signal, 0));
  etcpal_signal_post(&signal);
  TEST_ASSERT_TRUE(etcpal_signal_timed_wait(&signal, 10));
#endif

  etcpal_signal_destroy(&signal);
}

TEST_GROUP_RUNNER(etcpal_signal)
{
  etcpal_init(ETCPAL_FEATURE_TIMERS);
  RUN_TEST_CASE(etcpal_signal, create_and_destroy_works);
  RUN_TEST_CASE(etcpal_signal, try_wait_works);
  RUN_TEST_CASE(etcpal_signal, timed_wait_works);
  etcpal_deinit(ETCPAL_FEATURE_TIMERS);
}
