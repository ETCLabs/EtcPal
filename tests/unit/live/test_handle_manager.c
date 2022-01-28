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

#include "etcpal/handle_manager.h"

#include <limits.h>

#include "etcpal/common.h"
#include "unity_fixture.h"
#include "etc_fff_wrapper.h"

static int num_in_use_checks = 0;

/* Handle value in use functions. */
bool is_even(int handle_val, void* context)
{
  ETCPAL_UNUSED_ARG(context);

  ++num_in_use_checks;

  return (handle_val % 2) == 0;
}

bool is_anything(int handle_val, void* context)
{
  ETCPAL_UNUSED_ARG(handle_val);
  ETCPAL_UNUSED_ARG(context);

  ++num_in_use_checks;

  return true;
}

TEST_GROUP(etcpal_handle_manager);

TEST_SETUP(etcpal_handle_manager)
{
}

TEST_TEAR_DOWN(etcpal_handle_manager)
{
}

TEST(etcpal_handle_manager, handles_generate_with_max)
{
  for (int max = 0; max < 100; ++max)
  {
    IntHandleManager manager;
    init_int_handle_manager(&manager, max, is_even, NULL);

    num_in_use_checks = 0;

    for (int expected_handle = 0; expected_handle <= max; ++expected_handle)
    {
      int actual_handle = get_next_int_handle(&manager);
      TEST_ASSERT_EQUAL(actual_handle, expected_handle);
      TEST_ASSERT_EQUAL(num_in_use_checks, 0);
    }

    int expected_checks = 2;
    for (int i = 0; i < 10; ++i)
    {
      for (int expected_handle = 1; expected_handle <= max; expected_handle += 2, expected_checks += 2)
      {
        int actual_handle = get_next_int_handle(&manager);
        TEST_ASSERT_EQUAL(actual_handle, expected_handle);
        TEST_ASSERT_EQUAL(num_in_use_checks, expected_checks);
      }

      if ((max % 2) == 0)
        ++expected_checks;  // There's an extra check when wrapping around (even, even, odd)
    }
  }
}

TEST(etcpal_handle_manager, handles_generate_without_max)
{
  IntHandleManager manager;
  init_int_handle_manager(&manager, -1, is_even, NULL);

  num_in_use_checks = 0;

  for (int expected_handle = 0; expected_handle <= 100000; ++expected_handle)
  {
    int actual_handle = get_next_int_handle(&manager);
    TEST_ASSERT_EQUAL(actual_handle, expected_handle);
    TEST_ASSERT_EQUAL(num_in_use_checks, 0);
  }

  // Now test wrapping without max
  manager.last_handle = INT_MAX - 100001;
  for (int i = 100000; i >= 0; --i)
  {
    int actual_handle = get_next_int_handle(&manager);
    TEST_ASSERT_EQUAL(actual_handle, INT_MAX - i);
    TEST_ASSERT_EQUAL(num_in_use_checks, 0);
  }

  for (int expected_handle = 1, expected_checks = 2; expected_handle <= 100000;
       expected_handle += 2, expected_checks += 2)
  {
    int actual_handle = get_next_int_handle(&manager);
    TEST_ASSERT_EQUAL(actual_handle, expected_handle);
    TEST_ASSERT_EQUAL(num_in_use_checks, expected_checks);
  }
}

TEST(etcpal_handle_manager, handles_run_out)
{
  for (int max = 0; max < 100; ++max)
  {
    IntHandleManager manager;
    init_int_handle_manager(&manager, max, is_anything, NULL);

    num_in_use_checks = 0;

    for (int expected_handle = 0; expected_handle <= max; ++expected_handle)
    {
      int actual_handle = get_next_int_handle(&manager);
      TEST_ASSERT_EQUAL(actual_handle, expected_handle);
      TEST_ASSERT_EQUAL(num_in_use_checks, 0);
    }

    for (int i = 1; i <= 10; ++i)
    {
      int actual_handle = get_next_int_handle(&manager);
      TEST_ASSERT_EQUAL(actual_handle, -1);
      TEST_ASSERT_EQUAL(num_in_use_checks, (max + 1) * i);
    }
  }
}

TEST_GROUP_RUNNER(etcpal_handle_manager)
{
  RUN_TEST_CASE(etcpal_handle_manager, handles_generate_with_max);
  RUN_TEST_CASE(etcpal_handle_manager, handles_generate_without_max);
  RUN_TEST_CASE(etcpal_handle_manager, handles_run_out);
}
