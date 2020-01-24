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

#include "etcpal/lock.h"
#include "unity_fixture.h"

#include <stddef.h>
#include "etcpal/timer.h"

TEST_GROUP(etcpal_lock);

TEST_SETUP(etcpal_lock)
{
}

TEST_TEAR_DOWN(etcpal_lock)
{
}

TEST(etcpal_lock, mutex_create_and_destroy_works)
{
  etcpal_mutex_t mutex;

  // Basic creation and locking/unlocking.
  TEST_ASSERT_TRUE(etcpal_mutex_create(&mutex));
  TEST_ASSERT_TRUE(etcpal_mutex_lock(&mutex));
  etcpal_mutex_unlock(&mutex);
  etcpal_mutex_destroy(&mutex);

  // lock() should fail on a destroyed mutex.
  TEST_ASSERT_FALSE(etcpal_mutex_lock(&mutex));
}

TEST(etcpal_lock, mutex_try_lock_works)
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

TEST(etcpal_lock, mutex_timed_lock_works)
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

TEST(etcpal_lock, signal_create_and_destroy_works)
{
  etcpal_signal_t signal;

  // Basic creation.
  TEST_ASSERT_TRUE(etcpal_signal_create(&signal));

  etcpal_signal_post(&signal);
  TEST_ASSERT_TRUE(etcpal_signal_wait(&signal));

  // Wait should fail on a destroyed signal.
  etcpal_signal_post(&signal);
  etcpal_signal_destroy(&signal);
  TEST_ASSERT_FALSE(etcpal_signal_wait(&signal));
}

TEST(etcpal_lock, signal_try_wait_works)
{
  etcpal_signal_t signal;
  TEST_ASSERT_TRUE(etcpal_signal_create(&signal));

  // Signals shouldn't be created in the signaled state.
  TEST_ASSERT_FALSE(etcpal_signal_try_wait(&signal));

  etcpal_signal_post(&signal);
  TEST_ASSERT_TRUE(etcpal_signal_try_wait(&signal));

  etcpal_signal_destroy(&signal);
}

TEST(etcpal_lock, signal_timed_wait_works)
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

TEST(etcpal_lock, rwlock_create_and_destroy_works)
{
  etcpal_rwlock_t rwlock;

  // Basic creation and taking ownership.
  TEST_ASSERT_TRUE(etcpal_rwlock_create(&rwlock));
  TEST_ASSERT_TRUE(etcpal_rwlock_readlock(&rwlock));
  etcpal_rwlock_readunlock(&rwlock);
  TEST_ASSERT_TRUE(etcpal_rwlock_writelock(&rwlock));
  etcpal_rwlock_writeunlock(&rwlock);

  // Take should fail on a destroyed rwlock.
  etcpal_rwlock_destroy(&rwlock);
  TEST_ASSERT_FALSE(etcpal_rwlock_writelock(&rwlock));
}

TEST(etcpal_lock, rwlock_read_and_write_interaction)
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

TEST(etcpal_lock, rwlock_timed_lock_works)
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

TEST(etcpal_lock, sem_create_and_destroy_works)
{
  etcpal_sem_t sem;

  // Basic creation, with an initial count of 1
  TEST_ASSERT_TRUE(etcpal_sem_create(&sem, 1, 10));

  // Wait should decrement the count to 0
  TEST_ASSERT_TRUE(etcpal_sem_wait(&sem));

  // Now wait should fail
  TEST_ASSERT_FALSE(etcpal_sem_try_wait(&sem));

  // After posting, wait should work again
  TEST_ASSERT_TRUE(etcpal_sem_post(&sem));
  TEST_ASSERT_TRUE(etcpal_sem_wait(&sem));

  // Wait should fail on a destroyed semaphore
  TEST_ASSERT_TRUE(etcpal_sem_post(&sem));
  etcpal_sem_destroy(&sem);
  TEST_ASSERT_FALSE(etcpal_sem_wait(&sem));
}

TEST_GROUP_RUNNER(etcpal_lock)
{
  RUN_TEST_CASE(etcpal_lock, mutex_create_and_destroy_works);
  RUN_TEST_CASE(etcpal_lock, mutex_try_lock_works);
  RUN_TEST_CASE(etcpal_lock, mutex_timed_lock_works);
  RUN_TEST_CASE(etcpal_lock, signal_create_and_destroy_works);
  RUN_TEST_CASE(etcpal_lock, signal_try_wait_works);
  RUN_TEST_CASE(etcpal_lock, signal_timed_wait_works);
  RUN_TEST_CASE(etcpal_lock, rwlock_create_and_destroy_works);
  RUN_TEST_CASE(etcpal_lock, rwlock_read_and_write_interaction);
  RUN_TEST_CASE(etcpal_lock, rwlock_timed_lock_works);
  RUN_TEST_CASE(etcpal_lock, sem_create_and_destroy_works);
}
