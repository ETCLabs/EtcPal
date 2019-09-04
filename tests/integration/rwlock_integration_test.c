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

#include <stdio.h>
#include "etcpal/thread.h"

// Disable sprintf() warning on Windows/MSVC
#ifdef _MSC_VER
#pragma warning(disable : 4996)
#endif

#define NUM_WRITE_THREADS 10
#define NUM_ITERATIONS 10000

static lwpa_rwlock_t rwlock;
static int shared_var;
static bool read_thread_pass;

static void write_test_thread(void* arg)
{
  (void)arg;

  for (size_t i = 0; i < NUM_ITERATIONS; ++i)
  {
    lwpa_rwlock_writelock(&rwlock);
    ++shared_var;
    lwpa_rwlock_writeunlock(&rwlock);
    // Had to insert an artificial delay to get it to fail reliably when the mutexes don't work.
    // This ensures that each thread runs for long enough to get time-sliced multiple times.
    for (volatile size_t j = 0; j < 100; ++j)
      ;
  }
}

static void read_test_thread(void* arg)
{
  (void)arg;

  read_thread_pass = true;
  for (int i = 0; i < 10; ++i)
  {
    lwpa_rwlock_readlock(&rwlock);
    int val = shared_var;

    lwpa_thread_sleep(5);

    // Make sure the value hasn't changed.
    if (val != shared_var)
    {
      read_thread_pass = false;
      break;
    }
    lwpa_rwlock_readunlock(&rwlock);
    lwpa_thread_sleep(5);
  }
}

TEST_GROUP(rwlock_integration);

TEST_SETUP(rwlock_integration)
{
  shared_var = 0;
  TEST_ASSERT(lwpa_rwlock_create(&rwlock));
}

TEST_TEAR_DOWN(rwlock_integration)
{
  lwpa_rwlock_destroy(&rwlock);
}

// Test the actual read-write lock functionality. Start a number of threads and have them all
// increment the same variable inside a write lock. Meanwhile, another thread periodically reads the
// variable. At the end of the test, the value of the variable should be exactly the number of
// threads times the number of times each thread incremented the variable.
//
// Yes, this test isn't guaranteed to fail if the read-write locks don't work. But it's still a good
// test to run. Tests on several platform where the lock lines were commented showed failure very
// reliably.
TEST(rwlock_integration, rwlock_thread_test)
{
  lwpa_thread_t write_threads[NUM_WRITE_THREADS];
  lwpa_thread_t read_thread;

  LwpaThreadParams params;
  LWPA_THREAD_SET_DEFAULT_PARAMS(&params);

  TEST_ASSERT_TRUE(lwpa_thread_create(&read_thread, &params, read_test_thread, NULL));

  for (size_t i = 0; i < NUM_WRITE_THREADS; ++i)
  {
    char error_msg[50];
    sprintf(error_msg, "Failed on iteration %zu", i);
    TEST_ASSERT_TRUE_MESSAGE(lwpa_thread_create(&write_threads[i], &params, write_test_thread, NULL), error_msg);
  }

  for (size_t i = 0; i < NUM_WRITE_THREADS; ++i)
  {
    TEST_ASSERT_TRUE(lwpa_thread_join(&write_threads[i]));
  }

  TEST_ASSERT_TRUE(lwpa_thread_join(&read_thread));

  TEST_ASSERT(read_thread_pass);
  TEST_ASSERT_EQUAL(shared_var, (NUM_WRITE_THREADS * NUM_ITERATIONS));
}

TEST_GROUP_RUNNER(rwlock_integration)
{
  RUN_TEST_CASE(rwlock_integration, rwlock_thread_test);
}
