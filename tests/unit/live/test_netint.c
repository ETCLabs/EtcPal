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
#include "lwpa/netint.h"
#include "unity_fixture.h"

#include <string.h>
#include <stddef.h>
#include <stdio.h>

static size_t num_netints;

TEST_GROUP(lwpa_netint);

TEST_SETUP(lwpa_netint)
{
  lwpa_init(LWPA_FEATURE_NETINTS);
  num_netints = lwpa_netint_get_num_interfaces();
}

TEST_TEAR_DOWN(lwpa_netint)
{
  lwpa_deinit(LWPA_FEATURE_NETINTS);
}

TEST(lwpa_netint, netint_enumeration_works)
{
  TEST_ASSERT_GREATER_THAN_UINT(0u, num_netints);

  size_t num_defaults = 0;
  const LwpaNetintInfo* netint_list = lwpa_netint_get_interfaces();
  TEST_ASSERT_NOT_NULL(netint_list);
  for (const LwpaNetintInfo* netint = netint_list; netint < netint_list + num_netints; ++netint)
  {
    if (netint->is_default)
      ++num_defaults;
    TEST_ASSERT_GREATER_THAN_UINT(0u, strlen(netint->name));
    TEST_ASSERT_GREATER_THAN_UINT(0u, strlen(netint->friendly_name));
  }
  // There can be a maximum of two default interfaces: one each for IPv4 and IPv6.
  TEST_ASSERT_LESS_OR_EQUAL_UINT(2u, num_defaults);
}

/* TODO Figure out how we're gonna handle mallocing
TEST(lwpa_netint, copy_interfaces_works)
{
  // Test copying the full array
  auto netint_arr = std::make_unique<LwpaNetintInfo[]>(num_netints);
  size_t num_netints_returned = lwpa_netint_copy_interfaces(netint_arr.get(), num_netints);
  ASSERT_EQ(num_netints_returned, num_netints);

  // Make sure the array is equal to the list obtained from lwpa_netint_get_interfaces()
  EXPECT_EQ(0, memcmp(netint_arr.get(), lwpa_netint_get_interfaces(), num_netints));
}
*/

TEST(lwpa_netint, default_netint_is_consistent)
{
  TEST_ASSERT_GREATER_THAN_UINT(0u, num_netints);

  LwpaNetintInfo def_v4;
  LwpaNetintInfo def_v6;
  memset(&def_v4, 0, sizeof def_v4);
  memset(&def_v6, 0, sizeof def_v6);

  bool have_default_v4 = (kLwpaErrOk == lwpa_netint_get_default_interface(kLwpaIpTypeV4, &def_v4));
  bool have_default_v6 = (kLwpaErrOk == lwpa_netint_get_default_interface(kLwpaIpTypeV6, &def_v6));

  if (have_default_v4)
  {
    TEST_ASSERT(def_v4.is_default);
  }
  if (have_default_v6)
  {
    TEST_ASSERT(def_v6.is_default);
  }

  const LwpaNetintInfo* netint_list = lwpa_netint_get_interfaces();
  TEST_ASSERT_NOT_NULL(netint_list);
  for (const LwpaNetintInfo* netint = netint_list; netint < netint_list + num_netints; ++netint)
  {
    if (netint->is_default)
    {
      if (netint->addr.type == kLwpaIpTypeV4)
      {
        TEST_ASSERT_EQUAL_MEMORY(netint, &def_v4, sizeof def_v4);
      }
      else if (netint->addr.type == kLwpaIpTypeV6)
      {
        TEST_ASSERT_EQUAL_MEMORY(netint, &def_v6, sizeof def_v6);
      }
    }
  }
}

TEST(lwpa_netint, get_interface_for_dest_works_ipv4)
{
  TEST_ASSERT_GREATER_THAN_UINT(0u, num_netints);

  const LwpaNetintInfo* netint_list = lwpa_netint_get_interfaces();
  TEST_ASSERT_NOT_NULL(netint_list);

  // For each normally routable (non-loopback, non-link-local) network interface, check to make sure
  // that lwpa_netint_get_interface_for_dest() resolves to that interface when asked for a route to
  // the interface address itself.
  for (const LwpaNetintInfo* netint = netint_list; netint < netint_list + num_netints; ++netint)
  {
    if (!LWPA_IP_IS_V4(&netint->addr) || lwpa_ip_is_loopback(&netint->addr) || lwpa_ip_is_link_local(&netint->addr))
      continue;

    LwpaIpAddr test_addr = netint->addr;
    LwpaNetintInfo netint_res;
    TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_netint_get_interface_for_dest(&test_addr, &netint_res));

    // Put addresses in print form to test meaningful information in case of test failure
    char test_addr_str[LWPA_INET6_ADDRSTRLEN];
    char result_str[LWPA_INET6_ADDRSTRLEN];
    lwpa_inet_ntop(&test_addr, test_addr_str, LWPA_INET6_ADDRSTRLEN);
    lwpa_inet_ntop(&netint_res.addr, result_str, LWPA_INET6_ADDRSTRLEN);
    char test_msg[150];
    snprintf(test_msg, 150, "Address tried: %s, interface returned: %s", test_addr_str, result_str);

    TEST_ASSERT_TRUE_MESSAGE(lwpa_ip_equal(&netint_res.addr, &netint->addr), test_msg);
  }

  LwpaIpAddr ext_addr;
  LWPA_IP_SET_V4_ADDRESS(&ext_addr, 0xc8dc0302);  // 200.220.3.2
  LwpaNetintInfo def;
  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_netint_get_interface_for_dest(&ext_addr, &def));
  TEST_ASSERT(def.is_default);
}

TEST_GROUP_RUNNER(lwpa_netint)
{
  RUN_TEST_CASE(lwpa_netint, netint_enumeration_works);
  RUN_TEST_CASE(lwpa_netint, default_netint_is_consistent);
  RUN_TEST_CASE(lwpa_netint, get_interface_for_dest_works_ipv4);
}
