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
 * This file is a part of EtcPal. For more information, go to:
 * https://github.com/ETCLabs/EtcPal
 ******************************************************************************/

#include "etcpal/cpp/thread.h"

#include <cstdio>
#include <vector>
#include "unity_fixture.h"

extern "C" {
TEST_GROUP(etcpal_cpp_thread);

TEST_SETUP(etcpal_cpp_thread)
{
}

TEST_TEAR_DOWN(etcpal_cpp_thread)
{
}

using namespace std::chrono_literals;

TEST(etcpal_cpp_thread, placeholder)
{
  auto thread_func = []() { std::printf("Hello, world!\n"); };
  etcpal::Thread thrd(thread_func);
  thrd.Join();

  auto thread_func_2 = [](int number) { std::printf("The number is %d\n", number); };
  etcpal::Thread thrd_2(thread_func_2, 42);
  thrd_2.Join();

  std::vector<etcpal::Thread> threads;
  for (int i = 0; i < 10; ++i)
  {
    threads.push_back(etcpal::Thread([]() { std::printf("Hello hello hello hello hello hello hello\n"); }));
  }

  for (auto& thread : threads)
  {
    thread.Join();
  }
}

TEST_GROUP_RUNNER(etcpal_cpp_thread)
{
  RUN_TEST_CASE(etcpal_cpp_thread, placeholder);
}
}
