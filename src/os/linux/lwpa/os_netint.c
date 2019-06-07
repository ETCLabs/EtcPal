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

/* The os_netint implementation for Linux.
 *
 * Uses getifaddrs() to get an initial list of network interfaces. More info:
 * http://man7.org/linux/man-pages/man3/getifaddrs.3.html
 *
 * Then uses netlink sockets (specifically RTNETLINK) to fill out missing information about each
 * interface. More info:
 *
 * Netlink sockets: http://man7.org/linux/man-pages/man7/netlink.7.html
 * Netlink macros, for decoding netlink messages: http://man7.org/linux/man-pages/man3/netlink.3.html
 * RtNetlink sockets: http://man7.org/linux/man-pages/man7/rtnetlink.7.html
 * Some sample RtNetlink code: https://www.linuxjournal.com/article/8498
 */

#include "lwpa/netint.h"

#include <stdlib.h>
#include <limits.h>
#include <string.h>

#ifdef LWPA_NETINT_DEBUG_OUTPUT
#include <stdio.h>
#endif

#include <ifaddrs.h>
#include <errno.h>
#include <asm/types.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "lwpa/socket.h"
#include "lwpa/private/netint.h"
#include "os_error.h"

/***************************** Private types *********************************/

typedef struct RoutingTableEntry
{
  LwpaIpAddr addr;
  LwpaIpAddr mask;
  LwpaIpAddr gateway;
  int interface_index;
  int metric;
} RoutingTableEntry;

typedef struct RoutingTable
{
  RoutingTableEntry* entries;
  size_t size;
} RoutingTable;

/* A composite struct representing an RT_NETLINK request sent over a netlink socket. */
typedef struct RtNetlinkRequest
{
  struct nlmsghdr nl_header;
  struct rtmsg rt_msg;
} RtNetlinkRequest;

/**************************** Private variables ******************************/

static struct LwpaNetintState
{
  bool initialized;

  RoutingTable routing_table_v4;
  RoutingTable routing_table_v6;

  struct ifaddrs* os_addrs;
  size_t num_netints;
  LwpaNetintInfo* lwpa_netints;
} state;

/*********************** Private function prototypes *************************/

// Functions for building the routing tables
static lwpa_error_t build_routing_tables();
static lwpa_error_t build_routing_table(int family, RoutingTable* table);
static void free_routing_tables();
static void free_routing_table(RoutingTable* table);

// Interacting with RTNETLINK
static lwpa_error_t send_netlink_route_request(int socket, int family);
static lwpa_error_t receive_netlink_route_reply(int sock, int family, size_t buf_size, RoutingTable* table);
static lwpa_error_t parse_netlink_route_reply(int family, const char* buffer, size_t nl_msg_size, RoutingTable* table);

// Manipulating routing table entries
static void init_routing_table_entry(RoutingTableEntry* entry);
static int compare_routing_table_entries(const void* a, const void* b);

// Functions for enumerating the interfaces
static lwpa_error_t enumerate_netints();
static void get_default_gateway(LwpaNetintInfo* netint);
static void free_netints();

#if LWPA_NETINT_DEBUG_OUTPUT
static void debug_print_routing_table(RoutingTable* table);
#endif

/*************************** Function definitions ****************************/

lwpa_error_t lwpa_netint_init()
{
  lwpa_error_t res = kLwpaErrOk;
  if (!state.initialized)
  {
    res = enumerate_netints();
    if (res == kLwpaErrOk)
      state.initialized = true;
  }
  return res;
}

void lwpa_netint_deinit()
{
  if (state.initialized)
  {
    free_netints();
    state.initialized = false;
  }
}

size_t lwpa_netint_get_num_interfaces()
{
  return state.num_netints;
}

size_t lwpa_netint_get_interfaces(LwpaNetintInfo* netint_arr, size_t netint_arr_size)
{
  if (!netint_arr || netint_arr_size == 0)
    return 0;

  size_t addrs_copied = (netint_arr_size < state.num_netints ? netint_arr_size : state.num_netints);
  memcpy(netint_arr, state.lwpa_netints, addrs_copied * sizeof(LwpaNetintInfo));
  return addrs_copied;
}

lwpa_error_t build_routing_tables()
{
  lwpa_error_t res = build_routing_table(AF_INET, &state.routing_table_v4);
  if (res == kLwpaErrOk)
  {
    res = build_routing_table(AF_INET6, &state.routing_table_v6);
  }

  if (res != kLwpaErrOk)
    free_routing_tables();

#if LWPA_NETINT_DEBUG_OUTPUT
  debug_print_routing_table(&state.routing_table_v4);
  debug_print_routing_table(&state.routing_table_v6);
#endif

  return res;
}

