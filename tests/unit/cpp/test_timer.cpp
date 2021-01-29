/******************************************************************************
 * Copyright 2021 ETC Inc.
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
 * This file is a part of EtcPal. For more information, go to:
 * https://github.com/ETCLabs/EtcPal
 ******************************************************************************/

#include "etcpal/cpp/timer.h"

#include <chrono>
#include <limits>
#include "etcpal/common.h"
#include "etcpal/cpp/thread.h"
#include "unity_fixture.h"

extern "C" {

TEST_GROUP(etcpal_time_point);

TEST_SETUP(etcpal_time_point)
{
  TEST_ASSERT_EQUAL(kEtcPalErrOk, etcpal_init(ETCPAL_FEATURE_TIMERS));
}

TEST_TEAR_DOWN(etcpal_time_point)
{
  etcpal_deinit(ETCPAL_FEATURE_TIMERS);
}

TEST(etcpal_time_point, default_constructor_works)
{
  etcpal::TimePoint tp;
  TEST_ASSERT_EQUAL_UINT32(tp.value(), 0u);
}

TEST(etcpal_time_point, equality_operators_work)
{
  etcpal::TimePoint tp1(20);
  etcpal::TimePoint tp2(20);
  TEST_ASSERT_TRUE(tp1 == tp2);

  etcpal::TimePoint tp3(30);
  TEST_ASSERT_FALSE(tp1 == tp3);
  TEST_ASSERT_TRUE(tp1 != tp3);
}

TEST(etcpal_time_point, comparison_operators_work)
{
  etcpal::TimePoint tp1(20);
  etcpal::TimePoint tp2(30);

  TEST_ASSERT_TRUE(tp1 < tp2);
  TEST_ASSERT_FALSE(tp2 < tp1);

  TEST_ASSERT_TRUE(tp1 <= tp2);
  TEST_ASSERT_FALSE(tp2 <= tp1);

  TEST_ASSERT_FALSE(tp1 > tp2);
  TEST_ASSERT_TRUE(tp2 > tp1);

  TEST_ASSERT_FALSE(tp1 >= tp2);
  TEST_ASSERT_TRUE(tp2 >= tp1);

  // Test some common boundary conditions
  tp1 = 0x7fffffff;
  tp2 = 0x80000000;
  TEST_ASSERT_TRUE(tp2 > tp1);
  TEST_ASSERT_TRUE(tp1 < tp2);

  tp1 = 0xffffffff;
  tp2 = 0;
  TEST_ASSERT_TRUE(tp2 > tp1);
  TEST_ASSERT_TRUE(tp1 < tp2);
}

TEST(etcpal_time_point, value_getter_works)
{
  etcpal::TimePoint tp1(0);
  TEST_ASSERT_EQUAL_UINT32(tp1.value(), 0u);

  etcpal::TimePoint tp2(std::numeric_limits<uint32_t>::max());
  TEST_ASSERT_EQUAL_UINT32(tp2.value(), std::numeric_limits<uint32_t>::max());
}

TEST(etcpal_time_point, incremental_operators_work)
{
  etcpal::TimePoint tp(0xffffff00);
  tp += 0xff;
  TEST_ASSERT_EQUAL_UINT32(tp.value(), 0xffffffffu);
  tp += 1;
  TEST_ASSERT_EQUAL_UINT32(tp.value(), 0u);

  tp -= 1;
  TEST_ASSERT_EQUAL_UINT32(tp.value(), 0xffffffffu);
  tp -= 0xff;
  TEST_ASSERT_EQUAL_UINT32(tp.value(), 0xffffff00u);
}

TEST(etcpal_time_point, operator_minus_works)
{
  etcpal::TimePoint tp1(20);
  etcpal::TimePoint tp2(30);
  TEST_ASSERT_EQUAL_INT32(tp2 - tp1, 10);
  TEST_ASSERT_EQUAL_INT32(tp1 - tp2, -10);

  tp1 = 0xffffffff;
  tp2 = 0;
  TEST_ASSERT_EQUAL_INT32(tp2 - tp1, 1);
  TEST_ASSERT_EQUAL_INT32(tp1 - tp2, -1);
}

TEST_GROUP(etcpal_cpp_timer);

TEST_SETUP(etcpal_cpp_timer)
{
  TEST_ASSERT_EQUAL(kEtcPalErrOk, etcpal_init(ETCPAL_FEATURE_TIMERS));
}

TEST_TEAR_DOWN(etcpal_cpp_timer)
{
  etcpal_deinit(ETCPAL_FEATURE_TIMERS);
}

TEST(etcpal_cpp_timer, get_interval_works)
{
  const etcpal::Timer t1(0);
  TEST_ASSERT_EQUAL_UINT32(t1.GetInterval(), 0u);

  const etcpal::Timer t2(0xfffffff0u);
  TEST_ASSERT_EQUAL_UINT32(t2.GetInterval(), 0xfffffff0u);
}

TEST(etcpal_cpp_timer, timers_report_expired_properly)
{
  etcpal::Timer t1(0);
  etcpal::Timer t2(20);

  // A timer with a timeout of 0 should start expired.
  TEST_ASSERT_TRUE(t1.IsExpired());

  // The nonzero timer should not be expired yet.
  TEST_ASSERT_FALSE(t2.IsExpired());

  etcpal::Thread::Sleep(30);
  TEST_ASSERT_TRUE(t2.IsExpired());
  TEST_ASSERT_GREATER_OR_EQUAL_UINT32(20u, t2.GetElapsed());
}

TEST(etcpal_cpp_timer, reset_works)
{
  etcpal::Timer t(20);
  etcpal::Thread::Sleep(30);
  TEST_ASSERT_TRUE(t.IsExpired());

  t.Reset();
  TEST_ASSERT_FALSE(t.IsExpired());
  etcpal::Thread::Sleep(30);
  TEST_ASSERT_TRUE(t.IsExpired());
}

TEST(etcpal_cpp_timer, duration_interval_works)
{
  etcpal::Timer t1(std::chrono::milliseconds(20));
  TEST_ASSERT_EQUAL_UINT32(t1.GetInterval(), 20u);

  TEST_ASSERT_FALSE(t1.IsExpired());
  etcpal::Thread::Sleep(30);
  TEST_ASSERT_TRUE(t1.IsExpired());

  etcpal::Timer t2;
  t2.Start(std::chrono::milliseconds(20));
  TEST_ASSERT_EQUAL_UINT32(t1.GetInterval(), 20u);

  TEST_ASSERT_FALSE(t2.IsExpired());
  etcpal::Thread::Sleep(30);
  TEST_ASSERT_TRUE(t2.IsExpired());
}

TEST_GROUP_RUNNER(etcpal_cpp_timer)
{
  RUN_TEST_CASE(etcpal_time_point, default_constructor_works);
  RUN_TEST_CASE(etcpal_time_point, equality_operators_work);
  RUN_TEST_CASE(etcpal_time_point, comparison_operators_work);
  RUN_TEST_CASE(etcpal_time_point, value_getter_works);
  RUN_TEST_CASE(etcpal_time_point, incremental_operators_work);
  RUN_TEST_CASE(etcpal_time_point, operator_minus_works);
  RUN_TEST_CASE(etcpal_cpp_timer, get_interval_works);
  RUN_TEST_CASE(etcpal_cpp_timer, timers_report_expired_properly);
  RUN_TEST_CASE(etcpal_cpp_timer, reset_works);
  RUN_TEST_CASE(etcpal_cpp_timer, duration_interval_works);
}
}
