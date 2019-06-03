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

bool test_rwlock_create_destroy()
{
  lwpa_rwlock_t rwlock1, rwlock2, rwlock3;
  uint32_t start_time;
  bool ok;

  ok = lwpa_rwlock_create(&rwlock1);
  if (ok)
    ok = lwpa_rwlock_create(&rwlock2);
  if (ok)
    ok = lwpa_rwlock_create(&rwlock3);

  /* Should be able to take and give the write locks in any order */
  if (ok)
    ok = lwpa_rwlock_writelock(&rwlock1, LWPA_WAIT_FOREVER);
  if (ok)
    ok = lwpa_rwlock_writelock(&rwlock2, LWPA_WAIT_FOREVER);
  if (ok)
    ok = lwpa_rwlock_writelock(&rwlock3, LWPA_WAIT_FOREVER);
  if (ok)
  {
    lwpa_rwlock_writeunlock(&rwlock2);
    lwpa_rwlock_writeunlock(&rwlock3);
    lwpa_rwlock_writeunlock(&rwlock1);
  }
  if (ok)
    ok = lwpa_rwlock_writelock(&rwlock1, LWPA_WAIT_FOREVER);
  if (ok)
    ok = lwpa_rwlock_writelock(&rwlock2, LWPA_WAIT_FOREVER);
  if (ok)
    ok = lwpa_rwlock_writelock(&rwlock3, LWPA_WAIT_FOREVER);
  if (ok)
  {
    lwpa_rwlock_writeunlock(&rwlock1);
    lwpa_rwlock_writeunlock(&rwlock3);
    lwpa_rwlock_writeunlock(&rwlock2);
  }
  if (ok)
  {
    int i;
    for (i = 0; i < 100; ++i)
    {
      /* Should be able to get as many read locks as we want at once */
      ok = lwpa_rwlock_readlock(&rwlock1, LWPA_WAIT_FOREVER);
      if (!ok)
        break;
    }
  }
  if (ok)
  {
    start_time = getms();
    /* Write lock should fail on a read/write lock with readers */
    ok = !lwpa_rwlock_writelock(&rwlock1, 100);
  }
  if (ok)
    /* It should wait for at least the timeout specified, minus up to one tick. */
    ok = ((getms() - start_time) >= (100 - _time_get_resolution()));
  if (ok)
  {
    int i;
    for (i = 0; i < 100; ++i)
      lwpa_rwlock_readunlock(&rwlock1);
    /* When there are no more readers, write lock should succeed */
    ok = lwpa_rwlock_writelock(&rwlock1, LWPA_WAIT_FOREVER);
  }
  if (ok)
  {
    start_time = getms();
    /* Write lock should fail when there is already a write lock */
    ok = !lwpa_rwlock_writelock(&rwlock1, 100);
  }
  if (ok)
    /* It should wait for at least the timeout specified, minus up to one tick. */
    ok = ((getms() - start_time) >= (100 - _time_get_resolution()));
  if (ok)
  {
    lwpa_rwlock_writeunlock(&rwlock1);
    lwpa_rwlock_destroy(&rwlock1);
    lwpa_rwlock_destroy(&rwlock2);
    lwpa_rwlock_destroy(&rwlock3);
    /* Read lock should fail on a destroyed read/write lock. */
    ok = !lwpa_rwlock_readlock(&rwlock1, LWPA_WAIT_FOREVER);
  }
  if (ok)
    /* Write lock should fail on a destroyed read/write lock. */
    ok = !lwpa_rwlock_writelock(&rwlock2, LWPA_WAIT_FOREVER);
  if (ok)
    /* Write lock should fail on a destroyed read/write lock. */
    ok = !lwpa_rwlock_writelock(&rwlock3, LWPA_WAIT_FOREVER);
  return ok;
}

#define RWLOCK_TEST_NUM_WRITE_THREADS 10
#define RWLOCK_TEST_NUM_READ_THREADS 1
#define RWLOCK_TEST_NUM_THREADS (RWLOCK_TEST_NUM_WRITE_THREADS + RWLOCK_TEST_NUM_READ_THREADS)
#define RWLOCK_TEST_NUM_ITERATIONS 10000
static int g_shared_var;
struct thread_data
{
  lwpa_rwlock_t* rwlock;
  int thread_num;
  bool done;
  bool passed;
};

void write_test_thread(uint32_t initial_data)
{
  struct thread_data* td = (struct thread_data*)initial_data;
  int i;

  for (i = 0; i < RWLOCK_TEST_NUM_ITERATIONS; ++i)
  {
    lwpa_rwlock_writelock(td->rwlock, LWPA_WAIT_FOREVER);
    ++g_shared_var;
    lwpa_rwlock_writeunlock(td->rwlock);
  }
  td->passed = true;
  td->done = true;
}

