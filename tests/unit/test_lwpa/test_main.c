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

#include "unity.h"
#include "fff.h"
#include "test_common.h"

DEFINE_FFF_GLOBALS;

void setUp(void)
{
}

void tearDown(void)
{
}

int run_tests(void)
{
  int test_res;

#define CHECK_TEST(test_func) \
  test_res = test_func();     \
  if (test_res != 0)          \
    return test_res;

  CHECK_TEST(run_common_tests);
}
