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
#include "lwpa_lock.h"
#include "gtest/gtest.h"
#include <cstddef>
#include <Windows.h>
#include <process.h>

class SignalTest : public ::testing::Test
{
protected:
  SignalTest()
  {
    timeBeginPeriod(1);
  }

  virtual ~SignalTest()
  {
    timeEndPeriod(1);
  }

public:
  // For general usage
  lwpa_signal_t signal;
};

TEST_F(SignalTest, create_destroy)
{
  // Basic creation.
  ASSERT_TRUE(lwpa_signal_create(&signal));

  // Signals shouldn't be created in the signaled state.
  ASSERT_FALSE(lwpa_signal_wait(&signal, 0));

  lwpa_signal_post(&signal);
  ASSERT_TRUE(lwpa_signal_wait(&signal, LWPA_WAIT_FOREVER));

  DWORD start_time = timeGetTime();
  ASSERT_FALSE(lwpa_signal_wait(&signal, 100));
  ASSERT_GE(timeGetTime() - start_time, 99u);

  // Take should fail on a destroyed signal.
  lwpa_signal_post(&signal);
  lwpa_signal_destroy(&signal);
  ASSERT_FALSE(lwpa_signal_wait(&signal, LWPA_WAIT_FOREVER));
}

static unsigned __stdcall
signal_test_thread(void *pthread)
{
  SignalTest *mt = static_cast<SignalTest *>(pthread);
  if (mt)
  {
    for (size_t i = 0; i < 3; ++i)
      lwpa_signal_wait(&mt->signal, LWPA_WAIT_FOREVER);
    return 0;
  }
  return 1;
}

// Two tasks are created with higher priority than the main task. They wait on
// the same signal 3 times. Each post of the signal should wake up only one of
// the tasks, so 6 posts should end both threads.
TEST_F(SignalTest, threads)
{
  ASSERT_TRUE(lwpa_signal_create(&signal));
  HANDLE *threads = new HANDLE[2];
  ASSERT_TRUE(threads != NULL);
  for (size_t i = 0; i < 2; ++i)
  {
    HANDLE thread_id = reinterpret_cast<HANDLE>(_beginthreadex(NULL, 0,
                                                  signal_test_thread, this, 0,
                                                  NULL));
    ASSERT_TRUE(thread_id != NULL);
    threads[i] = thread_id;
  }

  for (size_t i = 0; i < 6; ++i)
  {
    Sleep(10);
    lwpa_signal_post(&signal);
  }

  ASSERT_EQ(WaitForMultipleObjects(2, threads, TRUE, INFINITE), WAIT_OBJECT_0);
  delete[] threads;
  lwpa_signal_destroy(&signal);
}
