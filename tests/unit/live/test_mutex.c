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

#include "etcpal/mutex.h"

#include "etcpal/common.h"
#include "etcpal/timer.h"
#include "unity_fixture.h"

TEST_GROUP(etcpal_mutex);

TEST_SETUP(etcpal_mutex)
{
}

TEST_TEAR_DOWN(etcpal_mutex)
{
}

TEST(etcpal_mutex, create_and_destroy_works)
{
  etcpal_mutex_t mutex;

  // Basic creation and locking/unlocking.
  TEST_ASSERT_TRUE(etcpal_mutex_create(&mutex));
  TEST_ASSERT_TRUE(etcpal_mutex_lock(&mutex));
  etcpal_mutex_unlock(&mutex);
  etcpal_mutex_destroy(&mutex);
}

TEST(etcpal_mutex, try_lock_works)
{
  etcpal_mutex_t mutex;
  TEST_ASSERT_TRUE(etcpal_mutex_create(&mutex));

  // try_lock() should succeed when the mutex is not locked.
  TEST_ASSERT_TRUE(etcpal_mutex_try_lock(&mutex));

  // try_lock() should fail when attempting to lock the mutex again.
  TEST_ASSERT_FALSE(etcpal_mutex_try_lock(&mutex));

  etcpal_mutex_unlock(&mutex);
  etcpal_mutex_destroy(&mutex);
}

TEST(etcpal_mutex, timed_lock_works)
{
  etcpal_mutex_t mutex;
  TEST_ASSERT_TRUE(etcpal_mutex_create(&mutex));

#if ETCPAL_MUTEX_HAS_TIMED_LOCK
  TEST_ASSERT_TRUE(etcpal_mutex_lock(&mutex));

  EtcPalTimer timer;
  etcpal_timer_start(&timer, 100);

  TEST_ASSERT_FALSE(etcpal_mutex_timed_lock(&mutex, 10));

  // An unfortunately necessary heuristic - we assert that at least half the specified time has
  // gone by, to account for OS slop.
  TEST_ASSERT_GREATER_THAN_UINT32(5, etcpal_timer_elapsed(&timer));

  etcpal_mutex_unlock(&mutex);
#else
  // On this platform, timed_lock() should behave the same as try_lock() if timeout_ms is 0, and
  // lock() if timeout_ms != 0.
  TEST_ASSERT_TRUE(etcpal_mutex_timed_lock(&mutex, 10));

  TEST_ASSERT_FALSE(etcpal_mutex_timed_lock(&mutex, 0));

  etcpal_mutex_unlock(&mutex);
#endif

  etcpal_mutex_destroy(&mutex);
}

TEST_GROUP_RUNNER(etcpal_mutex)
{
  etcpal_init(ETCPAL_FEATURE_TIMERS);
  RUN_TEST_CASE(etcpal_mutex, create_and_destroy_works);
  RUN_TEST_CASE(etcpal_mutex, try_lock_works);
  RUN_TEST_CASE(etcpal_mutex, timed_lock_works);
  etcpal_deinit(ETCPAL_FEATURE_TIMERS);
}