void read_test_thread(uint32_t initial_data)
{
  struct thread_data* td = (struct thread_data*)initial_data;
  int i;

  td->passed = true;
  for (i = 0; i < 10; ++i)
  {
    int val;
    lwpa_rwlock_readlock(td->rwlock, LWPA_WAIT_FOREVER);
    val = g_shared_var;
    _time_delay(50);
    /* Make sure the value hasn't changed. */
    if (val != g_shared_var)
    {
      td->passed = false;
      break;
    }
    lwpa_rwlock_readunlock(td->rwlock);
    _time_delay(50);
  }
  td->done = true;
}

/* Test the actual read-write lock functionality. Start a number of time-slice threads at the same
 * priority, and have them all increment the same variable inside a write lock. Meanwhile, a
 * higher-priority task periodically reads the variable. At the end of the test, the value of the
 * variable should be exactly the number of threads times the number of times each thread
 * incremented the variable.
 *
 * Yes, this test isn't guaranteed to fail if the read-write locks don't work. But it's still a good
 * test to run. A test on this platform where the lock lines were commented showed failure very
 * reliably. */
bool test_rwlock_threads()
{
  TASK_TEMPLATE_STRUCT rtstruct;
  int i;
  bool ok = true;
  lwpa_rwlock_t rwlock;
  struct thread_data threads[RWLOCK_TEST_NUM_THREADS];

  if (!lwpa_rwlock_create(&rwlock))
    return false;

  rtstruct.TASK_ADDRESS = write_test_thread;
  rtstruct.TASK_NAME = "lock_write_test";
  rtstruct.TASK_PRIORITY = 11;
  rtstruct.TASK_STACKSIZE = 1000;
  rtstruct.TASK_ATTRIBUTES = MQX_TIME_SLICE_TASK;
  rtstruct.DEFAULT_TIME_SLICE = 1;

  for (i = 0; i < RWLOCK_TEST_NUM_WRITE_THREADS; ++i)
  {
    _task_id id;
    threads[i].rwlock = &rwlock;
    threads[i].thread_num = i;
    threads[i].done = false;
    threads[i].passed = false;
    rtstruct.CREATION_PARAMETER = (uint32_t)&threads[i];
    id = _task_create(0, 0, (uint32_t)&rtstruct);
    if (id == MQX_NULL_TASK_ID)
      return false;
  }

  rtstruct.TASK_ADDRESS = read_test_thread;
  rtstruct.TASK_NAME = "lock_read_test";
  rtstruct.TASK_PRIORITY = 10;
  rtstruct.TASK_ATTRIBUTES = 0;
  rtstruct.DEFAULT_TIME_SLICE = 0;

  for (i = 0; i < RWLOCK_TEST_NUM_READ_THREADS; ++i)
  {
    _task_id id;
    int j = i + RWLOCK_TEST_NUM_WRITE_THREADS;
    threads[j].rwlock = &rwlock;
    threads[j].thread_num = j;
    threads[j].done = false;
    threads[j].passed = false;
    rtstruct.CREATION_PARAMETER = (uint32_t)&threads[j];
    id = _task_create(0, 0, (uint32_t)&rtstruct);
    if (id == MQX_NULL_TASK_ID)
      return false;
  }

  while (1)
  {
    bool keep_running = false;
    _time_delay(100);
    for (i = 0; i < RWLOCK_TEST_NUM_THREADS; ++i)
    {
      if (!threads[i].done)
      {
        keep_running = true;
        break;
      }
    }

    if (!keep_running)
      break;
  }
  for (i = 0; i < RWLOCK_TEST_NUM_THREADS; ++i)
  {
    if (!threads[i].passed)
    {
      ok = false;
      break;
    }
  }

  if (ok)
  {
    ok = (g_shared_var == (RWLOCK_TEST_NUM_WRITE_THREADS * RWLOCK_TEST_NUM_ITERATIONS));
  }

  lwpa_rwlock_destroy(&rwlock);
  return ok;
}

bool test_rwlock()
{
  bool ok;

  watchdog_kick();
  OUTPUT_TEST_BEGIN("rwlock_create_destroy");
  OUTPUT_TEST_RESULT((ok = test_rwlock_create_destroy()));
  if (ok)
  {
    watchdog_kick();
    OUTPUT_TEST_BEGIN("rwlock_threads");
    OUTPUT_TEST_RESULT((ok = test_rwlock_threads()));
  }
  return ok;
}
