/******************************************************************************
 * Copyright 2018 ETC Inc.
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
#include "lwpa/lock.h"
#include "gtest/gtest.h"
#include <cstddef>
#include <vector>
#include <thread>
#include <utility>

class MutexTest : public ::testing::Test
{
protected:
  MutexTest() : shared_var(0) { timeBeginPeriod(1); }

  virtual ~MutexTest() { timeEndPeriod(1); }

public:
  // Constants
  static const int MUTEX_TEST_NUM_THREADS = 10;
  static const int MUTEX_TEST_NUM_ITERATIONS = 10000;

  // For general usage
  lwpa_mutex_t mutex;

  // For thread test
  int shared_var;
};

TEST_F(MutexTest, create_destroy)
{
  // Basic creation and taking ownership.
  ASSERT_TRUE(lwpa_mutex_create(&mutex));
  ASSERT_TRUE(lwpa_mutex_take(&mutex, LWPA_WAIT_FOREVER));

  // On Windows, take succeeds when taking a mutex again from the same thread.
  ASSERT_TRUE(lwpa_mutex_take(&mutex, LWPA_WAIT_FOREVER));

  // Take should fail on a destroyed mutex.
  lwpa_mutex_give(&mutex);
  lwpa_mutex_destroy(&mutex);
  ASSERT_FALSE(lwpa_mutex_take(&mutex, LWPA_WAIT_FOREVER));
}

static void mutex_test_thread(MutexTest *fixture)
{
  if (fixture)
  {
    for (int i = 0; i < MutexTest::MUTEX_TEST_NUM_ITERATIONS; ++i)
    {
      lwpa_mutex_take(&fixture->mutex, LWPA_WAIT_FOREVER);
      ++fixture->shared_var;
      lwpa_mutex_give(&fixture->mutex);
      // Had to insert an artificial delay to get it to fail reliably when the mutexes don't work.
      // This ensures that each thread runs for long enough to get time-sliced multiple times.
      for (volatile size_t j = 0; j < 100; ++j)
        ;
    }
  }
}

// Test the actual mutex functionality. Start a number of threads and have them all increment the
// same variable inside a mutex. At the end of the test, the value of the variable should be exactly
// the number of threads times the number of times each thread incremented the variable.

// Yes, this test isn't guaranteed to fail if the mutexes don't work. But it's still a good test to
// run. Tests on several platforms where the mutex lines were commented showed failure very reliably.
TEST_F(MutexTest, threads)
{
  ASSERT_TRUE(lwpa_mutex_create(&mutex));

  std::vector<std::thread> threads;
  threads.reserve(MUTEX_TEST_NUM_THREADS);

  for (size_t i = 0; i < MUTEX_TEST_NUM_THREADS; ++i)
  {
    std::thread thread(mutex_test_thread, this);
    ASSERT_TRUE(thread.joinable());
    threads.push_back(std::move(thread));
  }

  for (auto &thread : threads)
    thread.join();

  ASSERT_EQ(shared_var, (MUTEX_TEST_NUM_THREADS * MUTEX_TEST_NUM_ITERATIONS));

  lwpa_mutex_destroy(&mutex);
}
