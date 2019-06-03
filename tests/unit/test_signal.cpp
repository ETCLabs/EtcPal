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
#include "gtest/gtest.h"
#include <cstddef>
#include <vector>
#include <thread>
#include <chrono>
#include <utility>

static_assert(std::ratio_less_equal<std::chrono::high_resolution_clock::period, std::milli>::value,
              "This platform does not have access to a millisecond-resolution clock. This test cannot be run.");

class SignalTest : public ::testing::Test
{
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

  auto start_time = std::chrono::high_resolution_clock::now();
  // Signal wait should fail if not signaled
  ASSERT_FALSE(lwpa_signal_wait(&signal, 100));
  // It should wait for at least the timeout specified, minus up to one ms
  auto time_taken =
      std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start_time);
  ASSERT_GE(time_taken.count(), 99);

  // Take should fail on a destroyed signal.
  lwpa_signal_post(&signal);
  lwpa_signal_destroy(&signal);
  ASSERT_FALSE(lwpa_signal_wait(&signal, LWPA_WAIT_FOREVER));
}

static void signal_test_thread(SignalTest* fixture)
{
  if (fixture)
  {
    for (size_t i = 0; i < 3; ++i)
      lwpa_signal_wait(&fixture->signal, LWPA_WAIT_FOREVER);
  }
}

using namespace std::chrono_literals;

// Two threads are created. They wait on the same signal 3 times. Each post of the signal should
// wake up only one of the threads, so 6 posts should end both threads.
TEST_F(SignalTest, threads)
{
  ASSERT_TRUE(lwpa_signal_create(&signal));

  std::vector<std::thread> threads;
  threads.reserve(2);

  for (size_t i = 0; i < 2; ++i)
  {
    std::thread thread(signal_test_thread, this);
    ASSERT_TRUE(thread.joinable());
    threads.push_back(std::move(thread));
  }

  for (size_t i = 0; i < 6; ++i)
  {
    std::this_thread::sleep_for(10ms);
    lwpa_signal_post(&signal);
  }

  for (auto& thread : threads)
    thread.join();

  lwpa_signal_destroy(&signal);
}
