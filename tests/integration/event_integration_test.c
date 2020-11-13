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

#include "etcpal/event.h"

#include "etcpal/common.h"
#include "etcpal/thread.h"
#include "unity_fixture.h"

#define NUM_ITERATIONS 3

static etcpal_event_t event;

TEST_GROUP(event_integration);

TEST_SETUP(event_integration)
{
  TEST_ASSERT_TRUE(etcpal_event_create(&event));
}

TEST_TEAR_DOWN(event_integration)
{
  etcpal_event_destroy(&event);
  // Allow some time for threads to be cleaned up on RTOS platforms
  etcpal_thread_sleep(200);
}

static void event_test_thread(void* arg)
{
  ETCPAL_UNUSED_ARG(arg);

  for (int i = 0; i < NUM_ITERATIONS; ++i)
  {
    TEST_ASSERT_EQUAL(etcpal_event_wait(&event, 0x80, ETCPAL_EVENT_CLEAR_ON_EXIT), 0x80);
  }
}

TEST(event_integration, one_wait_one_signal)
{
  etcpal_thread_t    wait_thread;
  EtcPalThreadParams params = ETCPAL_THREAD_PARAMS_INIT;

  TEST_ASSERT_EQUAL(etcpal_thread_create(&wait_thread, &params, event_test_thread, NULL), kEtcPalErrOk);

  for (int i = 0; i < NUM_ITERATIONS; ++i)
  {
    etcpal_thread_sleep(10);
    etcpal_event_set_bits(&event, 0x80);
  }

  TEST_ASSERT_EQUAL(etcpal_thread_join(&wait_thread), kEtcPalErrOk);
}

TEST_GROUP_RUNNER(event_integration)
{
  RUN_TEST_CASE(event_integration, one_wait_one_signal);
}
