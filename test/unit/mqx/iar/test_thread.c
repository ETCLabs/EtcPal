/******************************************************************************
 * Copyright 2018 ETC Inc.
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
#include "lwpatest.h"
#include "lwpa_thread.h"

static bool g_waitthread_run;

void wait_and_exit(void *param)
{
  while (g_waitthread_run)
    lwpa_thread_sleep(50);
}

/* Basic test of the three thread functions. */
bool test_thread_create_destroy()
{
  bool ok;
  uint32_t start_time;
  LwpaThreadParams params = {
      10,          /* thread_priority */
      1000,        /* stack_size */
      "cd_thread", /* thread_name */
      NULL         /* platform_data */
  };
  lwpa_thread_t wait_thread;

  g_waitthread_run = true;
  ok = lwpa_thread_create(&wait_thread, &params, wait_and_exit, NULL);
  if (ok)
  {
    start_time = getms();
    /* Stop should time out if the thread is still looping. */
    ok = !lwpa_thread_stop(&wait_thread, 100);
  }
  if (ok)
    /* It should wait for at least the timeout specified, minus up to one
     * tick. */
    ok = ((getms() - start_time) >= (100 - _time_get_resolution()));
  if (ok)
  {
    g_waitthread_run = false;
    /* Stop should work if the thread has exited. */
    ok = lwpa_thread_stop(&wait_thread, LWPA_WAIT_FOREVER);
  }
  return ok;
}

static volatile bool spin_task_run, oneshot_task_run;
static volatile bool spin_task_ran = false;
static volatile bool oneshot_task_ran = false;

void increment_and_spin(void *param)
{
  spin_task_ran = true;
  while (spin_task_run)
    ;
}

void oneshot(void *param)
{
  if (oneshot_task_run)
    oneshot_task_ran = true;
}

/* Test time slicing. One thread spins constantly; if time slicing works, the
 * second thread should get a chance to set its flag. Failure mode could be the
 * flag not being set or a full test timeout. */
bool test_thread_time_slice()
{
  bool ok;
  LwpaThreadParamsMqx mqx_params = {
      MQX_TIME_SLICE_TASK, /* task_attributes */
      1                    /* time_slice */
  };
  LwpaThreadParams params = {
      10,          /* thread_priority */
      1000,        /* stack_size */
      "ts_thread", /* thread_name */
      &mqx_params  /* platform_data */
  };
  lwpa_thread_t spin_task, oneshot_task;
  spin_task_run = true;
  oneshot_task_run = true;

  /* Start the spin task */
  ok = lwpa_thread_create(&spin_task, &params, increment_and_spin, NULL);
  /* Start the one-shot task */
  if (ok)
    ok = lwpa_thread_create(&oneshot_task, &params, oneshot, NULL);
  /* Give both tasks time to run */
  _time_delay(100);
  /* Stop the oneshot task */
  oneshot_task_run = false;
  lwpa_thread_stop(&oneshot_task, LWPA_WAIT_FOREVER);
  /* Stop the spin task */
  spin_task_run = false;
  lwpa_thread_stop(&spin_task, LWPA_WAIT_FOREVER);

  if (ok)
    /* Both tasks should have gotten a chance to run. */
    ok = (spin_task_ran && oneshot_task_ran);
  return ok;
}

static int shared_var;
static bool lp_thread_done, lp_thread_passed, hp_thread_done;

void low_priority_thread(void *param)
{
  lp_thread_passed = (shared_var == 1000);
  lp_thread_done = true;
}

void high_priority_thread(void *param)
{
  int i;
  for (i = 0; i < 1000; ++i)
  {
    int j;
    ++shared_var;
    /* Simulate some other work being done */
    for (j = 0; j < 5000; ++j)
      ;
  }
  hp_thread_done = true;
}

/* Test priorities. The high priority thread should be able to increment the
 * variable 1000 times before the low priority thread runs once, even though
 * the threads are time-sliced. */
bool test_thread_priorities()
{
  bool ok;
  LwpaThreadParamsMqx mqx_params = {
      MQX_TIME_SLICE_TASK, /* task_attributes */
      1                    /* time_slice */
  };
  LwpaThreadParams lp_params = {
      11,          /* thread_priority */
      1000,        /* stack_size */
      "lp_thread", /* thread_name */
      &mqx_params  /* platform_data */
  };
  LwpaThreadParams hp_params = {
      10,          /* thread_priority */
      1000,        /* stack_size */
      "hp_thread", /* thread_name */
      &mqx_params  /* platform_data */
  };
  lwpa_thread_t lp_thread, hp_thread;

  /* Start the low-priority thread */
  ok = lwpa_thread_create(&lp_thread, &lp_params, low_priority_thread, NULL);
  /* Start the high-priority thread */
  if (ok)
    ok = lwpa_thread_create(&hp_thread, &hp_params, high_priority_thread, NULL);
  while (!hp_thread_done || !lp_thread_done)
    /* Wait for both threads to finish. */
    _time_delay(50);
  /* Threads should already be finished, stop just cleans up */
  lwpa_thread_stop(&lp_thread, LWPA_WAIT_FOREVER);
  lwpa_thread_stop(&hp_thread, LWPA_WAIT_FOREVER);
  if (ok)
    /* The low-priority indicates that the shared value was as it expected. */
    ok = lp_thread_passed;
  return ok;
}

void test_thread()
{
  bool ok;

  OUTPUT_TEST_MODULE_BEGIN("thread");
  watchdog_kick();
  OUTPUT_TEST_BEGIN("thread_create_destroy");
  OUTPUT_TEST_RESULT((ok = test_thread_create_destroy()));
  if (ok)
  {
    watchdog_kick();
    OUTPUT_TEST_BEGIN("thread_time_slice");
    OUTPUT_TEST_RESULT((ok = test_thread_time_slice()));
  }
  if (ok)
  {
    watchdog_kick();
    OUTPUT_TEST_BEGIN("thread_priorities");
    OUTPUT_TEST_RESULT((ok = test_thread_priorities()));
  }
  OUTPUT_TEST_MODULE_END("thread", ok);
}
