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

/* The os_netint implementation for macOS.
 *
 * Uses getifaddrs() to get an initial list of network interfaces. More info: man 3 getifaddrs
 *
 * Then uses sysctl() and parses kernel-formatted routing table information to add supplementary
 * information and determine default routes.
 *
 * The method by which the routing information is obtained in BSD-derived kernels is an
 * underdocumented mess. I have tried to comment the documentation sources for how to parse the
 * system information as best I can; sometimes this info was obtained from (with no better source
 * being apparently available) a StackOverflow answer, or a Wikipedia page, or a print book, or
 * simply by empirical testing.
 */

#include "lwpa/netint.h"

#include <errno.h>
#include <limits.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#ifdef LWPA_NETINT_DEBUG_OUTPUT
#include <stdio.h>
#endif

#include <ifaddrs.h>
#include <net/if.h>
#include <net/if_dl.h>
#include <net/route.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/sysctl.h>

#include "lwpa/private/netint.h"
#include "os_error.h"

/***************************** Private types *********************************/

typedef enum
{
  kGwAddress,
  kGwIndex,
  kGwInvalid
} routing_gateway_t;

typedef struct RoutingGateway
{
  routing_gateway_t type;
  union
  {
    LwpaIpAddr addr;
    int ifindex;
  } u;
} RoutingGateway;

typedef struct RoutingTableEntry
{
  LwpaIpAddr addr;
  LwpaIpAddr mask;
  RoutingGateway gateway;
  int interface_index;
  int metric;
} RoutingTableEntry;

typedef struct RoutingTable
{
  RoutingTableEntry* entries;
  RoutingTableEntry* default_route;
  size_t size;
} RoutingTable;

/**************************** Private variables ******************************/

static struct LwpaNetintState
{
  bool initialized;

  RoutingTable routing_table_v4;
  RoutingTable routing_table_v6;

  struct ifaddrs* os_addrs;
  size_t num_netints;
  LwpaNetintInfo* lwpa_netints;

  bool have_default_netint_index_v4;
  size_t default_netint_index_v4;

  bool have_default_netint_index_v6;
  size_t default_netint_index_v6;
} state;

/*********************** Private function prototypes *************************/

// Functions for building the routing tables
static lwpa_error_t build_routing_tables();
static lwpa_error_t build_routing_table(int family, RoutingTable* table);
static void free_routing_tables();
static void free_routing_table(RoutingTable* table);

// Interacting with the BSD routing stack
static lwpa_error_t get_routing_table_dump(int family, uint8_t** buf, size_t* buf_len);
static lwpa_error_t parse_routing_table_dump(int family, uint8_t* buf, size_t buf_len, RoutingTable* table);

// Manipulating routing table entries
static void init_routing_table_entry(RoutingTableEntry* entry);
static int compare_routing_table_entries(const void* a, const void* b);

// Functions for enumerating the interfaces
static lwpa_error_t enumerate_netints();
static int compare_netints(const void* a, const void* b);
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
    memset(&state, 0, sizeof(state));
  }
}

/******************************************************************************
 * API Functions
 *****************************************************************************/

size_t lwpa_netint_get_num_interfaces()
{
  return (state.initialized ? state.num_netints : 0);
}

size_t lwpa_netint_get_interfaces(LwpaNetintInfo* netint_arr, size_t netint_arr_size)
{
  if (!state.initialized || !netint_arr || netint_arr_size == 0)
    return 0;

  size_t addrs_copied = (netint_arr_size < state.num_netints ? netint_arr_size : state.num_netints);
  memcpy(netint_arr, state.lwpa_netints, addrs_copied * sizeof(LwpaNetintInfo));
  return addrs_copied;
}

bool lwpa_netint_get_default_interface(lwpa_iptype_t type, LwpaNetintInfo* netint)
{
  if (state.initialized && netint)
  {
    if (type == kLwpaIpTypeV4 && state.have_default_netint_index_v4)
    {
      *netint = state.lwpa_netints[state.default_netint_index_v4];
      return true;
    }
    else if (type == kLwpaIpTypeV6 && state.have_default_netint_index_v6)
    {
      *netint = state.lwpa_netints[state.default_netint_index_v6];
      return true;
    }
  }
  return false;
}

