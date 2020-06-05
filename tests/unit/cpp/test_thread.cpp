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

#include "etcpal/cpp/thread.h"

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

TEST(etcpal_cpp_thread, default_constructor_works)
{
  etcpal::Thread thrd;
  TEST_ASSERT_FALSE(thrd.joinable());
  TEST_ASSERT_FALSE(thrd.Join().IsOk());
}

TEST(etcpal_cpp_thread, constructor_works_no_args)
{
  bool           thread_ran = false;
  etcpal::Thread thrd([&]() {
    etcpal::Thread::Sleep(1);
    thread_ran = true;
  });
  TEST_ASSERT_TRUE(thrd.joinable());
  TEST_ASSERT_TRUE(thrd.Join().IsOk());
  TEST_ASSERT_FALSE(thrd.joinable());
  TEST_ASSERT_TRUE(thread_ran);
}

TEST(etcpal_cpp_thread, constructor_works_with_args)
{
  bool           thread_ran = false;
  etcpal::Thread thrd(
      [&](unsigned int ms) {
        etcpal::Thread::Sleep(ms);
        thread_ran = true;
      },
      1u);
  TEST_ASSERT_TRUE(thrd.joinable());
  TEST_ASSERT_TRUE(thrd.Join().IsOk());
  TEST_ASSERT_FALSE(thrd.joinable());
  TEST_ASSERT_TRUE(thread_ran);
}

TEST(etcpal_cpp_thread, start_works_no_args)
{
  bool           thread_ran = false;
  etcpal::Thread thrd;
  TEST_ASSERT_TRUE(thrd.Start([&]() {
                         etcpal::Thread::Sleep(1);
                         thread_ran = true;
                       })
                       .IsOk());
  TEST_ASSERT_TRUE(thrd.joinable());
  TEST_ASSERT_TRUE(thrd.Join().IsOk());
  TEST_ASSERT_FALSE(thrd.joinable());
  TEST_ASSERT_TRUE(thread_ran);
}

TEST(etcpal_cpp_thread, start_works_with_args)
{
  bool           thread_ran = false;
  etcpal::Thread thrd;
  TEST_ASSERT_TRUE(thrd.Start(
                           [&](unsigned int ms) {
                             etcpal::Thread::Sleep(ms);
                             thread_ran = true;
                           },
                           1u)
                       .IsOk());
  TEST_ASSERT_TRUE(thrd.joinable());
  TEST_ASSERT_TRUE(thrd.Join().IsOk());
  TEST_ASSERT_FALSE(thrd.joinable());
  TEST_ASSERT_TRUE(thread_ran);
}

struct Foo
{
  bool did_run{false};
  void Bar()
  {
    etcpal::Thread::Sleep(1);
    did_run = true;
  }
};

TEST(etcpal_cpp_thread, member_function)
{
  Foo            foo;
  etcpal::Thread thrd(&Foo::Bar, &foo);
  TEST_ASSERT_TRUE(thrd.joinable());
  TEST_ASSERT_TRUE(thrd.Join().IsOk());
  TEST_ASSERT_FALSE(thrd.joinable());
  TEST_ASSERT_TRUE(foo.did_run);
}

TEST(etcpal_cpp_thread, move_constructor_works)
{
  bool           thread_ran = false;
  etcpal::Thread thrd;
  thrd.SetName("Test Thread Blah").Start([&]() {
    etcpal::Thread::Sleep(1);
    thread_ran = true;
  });
  TEST_ASSERT_TRUE(thrd.joinable());

  etcpal::Thread thrd2(std::move(thrd));
  // thrd should be in a default-constructed state
  TEST_ASSERT_FALSE(thrd.joinable());
  TEST_ASSERT_EQUAL_STRING(thrd.name(), ETCPAL_THREAD_DEFAULT_NAME);

  // thrd2 should be running and have thrd's parameters
  TEST_ASSERT_TRUE(thrd2.joinable());
  TEST_ASSERT_EQUAL_STRING(thrd2.name(), "Test Thread Blah");

  TEST_ASSERT_TRUE(thrd2.Join().IsOk());
  TEST_ASSERT_FALSE(thrd2.joinable());
  TEST_ASSERT_TRUE(thread_ran);
}

TEST(etcpal_cpp_thread, move_assignment_operator_works)
{
  bool           thread_ran = false;
  etcpal::Thread thrd;
  thrd.SetName("Test Thread Blah").Start([&]() {
    etcpal::Thread::Sleep(1);
    thread_ran = true;
  });
  TEST_ASSERT_TRUE(thrd.joinable());

  etcpal::Thread thrd2 = std::move(thrd);
  // thrd should be in a default-constructed state
  TEST_ASSERT_FALSE(thrd.joinable());
  TEST_ASSERT_EQUAL_STRING(thrd.name(), ETCPAL_THREAD_DEFAULT_NAME);

  // thrd2 should be running and have thrd's parameters
  TEST_ASSERT_TRUE(thrd2.joinable());
  TEST_ASSERT_EQUAL_STRING(thrd2.name(), "Test Thread Blah");

  TEST_ASSERT_TRUE(thrd2.Join().IsOk());
  TEST_ASSERT_FALSE(thrd2.joinable());
  TEST_ASSERT_TRUE(thread_ran);
}

TEST(etcpal_cpp_thread, param_setters_work)
{
  bool           thread_ran = false;
  etcpal::Thread thrd;

  thrd.SetName("My Thread")
      .SetPriority(ETCPAL_THREAD_DEFAULT_PRIORITY)
      .SetStackSize(ETCPAL_THREAD_DEFAULT_STACK)
      .SetPlatformData(nullptr)
      .Start([&]() {
        etcpal::Thread::Sleep(1u);
        thread_ran = true;
      });

  TEST_ASSERT_TRUE(thrd.joinable());
  TEST_ASSERT_TRUE(thrd.Join().IsOk());
  TEST_ASSERT_FALSE(thrd.joinable());
  TEST_ASSERT_TRUE(thread_ran);
}

TEST(etcpal_cpp_thread, sleep_ms_works)
{
  // Sleep for one millisecond
  etcpal::Thread::Sleep(1);
}

TEST(etcpal_cpp_thread, sleep_chrono_works)
{
  // Sleep for one millisecond
  etcpal::Thread::Sleep(std::chrono::milliseconds(1));
}

TEST_GROUP_RUNNER(etcpal_cpp_thread)
{
  RUN_TEST_CASE(etcpal_cpp_thread, default_constructor_works);
  RUN_TEST_CASE(etcpal_cpp_thread, constructor_works_no_args);
  RUN_TEST_CASE(etcpal_cpp_thread, constructor_works_with_args);
  RUN_TEST_CASE(etcpal_cpp_thread, start_works_no_args);
  RUN_TEST_CASE(etcpal_cpp_thread, start_works_with_args);
  RUN_TEST_CASE(etcpal_cpp_thread, member_function);
  RUN_TEST_CASE(etcpal_cpp_thread, move_constructor_works);
  RUN_TEST_CASE(etcpal_cpp_thread, move_assignment_operator_works);
  RUN_TEST_CASE(etcpal_cpp_thread, param_setters_work);
  RUN_TEST_CASE(etcpal_cpp_thread, sleep_ms_works);
  RUN_TEST_CASE(etcpal_cpp_thread, sleep_chrono_works);
}

}  // extern "C"
