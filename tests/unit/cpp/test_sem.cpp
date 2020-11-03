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

#include "etcpal/cpp/sem.h"
#include "unity_fixture.h"

extern "C" {
TEST_GROUP(etcpal_cpp_sem);

TEST_SETUP(etcpal_cpp_sem)
{
}

TEST_TEAR_DOWN(etcpal_cpp_sem)
{
}

TEST(etcpal_cpp_sem, create_and_destroy_works)
{
  // The semaphore has an initial count of 1
  etcpal::Semaphore sem(1);

  TEST_ASSERT_TRUE(sem.Wait());

  // The count should now be zero - wait should fail
  TEST_ASSERT_FALSE(sem.TryWait());

  TEST_ASSERT_TRUE(sem.Post());
  // The count should be 1 again. Wait should succeed.
  TEST_ASSERT_TRUE(sem.TryWait());

  TEST_ASSERT_TRUE(sem.Post());
}

TEST_GROUP_RUNNER(etcpal_cpp_sem)
{
  RUN_TEST_CASE(etcpal_cpp_sem, create_and_destroy_works);
}
}