lwpa_error_t lwpa_netint_get_interface_for_dest(const LwpaIpAddr* dest, LwpaNetintInfo* netint)
{
  if (!dest || !netint)
    return kLwpaErrInvalid;
  if (!state.initialized)
    return kLwpaErrNotInit;
  if (state.num_netints == 0)
    return kLwpaErrNoNetints;

  RoutingTable* table_to_use = (LWPA_IP_IS_V6(dest) ? &state.routing_table_v6 : &state.routing_table_v4);

  int index_found = -1;
  for (RoutingTableEntry* entry = table_to_use->entries; entry < table_to_use->entries + table_to_use->size; ++entry)
  {
    if (entry->interface_index < 0 || LWPA_IP_IS_INVALID(&entry->mask))
      continue;

    // Check each route to see if it matches the destination address explicitly
    if (lwpa_ip_network_portions_equal(&entry->addr, dest, &entry->mask))
    {
      index_found = entry->interface_index;
      break;
    }
  }

  // Fall back to the default route
  if (index_found < 0 && table_to_use->default_route)
    index_found = table_to_use->default_route->interface_index;

  // Find the network interface with the correct index
  if (index_found >= 0)
  {
    for (LwpaNetintInfo* netint_entry = state.lwpa_netints; netint_entry < state.lwpa_netints + state.num_netints;
         ++netint_entry)
    {
      if (netint_entry->ifindex == index_found)
      {
        *netint = *netint_entry;
        return kLwpaErrOk;
      }
    }
  }
  return kLwpaErrNotFound;
}

/******************************************************************************
 * Internal Functions
 *****************************************************************************/

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
  uint8_t* buf = NULL;
  size_t buf_len = 0;

  lwpa_error_t res = get_routing_table_dump(family, &buf, &buf_len);
  if (res == kLwpaErrOk)
  {
    res = parse_routing_table_dump(family, buf, buf_len, table);
  }

  if (buf)
    free(buf);
  return res;
}

// Get the routing table information from the system.
lwpa_error_t get_routing_table_dump(int family, uint8_t** buf, size_t* buf_len)
{
  // The MIB is a heirarchical series of codes determining the systcl operation to perform.
  int mib[6];
  mib[0] = CTL_NET;      // Networking messages
  mib[1] = PF_ROUTE;     // Routing messages
  mib[2] = 0;            // Reserved for this command, always 0
  mib[3] = family;       // Address family
  mib[4] = NET_RT_DUMP;  // Dump routing table entries
  mib[5] = 0;            // Reserved for this command, always 0

  // First pass just determines the size of buffer that is needed.
  int sysctl_res = sysctl(mib, 6, NULL, buf_len, NULL, 0);
  if ((sysctl_res != 0 && errno != ENOMEM) || *buf_len == 0)
    return errno_os_to_lwpa(errno);

  // Allocate the buffer
  *buf = (uint8_t*)malloc(*buf_len);
  if (!(*buf))
    return kLwpaErrNoMem;

  // Second pass to actually get the info
  sysctl_res = sysctl(mib, 6, *buf, buf_len, NULL, 0);
  if (sysctl_res != 0 || *buf_len == 0)
  {
    free(*buf);
    return errno_os_to_lwpa(errno);
  }

  return kLwpaErrOk;
}

#define SOCKADDR_ALIGN 4  // not sizeof(long)
#define SA_SIZE(sa) ((!(sa) || (sa)->sa_len == 0) ? SOCKADDR_ALIGN : (1 + (((sa)->sa_len - 1) | (SOCKADDR_ALIGN - 1))))

void get_addrs_from_route_entry(int addrs, struct sockaddr* sa, struct sockaddr** rti_info)
{
  for (int i = 0; i < RTAX_MAX; ++i)
  {
    if (addrs & (1 << i))
    {
      rti_info[i] = sa;
      sa = (struct sockaddr*)((char*)sa + SA_SIZE(sa));
    }
    else
    {
      rti_info[i] = NULL;
    }
  }
}

static void dest_from_route_entry(int family, const struct sockaddr* os_dst, LwpaIpAddr* lwpa_dst)
{
  if (family == AF_INET6)
    LWPA_IP_SET_V6_ADDRESS(lwpa_dst, ((struct sockaddr_in6*)os_dst)->sin6_addr.s6_addr);
  else
    LWPA_IP_SET_V4_ADDRESS(lwpa_dst, ntohl(((struct sockaddr_in*)os_dst)->sin_addr.s_addr));
}

