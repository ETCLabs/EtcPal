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
#include "etcpal/lock.h"
#include "unity_fixture.h"

#include <stddef.h>

TEST_GROUP(lwpa_lock);

TEST_SETUP(lwpa_lock)
{
}

TEST_TEAR_DOWN(lwpa_lock)
{
}

TEST(lwpa_lock, mutex_create_and_destroy_works)
{
  lwpa_mutex_t mutex;

  // Basic creation and taking ownership.
  TEST_ASSERT_TRUE(lwpa_mutex_create(&mutex));
  TEST_ASSERT_TRUE(lwpa_mutex_take(&mutex));

  // On Windows, take succeeds when taking a mutex again from the same thread.
#ifdef WIN32
  TEST_ASSERT_TRUE(lwpa_mutex_take(&mutex));
#else
  TEST_ASSERT_FALSE(lwpa_mutex_try_take(&mutex));
#endif

  lwpa_mutex_give(&mutex);

  // Take should fail on a destroyed mutex.
  lwpa_mutex_destroy(&mutex);
  TEST_ASSERT_FALSE(lwpa_mutex_take(&mutex));
}

TEST(lwpa_lock, rwlock_create_and_destroy_works)
{
  lwpa_rwlock_t rwlock;

  // Basic creation and taking ownership.
  TEST_ASSERT_TRUE(lwpa_rwlock_create(&rwlock));
  for (size_t i = 0; i < 100; ++i)
    TEST_ASSERT_TRUE(lwpa_rwlock_readlock(&rwlock));

  // Write lock should fail if there are readers
  TEST_ASSERT_FALSE(lwpa_rwlock_try_writelock(&rwlock));

  // When there are no more readers, write lock should succeed
  for (size_t i = 0; i < 100; ++i)
    lwpa_rwlock_readunlock(&rwlock);
  TEST_ASSERT_TRUE(lwpa_rwlock_writelock(&rwlock));

  // On Windows, take succeeds when taking a mutex again from the same thread.
#ifdef WIN32
  TEST_ASSERT_TRUE(lwpa_rwlock_writelock(&rwlock));
#else
  TEST_ASSERT_FALSE(lwpa_rwlock_try_writelock(&rwlock));
  TEST_ASSERT_FALSE(lwpa_rwlock_try_readlock(&rwlock));
#endif

  lwpa_rwlock_writeunlock(&rwlock);

  // Take should fail on a destroyed rwlock.
  lwpa_rwlock_destroy(&rwlock);
  TEST_ASSERT_FALSE(lwpa_rwlock_writelock(&rwlock));
}

TEST(lwpa_lock, signal_create_and_destroy_works)
{
  lwpa_signal_t signal;

  // Basic creation.
  TEST_ASSERT_TRUE(lwpa_signal_create(&signal));

  // Signals shouldn't be created in the signaled state.
  TEST_ASSERT_FALSE(lwpa_signal_poll(&signal));

  lwpa_signal_post(&signal);
  TEST_ASSERT_TRUE(lwpa_signal_wait(&signal));

  // Take should fail on a destroyed signal.
  lwpa_signal_post(&signal);
  lwpa_signal_destroy(&signal);
  TEST_ASSERT_FALSE(lwpa_signal_wait(&signal));
}

TEST_GROUP_RUNNER(lwpa_lock)
{
  RUN_TEST_CASE(lwpa_lock, mutex_create_and_destroy_works);
  RUN_TEST_CASE(lwpa_lock, rwlock_create_and_destroy_works);
  RUN_TEST_CASE(lwpa_lock, signal_create_and_destroy_works);
}
