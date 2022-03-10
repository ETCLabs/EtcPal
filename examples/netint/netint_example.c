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

/*
 * An example application showing usage of the etcpal_netint functions.
 * This example will print information about each network interface on your machine. It is
 * platform-neutral.
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "etcpal/netint.h"
#include "etcpal/socket.h"

#define FORMAT_BUF_SIZE 50

static char header_format[FORMAT_BUF_SIZE];
static char line_format[FORMAT_BUF_SIZE];
static char continuation_line_format[FORMAT_BUF_SIZE];

#define NAME_COL_HEADER  "Name"
#define ADDR_COL_HEADER  "Address(es)"
#define MAC_COL_HEADER   "MAC"
#define INDEX_COL_HEADER "OS Index"
#define STATE_COL_HEADER "State"

// Create a format string for printf based on the column width of the longest entry in each column
void create_format_strings(const EtcPalNetintInfo* netint_arr)
{
  size_t longest_name = 0;
  size_t longest_addr = 0;

  for (const EtcPalNetintInfo* netint = netint_arr; netint; netint = netint->next)
  {
    size_t name_len = strlen(netint->friendly_name);
    if (name_len > longest_name)
      longest_name = name_len;

    for (const EtcPalNetintAddr* addr = netint->addrs; addr; addr = addr->next)
    {
      char addr_str[ETCPAL_NETINT_ADDR_STRING_BYTES] = {'\0'};
      etcpal_netint_addr_to_string(addr, addr_str);

      size_t addr_len = strlen(addr_str);
      if (addr_len > longest_addr)
        longest_addr = addr_len;
    }
  }

  if (longest_name < sizeof(NAME_COL_HEADER))
    longest_name = sizeof(NAME_COL_HEADER);
  if (longest_addr < sizeof(ADDR_COL_HEADER))
    longest_addr = sizeof(ADDR_COL_HEADER);

  snprintf(header_format, FORMAT_BUF_SIZE, "%%-%zus %%-%zus %%-17s %%s %%s\n", longest_name, longest_addr);
  snprintf(line_format, FORMAT_BUF_SIZE, "%%-%zus %%-%zus %%-17s %%8u %%s\n", longest_name, longest_addr);

  // Fill the beginning of the continuation line with spaces the size of the NAME field
  memset(continuation_line_format, ' ', longest_name + 1);
  snprintf(&continuation_line_format[longest_name + 1], FORMAT_BUF_SIZE - longest_name - 1, "%%s\n");
}

int main(void)
{
  size_t num_interfaces = etcpal_netint_get_num_interfaces();
  if (num_interfaces == 0)
  {
    printf("Error: No network interfaces found on system.\n");
    return 1;
  }

  size_t   netint_buf_size = ETCPAL_NETINT_BUF_SIZE_ESTIMATE(5, 3);
  uint8_t* netint_buf      = (uint8_t*)malloc(netint_buf_size);
  assert(netint_buf);
  etcpal_error_t res = etcpal_netint_get_interfaces(netint_buf, &netint_buf_size);
  while (res == kEtcPalErrBufSize)
  {
    netint_buf = (uint8_t*)realloc(netint_buf, netint_buf_size);
    assert(netint_buf);
    res = etcpal_netint_get_interfaces(netint_buf, &netint_buf_size);
  }
  if (res != kEtcPalErrOk)
  {
    printf("Error getting network interfaces: %s\n", etcpal_strerror(res));
    free(netint_buf);
    return 1;
  }

  const EtcPalNetintInfo* netint_arr = (const EtcPalNetintInfo*)netint_buf;

  create_format_strings(netint_arr);

  printf("Network interfaces found:\n");
  printf(header_format, NAME_COL_HEADER, ADDR_COL_HEADER, MAC_COL_HEADER, INDEX_COL_HEADER, STATE_COL_HEADER);

  for (const EtcPalNetintInfo* netint = netint_arr; netint; netint = netint->next)
  {
    char mac_str[ETCPAL_MAC_STRING_BYTES];
    etcpal_mac_to_string(&netint->mac, mac_str);
    bool up = etcpal_netint_is_up(netint->index);

    if (!netint->addrs)
    {
      printf(line_format, netint->friendly_name, "None", mac_str, netint->index, up ? "Up" : "Down");
    }
    else
    {
      for (const EtcPalNetintAddr* addr = netint->addrs; addr; addr = addr->next)
      {
        char addr_str[ETCPAL_NETINT_ADDR_STRING_BYTES] = {'\0'};
        etcpal_netint_addr_to_string(addr, addr_str);
        if (addr == netint->addrs)
        {
          printf(line_format, netint->friendly_name, addr_str, mac_str, netint->index, up ? "Up" : "Down");
        }
        else
        {
          printf(continuation_line_format, addr_str);
        }
      }
    }
  }

  if (kEtcPalErrOk == etcpal_netint_get_default_interface(kEtcPalIpTypeV4, netint_buf, &netint_buf_size))
  {
    const EtcPalNetintInfo* default_netint = (const EtcPalNetintInfo*)netint_buf;
    printf("Default IPv4 interface: %s (%u)\n", default_netint->friendly_name, default_netint->index);
  }

  if (kEtcPalErrOk == etcpal_netint_get_default_interface(kEtcPalIpTypeV6, netint_buf, &netint_buf_size))
  {
    const EtcPalNetintInfo* default_netint = (const EtcPalNetintInfo*)netint_buf;
    printf("Default IPv6 interface: %s (%u)\n", default_netint->friendly_name, default_netint->index);
  }

  if (netint_buf)
    free(netint_buf);

  return 0;
}
