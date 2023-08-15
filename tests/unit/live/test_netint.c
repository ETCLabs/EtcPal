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
#include "unity_fixture.h"

#include <string.h>
#include <stddef.h>

#ifdef __MQX__
#define MQX_PROVIDES_STDIO !MQX_SUPPRESS_STDIO_MACROS
#else
#define MQX_PROVIDES_STDIO 0
#endif

#if !MQX_PROVIDES_STDIO
#include <stdio.h>
#endif

static EtcPalNetintInfo* netints     = NULL;
static size_t            num_netints = 0;

// Private functions for common operations

static void init_netints()
{
  num_netints = 0;
  TEST_ASSERT_EQUAL(kEtcPalErrBufSize, etcpal_netint_get_interfaces(NULL, &num_netints));
  TEST_ASSERT_GREATER_THAN_UINT(0u, num_netints);
  TEST_ASSERT_LESS_THAN_UINT(20u, num_netints);  // Sanity check num_netints isn't a garbage value

  netints = calloc(num_netints, sizeof(EtcPalNetintInfo));
  TEST_ASSERT_NOT_NULL(netints);
  TEST_ASSERT_EQUAL(kEtcPalErrOk, etcpal_netint_get_interfaces(netints, &num_netints));
}

static void refresh_netints()
{
  TEST_ASSERT_EQUAL(kEtcPalErrOk, etcpal_netint_refresh_interfaces());

  if (netints)
    free(netints);

  init_netints();
}

static int netint_info_cmp(const EtcPalNetintInfo* i1, const EtcPalNetintInfo* i2)
{
  int res = (int)(i1->index > i2->index) - (int)(i1->index < i2->index);
  if (res != 0)
    return res;

  res = etcpal_ip_cmp(&i1->addr, &i2->addr);
  if (res != 0)
    return res;

  res = etcpal_ip_cmp(&i1->mask, &i2->mask);
  if (res != 0)
    return res;

  res = ETCPAL_MAC_CMP(&i1->mac, &i2->mac);
  if (res != 0)
    return res;

  res = memcmp(&i1->id, &i2->id, ETCPAL_NETINTINFO_ID_LEN);
  if (res != 0)
    return res;

  res = memcmp(&i1->friendly_name, &i2->friendly_name, ETCPAL_NETINTINFO_FRIENDLY_NAME_LEN);
  if (res != 0)
    return res;

  return (int)(i1->is_default && !i2->is_default) - (int)(!i1->is_default && i2->is_default);
}

// Tests to run on the netint module without it being initialized first

TEST_GROUP(etcpal_netint_no_init);

TEST_SETUP(etcpal_netint_no_init)
{
}

TEST_TEAR_DOWN(etcpal_netint_no_init)
{
}

// Test that none of the API functions return successfully before the module is initialized. We
// generate valid data to pass to each one, to be sure they don't return e.g. kEtcPalErrInvalid
// instead.
TEST(etcpal_netint_no_init, api_does_not_work_before_initialization)
{
  size_t out_size = 0;
  TEST_ASSERT_EQUAL(kEtcPalErrNotInit, etcpal_netint_get_interfaces(NULL, &out_size));

  TEST_ASSERT_EQUAL(kEtcPalErrNotInit, etcpal_netint_get_interfaces_for_index(1, NULL, &out_size));

  unsigned int index = 0;
  TEST_ASSERT_EQUAL(kEtcPalErrNotInit, etcpal_netint_get_default_interface(kEtcPalIpTypeV4, &index));
  TEST_ASSERT_EQUAL(kEtcPalErrNotInit, etcpal_netint_get_default_interface(kEtcPalIpTypeV6, &index));

  EtcPalIpAddr dest;
  etcpal_string_to_ip(kEtcPalIpTypeV4, "8.8.8.8", &dest);
  TEST_ASSERT_EQUAL(kEtcPalErrNotInit, etcpal_netint_get_interface_for_dest(&dest, &index));
}

// The main test group. This initializes the module before each test and deinits it after.

TEST_GROUP(etcpal_netint);

TEST_SETUP(etcpal_netint)
{
  TEST_ASSERT_EQUAL(kEtcPalErrOk, etcpal_init(ETCPAL_FEATURE_NETINTS));
  init_netints();
}

TEST_TEAR_DOWN(etcpal_netint)
{
  if (netints)
    free(netints);

  etcpal_deinit(ETCPAL_FEATURE_NETINTS);
}

TEST(etcpal_netint, netint_enumeration_works)
{
  for (int i = 0; i < 3; ++i)
  {
    for (const EtcPalNetintInfo* netint = netints; netint < netints + num_netints; ++netint)
    {
      TEST_ASSERT_GREATER_THAN_UINT(0u, netint->index);
      TEST_ASSERT_GREATER_THAN_UINT(0u, strlen(netint->id));
      TEST_ASSERT_GREATER_THAN_UINT(0u, strlen(netint->friendly_name));
    }

    // Refresh network interfaces between test iterations
    refresh_netints();
  }
}

TEST(etcpal_netint, netints_are_in_index_order)
{
  for (int i = 0; i < 3; ++i)
  {
    unsigned int last_index = netints[0].index;
    for (const EtcPalNetintInfo* netint = netints + 1; netint < netints + num_netints; ++netint)
    {
      TEST_ASSERT_GREATER_OR_EQUAL(last_index, netint->index);
      last_index = netint->index;
    }

    // Refresh network interfaces between test iterations
    refresh_netints();
  }
}

