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
#include "lwpatest.h"
#include "lwpa_lock.h"

/* Test the basic signal functions. */
bool test_signal_create_destroy()
{
  bool ok;
  uint32_t start_time;
  lwpa_signal_t signal1, signal2;

  ok = lwpa_signal_create(&signal1);
  if (ok)
    ok = lwpa_signal_create(&signal2);
  if (ok)
    /* Signals shouldn't be created in the signaled state */
    ok = !lwpa_signal_wait(&signal1, 0);
  if (ok)
  {
    /* Wait should succeed on a signal that has previously been posted. */
    lwpa_signal_post(&signal1);
    ok = lwpa_signal_wait(&signal1, LWPA_WAIT_FOREVER);
  }
  if (ok)
  {
    start_time = getms();
    /* Wait should time out on a signal that hasn't been posted. */
    ok = !lwpa_signal_wait(&signal2, 100);
  }
  if (ok)
    /* It should wait for at least the timeout specified, minus up to one tick. */
    ok = (getms() - start_time >= (100 - _time_get_resolution()));
  if (ok)
  {
    /* Wait should succeed on a signal that has been posted. */
    lwpa_signal_post(&signal2);
    ok = lwpa_signal_wait(&signal2, LWPA_WAIT_FOREVER);
  }
  if (ok)
  {
    lwpa_signal_destroy(&signal1);
    lwpa_signal_destroy(&signal2);
    /* Wait should fail on a destroyed signal */
    ok = !lwpa_signal_wait(&signal1, LWPA_WAIT_FOREVER);
  }
  if (ok)
    ok = !lwpa_signal_wait(&signal2, LWPA_WAIT_FOREVER);
  return ok;
}

struct thread_data
{
  lwpa_signal_t *signal;
  int thread_num;
  bool passed;
};

void signal_test_thread(uint32_t initial_data)
{
  struct thread_data *td = (struct thread_data *)initial_data;
  int i;

  for (i = 0; i < 3; ++i)
    lwpa_signal_wait(td->signal, LWPA_WAIT_FOREVER);
  td->passed = true;
}

/* Two tasks are created with higher priority than the main task. They wait on the same signal 3
 * times. Each post of the signal should wake up only one of the tasks, so 6 posts should end both
 * threads. */
bool test_signal_threads()
{
  TASK_TEMPLATE_STRUCT mtstruct;
  lwpa_signal_t signal;
  struct thread_data threads[2];
  int i;

  if (!lwpa_signal_create(&signal))
    return false;

  mtstruct.TASK_ADDRESS = signal_test_thread;
  mtstruct.TASK_NAME = "signal_test";
  mtstruct.TASK_PRIORITY = 8;
  mtstruct.TASK_STACKSIZE = 1000;
  mtstruct.TASK_ATTRIBUTES = 0;
  mtstruct.DEFAULT_TIME_SLICE = 0;

  for (i = 0; i < 2; ++i)
  {
    _task_id id;
    threads[i].signal = &signal;
    threads[i].thread_num = i;
    threads[i].passed = false;
    mtstruct.CREATION_PARAMETER = (uint32_t)&threads[i];
    id = _task_create(0, 0, (uint32_t)&mtstruct);
    if (id == MQX_NULL_TASK_ID)
      return false;
  }

  for (i = 0; i < 6; ++i)
    lwpa_signal_post(&signal);

  while (1)
  {
    bool keep_running = false;
    for (i = 0; i < 2; ++i)
      if (!threads[i].passed)
      {
        keep_running = true;
        break;
      }

    if (!keep_running)
      break;
    _time_delay(100);
  }

  lwpa_signal_destroy(&signal);
  /* If we got here, the threads reported success; the failure mode is a watchdog timeout. */
  return true;
}

bool test_signal()
{
  bool ok;

  watchdog_kick();
  OUTPUT_TEST_BEGIN("signal_create_destroy");
  OUTPUT_TEST_RESULT((ok = test_signal_create_destroy()));
  if (ok)
  {
    watchdog_kick();
    OUTPUT_TEST_BEGIN("signal_threads");
    OUTPUT_TEST_RESULT((ok = test_signal_threads()));
  }
  return ok;
}