static void netmask_from_route_entry(int family, const struct sockaddr* os_netmask, LwpaIpAddr* lwpa_netmask)
{
  if (family == AF_INET)
  {
    size_t mask_offset = offsetof(struct sockaddr_in, sin_addr);
    const char* mask_ptr = &((char*)os_netmask)[mask_offset];

    if (os_netmask->sa_len > mask_offset)
    {
      uint32_t mask_val = 0;
      mask_val |= ((uint32_t)mask_ptr[0]) << 24;
      if (os_netmask->sa_len > (mask_offset + 1))
        mask_val |= ((uint32_t)mask_ptr[1]) << 16;
      if (os_netmask->sa_len > (mask_offset + 2))
        mask_val |= ((uint32_t)mask_ptr[2]) << 8;
      if (os_netmask->sa_len > (mask_offset + 3))
        mask_val |= ((uint32_t)mask_ptr[3]);
      LWPA_IP_SET_V4_ADDRESS(lwpa_netmask, mask_val);
    }
  }
  else if (family == AF_INET6)
  {
    size_t mask_offset = offsetof(struct sockaddr_in6, sin6_addr);
    const char* mask_ptr = &((char*)os_netmask)[mask_offset];

    if (os_netmask->sa_len > mask_offset)
    {
      uint8_t ip_buf[LWPA_IPV6_BYTES];
      memset(ip_buf, 0, LWPA_IPV6_BYTES);
      for (size_t mask_pos = 0; mask_pos < os_netmask->sa_len - mask_offset; ++mask_pos)
      {
        ip_buf[mask_pos] = mask_ptr[mask_pos];
      }
      LWPA_IP_SET_V6_ADDRESS(lwpa_netmask, ip_buf);
    }
  }
}

static void gateway_from_route_entry(const struct sockaddr* os_gw, RoutingGateway* lwpa_gw)
{
  if (os_gw->sa_family == AF_INET6)
  {
    lwpa_gw->type = kGwAddress;
    LWPA_IP_SET_V6_ADDRESS(&lwpa_gw->u.addr, ((struct sockaddr_in6*)os_gw)->sin6_addr.s6_addr);
  }
  else if (os_gw->sa_family == AF_INET)
  {
    lwpa_gw->type = kGwAddress;
    LWPA_IP_SET_V4_ADDRESS(&lwpa_gw->u.addr, ntohl(((struct sockaddr_in*)os_gw)->sin_addr.s_addr));
  }
  else if (os_gw->sa_family == AF_LINK)
  {
    lwpa_gw->type = kGwIndex;
    lwpa_gw->u.ifindex = ((struct sockaddr_dl*)os_gw)->sdl_index;
  }
}

