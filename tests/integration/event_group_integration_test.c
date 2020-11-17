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

#include "etcpal/event_group.h"

#include "etcpal/common.h"
#include "etcpal/thread.h"
#include "unity_fixture.h"

#define NUM_ITERATIONS 3

static etcpal_event_group_t event;

TEST_GROUP(event_group_integration);

TEST_SETUP(event_group_integration)
{
  TEST_ASSERT_TRUE(etcpal_event_group_create(&event, ETCPAL_EVENT_GROUP_AUTO_CLEAR));
}

TEST_TEAR_DOWN(event_group_integration)
{
  etcpal_event_group_destroy(&event);
  // Allow some time for threads to be cleaned up on RTOS platforms
  etcpal_thread_sleep(200);
}

static int num_events_received;

static void single_event_wait_thread(void* arg)
{
  ETCPAL_UNUSED_ARG(arg);

  for (int i = 0; i < NUM_ITERATIONS; ++i)
  {
    if (etcpal_event_group_wait(&event, 0x80, 0) == 0x80)
      ++num_events_received;
  }
}

TEST(event_group_integration, one_wait_one_signal_one_event)
{
  etcpal_thread_t    wait_thread;
  EtcPalThreadParams params = ETCPAL_THREAD_PARAMS_INIT;

  num_events_received = 0;

  TEST_ASSERT_EQUAL(etcpal_thread_create(&wait_thread, &params, single_event_wait_thread, NULL), kEtcPalErrOk);

  for (int i = 0; i < NUM_ITERATIONS; ++i)
  {
    etcpal_thread_sleep(10);
    etcpal_event_group_set_bits(&event, 0x80);
  }

  TEST_ASSERT_EQUAL(etcpal_thread_join(&wait_thread), kEtcPalErrOk);
  TEST_ASSERT_EQUAL(num_events_received, NUM_ITERATIONS);
}

#define EVENT_1 0x80
#define EVENT_2 0x08
static int event_1_num_received;
static int event_2_num_received;
bool       keep_running;

static void multiple_event_wait_thread(void* arg)
{
  ETCPAL_UNUSED_ARG(arg);

  do
  {
    etcpal_event_bits_t result = etcpal_event_group_timed_wait(&event, EVENT_1 | EVENT_2, 0, 0);
    if (result & EVENT_1)
      ++event_1_num_received;
    if (result & EVENT_2)
      ++event_2_num_received;
    etcpal_thread_sleep(10);
  } while (keep_running);
}

TEST(event_group_integration, one_wait_one_signal_multiple_events)
{
  etcpal_thread_t    wait_thread;
  EtcPalThreadParams params = ETCPAL_THREAD_PARAMS_INIT;

  event_1_num_received = 0;
  event_2_num_received = 0;
  keep_running = true;

  TEST_ASSERT_EQUAL(etcpal_thread_create(&wait_thread, &params, multiple_event_wait_thread, NULL), kEtcPalErrOk);

  for (int i = 0; i < 2; ++i)
  {
    etcpal_event_group_set_bits(&event, EVENT_1);
    etcpal_thread_sleep(50);
    etcpal_event_group_set_bits(&event, EVENT_2);
    etcpal_thread_sleep(50);
    etcpal_event_group_set_bits(&event, EVENT_1 | EVENT_2);
    etcpal_thread_sleep(50);
  }

  keep_running = false;
  TEST_ASSERT_EQUAL(etcpal_thread_join(&wait_thread), kEtcPalErrOk);
  TEST_ASSERT_EQUAL(event_1_num_received, 4);
  TEST_ASSERT_EQUAL(event_2_num_received, 4);
}

static int num_events_received;

static void unrequested_bits_test_thread(void* arg)
{
  ETCPAL_UNUSED_ARG(arg);

  for (int i = 0; i < 2; ++i)
  {
    etcpal_event_bits_t result = etcpal_event_group_wait(&event, 0x88, ETCPAL_EVENT_GROUP_WAIT_FOR_ALL);
    if (result != 0)
      ++num_events_received;
  }
}

TEST(event_group_integration, wait_does_not_return_on_unrequested_bits)
{
  etcpal_thread_t    wait_thread;
  EtcPalThreadParams params = ETCPAL_THREAD_PARAMS_INIT;
  TEST_ASSERT_EQUAL(etcpal_thread_create(&wait_thread, &params, unrequested_bits_test_thread, NULL), kEtcPalErrOk);

  num_events_received = 0;

  etcpal_event_group_set_bits(&event, 0x77);
  etcpal_event_group_set_bits(&event, 0x80);
  etcpal_event_group_set_bits(&event, 0x88);
  etcpal_thread_sleep(10);
  TEST_ASSERT_EQUAL(num_events_received, 1);
  etcpal_event_group_set_bits(&event, 0x88);

  TEST_ASSERT_EQUAL(etcpal_thread_join(&wait_thread), kEtcPalErrOk);
}

TEST_GROUP_RUNNER(event_group_integration)
{
  RUN_TEST_CASE(event_group_integration, one_wait_one_signal_one_event);
  RUN_TEST_CASE(event_group_integration, one_wait_one_signal_multiple_events);
  RUN_TEST_CASE(event_group_integration, wait_does_not_return_on_unrequested_bits);
}
