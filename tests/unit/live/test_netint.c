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

#include <stdalign.h>
#include <stddef.h>
#include <string.h>

#ifdef __MQX__
#define MQX_PROVIDES_STDIO !MQX_SUPPRESS_STDIO_MACROS
#else
#define MQX_PROVIDES_STDIO 0
#endif

#if !MQX_PROVIDES_STDIO
#include <stdio.h>
#endif

#if __APPLE__  // macOS has a lot of weird virtual interfaces
#define NETINT_BUF_SIZE ETCPAL_NETINT_BUF_SIZE_ESTIMATE(15, 3)
#else
#define NETINT_BUF_SIZE ETCPAL_NETINT_BUF_SIZE_ESTIMATE(5, 3)
#endif

alignas(EtcPalNetintInfo) uint8_t netint_buf[NETINT_BUF_SIZE];

TEST_GROUP(etcpal_netint);

TEST_SETUP(etcpal_netint)
{
}

TEST_TEAR_DOWN(etcpal_netint)
{
}

TEST(etcpal_netint, system_has_netints)
{
  TEST_ASSERT_GREATER_THAN_UINT(0u, etcpal_netint_get_num_interfaces());
}

TEST(etcpal_netint, netint_enumeration_works)
{
  size_t buf_size = NETINT_BUF_SIZE;
  TEST_ASSERT_EQUAL(kEtcPalErrOk, etcpal_netint_get_interfaces(netint_buf, &buf_size));

  for (const EtcPalNetintInfo* netint = (const EtcPalNetintInfo*)netint_buf; netint; netint = netint->next)
  {
    TEST_ASSERT_GREATER_THAN_UINT(0u, netint->index);
    TEST_ASSERT_GREATER_THAN_UINT(0u, strlen(netint->id));
    TEST_ASSERT_GREATER_THAN_UINT(0u, strlen(netint->friendly_name));
    for (const EtcPalNetintAddr* addr = netint->addrs; addr; addr = addr->next)
    {
      TEST_ASSERT_TRUE(!ETCPAL_IP_IS_INVALID(&addr->addr));
      TEST_ASSERT_GREATER_THAN_UINT(0u, addr->mask_length);
    }
  }
}

TEST(etcpal_netint, netints_match_num_netints)
{
  size_t buf_size = NETINT_BUF_SIZE;
  TEST_ASSERT_EQUAL(kEtcPalErrOk, etcpal_netint_get_interfaces(netint_buf, &buf_size));

  size_t expected_num_interfaces = etcpal_netint_get_num_interfaces();
  size_t num_interfaces          = 0;
  for (const EtcPalNetintInfo* netint = (const EtcPalNetintInfo*)netint_buf; netint; netint = netint->next)
  {
    ++num_interfaces;
  }
  TEST_ASSERT_EQUAL_UINT(expected_num_interfaces, num_interfaces);
}

TEST(etcpal_netint, get_netint_by_index_works)
{
  size_t buf_size = NETINT_BUF_SIZE;
  TEST_ASSERT_EQUAL(kEtcPalErrOk, etcpal_netint_get_interfaces(netint_buf, &buf_size));

  size_t                                   get_by_index_buf_size = NETINT_BUF_SIZE;
  alignas(EtcPalNetintInfo) static uint8_t get_by_index_buf[NETINT_BUF_SIZE];

  for (const EtcPalNetintInfo* netint = (const EtcPalNetintInfo*)netint_buf; netint; netint = netint->next)
  {
    // For each network interface, retrieve it again using etcpal_netint_get_interface_by_index()
    // and make sure it matches.
    get_by_index_buf_size = NETINT_BUF_SIZE;
    TEST_ASSERT_EQUAL(kEtcPalErrOk,
                      etcpal_netint_get_interface_by_index(netint->index, get_by_index_buf, &get_by_index_buf_size));
    TEST_ASSERT_LESS_OR_EQUAL_UINT(buf_size, get_by_index_buf_size);

    const EtcPalNetintInfo* netint_by_index = (const EtcPalNetintInfo*)get_by_index_buf;
    TEST_ASSERT_EQUAL_UINT(netint->index, netint_by_index->index);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(netint->mac.data, netint_by_index->mac.data, ETCPAL_MAC_BYTES);
    TEST_ASSERT_EQUAL_STRING(netint->id, netint_by_index->id);
    TEST_ASSERT_EQUAL_STRING(netint->friendly_name, netint_by_index->friendly_name);
    TEST_ASSERT_EQUAL_UINT32(netint->flags, netint_by_index->flags);

    // Quick heuristics - make sure they have the same number of addrs
    size_t num_addrs          = 0;
    size_t expected_num_addrs = 0;
    for (const EtcPalNetintAddr* addr = netint->addrs; addr; addr = addr->next)
      ++expected_num_addrs;
    for (const EtcPalNetintAddr* addr = netint_by_index->addrs; addr; addr = addr->next)
      ++num_addrs;
    TEST_ASSERT_EQUAL(expected_num_addrs, num_addrs);

    // Should be only one netint for each index.
    TEST_ASSERT_NULL(netint_by_index->next);
  }
}

