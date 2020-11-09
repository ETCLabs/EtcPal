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

#include "etcpal/cpp/mutex.h"
#include "unity_fixture.h"

extern "C" {
TEST_GROUP(etcpal_cpp_mutex);

TEST_SETUP(etcpal_cpp_mutex)
{
}

TEST_TEAR_DOWN(etcpal_cpp_mutex)
{
}

TEST(etcpal_cpp_mutex, create_and_destroy_works)
{
  etcpal::Mutex mutex;

  // Take ownership
  TEST_ASSERT_TRUE(mutex.Lock());

  TEST_ASSERT_FALSE(mutex.TryLock());

  mutex.Unlock();
}

TEST(etcpal_cpp_mutex, guard_works)
{
  etcpal::Mutex mutex;

  {
    // Take ownership via a mutex guard
    etcpal::MutexGuard guard(mutex);

    TEST_ASSERT_FALSE(mutex.TryLock());
  }

  // Lock should now be unlocked
  TEST_ASSERT_TRUE(mutex.TryLock());
  mutex.Unlock();
}

TEST_GROUP_RUNNER(etcpal_cpp_mutex)
{
  RUN_TEST_CASE(etcpal_cpp_mutex, create_and_destroy_works);
  RUN_TEST_CASE(etcpal_cpp_mutex, guard_works);
}
}
