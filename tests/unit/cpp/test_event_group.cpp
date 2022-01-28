/******************************************************************************
 * Copyright 2022 ETC Inc.
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

#include "etcpal/cpp/event_group.h"

#include "unity_fixture.h"

extern "C" {
TEST_GROUP(etcpal_cpp_event_group);

TEST_SETUP(etcpal_cpp_event_group)
{
}

TEST_TEAR_DOWN(etcpal_cpp_event_group)
{
}

TEST(etcpal_cpp_event_group, create_and_destroy_works)
{
  etcpal::EventGroup eg;
  TEST_ASSERT_EQUAL(eg.GetBits(), 0);
  TEST_ASSERT_EQUAL(eg.TryWait(0xff), 0);
}

TEST(etcpal_cpp_event_group, wait_and_set_works)
{
  etcpal::EventGroup eg;
  eg.SetBits(0x2);
  TEST_ASSERT_EQUAL(eg.Wait(0x2), 0x2);
}

TEST(etcpal_cpp_event_group, auto_clear_works)
{
  etcpal::EventGroup eg;
  eg.SetBits(0x2);
  TEST_ASSERT_EQUAL(eg.Wait(0x2, ETCPAL_EVENT_GROUP_AUTO_CLEAR), 0x2);
  TEST_ASSERT_EQUAL(eg.TryWait(0x2), 0);
}

TEST(etcpal_cpp_event_group, non_auto_clear_works)
{
  etcpal::EventGroup eg;
  eg.SetBits(0x2);
  TEST_ASSERT_EQUAL(eg.Wait(0x2), 0x2);
  TEST_ASSERT_EQUAL(eg.Wait(0x2), 0x2);
}

TEST(etcpal_cpp_event_group, get_bits_works)
{
  etcpal::EventGroup eg;
  eg.SetBits(0x3);
  TEST_ASSERT_EQUAL(eg.GetBits(), 0x3);
}

TEST(etcpal_cpp_event_group, clear_bits_works)
{
  etcpal::EventGroup eg;
  eg.SetBits(0x3);
  eg.ClearBits(0x1);
  TEST_ASSERT_EQUAL(eg.GetBits(), 0x2);
}

TEST_GROUP_RUNNER(etcpal_cpp_event_group)
{
  RUN_TEST_CASE(etcpal_cpp_event_group, create_and_destroy_works);
  RUN_TEST_CASE(etcpal_cpp_event_group, wait_and_set_works);
  RUN_TEST_CASE(etcpal_cpp_event_group, auto_clear_works);
  RUN_TEST_CASE(etcpal_cpp_event_group, non_auto_clear_works);
  RUN_TEST_CASE(etcpal_cpp_event_group, get_bits_works);
  RUN_TEST_CASE(etcpal_cpp_event_group, clear_bits_works);
}
}
