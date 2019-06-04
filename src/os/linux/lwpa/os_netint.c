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

#include <stdlib.h>
#include <ifaddrs.h>
#include <errno.h>
#include <asm/types.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "lwpa/socket.h"
#include "lwpa/private/netint.h"
#include "os_error.h"

/***************************** Private types *********************************/

typedef struct RoutingTableEntry
{
  LwpaIpAddr addr;
  LwpaIpAddr mask;
  int interface_index;
} RoutingTableEntry;

/**************************** Private variables ******************************/

static struct LwpaNetintState
{
  bool initialized;

  RoutingTableEntry* routing_table_v4;
  RoutingTableEntry* routing_table_v6;

  struct ifaddrs* os_addrs;
  size_t num_addrs;
  LwpaNetintInfo* lwpa_addrs;
} state;

/*********************** Private function prototypes *************************/

static lwpa_error_t build_routing_tables();
static lwpa_error_t build_routing_table(int family, RoutingTableEntry** table);
static void free_routing_tables();

static lwpa_error_t enumerate_os_addrs();
static void free_os_addrs();

/*************************** Function definitions ****************************/

lwpa_error_t lwpa_netint_init()
{
  lwpa_error_t res = kLwpaErrOk;
  if (!state.initialized)
  {
    res = re_enumerate_os_addrs();
    if (res == kLwpaErrOk)
      state.initialized = true;
  }
  return res;
}

void lwpa_netint_deinit()
{
  if (state.initialized)
  {
    free_os_addrs();
    state.initialized = false;
  }
}

size_t lwpa_netint_get_num_interfaces()
{
  return state.num_addrs;
}

size_t lwpa_netint_get_interfaces(LwpaNetintInfo* netint_arr, size_t netint_arr_size)
{
  if (!netint_arr || netint_arr_size == 0)
    return 0;

  size_t addrs_copied = (netint_arr_size < state.num_addrs ? netint_arr_size : state.num_addrs);
  memcpy(netint_arr, state.lwpa_addrs, addrs_copied * sizeof(LwpaNetintInfo));
  return addrs_copied;
}

lwpa_error_t build_routing_tables()
{
  state.routing_table_v4 = NULL;
  state.routing_table_v6 = NULL;

  lwpa_error_t res = build_routing_table(AF_INET, &state.routing_table_v4);
  if (res == kLwpaErrOk)
  {
    res = build_routing_table(AF_INET6, &state.routing_table_v6);
  }

  if (res != kLwpaErrOk)
    free_routing_tables();

  return res;
}

lwpa_error_t build_routing_table(int family, RoutingTableEntry** table)
{
  // Create a netlink socket, send a netlink request to get the routing table, and receive the
  // reply. If the buffer was not big enough, repeat (cannot reuse the same socket because there
  // could be issues with p

  lwpa_error_t result = kLwpaErrOk;
  bool done = false;
  size_t recvbufsize = 2048;  // Tests show this is usually enough for small routing tables
  while (result == kLwpaErrOk && !done)
  {
    struct sockaddr_nl addr;
    memset(&addr, 0, sizeof(struct sockaddr_nl));
    addr.nl_family = AF_NETLINK;

    int sock = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
    if (sock == -1)
      result = errno_os_to_lwpa(errno);

    if (result == kLwpaErrOk)
    {
      if (0 != bind(sock, (struct sockaddr*)&addr, sizeof(addr)))
        result = errno_os_to_lwpa(errno);
    }

    if (result)
      result = create_send_request(sock, family);

    if (result)
    {
      result = recv_parse_reply(sock, recvbufsize);
      switch (result)
      {
        case kLwpaErrOk:
          done = true;
          break;
        case kLwpaErrBufSize:
          recvbufsize *= 2;
          break;
        default:
      }
    }

    close(sock);
  }
  return result;
}

/* Quick helper for re_enumerate_os_addrs() to determine entries to skip in the linked list. */
static bool should_skip_ifaddr(const struct ifaddrs* ifaddr)
{
  // Skip an entry if it doesn't have an address, or if the address is not IPv4 or IPv6.
  return (!ifaddr->ifa_addr || (ifaddr->ifa_addr->sa_family != AF_INET && ifaddr->ifa_addr->sa_family != AF_INET6));
}

lwpa_error_t enumerate_os_addrs()
{
  lwpa_error_t res = build_routing_tables();
  if (res != kLwpaErrOk)
    return res;

  if (getifaddrs(&state.os_addrs) < 0)
  {
    return errno_os_to_lwpa(errno);
  }

  // Pass 1: Total the number of addresses
  state.num_addrs = 0;
  for (struct ifaddrs* ifaddr = state.os_addrs; ifaddr; ifaddr = ifaddr->ifa_next)
  {
    if (should_skip_ifaddr(ifaddr))
      continue;

    ++state.num_addrs;
  }

  if (state.num_addrs == 0)
  {
    freeifaddrs(state.os_addrs);
    return kLwpaErrNoNetints;
  }

  // Allocate our interface array
  state.lwpa_addrs = calloc(state.num_addrs, sizeof(LwpaNetintInfo));
  if (!state.lwpa_addrs)
  {
    freeifaddrs(state.os_addrs);
    return kLwpaErrNoMem;
  }

  // Pass 2: Fill in all the info about each address
  size_t current_lwpa_index = 0;
  for (struct ifaddrs* ifaddr = state.os_addrs; ifaddr; ifaddr = ifaddr->ifa_next)
  {
    if (should_skip_ifaddr(ifaddr))
      continue;

    LwpaNetintInfo* current_info = &state.lwpa_addrs[current_lwpa_index];

    // Interface name
    strncpy(current_info->name, ifaddr->ifa_name, LWPA_NETINTINFO_NAME_LEN);
    current_info->name[LWPA_NETINTINFO_NAME_LEN - 1] = '\0';

    // Interface address
    LwpaSockaddr temp_sockaddr;
    sockaddr_os_to_lwpa(&temp_sockaddr, &ifaddr->ifa_addr);
    current_info->addr = temp_sockaddr.ip;

    // Interface netmask
    sockaddr_os_to_lwpa(&temp_sockaddr, &ifaddr->ifa_netmask);
    current_info->mask = temp_sockaddr.ip;
  }

  return kLwpaErrOk;
}

void free_routing_tables()
{
  if (state.routing_table_v4)
  {
    free(state.routing_table_v4);
    state.routing_table_v4 = NULL;
  }
  if (state.routing_table_v6)
  {
    free(state.routing_table_v6);
    state.routing_table_v6 = NULL;
  }
}

void free_os_addrs()
{
  if (state.os_addrs)
  {
    freeifaddrs(state.os_addrs);
    state.os_addrs = NULL;
  }
  if (state.lwpa_addrs)
  {
    free(state.lwpa_addrs);
    state.lwpa_addrs = NULL;
  }
}
