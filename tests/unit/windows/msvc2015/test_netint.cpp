/******************************************************************************
* Copyright 2018 ETC Inc.
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
#include "lwpa_netint.h"
#include "gtest/gtest.h"
#include <cstring>
#include <cstddef>
#include <set>
#include <WS2tcpip.h>
#include <Windows.h>

class NetintTest : public ::testing::Test
{
protected:
  NetintTest()
    : num_netints(0)
  {
    num_netints = netint_get_num_interfaces();
  }

  virtual ~NetintTest()
  {
  }

  size_t num_netints;
};

TEST_F(NetintTest, enumerate)
{
  ASSERT_GT(num_netints, 0u);
  LwpaNetintInfo *netint_arr = new LwpaNetintInfo[num_netints];
  size_t num_netints_returned = netint_get_interfaces(netint_arr, num_netints);
  ASSERT_EQ(num_netints_returned, num_netints);

  size_t num_defaults = 0;
  for (LwpaNetintInfo *netint = netint_arr;
       netint < netint_arr + num_netints_returned; ++netint)
  {
    if (netint->is_default)
      ++num_defaults;
    ASSERT_GT(strlen(netint->name), 0u);
  }
  ASSERT_EQ(num_defaults, 1u);
  delete[] netint_arr;
}

TEST_F(NetintTest, default)
{
  ASSERT_GT(num_netints, 0u);

  LwpaNetintInfo def;
  LwpaNetintInfo *netint_arr = new LwpaNetintInfo[num_netints];
  memset(&def, 0, sizeof def);
  memset(netint_arr, 0, sizeof(struct LwpaNetintInfo) * num_netints);

  num_netints = netint_get_interfaces(netint_arr, num_netints);
  ASSERT_TRUE(netint_get_default_interface(&def));
  ASSERT_TRUE(def.is_default);
  for (LwpaNetintInfo *netint = netint_arr; netint < netint_arr + num_netints;
       ++netint)
  {
    if (netint->is_default)
    {
      ASSERT_EQ(0, memcmp(netint, &def, sizeof def));
      break;
    }
  }
  delete[] netint_arr;
}

TEST_F(NetintTest, IPv4routing)
{
  ASSERT_GT(num_netints, 0u);

  LwpaNetintInfo *netint_arr = new LwpaNetintInfo[num_netints];
  num_netints = netint_get_interfaces(netint_arr, num_netints);

  std::set<uint32_t> nets_already_tried;

  for (LwpaNetintInfo *netint = netint_arr; netint < netint_arr + num_netints;
       ++netint)
  {
    uint32_t net = lwpaip_v4_address(&netint->addr) &
                   lwpaip_v4_address(&netint->mask);
    LwpaIpAddr test_addr;
    
    if (nets_already_tried.find(net) != nets_already_tried.end())
      continue;
    nets_already_tried.insert(net);
    lwpaip_set_v4_address(&test_addr, net + 1);
    ASSERT_EQ(netint_get_iface_for_dest(&test_addr, netint_arr, num_netints),
              netint);
  }
  in_addr ext_addr_plat;
  ASSERT_EQ(1, inet_pton(AF_INET, "200.220.3.2", &ext_addr_plat));
  LwpaIpAddr ext_addr;
  lwpaip_set_v4_address(&ext_addr, ntohl(ext_addr_plat.s_addr));
  const LwpaNetintInfo *def = netint_get_iface_for_dest(&ext_addr, netint_arr,
                                                         num_netints);
  ASSERT_TRUE(def != NULL);
  ASSERT_TRUE(def->is_default);
}