lwpa_error_t build_routing_table(int family, RoutingTable* table)
{
  // Create a netlink socket, send a netlink request to get the routing table, and receive the
  // reply. If the buffer was not big enough, repeat (cannot reuse the same socket because we've
  // often received partial messages that must be discarded)

  lwpa_error_t result = kLwpaErrOk;
  bool done = false;
  size_t recv_buf_size = 2048;  // Tests show this is usually enough for small routing tables
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

    if (result == kLwpaErrOk)
      result = send_netlink_route_request(sock, family);

    if (result == kLwpaErrOk)
    {
      result = receive_netlink_route_reply(sock, family, recv_buf_size, table);
      switch (result)
      {
        case kLwpaErrOk:
          done = true;
          break;
        case kLwpaErrBufSize:
          recv_buf_size *= 2;
          result = kLwpaErrOk;
          break;
        default:
          break;
      }
    }

    close(sock);
  }
  return result;
}

lwpa_error_t send_netlink_route_request(int socket, int family)
{
  // Build the request
  RtNetlinkRequest req;
  memset(&req, 0, sizeof(req));
  req.nl_header.nlmsg_len = NLMSG_LENGTH(sizeof(RtNetlinkRequest));
  req.nl_header.nlmsg_type = RTM_GETROUTE;
  req.nl_header.nlmsg_flags = (__u16)(NLM_F_REQUEST | NLM_F_DUMP);
  req.nl_header.nlmsg_pid = (__u32)getpid();
  req.rt_msg.rtm_family = (unsigned char)family;
  req.rt_msg.rtm_table = RT_TABLE_MAIN;

  // Send it to the kernel
  struct sockaddr_nl naddr;
  memset(&naddr, 0, sizeof(naddr));
  naddr.nl_family = AF_NETLINK;

  if (sendto(socket, &req.nl_header, req.nl_header.nlmsg_len, 0, (struct sockaddr*)&naddr, sizeof(naddr)) >= 0)
    return kLwpaErrOk;
  else
    return errno_os_to_lwpa(errno);
}

lwpa_error_t receive_netlink_route_reply(int sock, int family, size_t buf_size, RoutingTable* table)
{
  // Allocate slightly larger than buf_size, so we can detect when more room is needed
  size_t real_size = buf_size + 20;
  char* buffer = (char*)malloc(real_size);
  if (!buffer)
    return kLwpaErrNoMem;
  memset(buffer, 0, real_size);

  char* cur_ptr = buffer;
  size_t nl_msg_size = 0;

  // Read from the socket until the NLMSG_DONE is returned in the type of the RTNETLINK message
  while (1)
  {
    // While we are receiving with real_size, checking against buf_size will detect when we've
    // passed the limit given by the app
    if (buf_size <= nl_msg_size)
    {
      free(buffer);
      return kLwpaErrBufSize;
    }

    ssize_t recv_res = recv(sock, cur_ptr, real_size - nl_msg_size, 0);
    if (recv_res == -1)
    {
      free(buffer);
      return errno_os_to_lwpa(errno);
    }

    struct nlmsghdr* nl_header = (struct nlmsghdr*)cur_ptr;

    if (nl_header->nlmsg_type == NLMSG_DONE)
      break;

    // Adjust our position in the buffer and size received
    cur_ptr += recv_res;
    nl_msg_size += (size_t)recv_res;
  }

  lwpa_error_t parse_res = parse_netlink_route_reply(family, buffer, nl_msg_size, table);

  free(buffer);
  return parse_res;
}