TEST(etcpal_netint, get_netints_by_index_works)
{
  for (int i = 0; i < 3; ++i)
  {
    unsigned int current_index = 0;

    // There are other tests covering that the netint list should be in order by index and that the
    // indexes must all be greater than 0, which simplifies this code.
    EtcPalNetintInfo* current_arr_by_index   = calloc(num_netints, sizeof(EtcPalNetintInfo));
    size_t            current_index_arr_size = 0;

    const EtcPalNetintInfo* netint           = netints;
    const EtcPalNetintInfo* netint_for_index = current_arr_by_index;
    while (netint < (netints + num_netints))
    {
      if (netint->index > current_index)
      {
        current_index = netint->index;

        // The previous get-by-index array should be exhausted
        TEST_ASSERT_EQUAL_PTR(current_arr_by_index + current_index_arr_size, netint_for_index);

        // Get the new one
        current_index_arr_size = num_netints;
        TEST_ASSERT_EQUAL(kEtcPalErrOk, etcpal_netint_get_interfaces_for_index(netint->index, current_arr_by_index,
                                                                               &current_index_arr_size));
        TEST_ASSERT_GREATER_THAN_UINT(0u, current_index_arr_size);

        netint_for_index = current_arr_by_index;
      }

      // Now check this netints element (netint) against the corresponding arr_by_index entry (netint_for_index)
      TEST_ASSERT_EQUAL(0, netint_info_cmp(netint, netint_for_index));
      ++netint;
      ++netint_for_index;
    }

    free(current_arr_by_index);

    // Refresh network interfaces between test iterations
    refresh_netints();
  }
}

TEST(etcpal_netint, default_netint_is_consistent)
{
  for (int i = 0; i < 3; ++i)
  {
    unsigned int def_v4 = 0;
    unsigned int def_v6 = 0;

    bool have_default_v4 = (kEtcPalErrOk == etcpal_netint_get_default_interface(kEtcPalIpTypeV4, &def_v4));
    bool have_default_v6 = (kEtcPalErrOk == etcpal_netint_get_default_interface(kEtcPalIpTypeV6, &def_v6));

    if (have_default_v4)
    {
      TEST_ASSERT_NOT_EQUAL(def_v4, 0u);
    }
    if (have_default_v6)
    {
      TEST_ASSERT_NOT_EQUAL(def_v6, 0u);
    }

    for (const EtcPalNetintInfo* netint = netints; netint < netints + num_netints; ++netint)
    {
      if (netint->is_default)
      {
        if (netint->addr.type == kEtcPalIpTypeV4)
        {
          TEST_ASSERT_EQUAL_UINT(netint->index, def_v4);
        }
        else if (netint->addr.type == kEtcPalIpTypeV6)
        {
          TEST_ASSERT_EQUAL_UINT(netint->index, def_v6);
        }
      }
    }

    // Refresh network interfaces between test iterations
    refresh_netints();
  }
}

TEST(etcpal_netint, get_interface_for_dest_works_ipv4)
{
  for (int i = 0; i < 3; ++i)
  {
    // For each normally routable (non-loopback, non-link-local) network interface, check to make sure
    // that etcpal_netint_get_interface_for_dest() resolves to that interface when asked for a route to
    // the interface address itself.
    for (const EtcPalNetintInfo* netint = netints; netint < netints + num_netints; ++netint)
    {
      if (!ETCPAL_IP_IS_V4(&netint->addr) || etcpal_ip_is_loopback(&netint->addr) ||
          etcpal_ip_is_link_local(&netint->addr))
        continue;

      EtcPalIpAddr test_addr        = netint->addr;
      unsigned int netint_index_res = 0;
      TEST_ASSERT_EQUAL(kEtcPalErrOk, etcpal_netint_get_interface_for_dest(&test_addr, &netint_index_res));

      // Put addresses in print form to test meaningful information in case of test failure
      char test_addr_str[ETCPAL_IP_STRING_BYTES];
      etcpal_ip_to_string(&test_addr, test_addr_str);
      char test_msg[150];
      snprintf(test_msg, 150, "Address tried: %s (interface %u), interface returned: %u", test_addr_str, netint->index,
               netint_index_res);

      TEST_ASSERT_EQUAL_UINT_MESSAGE(netint_index_res, netint->index, test_msg);
    }

    EtcPalIpAddr ext_addr;
    ETCPAL_IP_SET_V4_ADDRESS(&ext_addr, 0xc8dc0302);  // 200.220.3.2
    unsigned int netint_index_res     = 0;
    unsigned int netint_index_default = 0;
    TEST_ASSERT_EQUAL(kEtcPalErrOk, etcpal_netint_get_default_interface(kEtcPalIpTypeV4, &netint_index_default));
    TEST_ASSERT_EQUAL(kEtcPalErrOk, etcpal_netint_get_interface_for_dest(&ext_addr, &netint_index_res));
    TEST_ASSERT_EQUAL_UINT(netint_index_res, netint_index_default);

    // Refresh network interfaces between test iterations
    refresh_netints();
  }
}

TEST_GROUP_RUNNER(etcpal_netint)
{
  RUN_TEST_CASE(etcpal_netint_no_init, api_does_not_work_before_initialization);
  RUN_TEST_CASE(etcpal_netint, netint_enumeration_works);
  RUN_TEST_CASE(etcpal_netint, netints_are_in_index_order);
  RUN_TEST_CASE(etcpal_netint, get_netints_by_index_works);
  RUN_TEST_CASE(etcpal_netint, default_netint_is_consistent);
  RUN_TEST_CASE(etcpal_netint, get_interface_for_dest_works_ipv4);
}
