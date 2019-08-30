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
#include "lwpa/lock.h"
#include "unity_fixture.h"

#include <stdio.h>
#include "lwpa/thread.h"

// Constants
#define NUM_THREADS 10
#define NUM_ITERATIONS 10000

static int shared_var;
static lwpa_mutex_t mutex;

static void mutex_test_thread(void* arg)
{
  (void)arg;

  for (int i = 0; i < NUM_ITERATIONS; ++i)
  {
    lwpa_mutex_take(&mutex);
    ++shared_var;
    lwpa_mutex_give(&mutex);
    // Had to insert an artificial delay to get it to fail reliably when the mutexes don't work.
    // This ensures that each thread runs for long enough to get time-sliced multiple times.
    for (volatile size_t j = 0; j < 100; ++j)
      ;
  }
}

TEST_GROUP(mutex_integration);

TEST_SETUP(mutex_integration)
{
  shared_var = 0;
  TEST_ASSERT(lwpa_mutex_create(&mutex));
}

TEST_TEAR_DOWN(mutex_integration)
{
  lwpa_mutex_destroy(&mutex);
}

// Test the actual mutex functionality. Start a number of threads and have them all increment the
// same variable inside a mutex. At the end of the test, the value of the variable should be exactly
// the number of threads times the number of times each thread incremented the variable.

// Yes, this test isn't guaranteed to fail if the mutexes don't work. But it's still a good test to
// run. Tests on several platforms where the mutex lines were commented showed failure very reliably.
TEST(mutex_integration, mutex_thread_test)
{
  lwpa_thread_t threads[NUM_THREADS];

  LwpaThreadParams params;
  LWPA_THREAD_SET_DEFAULT_PARAMS(&params);

  for (size_t i = 0; i < NUM_THREADS; ++i)
  {
    char error_msg[50];
    sprintf(error_msg, "Failed on iteration %zu", i);
    TEST_ASSERT_TRUE_MESSAGE(lwpa_thread_create(&threads[i], &params, mutex_test_thread, NULL), error_msg);
  }

  for (size_t i = 0; i < NUM_THREADS; ++i)
    TEST_ASSERT_TRUE(lwpa_thread_join(&threads[i]));

  TEST_ASSERT_EQUAL(shared_var, (NUM_THREADS * NUM_ITERATIONS));
}

TEST_GROUP_RUNNER(mutex_integration)
{
  RUN_TEST_CASE(mutex_integration, mutex_thread_test);
}