lwpa_error_t parse_netlink_route_reply(int family, const char* buffer, size_t nl_msg_size, RoutingTable* table)
{
  table->size = 0;
  table->entries = NULL;

  // Parse the result
  // outer loop: loops thru all the NETLINK headers that also include the route entry header
  struct nlmsghdr* nl_header = (struct nlmsghdr*)buffer;
  for (; NLMSG_OK(nl_header, nl_msg_size); nl_header = NLMSG_NEXT(nl_header, nl_msg_size))
  {
    RoutingTableEntry new_entry;
    init_routing_table_entry(&new_entry);

    bool new_entry_valid = true;

    // get route entry header
    struct rtmsg* rt_message = (struct rtmsg*)NLMSG_DATA(nl_header);

    // Filter out entries from the local routing table. Netlink seems to give us those even though
    // we only asked for the main one.
    if (rt_message->rtm_type != RTN_LOCAL && rt_message->rtm_type != RTN_BROADCAST &&
        rt_message->rtm_type != RTN_ANYCAST)
    {
      // inner loop: loop thru all the attributes of one route entry.
      struct rtattr* rt_attributes = (struct rtattr*)RTM_RTA(rt_message);
      int rt_attr_size = (int)RTM_PAYLOAD(nl_header);
      for (; RTA_OK(rt_attributes, rt_attr_size); rt_attributes = RTA_NEXT(rt_attributes, rt_attr_size))
      {
        // We only care about the gateway and DST attribute
        if (rt_attributes->rta_type == RTA_DST)
        {
          if (family == AF_INET6)
            LWPA_IP_SET_V6_ADDRESS(&new_entry.addr, ((struct in6_addr*)RTA_DATA(rt_attributes))->s6_addr);
          else
            LWPA_IP_SET_V4_ADDRESS(&new_entry.addr, ntohl(((struct in_addr*)RTA_DATA(rt_attributes))->s_addr));
        }
        else if (rt_attributes->rta_type == RTA_GATEWAY)
        {
          if (family == AF_INET6)
            LWPA_IP_SET_V6_ADDRESS(&new_entry.gateway, ((struct in6_addr*)RTA_DATA(rt_attributes))->s6_addr);
          else
            LWPA_IP_SET_V4_ADDRESS(&new_entry.gateway, ntohl(((struct in_addr*)RTA_DATA(rt_attributes))->s_addr));
        }
        else if (rt_attributes->rta_type == RTA_OIF)
        {
          new_entry.interface_index = *((int*)RTA_DATA(rt_attributes));
        }
        else if (rt_attributes->rta_type == RTA_PRIORITY)
        {
          new_entry.metric = *((int*)RTA_DATA(rt_attributes));
        }
      }
    }
    else
    {
      new_entry_valid = false;
    }

    if (!LWPA_IP_IS_INVALID(&new_entry.addr))
    {
      new_entry.mask = lwpa_ip_mask_from_length(new_entry.addr.type, rt_message->rtm_dst_len);
    }

    // Insert the new entry into the list
    if (new_entry_valid)
    {
      ++table->size;
      if (table->entries)
        table->entries = (RoutingTableEntry*)realloc(table->entries, table->size * sizeof(RoutingTableEntry));
      else
        table->entries = (RoutingTableEntry*)malloc(sizeof(RoutingTableEntry));
      table->entries[table->size - 1] = new_entry;
    }
  }

  if (table->size > 0)
  {
    qsort(table->entries, table->size, sizeof(RoutingTableEntry), compare_routing_table_entries);
    return kLwpaErrOk;
  }
  else
  {
    return kLwpaErrSys;
  }
}

void init_routing_table_entry(RoutingTableEntry* entry)
{
  LWPA_IP_SET_INVALID(&entry->addr);
  LWPA_IP_SET_INVALID(&entry->mask);
  LWPA_IP_SET_INVALID(&entry->gateway);
  entry->interface_index = -1;
  entry->metric = INT_MAX;
}

int compare_routing_table_entries(const void* a, const void* b)
{
  RoutingTableEntry* e1 = (RoutingTableEntry*)a;
  RoutingTableEntry* e2 = (RoutingTableEntry*)b;

  unsigned int mask_length_1 = lwpa_ip_mask_length(&e1->mask);
  unsigned int mask_length_2 = lwpa_ip_mask_length(&e2->mask);
  if (mask_length_1 > mask_length_2)
  {
    return -1;
  }
  else if (mask_length_1 < mask_length_2)
  {
    return 1;
  }
  else
  {
    if (e1->metric < e2->metric)
      return -1;
    else if (e1->metric > e2->metric)
      return 1;
    else
      return 0;
  }
}

/* Quick helper for enumerate_netints() to determine entries to skip in the linked list. */
static bool should_skip_ifaddr(const struct ifaddrs* ifaddr)
{
  // Skip an entry if it doesn't have an address, or if the address is not IPv4 or IPv6.
  return (!ifaddr->ifa_addr || (ifaddr->ifa_addr->sa_family != AF_INET && ifaddr->ifa_addr->sa_family != AF_INET6));
}

