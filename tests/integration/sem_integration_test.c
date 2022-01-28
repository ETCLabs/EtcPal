/******************************************************************************
 * Copyright 2022 ETC Inc.
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

#include "etcpal/sem.h"
#include "unity_fixture.h"

#include <stdio.h>
#include "etcpal/common.h"
#include "etcpal/thread.h"

// Disable sprintf() warning on Windows/MSVC
#ifdef _MSC_VER
#pragma warning(disable : 4996)
#endif

// Constants
#define NUM_THREADS    5
#define NUM_ITERATIONS 3

static etcpal_sem_t sem;

static void sem_test_thread(void* arg)
{
  ETCPAL_UNUSED_ARG(arg);

  for (int i = 0; i < NUM_ITERATIONS; ++i)
  {
    (void)etcpal_sem_wait(&sem);
  }
}

TEST_GROUP(sem_integration);

TEST_SETUP(sem_integration)
{
}

TEST_TEAR_DOWN(sem_integration)
{
  // Allow some time for threads to be cleaned up on RTOS platforms
  etcpal_thread_sleep(200);
}

// Initialize the semaphore with a count of 0. Each thread just tries to take the semaphore NUM_ITERATIONS times. Post
// the semaphore (NUM_ITERATIONS * NUM_THREADS) times to wake up each thread the correct number of threads. Each thread
// should then terminate. The failure mode is that one or more threads hang and the test times out.
TEST(sem_integration, sem_thread_test_initial_zero)
{
  etcpal_thread_t threads[NUM_THREADS];

  TEST_ASSERT_TRUE(etcpal_sem_create(&sem, 0, NUM_THREADS * NUM_ITERATIONS));

  EtcPalThreadParams params = ETCPAL_THREAD_PARAMS_INIT;
  for (int i = 0; i < NUM_THREADS; ++i)
  {
    char error_msg[50];
    sprintf(error_msg, "Failed on iteration %d", i);
    TEST_ASSERT_EQUAL_MESSAGE(etcpal_thread_create(&threads[i], &params, sem_test_thread, NULL), kEtcPalErrOk,
                              error_msg);
  }

  for (int i = 0; i < NUM_ITERATIONS; ++i)
  {
    for (int j = 0; j < NUM_THREADS; ++j)
    {
      char error_msg[50];
      sprintf(error_msg, "Failed on external loop %d, internal loop %d", i, j);
      TEST_ASSERT_TRUE_MESSAGE(etcpal_sem_post(&sem), error_msg);
    }
  }

  for (int i = 0; i < NUM_THREADS; ++i)
    TEST_ASSERT_EQUAL(etcpal_thread_join(&threads[i]), kEtcPalErrOk);

  // The pass case here is that the test does not hang/timeout
  etcpal_sem_destroy(&sem);
}

// Initialize the semaphore with a count of (NUM_ITERATIONS * NUM_THREADS). Each thread just tries to take the
// semaphore NUM_ITERATIONS times, and should succeed each time. Each thread should then terminate. The failure mode is
// that one or more threads hang and the test times out.
TEST(sem_integration, sem_thread_test_initial_full)
{
  etcpal_thread_t threads[NUM_THREADS];

  TEST_ASSERT_TRUE(etcpal_sem_create(&sem, NUM_THREADS * NUM_ITERATIONS, NUM_THREADS * NUM_ITERATIONS));

  EtcPalThreadParams params = ETCPAL_THREAD_PARAMS_INIT;
  for (int i = 0; i < NUM_THREADS; ++i)
  {
    char error_msg[50];
    sprintf(error_msg, "Failed on iteration %d", i);
    TEST_ASSERT_EQUAL_MESSAGE(etcpal_thread_create(&threads[i], &params, sem_test_thread, NULL), kEtcPalErrOk,
                              error_msg);
  }

  for (int i = 0; i < NUM_THREADS; ++i)
    TEST_ASSERT_EQUAL(etcpal_thread_join(&threads[i]), kEtcPalErrOk);

#if ETCPAL_SEM_MUST_BE_BALANCED
  for (int i = 0; i < (NUM_ITERATIONS * NUM_THREADS); ++i)
  {
    char error_msg[50];
    sprintf(error_msg, "Failed on iteration %d", i);
    TEST_ASSERT_TRUE_MESSAGE(etcpal_sem_post(&sem), error_msg);
  }
#endif

  // The pass case here is that the test does not hang/timeout
  etcpal_sem_destroy(&sem);
}

TEST_GROUP_RUNNER(sem_integration)
{
  RUN_TEST_CASE(sem_integration, sem_thread_test_initial_zero);
  RUN_TEST_CASE(sem_integration, sem_thread_test_initial_full);
}
