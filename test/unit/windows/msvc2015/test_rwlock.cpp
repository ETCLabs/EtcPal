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

class RwlockTest : public ::testing::Test
{
protected:
  RwlockTest()
    : shared_var(0)
    , read_thread_pass(false)
  {
    timeBeginPeriod(1);
  }

  virtual ~RwlockTest()
  {
    timeEndPeriod(1);
  }

public:
  // Constants
  static const int RWLOCK_TEST_NUM_WRITE_THREADS = 10;
  static const int RWLOCK_TEST_NUM_ITERATIONS = 10000;

  // For general usage
  lwpa_rwlock_t rwlock;
  
  // For thread test
  int shared_var;
  bool read_thread_pass;
};

TEST_F(RwlockTest, create_destroy)
{
  // Basic creation and taking ownership.
  ASSERT_TRUE(lwpa_rwlock_create(&rwlock));
  for (size_t i = 0; i < 100; ++i)
    ASSERT_TRUE(lwpa_rwlock_readlock(&rwlock, LWPA_WAIT_FOREVER));

  // Write lock should fail if there are readers
  DWORD start_time = timeGetTime();
  ASSERT_FALSE(lwpa_rwlock_writelock(&rwlock, 100));
  // It should wait for at least the timeout specified, minus up to one ms
  ASSERT_GE(timeGetTime() - start_time, 99u);

  // When there are no more readers, write lock should succeed
  for (size_t i = 0; i < 100; ++i)
    lwpa_rwlock_readunlock(&rwlock);
  ASSERT_TRUE(lwpa_rwlock_writelock(&rwlock, LWPA_WAIT_FOREVER));

  // On Windows, take succeeds when taking a mutex again from the same thread.
  ASSERT_TRUE(lwpa_rwlock_writelock(&rwlock, LWPA_WAIT_FOREVER));

  // Take should fail on a destroyed rwlock.
  lwpa_rwlock_writeunlock(&rwlock);
  lwpa_rwlock_destroy(&rwlock);
  ASSERT_FALSE(lwpa_rwlock_writelock(&rwlock, LWPA_WAIT_FOREVER));
}

static unsigned __stdcall
write_test_thread(void *pthread)
{
  RwlockTest *rwlt = static_cast<RwlockTest *>(pthread);
  if (rwlt)
  {
    for (size_t i = 0; i < RwlockTest::RWLOCK_TEST_NUM_ITERATIONS; ++i)
    {
      lwpa_rwlock_writelock(&rwlt->rwlock, LWPA_WAIT_FOREVER);
      ++rwlt->shared_var;
      lwpa_rwlock_writeunlock(&rwlt->rwlock);
      // Had to insert an artificial delay to get it to fail reliably when the
      // mutexes don't work. This ensures that each thread runs for long enough
      // to get time-sliced multiple times.
      for (volatile size_t j = 0; j < 100; ++j) ;
    }
    return 0;
  }
  return 1;
}

static unsigned __stdcall
read_test_thread(void *pthread)
{
  RwlockTest *rwlt = static_cast<RwlockTest *>(pthread);
  if (rwlt)
  {
    rwlt->read_thread_pass = true;
    for (int i = 0; i < 10; ++i)
    {
      lwpa_rwlock_readlock(&rwlt->rwlock, LWPA_WAIT_FOREVER);
      int val = rwlt->shared_var;
      Sleep(5);
      // Make sure the value hasn't changed.
      if (val != rwlt->shared_var)
      {
        rwlt->read_thread_pass = false;
        break;
      }
      lwpa_rwlock_readunlock(&rwlt->rwlock);
      Sleep(5);
    }
    return 0;
  }
  return 1;
}

// Test the actual read-write lock functionality. Start a number of time-slice
// threads at the same priority, and have them all increment the same variable
// inside a write lock. Meanwhile, a higher-priority task periodically reads
// the variable. At the end of the test, the value of the variable should be
// exactly the number of threads times the number of times each thread
// incremented the variable.
//
// Yes, this test isn't guaranteed to fail if the read-write locks don't work.
// But it's still a good test to run. A test on this platform where the lock
// lines were commented showed failure very reliably.
TEST_F(RwlockTest, threads)
{
  ASSERT_TRUE(lwpa_rwlock_create(&rwlock));
  HANDLE *threads = new HANDLE[RWLOCK_TEST_NUM_WRITE_THREADS + 1];
  ASSERT_TRUE(threads != NULL);
  HANDLE thread_id = reinterpret_cast<HANDLE>(_beginthreadex(NULL, 0,
                                                read_test_thread, this, 0,
                                                NULL));
  ASSERT_TRUE(thread_id != NULL);
  threads[0] = thread_id;
  for (size_t i = 0; i < RWLOCK_TEST_NUM_WRITE_THREADS; ++i)
  {
    thread_id = reinterpret_cast<HANDLE>(_beginthreadex(NULL, 0,
                                            write_test_thread, this, 0, NULL));
    ASSERT_TRUE(thread_id != NULL);
    threads[i + 1] = thread_id;
  }

  ASSERT_EQ(WaitForMultipleObjects(RWLOCK_TEST_NUM_WRITE_THREADS + 1, threads,
                                   TRUE, INFINITE), WAIT_OBJECT_0);
  ASSERT_TRUE(read_thread_pass);
  ASSERT_EQ(shared_var, (RWLOCK_TEST_NUM_WRITE_THREADS *
                         RWLOCK_TEST_NUM_ITERATIONS));
  delete[] threads;
  lwpa_rwlock_destroy(&rwlock);
}