TEST(etcpal_netint, default_netint_is_consistent)
{
  TEST_ASSERT_GREATER_THAN_UINT(0u, etcpal_netint_get_num_interfaces());

  unsigned int def_v4 = 0;
  unsigned int def_v6 = 0;

  bool have_default_v4 = (kEtcPalErrOk == etcpal_netint_get_default_interface_index(kEtcPalIpTypeV4, &def_v4));
  bool have_default_v6 = (kEtcPalErrOk == etcpal_netint_get_default_interface_index(kEtcPalIpTypeV6, &def_v6));

  if (have_default_v4)
  {
    TEST_ASSERT_NOT_EQUAL(def_v4, 0u);
  }
  if (have_default_v6)
  {
    TEST_ASSERT_NOT_EQUAL(def_v6, 0u);
  }

  size_t buf_size = NETINT_BUF_SIZE;
  TEST_ASSERT_EQUAL(kEtcPalErrOk, etcpal_netint_get_interfaces(netint_buf, &buf_size));
  for (const EtcPalNetintInfo* netint = (const EtcPalNetintInfo*)netint_buf; netint; netint = netint->next)
  {
    if (netint->flags & ETCPAL_NETINT_FLAG_DEFAULT_V4)
    {
      TEST_ASSERT_TRUE(have_default_v4);
      TEST_ASSERT_EQUAL_UINT(netint->index, def_v4);
    }
    if (netint->flags & ETCPAL_NETINT_FLAG_DEFAULT_V6)
    {
      TEST_ASSERT_TRUE(have_default_v6);
      TEST_ASSERT_EQUAL_UINT(netint->index, def_v6);
    }
  }
}

TEST(etcpal_netint, netint_addr_to_string_works)
{
  EtcPalNetintAddr netint_addr;
  ETCPAL_IP_SET_V4_ADDRESS(&netint_addr.addr, 0xffffffff);
  netint_addr.mask_length = 32;

  char str_buf[ETCPAL_NETINT_ADDR_STRING_BYTES];
  TEST_ASSERT_EQUAL(kEtcPalErrOk, etcpal_netint_addr_to_string(&netint_addr, str_buf));
  TEST_ASSERT_EQUAL_STRING("255.255.255.255/32", str_buf);

  netint_addr.mask_length = 16;
  TEST_ASSERT_EQUAL(kEtcPalErrOk, etcpal_netint_addr_to_string(&netint_addr, str_buf));
  TEST_ASSERT_EQUAL_STRING("255.255.255.255/16", str_buf);

  uint8_t v6_addr[16];
  memset(v6_addr, 0xff, 16);
  ETCPAL_IP_SET_V6_ADDRESS(&netint_addr.addr, v6_addr);
  netint_addr.mask_length = 64;

  TEST_ASSERT_EQUAL(kEtcPalErrOk, etcpal_netint_addr_to_string(&netint_addr, str_buf));
  TEST_ASSERT((0 == strcmp(str_buf, "ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff/64")) ||
              (0 == strcmp(str_buf, "FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF/64")));
}

TEST_GROUP_RUNNER(etcpal_netint)
{
  RUN_TEST_CASE(etcpal_netint, system_has_netints);
  RUN_TEST_CASE(etcpal_netint, netint_enumeration_works);
  RUN_TEST_CASE(etcpal_netint, netints_match_num_netints);
  RUN_TEST_CASE(etcpal_netint, get_netint_by_index_works);
  RUN_TEST_CASE(etcpal_netint, default_netint_is_consistent);
  RUN_TEST_CASE(etcpal_netint, netint_addr_to_string_works)
}
