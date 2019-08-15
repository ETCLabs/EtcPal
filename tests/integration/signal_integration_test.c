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
#include "test_main.h"
#include "lwpa/thread.h"

// For general usage
lwpa_signal_t signal;

static void signal_test_thread(void* arg)
{
  (void)arg;

  for (size_t i = 0; i < 3; ++i)
    lwpa_signal_wait(&signal);
}

// Two threads are created. They wait on the same signal 3 times. Each post of the signal should
// wake up only one of the threads, so 6 posts should end both threads.
TEST(lwpa_integration, signal_thread_test)
{
  TEST_ASSERT_TRUE(lwpa_signal_create(&signal));

  lwpa_thread_t threads[2];

  LwpaThreadParams params;
  LWPA_THREAD_SET_DEFAULT_PARAMS(&params);

  for (size_t i = 0; i < 2; ++i)
  {
    TEST_ASSERT_TRUE(lwpa_thread_create(&threads[i], &params, signal_test_thread, NULL));
  }

  for (size_t i = 0; i < 6; ++i)
  {
    lwpa_thread_sleep(10);
    lwpa_signal_post(&signal);
  }

  for (size_t i = 0; i < 2; ++i)
    TEST_ASSERT_TRUE(lwpa_thread_join(&threads[i]));

  lwpa_signal_destroy(&signal);
}
