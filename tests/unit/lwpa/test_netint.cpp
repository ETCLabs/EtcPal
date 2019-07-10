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
#include "gtest/gtest.h"
#include <cstring>
#include <cstddef>
#include <memory>
#include <set>

class NetintTest : public ::testing::Test
{
protected:
  NetintTest()
  {
    lwpa_init(LWPA_FEATURE_NETINTS);
    num_netints = lwpa_netint_get_num_interfaces();
  }
  ~NetintTest() { lwpa_deinit(LWPA_FEATURE_NETINTS); }

  size_t num_netints{0};
};

TEST_F(NetintTest, enumerate)
{
  ASSERT_GT(num_netints, 0u);
  auto netint_arr = std::make_unique<LwpaNetintInfo[]>(num_netints);
  size_t num_netints_returned = lwpa_netint_get_interfaces(netint_arr.get(), num_netints);
  ASSERT_EQ(num_netints_returned, num_netints);

  size_t num_defaults = 0;
  for (LwpaNetintInfo* netint = netint_arr.get(); netint < netint_arr.get() + num_netints_returned; ++netint)
  {
    if (netint->is_default)
      ++num_defaults;
    ASSERT_GT(strlen(netint->name), 0u);
  }
  // There can be a maximum of two default interfaces: one each for IPv4 and IPv6.
  ASSERT_TRUE(num_defaults <= 2);
}

TEST_F(NetintTest, default)
{
  ASSERT_GT(num_netints, 0u);

  LwpaNetintInfo def_v4;
  LwpaNetintInfo def_v6;
  memset(&def_v4, 0, sizeof def_v4);
  memset(&def_v6, 0, sizeof def_v6);

  auto netint_arr = std::make_unique<LwpaNetintInfo[]>(num_netints);
  memset(netint_arr.get(), 0, sizeof(struct LwpaNetintInfo) * num_netints);

  num_netints = lwpa_netint_get_interfaces(netint_arr.get(), num_netints);

  bool have_default_v4 = (kLwpaErrOk == lwpa_netint_get_default_interface(kLwpaIpTypeV4, &def_v4));
  bool have_default_v6 = (kLwpaErrOk == lwpa_netint_get_default_interface(kLwpaIpTypeV6, &def_v6));

  if (have_default_v4)
  {
    EXPECT_TRUE(def_v4.is_default);
  }
  if (have_default_v6)
  {
    EXPECT_TRUE(def_v6.is_default);
  }

  for (LwpaNetintInfo* netint = netint_arr.get(); netint < netint_arr.get() + num_netints; ++netint)
  {
    if (netint->is_default)
    {
      if (netint->addr.type == kLwpaIpTypeV4)
      {
        EXPECT_EQ(0, memcmp(netint, &def_v4, sizeof def_v4));
      }
      else if (netint->addr.type == kLwpaIpTypeV6)
      {
        EXPECT_EQ(0, memcmp(netint, &def_v6, sizeof def_v6));
      }
    }
  }
}

TEST_F(NetintTest, ipv4_routing)
{
  ASSERT_GT(num_netints, 0u);

  auto netint_arr = std::make_unique<LwpaNetintInfo[]>(num_netints);
  num_netints = lwpa_netint_get_interfaces(netint_arr.get(), num_netints);

  // For each normally routable (non-loopback, non-link-local) network interface, check to make sure
  // that lwpa_netint_get_interface_for_dest() resolves to that interface when asked for a route to
  // the interface address itself.
  for (LwpaNetintInfo* netint = netint_arr.get(); netint < netint_arr.get() + num_netints; ++netint)
  {
    if (!LWPA_IP_IS_V4(&netint->addr) || lwpa_ip_is_loopback(&netint->addr) || lwpa_ip_is_link_local(&netint->addr))
      continue;

    LwpaIpAddr test_addr = netint->addr;
    LwpaNetintInfo netint_res;
    ASSERT_EQ(kLwpaErrOk, lwpa_netint_get_interface_for_dest(&test_addr, &netint_res));

    // Put addresses in print form to test meaningful information in case of test failure
    char test_addr_str[LWPA_INET6_ADDRSTRLEN];
    char result_str[LWPA_INET6_ADDRSTRLEN];
    lwpa_inet_ntop(&test_addr, test_addr_str, LWPA_INET6_ADDRSTRLEN);
    lwpa_inet_ntop(&netint_res.addr, result_str, LWPA_INET6_ADDRSTRLEN);

    EXPECT_TRUE(lwpa_ip_equal(&netint_res.addr, &netint->addr))
        << "Address tried: " << test_addr_str << ", interface returned: " << result_str;
  }

  LwpaIpAddr ext_addr;
  LWPA_IP_SET_V4_ADDRESS(&ext_addr, 0xc8dc0302);  // 200.220.3.2
  LwpaNetintInfo def;
  ASSERT_EQ(kLwpaErrOk, lwpa_netint_get_interface_for_dest(&ext_addr, &def));
  ASSERT_TRUE(def.is_default);
}
