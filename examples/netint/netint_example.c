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

#define FORMAT_BUF_SIZE 50

static char header_format[FORMAT_BUF_SIZE];
static char line_format[FORMAT_BUF_SIZE];

#define NAME_COL_HEADER "Name"
#define ADDR_COL_HEADER "Address"
#define NETMASK_COL_HEADER "Netmask"
#define GATEWAY_COL_HEADER "Gateway"
#define MAC_COL_HEADER "MAC"
#define INDEX_COL_HEADER "OS Index"

// Create a format string for printf based on the column width of the longest entry in each column
void create_format_strings(const LwpaNetintInfo* netint_arr, size_t num_interfaces)
{
  size_t longest_name = 0;
  size_t longest_addr = 0;
  size_t longest_netmask = 0;
  size_t longest_gateway = 0;

  for (const LwpaNetintInfo* netint = netint_arr; netint < netint_arr + num_interfaces; ++netint)
  {
    char addr_str[LWPA_INET6_ADDRSTRLEN] = {'\0'};
    char netmask_str[LWPA_INET6_ADDRSTRLEN] = {'\0'};
    char gw_str[LWPA_INET6_ADDRSTRLEN] = {'\0'};

    lwpa_inet_ntop(&netint->addr, addr_str, LWPA_INET6_ADDRSTRLEN);
    lwpa_inet_ntop(&netint->mask, netmask_str, LWPA_INET6_ADDRSTRLEN);
    lwpa_inet_ntop(&netint->gate, gw_str, LWPA_INET6_ADDRSTRLEN);

    size_t name_len = strlen(netint->name);
    size_t addr_len = strlen(addr_str);
    size_t netmask_len = strlen(netmask_str);
    size_t gw_len = strlen(gw_str);

    if (name_len > longest_name)
      longest_name = name_len;
    if (addr_len > longest_addr)
      longest_addr = addr_len;
    if (netmask_len > longest_netmask)
      longest_netmask = netmask_len;
    if (gw_len > longest_gateway)
      longest_gateway = gw_len;
  }

  if (longest_name < sizeof(NAME_COL_HEADER))
    longest_name = sizeof(NAME_COL_HEADER);
  if (longest_addr < sizeof(ADDR_COL_HEADER))
    longest_addr = sizeof(ADDR_COL_HEADER);
  if (longest_netmask < sizeof(NETMASK_COL_HEADER))
    longest_netmask = sizeof(NETMASK_COL_HEADER);
  if (longest_gateway < sizeof(GATEWAY_COL_HEADER))
    longest_gateway = sizeof(GATEWAY_COL_HEADER);

  snprintf(header_format, FORMAT_BUF_SIZE, "%%-%zus %%-%zus %%-%zus %%-%zus %%-17s %%s\n", longest_name, longest_addr,
           longest_netmask, longest_gateway);
  snprintf(line_format, FORMAT_BUF_SIZE, "%%-%zus %%-%zus %%-%zus %%-%zus %%-17s %%8d\n", longest_name, longest_addr,
           longest_netmask, longest_gateway);
}

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

  create_format_strings(netint_arr, num_interfaces);

  printf("Network interfaces found:\n");
  printf(header_format, NAME_COL_HEADER, ADDR_COL_HEADER, NETMASK_COL_HEADER, GATEWAY_COL_HEADER, MAC_COL_HEADER,
         INDEX_COL_HEADER);
  for (LwpaNetintInfo* netint = netint_arr; netint < netint_arr + num_interfaces; ++netint)
  {
    char addr_str[LWPA_INET6_ADDRSTRLEN] = {'\0'};
    char netmask_str[LWPA_INET6_ADDRSTRLEN] = {'\0'};
    char gw_str[LWPA_INET6_ADDRSTRLEN] = {'\0'};
    char mac_str[18];
    lwpa_inet_ntop(&netint->addr, addr_str, LWPA_INET6_ADDRSTRLEN);
    lwpa_inet_ntop(&netint->mask, netmask_str, LWPA_INET6_ADDRSTRLEN);
    lwpa_inet_ntop(&netint->gate, gw_str, LWPA_INET6_ADDRSTRLEN);
    snprintf(mac_str, 18, "%02x:%02x:%02x:%02x:%02x:%02x", netint->mac[0], netint->mac[1], netint->mac[2],
             netint->mac[3], netint->mac[4], netint->mac[5]);

    printf(line_format, netint->name, addr_str, netmask_str, gw_str, mac_str, netint->ifindex);
  }

  lwpa_deinit(LWPA_FEATURE_NETINTS);

  return 0;
}
