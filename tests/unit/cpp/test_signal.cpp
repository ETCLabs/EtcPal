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

#include "etcpal/cpp/signal.h"
#include "unity_fixture.h"

extern "C" {
TEST_GROUP(etcpal_cpp_signal);

TEST_SETUP(etcpal_cpp_signal)
{
}

TEST_TEAR_DOWN(etcpal_cpp_signal)
{
}

TEST(etcpal_cpp_signal, create_and_destroy_works)
{
  etcpal::Signal sig;

  // Signals shouldn't be created in the signaled state.
  TEST_ASSERT_FALSE(sig.TryWait());

  sig.Notify();
  TEST_ASSERT_TRUE(sig.Wait());
}

TEST_GROUP_RUNNER(etcpal_cpp_signal)
{
  RUN_TEST_CASE(etcpal_cpp_signal, create_and_destroy_works);
}
}
