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
#include "unity_fixture.h"
#include "fff.h"

#include "lwpa/netint.h"
#include "lwpa/log.h"

TEST_GROUP(lwpa_common);

TEST_SETUP(lwpa_common)
{
}

TEST_TEAR_DOWN(lwpa_common)
{
}

// Test the LWPA_FEATURES_ALL_BUT() macro
TEST(lwpa_common, features_all_but_macro_works)
{
  lwpa_features_t mask = LWPA_FEATURES_ALL_BUT(LWPA_FEATURE_SOCKETS);
  TEST_ASSERT(mask & LWPA_FEATURE_NETINTS);
  TEST_ASSERT(mask & LWPA_FEATURE_TIMERS);
  TEST_ASSERT(mask & LWPA_FEATURE_LOGGING);
  TEST_ASSERT_UNLESS(mask & LWPA_FEATURE_SOCKETS);

  mask = LWPA_FEATURES_ALL_BUT(LWPA_FEATURE_LOGGING);
  TEST_ASSERT(mask & LWPA_FEATURE_SOCKETS);
  TEST_ASSERT(mask & LWPA_FEATURE_NETINTS);
  TEST_ASSERT(mask & LWPA_FEATURE_TIMERS);
  TEST_ASSERT_UNLESS(mask & LWPA_FEATURE_LOGGING);
}

// Test multiple calls of lwpa_init() for the netint module.
TEST(lwpa_common, netint_double_init_works)
{
  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_init(LWPA_FEATURE_NETINTS));
  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_init(LWPA_FEATURE_NETINTS));

  lwpa_deinit(LWPA_FEATURE_NETINTS);

  // After 2 inits and one deinit, we should still be able to make valid calls to the module.
  unsigned int def_netint;
  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_netint_get_default_interface(kLwpaIpTypeV4, &def_netint));

  lwpa_deinit(LWPA_FEATURE_NETINTS);
}

// A shim from the lwpa_log module to fff.
FAKE_VOID_FUNC(common_test_log_callback, void*, const LwpaLogStrings*);

// Test multiple calls of lwpa_init() for the log module.
TEST(lwpa_common, log_double_init_works)
{
  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_init(LWPA_FEATURE_LOGGING));
  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_init(LWPA_FEATURE_LOGGING));

  LwpaLogParams params;
  params.action = kLwpaLogCreateHumanReadableLog;
  params.log_fn = common_test_log_callback;
  params.log_mask = LWPA_LOG_UPTO(LWPA_LOG_DEBUG);
  params.time_fn = NULL;
  params.context = NULL;

  TEST_ASSERT_TRUE(lwpa_validate_log_params(&params));

  lwpa_log(&params, LWPA_LOG_INFO, "Log message");
  TEST_ASSERT_EQUAL(common_test_log_callback_fake.call_count, 1);

  lwpa_deinit(LWPA_FEATURE_LOGGING);

  // After 2 inits and one deinit, we should still be able to use the lwpa_log() function and get
  // callbacks.
  lwpa_log(&params, LWPA_LOG_INFO, "Log message");
  TEST_ASSERT_EQUAL(common_test_log_callback_fake.call_count, 2);

  lwpa_deinit(LWPA_FEATURE_LOGGING);
}

TEST_GROUP_RUNNER(lwpa_common)
{
  RUN_TEST_CASE(lwpa_common, features_all_but_macro_works);
  RUN_TEST_CASE(lwpa_common, netint_double_init_works);
  RUN_TEST_CASE(lwpa_common, log_double_init_works);
}
