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

#include <ifaddrs.h>
#include <errno.h>
#include <asm/types.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
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
  size_t num_addrs;
  LwpaNetintInfo* lwpa_addrs;
} state;

/*********************** Private function prototypes *************************/

// Functions for building the routing tables
static lwpa_error_t build_routing_tables();
static lwpa_error_t build_routing_table(int family, RoutingTable* table);
static void free_routing_tables();
static void free_routing_table(RoutingTable* table);

// Interacting with RTNETLINK
static lwpa_error_t send_netlink_route_request(int socket, int family);
static lwpa_error_t receive_netlink_route_reply(int sock, size_t buf_size, RoutingTable* table);
static void cidr_length_to_v4_mask(unsigned char length, LwpaIpAddr* v4_mask);
static void cidr_length_to_v6_mask(unsigned char length, LwpaIpAddr* v6_mask);

// Manipulating
static void init_routing_table_entry(RoutingTableEntry* entry);
static int compare_routing_table_entries(const void* a, const void* b);

// Functions for enumerating the interfaces
static lwpa_error_t enumerate_os_addrs();
static void free_os_addrs();

/*************************** Function definitions ****************************/

lwpa_error_t lwpa_netint_init()
{
  lwpa_error_t res = kLwpaErrOk;
  if (!state.initialized)
  {
    res = enumerate_os_addrs();
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
  lwpa_error_t res = build_routing_table(AF_INET, &state.routing_table_v4);
  if (res == kLwpaErrOk)
  {
    res = build_routing_table(AF_INET6, &state.routing_table_v6);
  }

  if (res != kLwpaErrOk)
    free_routing_tables();

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

    if (result)
      result = send_netlink_route_request(sock, family);

    if (result)
    {
      result = receive_netlink_route_reply(sock, recv_buf_size, table);
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

lwpa_error_t receive_netlink_route_reply(int sock, size_t buf_size, RoutingTable* table)
{
  // Allocate slightly larger than buf_size, so we can detect when more room is needed
  size_t real_size = buf_size + 20;
  char* buffer = (char*)malloc(real_size);
  if (!buffer)
    return kLwpaErrNoMem;
  memset(buffer, 0, real_size);

  struct nlmsghdr* nl_header;
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

    nl_header = (struct nlmsghdr*)cur_ptr;

    if (nl_header->nlmsg_type == NLMSG_DONE)
      break;

    // Each message represents one route
    ++table->size;

    // Adjust our position in the buffer and size received
    cur_ptr += recv_res;
    nl_msg_size += (size_t)recv_res;
  }

  table->entries = calloc(table->size, sizeof(RoutingTableEntry));
  if (!table->entries)
  {
    table->size = 0;
    free(buffer);
    return kLwpaErrNoMem;
  }

  // Parse the result
  // outer loop: loops thru all the NETLINK headers that also include the route entry header
  nl_header = (struct nlmsghdr*)buffer;
  RoutingTableEntry* entry = table->entries;
  for (; NLMSG_OK(nl_header, nl_msg_size); nl_header = NLMSG_NEXT(nl_header, nl_msg_size))
  {
    init_routing_table_entry(entry);

    struct rtmsg* rt_message;
    int rt_attr_size;
    struct rtattr* rt_attributes;

    // get route entry header
    rt_message = (struct rtmsg*)NLMSG_DATA(nl_header);

    // inner loop: loop thru all the attributes of one route entry.
    rt_attributes = (struct rtattr*)RTM_RTA(rt_message);
    rt_attr_size = (int)RTM_PAYLOAD(nl_header);
    for (; RTA_OK(rt_attributes, rt_attr_size); rt_attributes = RTA_NEXT(rt_attributes, rt_attr_size))
    {
      // We only care about the gateway and DST attribute
      if (rt_attributes->rta_type == RTA_DST)
      {
        if (rt_attributes->rta_len == sizeof(struct in6_addr))
          LWPA_IP_SET_V6_ADDRESS(&entry->addr, ((struct in6_addr*)RTA_DATA(rt_attributes))->s6_addr);
        else
          LWPA_IP_SET_V4_ADDRESS(&entry->addr, ntohl(((struct in_addr*)RTA_DATA(rt_attributes))->s_addr));
      }
      else if (rt_attributes->rta_type == RTA_GATEWAY)
      {
        if (rt_attributes->rta_len == sizeof(struct in6_addr))
          LWPA_IP_SET_V6_ADDRESS(&entry->gateway, ((struct in6_addr*)RTA_DATA(rt_attributes))->s6_addr);
        else
          LWPA_IP_SET_V4_ADDRESS(&entry->gateway, ntohl(((struct in_addr*)RTA_DATA(rt_attributes))->s_addr));
      }
      else if (rt_attributes->rta_type == RTA_OIF)
      {
        entry->interface_index = *((int*)RTA_DATA(rt_attributes));
      }
      else if (rt_attributes->rta_type == RTA_METRICS)
      {
        entry->metric = *((int*)RTA_DATA(rt_attributes));
      }
    }

    if (LWPA_IP_IS_V4(&entry->addr))
    {
      cidr_length_to_v4_mask(rt_message->rtm_dst_len, &entry->mask);
    }
    else if (LWPA_IP_IS_V6(&entry->addr))
    {
      cidr_length_to_v6_mask(rt_message->rtm_dst_len, &entry->mask);
    }

    ++entry;
  }

  if (buffer)
    free(buffer);

  qsort(table->entries, table->size, sizeof(RoutingTableEntry), compare_routing_table_entries);

  return kLwpaErrOk;
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
    return 1;
  else if (mask_length_1 == mask_length_2)
    return 0;
  else
    return -1;
}

void cidr_length_to_v4_mask(unsigned char length, LwpaIpAddr* v4_mask)
{
  unsigned char length_remaining = length;
  uint32_t mask_val = 0;
  uint32_t bit_to_set = 0x8000;

  // Cannot rely on signed/arithmetic shifts here because that behavior is implementation-defined
  // in ANSI C
  for (; length_remaining > 0; --length_remaining)
  {
    mask_val |= bit_to_set;
    bit_to_set >>= 1;
  }

  LWPA_IP_SET_V4_ADDRESS(v4_mask, mask_val);
}

void cidr_length_to_v6_mask(unsigned char length, LwpaIpAddr* v6_mask)
{
  uint8_t mask_buf[LWPA_IPV6_BYTES];
  size_t mask_buf_index = 0;
  unsigned char length_remaining = length;
  uint8_t bit_to_set = 0x80;

  // Cannot rely on signed/arithmetic shifts here because that behavior is implementation-defined
  // in ANSI C
  for (; length_remaining > 0; --length_remaining)
  {
    mask_buf[mask_buf_index] |= bit_to_set;
    bit_to_set >>= 1;
    if (bit_to_set == 0)
    {
      if (++mask_buf_index >= LWPA_IPV6_BYTES)
        break;

      bit_to_set = 0x80;
    }
  }

  LWPA_IP_SET_V6_ADDRESS(v6_mask, mask_buf);
}

/* Quick helper for enumerate_os_addrs() to determine entries to skip in the linked list. */
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
    sockaddr_os_to_lwpa(&temp_sockaddr, ifaddr->ifa_addr);
    current_info->addr = temp_sockaddr.ip;

    // Interface netmask
    sockaddr_os_to_lwpa(&temp_sockaddr, ifaddr->ifa_netmask);
    current_info->mask = temp_sockaddr.ip;
  }

  return kLwpaErrOk;
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
