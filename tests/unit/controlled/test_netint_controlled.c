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

#include "etcpal/netint.h"
#include "etcpal/private/netint.h"
#include "unity_fixture.h"
#include "etc_fff_wrapper.h"

#define NUM_TEST_NETINTS 1

static EtcPalNetintInfo test_netints[NUM_TEST_NETINTS] = {{0}};

etcpal_error_t test_enum_interfaces(CachedNetintInfo* cache)
{
  cache->netints     = test_netints;
  cache->num_netints = NUM_TEST_NETINTS;

  return kEtcPalErrOk;
}

ETC_FAKE_VALUE_FUNC(etcpal_error_t, os_enumerate_interfaces, CachedNetintInfo*);
ETC_FAKE_VOID_FUNC(os_free_interfaces, CachedNetintInfo*);
ETC_FAKE_VALUE_FUNC(etcpal_error_t, os_resolve_route, const EtcPalIpAddr*, const CachedNetintInfo*, unsigned int*);
ETC_FAKE_VALUE_FUNC(bool, os_netint_is_up, unsigned int, const CachedNetintInfo*);

TEST_GROUP(netint_controlled);

TEST_SETUP(netint_controlled)
{
  RESET_FAKE(os_enumerate_interfaces);
  RESET_FAKE(os_free_interfaces);
  RESET_FAKE(os_resolve_route);
  RESET_FAKE(os_netint_is_up);

  os_enumerate_interfaces_fake.custom_fake = test_enum_interfaces;

  TEST_ASSERT_EQUAL(kEtcPalErrOk, etcpal_netint_init());
}

TEST_TEAR_DOWN(netint_controlled)
{
  etcpal_netint_deinit();
}

TEST(netint_controlled, netint_is_up_works)
{
  os_netint_is_up_fake.return_val = true;
  TEST_ASSERT_TRUE(etcpal_netint_is_up(1));
  TEST_ASSERT_EQUAL_UINT(1, os_netint_is_up_fake.arg0_val);

  os_netint_is_up_fake.return_val = false;
  TEST_ASSERT_FALSE(etcpal_netint_is_up(2));
  TEST_ASSERT_EQUAL_UINT(2, os_netint_is_up_fake.arg0_val);
}

TEST_GROUP_RUNNER(netint_controlled)
{
  RUN_TEST_CASE(netint_controlled, netint_is_up_works);
}
