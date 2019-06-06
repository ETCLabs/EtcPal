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
  LwpaNetintInfo* netint_arr = new LwpaNetintInfo[num_netints];
  size_t num_netints_returned = lwpa_netint_get_interfaces(netint_arr, num_netints);
  ASSERT_EQ(num_netints_returned, num_netints);

  size_t num_defaults = 0;
  for (LwpaNetintInfo* netint = netint_arr; netint < netint_arr + num_netints_returned; ++netint)
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
  LwpaNetintInfo* netint_arr = new LwpaNetintInfo[num_netints];
  memset(&def, 0, sizeof def);
  memset(netint_arr, 0, sizeof(struct LwpaNetintInfo) * num_netints);

  num_netints = lwpa_netint_get_interfaces(netint_arr, num_netints);
  ASSERT_TRUE(lwpa_netint_get_default_interface(&def));
  ASSERT_TRUE(def.is_default);
  for (LwpaNetintInfo* netint = netint_arr; netint < netint_arr + num_netints; ++netint)
  {
    if (netint->is_default)
    {
      ASSERT_EQ(0, memcmp(netint, &def, sizeof def));
      break;
    }
  }
  delete[] netint_arr;
}

TEST_F(NetintTest, ipv4_routing)
{
  ASSERT_GT(num_netints, 0u);

  LwpaNetintInfo* netint_arr = new LwpaNetintInfo[num_netints];
  num_netints = lwpa_netint_get_interfaces(netint_arr, num_netints);

  std::set<uint32_t> nets_already_tried;

  for (LwpaNetintInfo* netint = netint_arr; netint < netint_arr + num_netints; ++netint)
  {
    uint32_t net = LWPA_IP_V4_ADDRESS(&netint->addr) & LWPA_IP_V4_ADDRESS(&netint->mask);
    LwpaIpAddr test_addr;

    if (nets_already_tried.find(net) != nets_already_tried.end())
      continue;
    nets_already_tried.insert(net);
    LWPA_IP_SET_V4_ADDRESS(&test_addr, net + 1);

    LwpaNetintInfo netint_res;
    ASSERT_EQ(kLwpaErrOk, lwpa_netint_get_interface_for_dest(&test_addr, &netint_res));
    EXPECT_TRUE(lwpa_ip_equal(&netint_res.addr, &netint->addr));
  }

  LwpaIpAddr ext_addr;
  LWPA_IP_SET_V4_ADDRESS(&ext_addr, 0xc8dc0302);  // 200.220.3.2
  LwpaNetintInfo def;
  ASSERT_EQ(kLwpaErrOk, lwpa_netint_get_interface_for_dest(&ext_addr, &def));
  ASSERT_TRUE(def.is_default);
}
