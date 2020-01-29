/******************************************************************************
 * Copyright 2020 ETC Inc.
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
#include "unity_fixture.h"

extern "C" {

TEST_GROUP(etcpal_time_point);

TEST_SETUP(etcpal_time_point)
{
}

TEST_TEAR_DOWN(etcpal_time_point)
{
}

TEST(etcpal_time_point, placeholder)
{
  etcpal::TimePoint p{20};
  etcpal::TimePoint u = 20;
  TEST_ASSERT_TRUE(p == u);
}

TEST_GROUP(etcpal_cpp_timer);

TEST_SETUP(etcpal_cpp_timer)
{
}

TEST_TEAR_DOWN(etcpal_cpp_timer)
{
}

TEST(etcpal_cpp_timer, placeholder)
{
  etcpal::Timer t(20);
}

TEST_GROUP_RUNNER(etcpal_cpp_timer)
{
  RUN_TEST_CASE(etcpal_time_point, placeholder);
  RUN_TEST_CASE(etcpal_cpp_timer, placeholder);
}
}
