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
#include "lwpa/timer.h"
#include "gtest/gtest.h"
#include <thread>

class TimerTest : public ::testing::Test
{
};

using namespace std::chrono_literals;

TEST_F(TimerTest, getms)
{
  uint32_t t1, t2;
  t1 = lwpa_getms();
  std::this_thread::sleep_for(10ms);
  t2 = lwpa_getms();

  ASSERT_NE(t1, 0u);
  ASSERT_NE(t2, 0u);
  ASSERT_GE((int32_t)t2 - (int32_t)t1, 0);
}

TEST_F(TimerTest, timeouts)
{
  LwpaTimer t1, t2;

  lwpa_timer_start(&t1, 0);
  lwpa_timer_start(&t2, 100);

  // A timer with a timeout of 0 should start expired.
  ASSERT_TRUE(lwpa_timer_isexpired(&t1));

  // The nonzero timeout should not be expired yet.
  ASSERT_FALSE(lwpa_timer_isexpired(&t2));

  std::this_thread::sleep_for(110ms);

  // Now it should.
  ASSERT_TRUE(lwpa_timer_isexpired(&t2));
  ASSERT_GE(lwpa_timer_elapsed(&t2), 100u);

  // Test resetting the timer.
  lwpa_timer_reset(&t2);
  ASSERT_FALSE(lwpa_timer_isexpired(&t2));

  // And test the timeout one more time.
  std::this_thread::sleep_for(110ms);
  ASSERT_TRUE(lwpa_timer_isexpired(&t2));
  ASSERT_GE(lwpa_timer_elapsed(&t2), 100u);
}