lwpa_error_t enumerate_netints()
{
  lwpa_error_t res = build_routing_tables();
  if (res != kLwpaErrOk)
    return res;

  int ioctl_sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (ioctl_sock == -1)
    return errno_os_to_lwpa(errno);

  if (getifaddrs(&state.os_addrs) < 0)
  {
    close(ioctl_sock);
    return errno_os_to_lwpa(errno);
  }

  // Pass 1: Total the number of addresses
  state.num_netints = 0;
  for (struct ifaddrs* ifaddr = state.os_addrs; ifaddr; ifaddr = ifaddr->ifa_next)
  {
    if (should_skip_ifaddr(ifaddr))
      continue;

    ++state.num_netints;
  }

  if (state.num_netints == 0)
  {
    freeifaddrs(state.os_addrs);
    close(ioctl_sock);
    return kLwpaErrNoNetints;
  }

  // Allocate our interface array
  state.lwpa_netints = (LwpaNetintInfo*)calloc(state.num_netints, sizeof(LwpaNetintInfo));
  if (!state.lwpa_netints)
  {
    freeifaddrs(state.os_addrs);
    close(ioctl_sock);
    return kLwpaErrNoMem;
  }

  // Pass 2: Fill in all the info about each address
  size_t current_lwpa_index = 0;
  for (struct ifaddrs* ifaddr = state.os_addrs; ifaddr; ifaddr = ifaddr->ifa_next)
  {
    if (should_skip_ifaddr(ifaddr))
      continue;

    LwpaNetintInfo* current_info = &state.lwpa_netints[current_lwpa_index++];

    // Interface name
    strncpy(current_info->name, ifaddr->ifa_name, LWPA_NETINTINFO_NAME_LEN);
    current_info->name[LWPA_NETINTINFO_NAME_LEN - 1] = '\0';

    // Interface address
    LwpaSockaddr temp_sockaddr;
    sockaddr_os_to_lwpa(&temp_sockaddr, ifaddr->ifa_addr);
    current_info->addr = temp_sockaddr.ip;

    // Interface netmask
    sockaddr_os_to_lwpa(&temp_sockaddr, ifaddr->ifa_netmask);
    current_info->mask = temp_sockaddr.ip;

    // Default gateway
    get_default_gateway(current_info);

    // Struct ifreq to use with ioctl() calls
    struct ifreq if_req;
    strncpy(if_req.ifr_name, ifaddr->ifa_name, IFNAMSIZ);

    // Hardware address
    int ioctl_res = ioctl(ioctl_sock, SIOCGIFHWADDR, &if_req);
    if (ioctl_res == 0)
      memcpy(current_info->mac, if_req.ifr_hwaddr.sa_data, LWPA_NETINTINFO_MAC_LEN);
    else
      memset(current_info->mac, 0, LWPA_NETINTINFO_MAC_LEN);

    // Interface index
    ioctl_res = ioctl(ioctl_sock, SIOCGIFINDEX, &if_req);
    if (ioctl_res == 0)
      current_info->ifindex = if_req.ifr_ifindex;
    else
      current_info->ifindex = -1;
  }

  return kLwpaErrOk;
}

void get_default_gateway(LwpaNetintInfo* netint)
{
  RoutingTable* table_to_use = (LWPA_IP_IS_V6(&netint->addr) ? &state.routing_table_v6 : &state.routing_table_v4);

  for (RoutingTableEntry* entry = table_to_use->entries; entry < table_to_use->entries + table_to_use->size; ++entry)
  {
    if (!LWPA_IP_IS_INVALID(&entry->gateway) &&
        lwpa_ip_network_portions_equal(&entry->addr, &netint->addr, &netint->mask))
    {
      netint->gate = entry->gateway;
      return;
    }
  }
  // Reached the end of the routing table - no default gateway
  LWPA_IP_SET_INVALID(&netint->gate);
}

void free_routing_tables()
{
  free_routing_table(&state.routing_table_v4);
  free_routing_table(&state.routing_table_v6);
}

void free_routing_table(RoutingTable* table)
{
  if (table->entries)
  {
    free(table->entries);
    table->entries = NULL;
  }
  table->size = 0;
}

void free_netints()
{
  if (state.os_addrs)
  {
    freeifaddrs(state.os_addrs);
    state.os_addrs = NULL;
  }
  if (state.lwpa_netints)
  {
    free(state.lwpa_netints);
    state.lwpa_netints = NULL;
  }

  free_routing_tables();
}

#if LWPA_NETINT_DEBUG_OUTPUT
void debug_print_routing_table(RoutingTable* table)
{
  printf("%-40s %-40s %-40s %s %s\n", "Address", "Mask", "Gateway", "Metric", "Index");
  for (RoutingTableEntry* entry = table->entries; entry < table->entries + table->size; ++entry)
  {
    char addr_str[LWPA_INET6_ADDRSTRLEN];
    char mask_str[LWPA_INET6_ADDRSTRLEN];
    char gw_str[LWPA_INET6_ADDRSTRLEN];

    if (!LWPA_IP_IS_INVALID(&entry->addr))
      lwpa_inet_ntop(&entry->addr, addr_str, LWPA_INET6_ADDRSTRLEN);
    else
      strcpy(addr_str, "0.0.0.0");

    if (!LWPA_IP_IS_INVALID(&entry->mask))
      lwpa_inet_ntop(&entry->mask, mask_str, LWPA_INET6_ADDRSTRLEN);
    else
      strcpy(mask_str, "0.0.0.0");

    if (!LWPA_IP_IS_INVALID(&entry->gateway))
      lwpa_inet_ntop(&entry->gateway, gw_str, LWPA_INET6_ADDRSTRLEN);
    else
      strcpy(gw_str, "0.0.0.0");

    printf("%-40s %-40s %-40s %d %d\n", addr_str, mask_str, gw_str, entry->metric, entry->interface_index);
  }
}
#endif