lwpa_error_t parse_routing_table_dump(int family, uint8_t* buf, size_t buf_len, RoutingTable* table)
{
  table->size = 0;
  table->entries = NULL;
  table->default_route = NULL;

  size_t buf_pos = 0;

  // Parse the result
  // Loop through all routing table entries returned in the buffer
  while (buf_pos < buf_len)
  {
    RoutingTableEntry new_entry;
    init_routing_table_entry(&new_entry);

    bool new_entry_valid = true;

    // get route entry header
    struct rt_msghdr* rmsg = (struct rt_msghdr*)(&buf[buf_pos]);

    // Filter out entries from the local routing table, broadcast and ARP routes.
    if (!(rmsg->rtm_flags & (RTF_LLDATA | RTF_LOCAL | RTF_BROADCAST)))
    {
      struct sockaddr* addr_start = (struct sockaddr*)(rmsg + 1);
      struct sockaddr* rti_info[RTAX_MAX];
      get_addrs_from_route_entry(rmsg->rtm_addrs, addr_start, rti_info);

      // We only care about the gateway and DST attribute
      if (rti_info[RTAX_DST] != NULL)
      {
        dest_from_route_entry(family, rti_info[RTAX_DST], &new_entry.addr);
      }
      if (rti_info[RTAX_GATEWAY] != NULL)
      {
        gateway_from_route_entry(rti_info[RTAX_GATEWAY], &new_entry.gateway);
      }
      if (rti_info[RTAX_NETMASK] != NULL)
      {
        netmask_from_route_entry(family, rti_info[RTAX_NETMASK], &new_entry.mask);
      }
      // else if (rt_attributes->rta_type == RTA_OIF)
      //{
      //  new_entry.interface_index = *((int*)RTA_DATA(rt_attributes));
      //}
      // else if (rt_attributes->rta_type == RTA_PRIORITY)
      //{
      //  new_entry.metric = *((int*)RTA_DATA(rt_attributes));
      //}
    }
    else
    {
      new_entry_valid = false;
    }

    //    if (!LWPA_IP_IS_INVALID(&new_entry.addr))
    //    {
    //      new_entry.mask = lwpa_ip_mask_from_length(new_entry.addr.type, rt_message->rtm_dst_len);
    //    }

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

    buf_pos += rmsg->rtm_msglen;
  }

  if (table->size > 0)
  {
    qsort(table->entries, table->size, sizeof(RoutingTableEntry), compare_routing_table_entries);

    // Mark the default route with the lowest metric (the first one we encounter after sorting the
    // table)
    for (RoutingTableEntry* entry = table->entries; entry < table->entries + table->size; ++entry)
    {
      if (LWPA_IP_IS_INVALID(&entry->addr) && LWPA_IP_IS_INVALID(&entry->mask) && !LWPA_IP_IS_INVALID(&entry->gateway))
      {
        table->default_route = entry;
        break;
      }
    }
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
  entry->gateway.type = kGwInvalid;
  entry->interface_index = -1;
  entry->metric = INT_MAX;
}

int compare_routing_table_entries(const void* a, const void* b)
{
  RoutingTableEntry* e1 = (RoutingTableEntry*)a;
  RoutingTableEntry* e2 = (RoutingTableEntry*)b;

  unsigned int mask_length_1 = lwpa_ip_mask_length(&e1->mask);
  unsigned int mask_length_2 = lwpa_ip_mask_length(&e2->mask);

  // Sort by mask length in descending order - within the same mask length, sort by metric in
  // ascending order.
  if (mask_length_1 == mask_length_2)
  {
    return (e1->metric > e2->metric) - (e1->metric < e2->metric);
  }
  else
  {
    return (mask_length_1 < mask_length_2) - (mask_length_1 > mask_length_2);
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
  return res;
  //  if (res != kLwpaErrOk)
  //    return res;
  //
  //  int ioctl_sock = socket(AF_INET, SOCK_DGRAM, 0);
  //  if (ioctl_sock == -1)
  //    return errno_os_to_lwpa(errno);
  //
  //  if (getifaddrs(&state.os_addrs) < 0)
  //  {
  //    close(ioctl_sock);
  //    return errno_os_to_lwpa(errno);
  //  }
  //
  //  // Pass 1: Total the number of addresses
  //  state.num_netints = 0;
  //  for (struct ifaddrs* ifaddr = state.os_addrs; ifaddr; ifaddr = ifaddr->ifa_next)
  //  {
  //    if (should_skip_ifaddr(ifaddr))
  //      continue;
  //
  //    ++state.num_netints;
  //  }
  //
  //  if (state.num_netints == 0)
  //  {
  //    freeifaddrs(state.os_addrs);
  //    close(ioctl_sock);
  //    return kLwpaErrNoNetints;
  //  }
  //
  //  // Allocate our interface array
  //  state.lwpa_netints = (LwpaNetintInfo*)calloc(state.num_netints, sizeof(LwpaNetintInfo));
  //  if (!state.lwpa_netints)
  //  {
  //    freeifaddrs(state.os_addrs);
  //    close(ioctl_sock);
  //    return kLwpaErrNoMem;
  //  }
  //
  //  // Pass 2: Fill in all the info about each address
  //  size_t current_lwpa_index = 0;
  //  for (struct ifaddrs* ifaddr = state.os_addrs; ifaddr; ifaddr = ifaddr->ifa_next)
  //  {
  //    if (should_skip_ifaddr(ifaddr))
  //      continue;
  //
  //    LwpaNetintInfo* current_info = &state.lwpa_netints[current_lwpa_index];
  //
  //    // Interface name
  //    strncpy(current_info->name, ifaddr->ifa_name, LWPA_NETINTINFO_NAME_LEN);
  //    current_info->name[LWPA_NETINTINFO_NAME_LEN - 1] = '\0';
  //
  //    // Interface address
  //    LwpaSockaddr temp_sockaddr;
  //    sockaddr_os_to_lwpa(&temp_sockaddr, ifaddr->ifa_addr);
  //    current_info->addr = temp_sockaddr.ip;
  //
  //    // Interface netmask
  //    sockaddr_os_to_lwpa(&temp_sockaddr, ifaddr->ifa_netmask);
  //    current_info->mask = temp_sockaddr.ip;
  //
  //    // Struct ifreq to use with ioctl() calls
  //    struct ifreq if_req;
  //    strncpy(if_req.ifr_name, ifaddr->ifa_name, IFNAMSIZ);
  //
  //    // Hardware address
  //    int ioctl_res = ioctl(ioctl_sock, SIOCGIFHWADDR, &if_req);
  //    if (ioctl_res == 0)
  //      memcpy(current_info->mac, if_req.ifr_hwaddr.sa_data, LWPA_NETINTINFO_MAC_LEN);
  //    else
  //      memset(current_info->mac, 0, LWPA_NETINTINFO_MAC_LEN);
  //
  //    // Interface index
  //    ioctl_res = ioctl(ioctl_sock, SIOCGIFINDEX, &if_req);
  //    if (ioctl_res == 0)
  //      current_info->ifindex = if_req.ifr_ifindex;
  //    else
  //      current_info->ifindex = -1;
  //
  //    // Is Default
  //    if (LWPA_IP_IS_V4(&current_info->addr) && state.routing_table_v4.default_route &&
  //        current_info->ifindex == state.routing_table_v4.default_route->interface_index)
  //    {
  //      current_info->is_default = true;
  //    }
  //    else if (LWPA_IP_IS_V6(&current_info->addr) && state.routing_table_v6.default_route &&
  //             current_info->ifindex == state.routing_table_v6.default_route->interface_index)
  //    {
  //      current_info->is_default = true;
  //    }
  //
  //    current_lwpa_index++;
  //  }
  //
  //  // Sort the interfaces by OS index
  //  qsort(state.lwpa_netints, state.num_netints, sizeof(LwpaNetintInfo), compare_netints);
  //
  //  // Store the locations of the default netints for access by the API function
  //  for (size_t i = 0; i < state.num_netints; ++i)
  //  {
  //    LwpaNetintInfo* netint = &state.lwpa_netints[i];
  //    if (LWPA_IP_IS_V4(&netint->addr) && netint->is_default)
  //    {
  //      state.have_default_netint_index_v4 = true;
  //      state.default_netint_index_v4 = i;
  //    }
  //    else if (LWPA_IP_IS_V6(&netint->addr) && netint->is_default)
  //    {
  //      state.have_default_netint_index_v6 = true;
  //      state.default_netint_index_v6 = i;
  //    }
  //  }
  //  close(ioctl_sock);
  // return kLwpaErrOk;
}

int compare_netints(const void* a, const void* b)
{
  LwpaNetintInfo* netint1 = (LwpaNetintInfo*)a;
  LwpaNetintInfo* netint2 = (LwpaNetintInfo*)b;

  return (netint1->ifindex > netint2->ifindex) - (netint1->ifindex < netint2->ifindex);
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
  printf("%-40s %-40s %-40s %-10s %s\n", "Address", "Mask", "Gateway", "Metric", "Index");
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

    if (entry->gateway.type == kGwAddress && !LWPA_IP_IS_INVALID(&entry->gateway.u.addr))
      lwpa_inet_ntop(&entry->gateway.u.addr, gw_str, LWPA_INET6_ADDRSTRLEN);
    else if (entry->gateway.type == kGwIndex)
      snprintf(gw_str, LWPA_INET6_ADDRSTRLEN, "link %d", entry->gateway.u.ifindex);
    else
      strcpy(gw_str, "");

    printf("%-40s %-40s %-40s %-10d %d\n", addr_str, mask_str, gw_str, entry->metric, entry->interface_index);
  }
}
#endif
