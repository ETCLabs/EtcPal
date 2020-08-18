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

#include "etcpal/lock.h"
#include "unity_fixture.h"

#include <stdio.h>
#include "etcpal/common.h"
#include "etcpal/thread.h"

// Disable sprintf() warning on Windows/MSVC
#ifdef _MSC_VER
#pragma warning(disable : 4996)
#endif

// Constants
#define NUM_THREADS 10
#define NUM_ITERATIONS 10000

static int            shared_var;
static etcpal_mutex_t mutex;

static void mutex_test_thread(void* arg)
{
  ETCPAL_UNUSED_ARG(arg);

  for (int i = 0; i < NUM_ITERATIONS; ++i)
  {
    (void)etcpal_mutex_lock(&mutex);
    ++shared_var;
    etcpal_mutex_unlock(&mutex);
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
  TEST_ASSERT(etcpal_mutex_create(&mutex));
}

TEST_TEAR_DOWN(mutex_integration)
{
  etcpal_mutex_destroy(&mutex);
}

// Test the actual mutex functionality. Start a number of threads and have them all increment the
// same variable inside a mutex. At the end of the test, the value of the variable should be exactly
// the number of threads times the number of times each thread incremented the variable.

// Yes, this test isn't guaranteed to fail if the mutexes don't work. But it's still a good test to
// run. Tests on several platforms where the mutex lines were commented showed failure very reliably.
TEST(mutex_integration, mutex_thread_test)
{
  etcpal_thread_t threads[NUM_THREADS];

  EtcPalThreadParams params = ETCPAL_THREAD_PARAMS_INIT;
  for (size_t i = 0; i < NUM_THREADS; ++i)
  {
    char error_msg[50];
    sprintf(error_msg, "Failed on iteration %zu", i);
    TEST_ASSERT_EQUAL_MESSAGE(etcpal_thread_create(&threads[i], &params, mutex_test_thread, NULL), kEtcPalErrOk,
                              error_msg);
  }

  for (size_t i = 0; i < NUM_THREADS; ++i)
    TEST_ASSERT_EQUAL(etcpal_thread_join(&threads[i]), kEtcPalErrOk);

  TEST_ASSERT_EQUAL(shared_var, (NUM_THREADS * NUM_ITERATIONS));
}

TEST_GROUP_RUNNER(mutex_integration)
{
  RUN_TEST_CASE(mutex_integration, mutex_thread_test);
}
