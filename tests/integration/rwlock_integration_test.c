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
#include "lwpa/lock.h"
#include "unity_fixture.h"

#include "lwpa/thread.h"

#define NUM_WRITE_THREADS 10
#define NUM_ITERATIONS 10000

lwpa_rwlock_t rwlock;
int shared_var;
bool read_thread_pass;

static void write_test_thread(RwlockTest* fixture)
{
  if (fixture)
  {
    for (size_t i = 0; i < RwlockTest::RWLOCK_TEST_NUM_ITERATIONS; ++i)
    {
      lwpa_rwlock_writelock(&fixture->rwlock);
      ++fixture->shared_var;
      lwpa_rwlock_writeunlock(&fixture->rwlock);
      // Had to insert an artificial delay to get it to fail reliably when the mutexes don't work.
      // This ensures that each thread runs for long enough to get time-sliced multiple times.
      for (volatile size_t j = 0; j < 100; ++j)
        ;
    }
  }
}

using namespace std::chrono_literals;

static void read_test_thread(RwlockTest* fixture)
{
  if (fixture)
  {
    fixture->read_thread_pass = true;
    for (int i = 0; i < 10; ++i)
    {
      lwpa_rwlock_readlock(&fixture->rwlock);
      int val = fixture->shared_var;

      std::this_thread::sleep_for(5ms);

      // Make sure the value hasn't changed.
      if (val != fixture->shared_var)
      {
        fixture->read_thread_pass = false;
        break;
      }
      lwpa_rwlock_readunlock(&fixture->rwlock);
      std::this_thread::sleep_for(5ms);
    }
  }
}

// Test the actual read-write lock functionality. Start a number of threads and have them all
// increment the same variable inside a write lock. Meanwhile, another thread periodically reads the
// variable. At the end of the test, the value of the variable should be exactly the number of
// threads times the number of times each thread incremented the variable.
//
// Yes, this test isn't guaranteed to fail if the read-write locks don't work. But it's still a good
// test to run. Tests on several platform where the lock lines were commented showed failure very
// reliably.
TEST_F(RwlockTest, threads)
{
  ASSERT_TRUE(lwpa_rwlock_create(&rwlock));

  std::vector<std::thread> threads;
  threads.reserve(RWLOCK_TEST_NUM_WRITE_THREADS + 1);

  std::thread read_thread(read_test_thread, this);
  ASSERT_TRUE(read_thread.joinable());
  threads.push_back(std::move(read_thread));

  for (size_t i = 0; i < RWLOCK_TEST_NUM_WRITE_THREADS; ++i)
  {
    std::thread write_thread(write_test_thread, this);
    ASSERT_TRUE(write_thread.joinable());
    threads.push_back(std::move(write_thread));
  }

  for (auto& thread : threads)
    thread.join();

  EXPECT_TRUE(read_thread_pass);
  EXPECT_EQ(shared_var, (RWLOCK_TEST_NUM_WRITE_THREADS * RWLOCK_TEST_NUM_ITERATIONS));

  lwpa_rwlock_destroy(&rwlock);
}
