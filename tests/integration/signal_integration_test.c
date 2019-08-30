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

// For general usage
static lwpa_signal_t sig;

static void signal_test_thread(void* arg)
{
  (void)arg;

  for (size_t i = 0; i < 3; ++i)
    lwpa_signal_wait(&sig);
}

TEST_GROUP(signal_integration);

TEST_SETUP(signal_integration)
{
  TEST_ASSERT(lwpa_signal_create(&sig));
}

TEST_TEAR_DOWN(signal_integration)
{
  lwpa_signal_destroy(&sig);
}

// Two threads are created. They wait on the same signal 3 times. Each post of the signal should
// wake up only one of the threads, so 6 posts should end both threads.
TEST(signal_integration, signal_thread_test)
{
  lwpa_thread_t threads[2];

  LwpaThreadParams params;
  LWPA_THREAD_SET_DEFAULT_PARAMS(&params);

  for (size_t i = 0; i < 2; ++i)
  {
    char error_msg[50];
    sprintf(error_msg, "Failed on iteration %zu", i);
    TEST_ASSERT_TRUE_MESSAGE(lwpa_thread_create(&threads[i], &params, signal_test_thread, NULL), error_msg);
  }

  for (size_t i = 0; i < 6; ++i)
  {
    lwpa_thread_sleep(10);
    lwpa_signal_post(&sig);
  }

  for (size_t i = 0; i < 2; ++i)
    TEST_ASSERT_TRUE(lwpa_thread_join(&threads[i]));
}

TEST_GROUP_RUNNER(signal_integration)
{
  RUN_TEST_CASE(signal_integration, signal_thread_test);
}
