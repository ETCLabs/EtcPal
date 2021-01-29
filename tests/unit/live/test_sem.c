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

#include "etcpal/sem.h"

#include "etcpal/common.h"
#include "etcpal/timer.h"
#include "unity_fixture.h"

TEST_GROUP(etcpal_sem);

TEST_SETUP(etcpal_sem)
{
}

TEST_TEAR_DOWN(etcpal_sem)
{
}

TEST(etcpal_sem, create_and_destroy_works)
{
  etcpal_sem_t sem;

  // Basic creation, with an initial count of 1
  TEST_ASSERT_TRUE(etcpal_sem_create(&sem, 1, 10));
  TEST_ASSERT_TRUE(etcpal_sem_wait(&sem));
  TEST_ASSERT_TRUE(etcpal_sem_post(&sem));
  etcpal_sem_destroy(&sem);
}

TEST(etcpal_sem, counting_works)
{
  etcpal_sem_t sem;
  TEST_ASSERT_TRUE(etcpal_sem_create(&sem, 5, 10));

  // After decrementing the semaphore's count to 0, waiting should no longer work.
  for (int i = 0; i < 5; ++i)
    TEST_ASSERT_TRUE(etcpal_sem_wait(&sem));
  TEST_ASSERT_FALSE(etcpal_sem_try_wait(&sem));

  // After posting, wait should work again
  TEST_ASSERT_TRUE(etcpal_sem_post(&sem));
  TEST_ASSERT_TRUE(etcpal_sem_wait(&sem));

  // Some platforms require that semaphores start and end on the same count.
  for (int i = 0; i < 5; ++i)
    TEST_ASSERT_TRUE(etcpal_sem_post(&sem));

  etcpal_sem_destroy(&sem);
}

TEST(etcpal_sem, try_wait_works)
{
  etcpal_sem_t sem;
  TEST_ASSERT_TRUE(etcpal_sem_create(&sem, 1, 10));

  // Count is 1, wait should work
  TEST_ASSERT_TRUE(etcpal_sem_try_wait(&sem));

  // Count is 0, wait should not work
  TEST_ASSERT_FALSE(etcpal_sem_try_wait(&sem));

  TEST_ASSERT_TRUE(etcpal_sem_post(&sem));
  etcpal_sem_destroy(&sem);
}

TEST(etcpal_sem, timed_wait_works)
{
  etcpal_sem_t sem;
  // Create with an initial count of 0 - wait should not work
  TEST_ASSERT_TRUE(etcpal_sem_create(&sem, 0, 10));

#if ETCPAL_SEM_HAS_TIMED_WAIT
  EtcPalTimer timer;
  etcpal_timer_start(&timer, 100);

  TEST_ASSERT_FALSE(etcpal_sem_timed_wait(&sem, 10));

  // An unfortunately necessary heuristic - we assert that at least half the specified time has
  // gone by, to account for OS slop.
  TEST_ASSERT_GREATER_THAN_UINT32(5, etcpal_timer_elapsed(&timer));
#else
  // On this platform, timed_wait() should behave the same as try_wait() if timeout_ms is 0, and
  // wait() if timeout_ms != 0.
  TEST_ASSERT_FALSE(etcpal_sem_timed_wait(&sem, 0));
  TEST_ASSERT_TRUE(etcpal_sem_post(&sem));
  TEST_ASSERT_TRUE(etcpal_sem_timed_wait(&sem, 10));
#endif

  etcpal_sem_destroy(&sem);
}

#if ETCPAL_SEM_HAS_MAX_COUNT
TEST(etcpal_sem, max_count)
{
  etcpal_sem_t sem;
  TEST_ASSERT_TRUE(etcpal_sem_create(&sem, 0, 5));

  for (int i = 0; i < 5; ++i)
    TEST_ASSERT_TRUE(etcpal_sem_post(&sem));

  // After the semaphore's count reaches 5, should not be able to post it again.
  TEST_ASSERT_FALSE(etcpal_sem_post(&sem));

  // The semaphore's count should be 5 - should be able to decrement it 5 times, then not anymore.
  for (int i = 0; i < 5; ++i)
    TEST_ASSERT_TRUE(etcpal_sem_wait(&sem));

  TEST_ASSERT_FALSE(etcpal_sem_try_wait(&sem));

  etcpal_sem_destroy(&sem);
}
#endif

TEST_GROUP_RUNNER(etcpal_sem)
{
  etcpal_init(ETCPAL_FEATURE_TIMERS);
  RUN_TEST_CASE(etcpal_sem, create_and_destroy_works);
  RUN_TEST_CASE(etcpal_sem, counting_works);
  RUN_TEST_CASE(etcpal_sem, try_wait_works);
  RUN_TEST_CASE(etcpal_sem, timed_wait_works);
#if ETCPAL_SEM_HAS_MAX_COUNT
  RUN_TEST_CASE(etcpal_sem, max_count);
#endif
  etcpal_deinit(ETCPAL_FEATURE_TIMERS);
}
