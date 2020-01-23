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

// Constants
#define NUM_THREADS 5
#define NUM_ITERATIONS 3

static etcpal_sem_t sem;

static void sem_test_thread(void* arg)
{
  (void)arg;

  for (int i = 0; i < NUM_ITERATIONS; ++i)
  {
    etcpal_sem_wait(&sem);
  }
}

TEST_GROUP(sem_integration);

TEST_SETUP(sem_integration)
{
  TEST_ASSERT_TRUE(etcpal_sem_create(&sem, NUM_THREADS, NUM_THREADS * NUM_ITERATIONS));
}

TEST_TEAR_DOWN(sem_integration)
{
  etcpal_sem_destroy(&sem);
}

// Initialize the semaphore with a count of NUM_THREADS; each thread should be able to take the semaphore once.
// Each thread just tries to take the semaphore NUM_ITERATIONS times. Post the semaphore
// (NUM_ITERATIONS - 1) * NUM_THREADS times to wake up each thread the correct number of threads. Each thread should
// then terminate. The failure mode is that one or more threads hang and the test times out.
TEST(sem_integration, sem_thread_test)
{
  etcpal_thread_t threads[NUM_THREADS];

  EtcPalThreadParams params = {ETCPAL_THREAD_DEFAULT_PARAMS_INIT};
  for (int i = 0; i < NUM_THREADS; ++i)
  {
    char error_msg[50];
    sprintf(error_msg, "Failed on iteration %d", i);
    TEST_ASSERT_EQUAL_MESSAGE(etcpal_thread_create(&threads[i], &params, sem_test_thread, NULL), kEtcPalErrOk,
                              error_msg);
  }

  for (int i = 0; i < NUM_ITERATIONS - 1; ++i)
  {
    for (int j = 0; j < NUM_THREADS; ++j)
    {
      TEST_ASSERT_TRUE(etcpal_sem_post(&sem));
    }
  }

  for (int i = 0; i < NUM_THREADS; ++i)
    TEST_ASSERT_EQUAL(etcpal_thread_join(&threads[i]), kEtcPalErrOk);

  // The pass case here is that the test does not hang/timeout
}

TEST_GROUP_RUNNER(sem_integration)
{
  RUN_TEST_CASE(sem_integration, sem_thread_test);
}
