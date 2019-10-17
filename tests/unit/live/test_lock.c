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

  // Basic creation and taking ownership.
  TEST_ASSERT_TRUE(etcpal_mutex_create(&mutex));
  TEST_ASSERT_TRUE(etcpal_mutex_take(&mutex));

  // On Windows, take succeeds when taking a mutex again from the same thread.
#ifdef WIN32
  TEST_ASSERT_TRUE(etcpal_mutex_take(&mutex));
#else
  TEST_ASSERT_FALSE(etcpal_mutex_try_take(&mutex));
#endif

  etcpal_mutex_give(&mutex);

  // Take should fail on a destroyed mutex.
  etcpal_mutex_destroy(&mutex);
  TEST_ASSERT_FALSE(etcpal_mutex_take(&mutex));
}

TEST(etcpal_lock, rwlock_create_and_destroy_works)
{
  etcpal_rwlock_t rwlock;

  // Basic creation and taking ownership.
  TEST_ASSERT_TRUE(etcpal_rwlock_create(&rwlock));
  for (size_t i = 0; i < 100; ++i)
    TEST_ASSERT_TRUE(etcpal_rwlock_readlock(&rwlock));

  // Write lock should fail if there are readers
  TEST_ASSERT_FALSE(etcpal_rwlock_try_writelock(&rwlock));

  // When there are no more readers, write lock should succeed
  for (size_t i = 0; i < 100; ++i)
    etcpal_rwlock_readunlock(&rwlock);
  TEST_ASSERT_TRUE(etcpal_rwlock_writelock(&rwlock));

  // On Windows, take succeeds when taking a mutex again from the same thread.
#ifdef WIN32
  TEST_ASSERT_TRUE(etcpal_rwlock_writelock(&rwlock));
#else
  TEST_ASSERT_FALSE(etcpal_rwlock_try_writelock(&rwlock));
  TEST_ASSERT_FALSE(etcpal_rwlock_try_readlock(&rwlock));
#endif

  etcpal_rwlock_writeunlock(&rwlock);

  // Take should fail on a destroyed rwlock.
  etcpal_rwlock_destroy(&rwlock);
  TEST_ASSERT_FALSE(etcpal_rwlock_writelock(&rwlock));
}

TEST(etcpal_lock, signal_create_and_destroy_works)
{
  etcpal_signal_t signal;

  // Basic creation.
  TEST_ASSERT_TRUE(etcpal_signal_create(&signal));

  // Signals shouldn't be created in the signaled state.
  TEST_ASSERT_FALSE(etcpal_signal_poll(&signal));

  etcpal_signal_post(&signal);
  TEST_ASSERT_TRUE(etcpal_signal_wait(&signal));

  // Take should fail on a destroyed signal.
  etcpal_signal_post(&signal);
  etcpal_signal_destroy(&signal);
  TEST_ASSERT_FALSE(etcpal_signal_wait(&signal));
}

TEST_GROUP_RUNNER(etcpal_lock)
{
  RUN_TEST_CASE(etcpal_lock, mutex_create_and_destroy_works);
  RUN_TEST_CASE(etcpal_lock, rwlock_create_and_destroy_works);
  RUN_TEST_CASE(etcpal_lock, signal_create_and_destroy_works);
}
