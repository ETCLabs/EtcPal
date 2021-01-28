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

#include "etcpal/recursive_mutex.h"
#include "unity_fixture.h"

TEST_GROUP(etcpal_recursive_mutex);

TEST_SETUP(etcpal_recursive_mutex)
{
}

TEST_TEAR_DOWN(etcpal_recursive_mutex)
{
}

TEST(etcpal_recursive_mutex, create_and_destroy_works)
{
  etcpal_recursive_mutex_t mutex;

  // Basic creation and locking/unlocking.
  TEST_ASSERT_TRUE(etcpal_recursive_mutex_create(&mutex));
  TEST_ASSERT_TRUE(etcpal_recursive_mutex_lock(&mutex));
  TEST_ASSERT_TRUE(etcpal_recursive_mutex_lock(&mutex));  // Double lock because it's recursive
  etcpal_recursive_mutex_unlock(&mutex);
  etcpal_recursive_mutex_unlock(&mutex);
  etcpal_recursive_mutex_destroy(&mutex);
}

TEST(etcpal_recursive_mutex, try_lock_works)
{
  etcpal_recursive_mutex_t mutex;
  TEST_ASSERT_TRUE(etcpal_recursive_mutex_create(&mutex));

  // try_lock() should succeed when the mutex is not locked.
  TEST_ASSERT_TRUE(etcpal_recursive_mutex_try_lock(&mutex));

  // try_lock() should also succeed when attempting to lock the mutex again.
  TEST_ASSERT_TRUE(etcpal_recursive_mutex_try_lock(&mutex));

  etcpal_recursive_mutex_unlock(&mutex);
  etcpal_recursive_mutex_unlock(&mutex);
  etcpal_recursive_mutex_destroy(&mutex);
}

TEST(etcpal_recursive_mutex, timed_lock_works)
{
  etcpal_recursive_mutex_t mutex;
  TEST_ASSERT_TRUE(etcpal_recursive_mutex_create(&mutex));

  // Can't truly unit test timed_lock from one thread on a recursive mutex, but we'll just make
  // sure it works on a basic level
  TEST_ASSERT_TRUE(etcpal_recursive_mutex_timed_lock(&mutex, 10));
  TEST_ASSERT_TRUE(etcpal_recursive_mutex_timed_lock(&mutex, 0));
  etcpal_recursive_mutex_unlock(&mutex);
  etcpal_recursive_mutex_unlock(&mutex);

  etcpal_recursive_mutex_destroy(&mutex);
}

TEST_GROUP_RUNNER(etcpal_recursive_mutex)
{
  RUN_TEST_CASE(etcpal_recursive_mutex, create_and_destroy_works);
  RUN_TEST_CASE(etcpal_recursive_mutex, try_lock_works);
  RUN_TEST_CASE(etcpal_recursive_mutex, timed_lock_works);
}
