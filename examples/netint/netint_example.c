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

/* An example application showing usage of the lwpa_netint functions.
 * This example will print information about each network interface on your machine. It is
 * platform-neutral.
 */

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "lwpa/netint.h"
#include "lwpa/socket.h"

int main()
{
  lwpa_error_t init_res = lwpa_init(LWPA_FEATURE_NETINTS);
  if (init_res != kLwpaErrOk)
  {
    printf("lwpa_init() failed with error: '%s'\n", lwpa_strerror(init_res));
    return 1;
  }

  size_t num_interfaces = lwpa_netint_get_num_interfaces();
  if (num_interfaces == 0)
  {
    printf("Error: No network interfaces found on system.\n");
    lwpa_deinit(LWPA_FEATURE_NETINTS);
    return 1;
  }

  LwpaNetintInfo* netint_arr = (LwpaNetintInfo*)calloc(num_interfaces, sizeof(LwpaNetintInfo));
  assert(netint_arr);
  num_interfaces = lwpa_netint_get_interfaces(netint_arr, num_interfaces);

  printf("Network interfaces found:\n");
  printf("%-46s %-46s %-46s %s", "Address", "Netmask", "Gateway", "OS Index\n");
  for (LwpaNetintInfo* netint = netint_arr; netint < netint_arr + num_interfaces; ++netint)
  {
    char addr_str[LWPA_INET6_ADDRSTRLEN];
    char netmask_str[LWPA_INET6_ADDRSTRLEN];
    char gw_str[LWPA_INET6_ADDRSTRLEN];
    lwpa_inet_ntop(&netint->addr, addr_str, LWPA_INET6_ADDRSTRLEN);
    lwpa_inet_ntop(&netint->mask, netmask_str, LWPA_INET6_ADDRSTRLEN);
    lwpa_inet_ntop(&netint->gate, gw_str, LWPA_INET6_ADDRSTRLEN);

    printf("%-46s %-46s %-46s %8d\n", addr_str, netmask_str, gw_str, netint->ifindex);
  }

  lwpa_deinit(LWPA_FEATURE_NETINTS);

  return 0;
}
