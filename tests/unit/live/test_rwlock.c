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

#include "etcpal/rwlock.h"

#include "etcpal/common.h"
#include "etcpal/timer.h"
#include "unity_fixture.h"

TEST_GROUP(etcpal_rwlock);

TEST_SETUP(etcpal_rwlock)
{
}

TEST_TEAR_DOWN(etcpal_rwlock)
{
}

TEST(etcpal_rwlock, create_and_destroy_works)
{
  etcpal_rwlock_t rwlock;

  // Basic creation and taking ownership.
  TEST_ASSERT_TRUE(etcpal_rwlock_create(&rwlock));
  TEST_ASSERT_TRUE(etcpal_rwlock_readlock(&rwlock));
  etcpal_rwlock_readunlock(&rwlock);
  TEST_ASSERT_TRUE(etcpal_rwlock_writelock(&rwlock));
  etcpal_rwlock_writeunlock(&rwlock);
  etcpal_rwlock_destroy(&rwlock);
}

TEST(etcpal_rwlock, read_and_write_interaction)
{
  etcpal_rwlock_t rwlock;
  TEST_ASSERT_TRUE(etcpal_rwlock_create(&rwlock));

  for (int i = 0; i < 100; ++i)
    TEST_ASSERT_TRUE(etcpal_rwlock_readlock(&rwlock));

  // Write lock should fail if there are readers
  TEST_ASSERT_FALSE(etcpal_rwlock_try_writelock(&rwlock));

  for (int i = 0; i < 100; ++i)
    etcpal_rwlock_readunlock(&rwlock);

  TEST_ASSERT_TRUE(etcpal_rwlock_writelock(&rwlock));

  // Read lock and write locks should now fail
  TEST_ASSERT_FALSE(etcpal_rwlock_try_writelock(&rwlock));
  TEST_ASSERT_FALSE(etcpal_rwlock_try_readlock(&rwlock));

  etcpal_rwlock_writeunlock(&rwlock);
  etcpal_rwlock_destroy(&rwlock);
}

TEST(etcpal_rwlock, timed_lock_works)
{
  etcpal_rwlock_t rwlock;
  TEST_ASSERT_TRUE(etcpal_rwlock_create(&rwlock));

#if ETCPAL_MUTEX_HAS_TIMED_LOCK
  EtcPalTimer timer;

  // Test timed_writelock()
  TEST_ASSERT_TRUE(etcpal_rwlock_readlock(&rwlock));
  etcpal_timer_start(&timer, 100);
  TEST_ASSERT_FALSE(etcpal_rwlock_timed_writelock(&rwlock, 10));

  // An unfortunately necessary heuristic - we assert that at least half the specified time has
  // gone by, to account for OS slop.
  TEST_ASSERT_GREATER_THAN_UINT32(5, etcpal_timer_elapsed(&timer));

  etcpal_rwlock_readunlock(&rwlock);

  // Test timed_readlock()

  TEST_ASSERT_TRUE(etcpal_rwlock_writelock(&rwlock));
  etcpal_timer_start(&timer, 100);
  TEST_ASSERT_FALSE(etcpal_rwlock_timed_readlock(&rwlock, 10));

  TEST_ASSERT_GREATER_THAN_UINT32(5, etcpal_timer_elapsed(&timer));

  etcpal_rwlock_writeunlock(&rwlock);
#else
  // On this platform, timed_lock() should behave the same as try_lock() if timeout_ms is 0, and
  // lock() if timeout_ms != 0.

  // Test timed_writelock()

  TEST_ASSERT_TRUE(etcpal_rwlock_timed_readlock(&rwlock, 10));
  TEST_ASSERT_FALSE(etcpal_rwlock_timed_writelock(&rwlock, 0));
  etcpal_rwlock_readunlock(&rwlock);

  // Test timed_readlock()

  TEST_ASSERT_TRUE(etcpal_rwlock_timed_writelock(&rwlock, 10));
  TEST_ASSERT_FALSE(etcpal_rwlock_timed_readlock(&rwlock, 0));
  etcpal_rwlock_writeunlock(&rwlock);
#endif

  etcpal_rwlock_destroy(&rwlock);
}

TEST_GROUP_RUNNER(etcpal_rwlock)
{
  etcpal_init(ETCPAL_FEATURE_TIMERS);
  RUN_TEST_CASE(etcpal_rwlock, create_and_destroy_works);
  RUN_TEST_CASE(etcpal_rwlock, read_and_write_interaction);
  RUN_TEST_CASE(etcpal_rwlock, timed_lock_works);
  etcpal_deinit(ETCPAL_FEATURE_TIMERS);
}
