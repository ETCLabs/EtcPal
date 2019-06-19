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
#include "gmock/gmock.h"

#include "lwpa/netint.h"
#include "lwpa/log.h"

class CommonTest : public ::testing::Test
{
public:
  MOCK_METHOD0(LogCallback, void());
};

// Test the LWPA_FEATURES_ALL_BUT() macro
TEST_F(CommonTest, features_all_but)
{
  lwpa_features_t mask = LWPA_FEATURES_ALL_BUT(LWPA_FEATURE_SOCKETS);
  EXPECT_TRUE(mask & LWPA_FEATURE_NETINTS);
  EXPECT_TRUE(mask & LWPA_FEATURE_TIMERS);
  EXPECT_TRUE(mask & LWPA_FEATURE_LOGGING);
  EXPECT_FALSE(mask & LWPA_FEATURE_SOCKETS);

  mask = LWPA_FEATURES_ALL_BUT(LWPA_FEATURE_LOGGING);
  EXPECT_TRUE(mask & LWPA_FEATURE_SOCKETS);
  EXPECT_TRUE(mask & LWPA_FEATURE_NETINTS);
  EXPECT_TRUE(mask & LWPA_FEATURE_TIMERS);
  EXPECT_FALSE(mask & LWPA_FEATURE_LOGGING);
}

// Test multiple calls of lwpa_init() for the netint module.
TEST_F(CommonTest, netint_double_init)
{
  ASSERT_EQ(kLwpaErrOk, lwpa_init(LWPA_FEATURE_NETINTS));
  ASSERT_EQ(kLwpaErrOk, lwpa_init(LWPA_FEATURE_NETINTS));

  lwpa_deinit(LWPA_FEATURE_NETINTS);

  // After 2 inits and one deinit, we should still be able to make valid calls to the module.
  LwpaNetintInfo def_netint;
  EXPECT_TRUE(lwpa_netint_get_default_interface(kLwpaIpTypeV4, &def_netint));

  lwpa_deinit(LWPA_FEATURE_NETINTS);
}

// A shim from the lwpa_log module to GoogleMock.
extern "C" void log_cb(void* context, const char* syslog_str, const char* human_str, const char* raw_str)
{
  (void)syslog_str;
  (void)human_str;
  (void)raw_str;

  CommonTest* test_fixture = static_cast<CommonTest*>(context);
  test_fixture->LogCallback();
}

// Test multiple calls of lwpa_init() for the log module.
TEST_F(CommonTest, log_double_init)
{
  ASSERT_EQ(kLwpaErrOk, lwpa_init(LWPA_FEATURE_LOGGING));
  ASSERT_EQ(kLwpaErrOk, lwpa_init(LWPA_FEATURE_LOGGING));

  LwpaLogParams params;
  params.action = kLwpaLogCreateHumanReadableLog;
  params.log_fn = log_cb;
  params.log_mask = LWPA_LOG_UPTO(LWPA_LOG_DEBUG);
  params.time_fn = nullptr;
  params.context = this;

  ASSERT_TRUE(lwpa_validate_log_params(&params));

  EXPECT_CALL(*this, LogCallback);
  lwpa_log(&params, LWPA_LOG_INFO, "Log message");

  lwpa_deinit(LWPA_FEATURE_LOGGING);

  // After 2 inits and one deinit, we should still be able to use the lwpa_log() function and get
  // callbacks.
  EXPECT_CALL(*this, LogCallback);
  lwpa_log(&params, LWPA_LOG_INFO, "Log message");

  lwpa_deinit(LWPA_FEATURE_LOGGING);
}
