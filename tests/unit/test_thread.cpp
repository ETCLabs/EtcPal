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
#include "lwpa/thread.h"
#include "gtest/gtest.h"
#include <chrono>
#include <thread>

static_assert(std::ratio_less_equal<std::chrono::high_resolution_clock::period, std::milli>::value,
              "This platform does not have access to a millisecond-resolution clock. This test cannot be run.");

class ThreadTest : public ::testing::Test
{
public:
  // For create_destroy
  volatile bool waitthread_run{false};

  // for time_slice
  volatile bool spin_task_run{false};
  volatile bool spin_task_ran{false};
  volatile bool oneshot_task_run{false};
  volatile bool oneshot_task_ran{false};
};

void wait_and_exit(void *param)
{
  ThreadTest *tt = static_cast<ThreadTest *>(param);
  if (tt)
  {
    while (tt->waitthread_run)
      lwpa_thread_sleep(5);
  }
}

// Basic test of the three thread functions.
TEST_F(ThreadTest, create_destroy)
{
  LwpaThreadParams params = {
      LWPA_THREAD_DEFAULT_PRIORITY,  // thread_priority
      LWPA_THREAD_DEFAULT_STACK,     // thread_stack
      "cd_thread",                   // thread_name
      NULL                           // platform_data
  };
  lwpa_thread_t wait_thread;

  waitthread_run = true;
  ASSERT_TRUE(lwpa_thread_create(&wait_thread, &params, wait_and_exit, this));

  // Stop should time out if the thread is still looping.
  auto start_time = std::chrono::high_resolution_clock::now();

  ASSERT_FALSE(lwpa_thread_stop(&wait_thread, 100));
  // It should wait for at least the timeout specified, minus up to one ms.
  auto time_taken =
      std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start_time);
  ASSERT_GE(time_taken.count(), 99);

  // Stop should work if the thread has exited.
  waitthread_run = false;
  ASSERT_TRUE(lwpa_thread_stop(&wait_thread, LWPA_WAIT_FOREVER));
}

void increment_and_spin(void *param)
{
  ThreadTest *tt = static_cast<ThreadTest *>(param);
  if (tt)
  {
    tt->spin_task_ran = true;
    while (tt->spin_task_run)
      ;
  }
}

void oneshot(void *param)
{
  ThreadTest *tt = static_cast<ThreadTest *>(param);
  if (tt)
  {
    if (tt->oneshot_task_run)
      tt->oneshot_task_ran = true;
  }
}

using namespace std::chrono_literals;

// Test time slicing. One thread spins constantly; if time slicing works, the second thread should
// get a chance to set its flag. Failure mode could be the flag not being set or a full test timeout.
TEST_F(ThreadTest, time_slice)
{
  LwpaThreadParams params = {
      LWPA_THREAD_DEFAULT_PRIORITY,  // thread_priority
      LWPA_THREAD_DEFAULT_STACK,     // thread_stack
      "ts_thread",                   // thread_name
      NULL                           // platform_data
  };
  lwpa_thread_t spin_task, oneshot_task;
  spin_task_run = true;
  oneshot_task_run = true;

  // Create the spin task.
  ASSERT_TRUE(lwpa_thread_create(&spin_task, &params, increment_and_spin, this));
  // Create the oneshot task.
  ASSERT_TRUE(lwpa_thread_create(&oneshot_task, &params, oneshot, this));
  // Give both tasks time to run.
  std::this_thread::sleep_for(100ms);
  // Stop the tasks
  oneshot_task_run = false;
  ASSERT_TRUE(lwpa_thread_stop(&oneshot_task, LWPA_WAIT_FOREVER));
  spin_task_run = false;
  ASSERT_TRUE(lwpa_thread_stop(&spin_task, LWPA_WAIT_FOREVER));
  ASSERT_TRUE(spin_task_ran);
  ASSERT_TRUE(oneshot_task_ran);
}
