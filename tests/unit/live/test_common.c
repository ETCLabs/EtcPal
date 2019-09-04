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
#include "etcpal/common.h"
#include "unity_fixture.h"
#include "fff.h"

#include "etcpal/netint.h"
#include "etcpal/log.h"

TEST_GROUP(etcpal_common);

TEST_SETUP(etcpal_common)
{
}

TEST_TEAR_DOWN(etcpal_common)
{
}

// Test the LWPA_FEATURES_ALL_BUT() macro
TEST(etcpal_common, features_all_but_macro_works)
{
  etcpal_features_t mask = LWPA_FEATURES_ALL_BUT(LWPA_FEATURE_SOCKETS);
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

// Test multiple calls of etcpal_init() for the netint module.
TEST(etcpal_common, netint_double_init_works)
{
  TEST_ASSERT_EQUAL(kEtcPalErrOk, etcpal_init(LWPA_FEATURE_NETINTS));
  TEST_ASSERT_EQUAL(kEtcPalErrOk, etcpal_init(LWPA_FEATURE_NETINTS));

  etcpal_deinit(LWPA_FEATURE_NETINTS);

  // After 2 inits and one deinit, we should still be able to make valid calls to the module.
  unsigned int def_netint;
  TEST_ASSERT_EQUAL(kEtcPalErrOk, etcpal_netint_get_default_interface(kEtcPalIpTypeV4, &def_netint));

  etcpal_deinit(LWPA_FEATURE_NETINTS);
}

// A shim from the etcpal_log module to fff.
FAKE_VOID_FUNC(common_test_log_callback, void*, const LwpaLogStrings*);

// Test multiple calls of etcpal_init() for the log module.
TEST(etcpal_common, log_double_init_works)
{
  TEST_ASSERT_EQUAL(kEtcPalErrOk, etcpal_init(LWPA_FEATURE_LOGGING));
  TEST_ASSERT_EQUAL(kEtcPalErrOk, etcpal_init(LWPA_FEATURE_LOGGING));

  LwpaLogParams params;
  params.action = kEtcPalLogCreateHumanReadableLog;
  params.log_fn = common_test_log_callback;
  params.log_mask = LWPA_LOG_UPTO(LWPA_LOG_DEBUG);
  params.time_fn = NULL;
  params.context = NULL;

  TEST_ASSERT_TRUE(etcpal_validate_log_params(&params));

  etcpal_log(&params, LWPA_LOG_INFO, "Log message");
  TEST_ASSERT_EQUAL(common_test_log_callback_fake.call_count, 1);

  etcpal_deinit(LWPA_FEATURE_LOGGING);

  // After 2 inits and one deinit, we should still be able to use the etcpal_log() function and get
  // callbacks.
  etcpal_log(&params, LWPA_LOG_INFO, "Log message");
  TEST_ASSERT_EQUAL(common_test_log_callback_fake.call_count, 2);

  etcpal_deinit(LWPA_FEATURE_LOGGING);
}

TEST_GROUP_RUNNER(etcpal_common)
{
  RUN_TEST_CASE(etcpal_common, features_all_but_macro_works);
  RUN_TEST_CASE(etcpal_common, netint_double_init_works);
  RUN_TEST_CASE(etcpal_common, log_double_init_works);
}
