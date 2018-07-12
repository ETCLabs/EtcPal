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
#include "lwpa_lock.h"
#include "gtest/gtest.h"
#include <cstddef>
#include <Windows.h>
#include <process.h>

class MutexTest : public ::testing::Test
{
protected:
  MutexTest()
    : shared_var(0)
  {
    timeBeginPeriod(1);
  }

  virtual ~MutexTest()
  {
    timeEndPeriod(1);
  }

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

static unsigned __stdcall
mutex_test_thread(void *pthread)
{
  MutexTest *mt = static_cast<MutexTest *>(pthread);
  if (mt)
  {
    for (int i = 0; i < MutexTest::MUTEX_TEST_NUM_ITERATIONS; ++i)
    {
      lwpa_mutex_take(&mt->mutex, LWPA_WAIT_FOREVER);
      ++mt->shared_var;
      lwpa_mutex_give(&mt->mutex);
      // Had to insert an artificial delay to get it to fail reliably when the
      // mutexes don't work. This ensures that each thread runs for long enough
      // to get time-sliced multiple times.
      for (volatile size_t j = 0; j < 100; ++j) ;
    }
    return 0;
  }
  return 1;
}

// Test the actual mutex functionality. Start a number of time-slice threads at
// the same priority, and have them all increment the same variable inside a
// mutex. At the end of the test, the value of the variable should be exactly
// the number of threads times the number of times each thread incremented the
// variable.

// Yes, this test isn't guaranteed to fail if the mutexes don't work. But it's
// still a good test to run. A test on this platform where the mutex lines were
// commented showed failure very reliably.
TEST_F(MutexTest, threads)
{
  ASSERT_TRUE(lwpa_mutex_create(&mutex));
  HANDLE *threads = new HANDLE[MUTEX_TEST_NUM_THREADS];
  ASSERT_TRUE(threads != NULL);
  for (size_t i = 0; i < MUTEX_TEST_NUM_THREADS; ++i)
  {
    HANDLE thread_id = reinterpret_cast<HANDLE>(_beginthreadex(NULL, 0,
                                                  mutex_test_thread, this, 0,
                                                  NULL));
    ASSERT_TRUE(thread_id != NULL);
    threads[i] = thread_id;
  }

  ASSERT_EQ(WaitForMultipleObjects(MUTEX_TEST_NUM_THREADS, threads, TRUE,
                                   INFINITE), WAIT_OBJECT_0);
  ASSERT_EQ(shared_var, (MUTEX_TEST_NUM_THREADS * MUTEX_TEST_NUM_ITERATIONS));
  delete[] threads;
  lwpa_mutex_destroy(&mutex);
}
