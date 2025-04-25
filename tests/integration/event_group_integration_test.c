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

#include "etcpal/event_group.h"

#include "etcpal/common.h"
#include "etcpal/mutex.h"
#include "etcpal/signal.h"
#include "etcpal/thread.h"
#include "unity_fixture.h"

#define NUM_ITERATIONS 3

static etcpal_event_group_t event = ETCPAL_EVENT_GROUP_INIT;

TEST_GROUP(event_group_integration);

TEST_SETUP(event_group_integration)
{
  TEST_ASSERT_TRUE(etcpal_event_group_create(&event));
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
    if (etcpal_event_group_wait(&event, 0x80, ETCPAL_EVENT_GROUP_AUTO_CLEAR) == 0x80)
      ++num_events_received;
  }
}

TEST(event_group_integration, one_wait_one_signal_one_event)
{
  etcpal_thread_t    wait_thread = ETCPAL_THREAD_INIT;
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
static int      event_1_num_received;
static int      event_2_num_received;
etcpal_signal_t wait_thread_stop_signal = ETCPAL_SIGNAL_INIT;

static void multiple_event_wait_thread(void* arg)
{
  ETCPAL_UNUSED_ARG(arg);

  do
  {
    etcpal_event_bits_t result =
        etcpal_event_group_timed_wait(&event, EVENT_1 | EVENT_2, ETCPAL_EVENT_GROUP_AUTO_CLEAR, 0);
    if (result & EVENT_1)
      ++event_1_num_received;
    if (result & EVENT_2)
      ++event_2_num_received;
    etcpal_thread_sleep(10);
  } while (!etcpal_signal_try_wait(&wait_thread_stop_signal));
}

TEST(event_group_integration, one_wait_one_signal_multiple_events)
{
  etcpal_thread_t    wait_thread = ETCPAL_THREAD_INIT;
  EtcPalThreadParams params = ETCPAL_THREAD_PARAMS_INIT;

  event_1_num_received = 0;
  event_2_num_received = 0;

  TEST_ASSERT_TRUE(etcpal_signal_create(&wait_thread_stop_signal));
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

  etcpal_signal_post(&wait_thread_stop_signal);

  TEST_ASSERT_EQUAL(etcpal_thread_join(&wait_thread), kEtcPalErrOk);
  etcpal_signal_destroy(&wait_thread_stop_signal);

  TEST_ASSERT_EQUAL(event_1_num_received, 4);
  TEST_ASSERT_EQUAL(event_2_num_received, 4);
}

static int            num_events_received;
static etcpal_mutex_t num_events_received_lock = ETCPAL_MUTEX_INIT;

static void unrequested_bits_test_thread(void* arg)
{
  ETCPAL_UNUSED_ARG(arg);

  for (int i = 0; i < 2; ++i)
  {
    etcpal_event_bits_t result =
        etcpal_event_group_wait(&event, 0x88, ETCPAL_EVENT_GROUP_WAIT_FOR_ALL | ETCPAL_EVENT_GROUP_AUTO_CLEAR);
    if (result != 0)
    {
      TEST_ASSERT_TRUE(etcpal_mutex_lock(&num_events_received_lock));
      ++num_events_received;
      etcpal_mutex_unlock(&num_events_received_lock);
    }
  }
}

TEST(event_group_integration, wait_does_not_return_on_unrequested_bits)
{
  etcpal_thread_t    wait_thread = ETCPAL_THREAD_INIT;
  EtcPalThreadParams params = ETCPAL_THREAD_PARAMS_INIT;

  num_events_received = 0;
  TEST_ASSERT_TRUE(etcpal_mutex_create(&num_events_received_lock));

  TEST_ASSERT_EQUAL(etcpal_thread_create(&wait_thread, &params, unrequested_bits_test_thread, NULL), kEtcPalErrOk);

  etcpal_event_group_set_bits(&event, 0x77);
  etcpal_event_group_set_bits(&event, 0x80);
  etcpal_event_group_set_bits(&event, 0x88);
  etcpal_thread_sleep(10);

  TEST_ASSERT_TRUE(etcpal_mutex_lock(&num_events_received_lock));
  TEST_ASSERT_EQUAL(num_events_received, 1);
  etcpal_mutex_unlock(&num_events_received_lock);

  etcpal_event_group_set_bits(&event, 0x88);
  TEST_ASSERT_EQUAL(etcpal_thread_join(&wait_thread), kEtcPalErrOk);

  etcpal_mutex_destroy(&num_events_received_lock);
}

TEST_GROUP_RUNNER(event_group_integration)
{
  RUN_TEST_CASE(event_group_integration, one_wait_one_signal_one_event);
  RUN_TEST_CASE(event_group_integration, one_wait_one_signal_multiple_events);
  RUN_TEST_CASE(event_group_integration, wait_does_not_return_on_unrequested_bits);
}
