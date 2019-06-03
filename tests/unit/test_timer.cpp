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

#include "lwpa/common.h"
// Do some C-style mocking
#include "lwpa/int.h"
extern "C" uint32_t lwpa_getms();
#define lwpa_getms lwpa_fake_getms
#include "lwpa/timer.h"

#include "gtest/gtest.h"
#include <thread>

class TimerTest : public ::testing::Test
{
  void SetUp() override { ASSERT_EQ(kLwpaErrOk, lwpa_init(LWPA_FEATURE_TIMERS)); }
  void TearDown() override { lwpa_deinit(LWPA_FEATURE_TIMERS); }
};

static bool mocking_getms;
static uint32_t getms_returnval;

// The getms mock
extern "C" uint32_t lwpa_fake_getms()
{
  if (mocking_getms)
  {
    return getms_returnval;
  }
  else
  {
#undef lwpa_getms
    return lwpa_getms();
#define lwpa_getms lwpa_fake_getms
  }
}

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

TEST_F(TimerTest, wraparound)
{
  LwpaTimer t1;

  // Test the wraparound case by forcing a wraparound value returned from lwpa_getms()
  mocking_getms = true;
  getms_returnval = 0xfffffff0u;

  lwpa_timer_start(&t1, 0x20);

  // We've wrapped around but have not exceeded the interval yet
  getms_returnval = 0x0f;
  ASSERT_FALSE(lwpa_timer_isexpired(&t1));
  ASSERT_EQ(lwpa_timer_elapsed(&t1), 0x1fu);

  getms_returnval = 0x11;
  ASSERT_TRUE(lwpa_timer_isexpired(&t1));
}