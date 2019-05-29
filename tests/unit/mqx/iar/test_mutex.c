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

/* Basic test of the four mutex functions. */
bool test_mutex_create_destroy()
{
  bool ok;
  uint32_t start_time;
  lwpa_mutex_t mutex1, mutex2;

  ok = lwpa_mutex_create(&mutex1);
  if (ok)
    ok = lwpa_mutex_create(&mutex2);
  if (ok)
    ok = lwpa_mutex_take(&mutex1, LWPA_WAIT_FOREVER);
  if (ok)
    ok = lwpa_mutex_take(&mutex2, LWPA_WAIT_FOREVER);
  if (ok)
  {
    lwpa_mutex_give(&mutex2);
    lwpa_mutex_destroy(&mutex2);
    /* Take should fail on a destroyed mutex. */
    ok = !lwpa_mutex_take(&mutex2, LWPA_WAIT_FOREVER);
  }
  if (ok)
  {
    start_time = getms();
    /* Take should time out on an already-taken mutex. */
    ok = !lwpa_mutex_take(&mutex1, 100);
  }
  if (ok)
    /* It should wait for at least the timeout specified, minus up to one tick. */
    ok = ((getms() - start_time) >= (100 - _time_get_resolution()));
  if (ok)
  {
    lwpa_mutex_give(&mutex1);
    lwpa_mutex_destroy(&mutex1);
    /* Take should fail on a destroyed mutex. */
    ok = !lwpa_mutex_take(&mutex1, LWPA_WAIT_FOREVER);
  }
  return ok;
}

#define MUTEX_TEST_NUM_THREADS 10
#define MUTEX_TEST_NUM_ITERATIONS 10000
static bool mt_terminated[MUTEX_TEST_NUM_THREADS];
static int g_shared_var;
struct thread_data
{
  lwpa_mutex_t *mutex;
  int thread_num;
};

void mutex_test_thread(uint32_t initial_data)
{
  struct thread_data *td = (struct thread_data *)initial_data;
  int i;

  for (i = 0; i < MUTEX_TEST_NUM_ITERATIONS; ++i)
  {
    lwpa_mutex_take(td->mutex, LWPA_WAIT_FOREVER);
    ++g_shared_var;
    lwpa_mutex_give(td->mutex);
  }
  mt_terminated[td->thread_num] = true;
}

/* Test the actual mutex functionality. Start a number of time-slice threads at the same priority,
 * and have them all increment the same variable inside a mutex. At the end of the test, the value
 * of the variable should be exactly the number of threads times the number of times each thread
 * incremented the variable.
 *
 * Yes, this test isn't guaranteed to fail if the mutexes don't work. But it's still a good test to
 * run. A test on this platform where the mutex lines were commented showed failure very reliably.
 */
bool test_mutex_threads()
{
  TASK_TEMPLATE_STRUCT mtstruct;
  int i;
  lwpa_mutex_t mutex;
  struct thread_data threads[MUTEX_TEST_NUM_THREADS];

  if (!lwpa_mutex_create(&mutex))
    return false;

  mtstruct.TASK_ADDRESS = mutex_test_thread;
  mtstruct.TASK_NAME = "mutex_test";
  mtstruct.TASK_PRIORITY = 10;
  mtstruct.TASK_STACKSIZE = 1000;
  mtstruct.TASK_ATTRIBUTES = MQX_TIME_SLICE_TASK;
  mtstruct.DEFAULT_TIME_SLICE = 1;

  for (i = 0; i < MUTEX_TEST_NUM_THREADS; ++i)
  {
    _task_id id;

    threads[i].mutex = &mutex;
    threads[i].thread_num = i;
    mtstruct.CREATION_PARAMETER = (uint32_t)&threads[i];
    id = _task_create(0, 0, (uint32_t)&mtstruct);
    if (id == MQX_NULL_TASK_ID)
      return false;
  }

  while (1)
  {
    bool keep_running = false;
    _time_delay(100);
    for (i = 0; i < MUTEX_TEST_NUM_THREADS; ++i)
      if (!mt_terminated[i])
      {
        keep_running = true;
        break;
      }

    if (!keep_running)
      break;
  }

  lwpa_mutex_destroy(&mutex);
  return g_shared_var == (MUTEX_TEST_NUM_THREADS * MUTEX_TEST_NUM_ITERATIONS);
}

bool test_mutex()
{
  bool ok;

  watchdog_kick();
  OUTPUT_TEST_BEGIN("mutex_create_destroy");
  OUTPUT_TEST_RESULT((ok = test_mutex_create_destroy()));
  if (ok)
  {
    watchdog_kick();
    OUTPUT_TEST_BEGIN("mutex_threads");
    OUTPUT_TEST_RESULT((ok = test_mutex_threads()));
  }
  return ok;
}
