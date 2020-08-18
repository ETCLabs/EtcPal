/******************************************************************************
 * Copyright 2020 ETC Inc.
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
 * The os_netint implementation for macOS.
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
 *
 * Some info comes from the following books, which are referenced below:
 * - Unix Network Programming: The Sockets Networking API (Stevens, Fenner, Rudoff)
 * - TCP/IP Illustrated, Volume 2: The Implementation (Fall, Stevens)
 */

#include "etcpal/netint.h"

#include <errno.h>
#include <limits.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#ifdef ETCPAL_NETINT_DEBUG_OUTPUT
#include <stdio.h>
#endif

#include <ifaddrs.h>
#include <net/if.h>
#include <net/if_dl.h>
#include <net/route.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/sysctl.h>

#include "etcpal/common.h"
#include "etcpal/private/netint.h"
#include "os_error.h"

/***************************** Private types *********************************/

typedef struct RoutingTableEntry
{
  EtcPalIpAddr addr;
  EtcPalIpAddr mask;
  EtcPalIpAddr gateway;
  unsigned int interface_index;
  /* Darwin seems to set this flag on default entries that are not chosen as *the* system-wide
   * default. I have not seen this behavior documented anywhere, but it's the only way I can tell
   * to determine which route is truly the default route. */
  bool interface_scope;
} RoutingTableEntry;

typedef struct RoutingTable
{
  RoutingTableEntry* entries;
  RoutingTableEntry* default_route;
  size_t             size;
} RoutingTable;

/**************************** Private variables ******************************/

RoutingTable routing_table_v4;
RoutingTable routing_table_v6;

/*********************** Private function prototypes *************************/

// Functions for building the routing tables
static etcpal_error_t build_routing_tables();
static etcpal_error_t build_routing_table(int family, RoutingTable* table);
static void           free_routing_tables();
static void           free_routing_table(RoutingTable* table);

// Interacting with the BSD routing stack
static etcpal_error_t get_routing_table_dump(int family, uint8_t** buf, size_t* buf_len);
static etcpal_error_t parse_routing_table_dump(int family, uint8_t* buf, size_t buf_len, RoutingTable* table);

// Manipulating routing table entries
static void init_routing_table_entry(RoutingTableEntry* entry);
static int  compare_routing_table_entries(const void* a, const void* b);

#if ETCPAL_NETINT_DEBUG_OUTPUT
static void debug_print_routing_table(RoutingTable* table);
#endif

/*************************** Function definitions ****************************/

/* Quick helper for enumerate_netints() to determine entries to skip in the linked list. */
static size_t count_ifaddrs(const struct ifaddrs* ifaddrs)
{
  size_t total = 0;
  for (const struct ifaddrs* ifaddr = ifaddrs; ifaddr; ifaddr = ifaddr->ifa_next)
  {
    // Skip an entry if it is down, doesn't have an address, or if the address is not IPv4 or IPv6.
    if ((ifaddr->ifa_flags & IFF_UP) && ifaddr->ifa_addr &&
        (ifaddr->ifa_addr->sa_family == AF_INET || ifaddr->ifa_addr->sa_family == AF_INET6))
    {
      ++total;
    }
  }
  return total;
}

etcpal_error_t os_enumerate_interfaces(CachedNetintInfo* cache)
{
  etcpal_error_t res = build_routing_tables();
  if (res != kEtcPalErrOk)
    return res;

  // Fill in the default index information
  if (routing_table_v4.default_route)
  {
    cache->def.v4_valid = true;
    cache->def.v4_index = routing_table_v4.default_route->interface_index;
  }
  if (routing_table_v6.default_route)
  {
    cache->def.v6_valid = true;
    cache->def.v6_index = routing_table_v6.default_route->interface_index;
  }

  struct ifaddrs* os_addrs;
  if (getifaddrs(&os_addrs) < 0)
  {
    return errno_os_to_etcpal(errno);
  }

  // Pass 1: Total the number of addresses
  cache->num_netints = count_ifaddrs(os_addrs);

  if (cache->num_netints == 0)
  {
    freeifaddrs(os_addrs);
    return kEtcPalErrNoNetints;
  }

  // Allocate our interface array
  cache->netints = (EtcPalNetintInfo*)calloc(cache->num_netints, sizeof(EtcPalNetintInfo));
  if (!cache->netints)
  {
    freeifaddrs(os_addrs);
    return kEtcPalErrNoMem;
  }

  // Pass 2: Fill in all the info about each address
  size_t              current_etcpal_index = 0;
  char*               link_name = NULL;
  struct sockaddr_dl* link_addr = NULL;

  for (struct ifaddrs* ifaddr = os_addrs; ifaddr; ifaddr = ifaddr->ifa_next)
  {
    // An AF_LINK entry appears before one or more internet address entries for each interface.
    // Save the current AF_LINK entry for later use. If the entry is an IPv4 or IPv6 address, we
    // can proceed.
    if (ifaddr->ifa_addr)
    {
      if (ifaddr->ifa_addr->sa_family == AF_LINK)
      {
        link_name = ifaddr->ifa_name;
        link_addr = (struct sockaddr_dl*)(ifaddr->ifa_addr);
        continue;
      }
      else if (ifaddr->ifa_addr->sa_family != AF_INET && ifaddr->ifa_addr->sa_family != AF_INET6)
      {
        continue;
      }
    }
    else
    {
      continue;
    }

    EtcPalNetintInfo* current_info = &cache->netints[current_etcpal_index];

    // Interface name
    strncpy(current_info->id, ifaddr->ifa_name, ETCPAL_NETINTINFO_ID_LEN);
    current_info->id[ETCPAL_NETINTINFO_ID_LEN - 1] = '\0';
    strncpy(current_info->friendly_name, ifaddr->ifa_name, ETCPAL_NETINTINFO_FRIENDLY_NAME_LEN);
    current_info->friendly_name[ETCPAL_NETINTINFO_FRIENDLY_NAME_LEN - 1] = '\0';

    // Interface address
    ip_os_to_etcpal(ifaddr->ifa_addr, &current_info->addr);

    // Interface netmask
    ip_os_to_etcpal(ifaddr->ifa_netmask, &current_info->mask);

    // Make sure we match the corresponding link information
    if (link_name && link_addr && 0 == strcmp(ifaddr->ifa_name, link_name))
    {
      current_info->index = link_addr->sdl_index;
      memcpy(current_info->mac.data, link_addr->sdl_data + link_addr->sdl_nlen, ETCPAL_MAC_BYTES);
    }
    else
    {
      // Backup - get the interface index using if_nametoindex
      current_info->index = if_nametoindex(ifaddr->ifa_name);
      memset(current_info->mac.data, 0, ETCPAL_MAC_BYTES);
    }

    // Is Default
    if (ETCPAL_IP_IS_V4(&current_info->addr) && routing_table_v4.default_route &&
        current_info->index == routing_table_v4.default_route->interface_index)
    {
      current_info->is_default = true;
    }
    else if (ETCPAL_IP_IS_V6(&current_info->addr) && routing_table_v6.default_route &&
             current_info->index == routing_table_v6.default_route->interface_index)
    {
      current_info->is_default = true;
    }

    current_etcpal_index++;
  }

  freeifaddrs(os_addrs);
  return kEtcPalErrOk;
}

void os_free_interfaces(CachedNetintInfo* cache)
{
  if (cache->netints)
  {
    free(cache->netints);
    cache->netints = NULL;
  }

  free_routing_tables();
}

etcpal_error_t os_resolve_route(const EtcPalIpAddr* dest, const CachedNetintInfo* cache, unsigned int* index)
{
  ETCPAL_UNUSED_ARG(cache);

  RoutingTable* table_to_use = (ETCPAL_IP_IS_V6(dest) ? &routing_table_v6 : &routing_table_v4);

  unsigned int index_found = 0;
  for (RoutingTableEntry* entry = table_to_use->entries; entry < table_to_use->entries + table_to_use->size; ++entry)
  {
    if (entry->interface_index == 0 || ETCPAL_IP_IS_INVALID(&entry->mask))
      continue;

    // Check each route to see if it matches the destination address explicitly
    if (etcpal_ip_network_portions_equal(&entry->addr, dest, &entry->mask))
    {
      index_found = entry->interface_index;
      break;
    }
  }

  // Fall back to the default route
  if (index_found == 0 && table_to_use->default_route)
    index_found = table_to_use->default_route->interface_index;

  if (index_found > 0)
  {
    *index = index_found;
    return kEtcPalErrOk;
  }
  else
  {
    return kEtcPalErrNotFound;
  }
}

/******************************************************************************
 * Internal Functions
 *****************************************************************************/

etcpal_error_t build_routing_tables()
{
  etcpal_error_t res = build_routing_table(AF_INET, &routing_table_v4);
  if (res == kEtcPalErrOk)
  {
    res = build_routing_table(AF_INET6, &routing_table_v6);
  }

  if (res != kEtcPalErrOk)
    free_routing_tables();

#if ETCPAL_NETINT_DEBUG_OUTPUT
  debug_print_routing_table(&routing_table_v4);
  debug_print_routing_table(&routing_table_v6);
#endif

  return res;
}

etcpal_error_t build_routing_table(int family, RoutingTable* table)
{
  uint8_t* buf = NULL;
  size_t   buf_len = 0;

  etcpal_error_t res = get_routing_table_dump(family, &buf, &buf_len);
  if (res == kEtcPalErrOk)
  {
    res = parse_routing_table_dump(family, buf, buf_len, table);
  }

  if (buf)
    free(buf);
  return res;
}

// Get the routing table information from the system.
etcpal_error_t get_routing_table_dump(int family, uint8_t** buf, size_t* buf_len)
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
    return errno_os_to_etcpal(errno);

  // Allocate the buffer
  *buf = (uint8_t*)malloc(*buf_len);
  if (!(*buf))
    return kEtcPalErrNoMem;

  // Second pass to actually get the info
  sysctl_res = sysctl(mib, 6, *buf, buf_len, NULL, 0);
  if (sysctl_res != 0 || *buf_len == 0)
  {
    free(*buf);
    return errno_os_to_etcpal(errno);
  }

  return kEtcPalErrOk;
}

/* These two macros and the following function were taken from the Unix Network Programming book
 * (sect. 18.3) but they required some modification, because they mistakenly assert that the
 * sockaddr structures following a route message header are aligned to sizeof(unsigned long), when
 * in fact on 64-bit macOS platforms they still seem to be aligned on 4-byte boundaries.
 * (sizeof(unsigned long) is 8 on 64-bit platforms)
 */
#define SOCKADDR_ALIGN 4  // not sizeof(unsigned long)
#define SA_SIZE(sa) ((!(sa) || (sa)->sa_len == 0) ? SOCKADDR_ALIGN : (1 + (((sa)->sa_len - 1) | (SOCKADDR_ALIGN - 1))))

static void get_addrs_from_route_entry(int addrs, struct sockaddr* sa, struct sockaddr** rti_info)
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

/* Get the route destination from a normal socket address structure packed as part of a route
 * entry.
 */
static void dest_from_route_entry(int family, const struct sockaddr* os_dst, EtcPalIpAddr* etcpal_dst)
{
  if (family == AF_INET6)
    ETCPAL_IP_SET_V6_ADDRESS(etcpal_dst, ((struct sockaddr_in6*)os_dst)->sin6_addr.s6_addr);
  else
    ETCPAL_IP_SET_V4_ADDRESS(etcpal_dst, ntohl(((struct sockaddr_in*)os_dst)->sin_addr.s_addr));
}

/* Get the netmask from a socket address structure packed as part of a route entry.
 *
 * The netmask sockaddrs take the form of a struct sockaddr, but they are incomplete (they are
 * variable-length and not always the full length of the struct). The length field of the sockaddr
 * correctly indicates where the structure is truncated, EXCEPT for the special case of a zero
 * length, which indicates that the structure contains no other information (an implied zero
 * netmask) but nevertheless occupies four bytes.
 *
 * If you have made it this far without throwing a full wine bottle at your computer screen, I
 * congratulate you.
 *
 * This method for determining an IPv4 netmask comes from the Unix Network Programming book, sect.
 * 18.3. It also references TCP/IP Illustrated Volume 2, fig. 18.21, which presents the way this
 * data is held in the kernel. The method for obtaining an IPv6 netmask is undocumented anywhere,
 * but inferred from the IPv4 method and empirical debugging.
 *
 * Here are some examples:
 *
 *  IPv4 netmask in struct sockaddr_in
 * +------------------------------------+
 * | sin_len, indicates length          |
 * +------------------------------------+
 * | sin_family, contains no valid data |
 * +------------------------------------+
 * | sin_port, contains no valid data   |
 * |                                    |
 * +------------------------------------+
 * | sin_addr, contains one to four     |
 * | bytes of netmask, indicated by     |
 * | (sin_len - (this field's offset)). |
 * | The remaining netmask bytes are    |
 * | inferred to be zero.               |
 * +------------------------------------+
 *
 *  IPv6 netmask in struct sockaddr_in6
 * +------------------------------------+
 * | sin6_len, indicates length         |
 * +------------------------------------+
 * | sin6_family, contains no valid data|
 * +------------------------------------+
 * | sin6_port, contains no valid data  |
 * |                                    |
 * +------------------------------------+
 * | sin6_flowinfo, contains no valid   |
 * | data                               |
 * |                                    |
 * |                                    |
 * +------------------------------------+
 * | sin6_addr, contains one to sixteen |
 * | bytes of netmask, indicated by     |
 * | (sin6_len - (this field's offset)).|
 * | The remaining netmask bytes are    |
 * | inferred to be zero.               |
 * +------------------------------------+
 */
static void netmask_from_route_entry(int family, const struct sockaddr* os_netmask, EtcPalIpAddr* etcpal_netmask)
{
  if (family == AF_INET)
  {
    size_t      mask_offset = offsetof(struct sockaddr_in, sin_addr);
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
      ETCPAL_IP_SET_V4_ADDRESS(etcpal_netmask, mask_val);
    }
  }
  else if (family == AF_INET6)
  {
    size_t      mask_offset = offsetof(struct sockaddr_in6, sin6_addr);
    const char* mask_ptr = &((char*)os_netmask)[mask_offset];

    if (os_netmask->sa_len > mask_offset)
    {
      uint8_t ip_buf[ETCPAL_IPV6_BYTES];
      memset(ip_buf, 0, ETCPAL_IPV6_BYTES);
      for (size_t mask_pos = 0; mask_pos < os_netmask->sa_len - mask_offset; ++mask_pos)
      {
        ip_buf[mask_pos] = mask_ptr[mask_pos];
      }
      ETCPAL_IP_SET_V6_ADDRESS(etcpal_netmask, ip_buf);
    }
  }
}

/* Get the gateway from a socket address structure packed as part of a route entry. The gateway
 * can either be a network socket address (indicating an actual gateway route) or a datalink
 * sockaddr, indicating that this route is "on-link" (on a local subnet only).
 */
static void gateway_from_route_entry(const struct sockaddr* os_gw, EtcPalIpAddr* etcpal_gw)
{
  if (os_gw->sa_family == AF_INET6)
  {
    ETCPAL_IP_SET_V6_ADDRESS(etcpal_gw, ((struct sockaddr_in6*)os_gw)->sin6_addr.s6_addr);
  }
  else if (os_gw->sa_family == AF_INET)
  {
    ETCPAL_IP_SET_V4_ADDRESS(etcpal_gw, ntohl(((struct sockaddr_in*)os_gw)->sin_addr.s_addr));
  }
}

#ifdef RTF_LLDATA
#define RTF_LINK_FLAG RTF_LLDATA
#else
#define RTF_LINK_FLAG RTF_LLINFO
#endif

/* Anyone debugging this code might benefit from this: a mapping of netstat -r flags to
 * rmsg->rtm_flags values.
 *
 *  1       RTF_PROTO1       Protocol specific routing flag #1
 *  2       RTF_PROTO2       Protocol specific routing flag #2
 *  3       RTF_PROTO3       Protocol specific routing flag #3
 *  B       RTF_BLACKHOLE    Just discard packets (during updates)
 *  b       RTF_BROADCAST    The route represents a broadcast address
 *  C       RTF_CLONING      Generate new routes on use
 *  c       RTF_PRCLONING    Protocol-specified generate new routes on use
 *  D       RTF_DYNAMIC      Created dynamically (by redirect)
 *  G       RTF_GATEWAY      Destination requires forwarding by intermediary
 *  H       RTF_HOST         Host entry (net otherwise)
 *  I       RTF_IFSCOPE      Route is associated with an interface scope
 *  i       RTF_IFREF        Route is holding a reference to the interface
 *  L       RTF_LLINFO       Valid protocol to link address translation
 *  M       RTF_MODIFIED     Modified dynamically (by redirect)
 *  m       RTF_MULTICAST    The route represents a multicast address
 *  R       RTF_REJECT       Host or net unreachable
 *  r       RTF_ROUTER       Host is a default router
 *  S       RTF_STATIC       Manually added
 *  U       RTF_UP           Route usable
 *  W       RTF_WASCLONED    Route was generated as a result of cloning
 *  X       RTF_XRESOLVE     External daemon translates proto to link address
 *  Y       RTF_PROXY        Proxying; cloned routes will not be scoped
 */

/* Build our internal representation of the system routing tables from the data buffer that
 * sysctl() gives us. Relies on helper functions above. Much trial and error was involved in making
 * this function work correctly.
 */
etcpal_error_t parse_routing_table_dump(int family, uint8_t* buf, size_t buf_len, RoutingTable* table)
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

    // Filter out entries:
    // - from the local routing table (RTF_LOCAL)
    // - Representing broadcast routes (RTF_BROADCAST)
    // - Representing ARP routes (RTF_LLDATA or RTF_LLINFO on older versions)
    // - Cloned routes (RTF_WASCLONED)
    if (!(rmsg->rtm_flags & (RTF_LINK_FLAG | RTF_LOCAL | RTF_BROADCAST | RTF_WASCLONED)))
    {
      struct sockaddr* addr_start = (struct sockaddr*)(rmsg + 1);
      struct sockaddr* rti_info[RTAX_MAX];
      get_addrs_from_route_entry(rmsg->rtm_addrs, addr_start, rti_info);

      if (rti_info[RTAX_DST] != NULL)
      {
        dest_from_route_entry(family, rti_info[RTAX_DST], &new_entry.addr);
      }
      if (rti_info[RTAX_GATEWAY] != NULL)
      {
        gateway_from_route_entry(rti_info[RTAX_GATEWAY], &new_entry.gateway);
      }

      // If this is a host route, it indicates that it has an address with a full netmask (and the
      // netmask field is not included)
      if (rmsg->rtm_flags & RTF_HOST)
      {
        if (family == AF_INET6)
          new_entry.mask = etcpal_ip_mask_from_length(kEtcPalIpTypeV6, 128);
        else
          new_entry.mask = etcpal_ip_mask_from_length(kEtcPalIpTypeV4, 32);
      }
      else if (rti_info[RTAX_NETMASK] != NULL)
      {
        netmask_from_route_entry(family, rti_info[RTAX_NETMASK], &new_entry.mask);
      }

      if (rmsg->rtm_flags & RTF_IFSCOPE)
      {
        new_entry.interface_scope = true;
      }

      new_entry.interface_index = rmsg->rtm_index;
    }
    else
    {
      new_entry_valid = false;
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

    buf_pos += rmsg->rtm_msglen;
  }

  if (table->size > 0)
  {
    qsort(table->entries, table->size, sizeof(RoutingTableEntry), compare_routing_table_entries);

    // Mark the system-wide default route: the first default route we encounter after sorting the table.
    for (RoutingTableEntry* entry = table->entries; entry < table->entries + table->size; ++entry)
    {
      if ((ETCPAL_IP_IS_INVALID(&entry->addr) || etcpal_ip_is_wildcard(&entry->addr)) &&
          (ETCPAL_IP_IS_INVALID(&entry->mask) || etcpal_ip_is_wildcard(&entry->mask)) &&
          !ETCPAL_IP_IS_INVALID(&entry->gateway))
      {
        table->default_route = entry;
        break;
      }
    }
    return kEtcPalErrOk;
  }
  else
  {
    return kEtcPalErrSys;
  }
}

void init_routing_table_entry(RoutingTableEntry* entry)
{
  ETCPAL_IP_SET_INVALID(&entry->addr);
  ETCPAL_IP_SET_INVALID(&entry->mask);
  ETCPAL_IP_SET_INVALID(&entry->gateway);
  entry->interface_index = 0;
  entry->interface_scope = false;
}

int compare_routing_table_entries(const void* a, const void* b)
{
  RoutingTableEntry* e1 = (RoutingTableEntry*)a;
  RoutingTableEntry* e2 = (RoutingTableEntry*)b;

  unsigned int mask_length_1 = etcpal_ip_mask_length(&e1->mask);
  unsigned int mask_length_2 = etcpal_ip_mask_length(&e2->mask);

  // Sort by mask length in descending order - within the same mask length, sort by whether one
  // entry has the "interface scope" flag set (unset gets priority, e.g. listed first).
  if (mask_length_1 == mask_length_2)
  {
    return (e1->interface_scope && !e2->interface_scope) - (!e1->interface_scope && e2->interface_scope);
  }
  else
  {
    return (mask_length_1 < mask_length_2) - (mask_length_1 > mask_length_2);
  }
}

void free_routing_tables()
{
  free_routing_table(&routing_table_v4);
  free_routing_table(&routing_table_v6);
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

#if ETCPAL_NETINT_DEBUG_OUTPUT
void debug_print_routing_table(RoutingTable* table)
{
  printf("%-40s %-40s %-40s %s %s\n", "Address", "Mask", "Gateway", "Index", "Name");

  for (RoutingTableEntry* entry = table->entries; entry < table->entries + table->size; ++entry)
  {
    char addr_str[ETCPAL_IP_STRING_BYTES];
    char mask_str[ETCPAL_IP_STRING_BYTES];
    char gw_str[ETCPAL_IP_STRING_BYTES];
    char ifname_str[IF_NAMESIZE];

    if (!ETCPAL_IP_IS_INVALID(&entry->addr))
      etcpal_ip_to_string(&entry->addr, addr_str);
    else
      strcpy(addr_str, "0.0.0.0");

    if (!ETCPAL_IP_IS_INVALID(&entry->mask))
      etcpal_ip_to_string(&entry->mask, mask_str);
    else
      strcpy(mask_str, "0.0.0.0");

    if (!ETCPAL_IP_IS_INVALID(&entry->gateway))
      etcpal_ip_to_string(&entry->gateway, gw_str);
    else
      strcpy(gw_str, "");

    if_indextoname(entry->interface_index, ifname_str);

    printf("%-40s %-40s %-40s %-5u %s\n", addr_str, mask_str, gw_str, entry->interface_index, ifname_str);
  }

  if (table->default_route)
  {
    char gw_str[ETCPAL_IP_STRING_BYTES];
    etcpal_ip_to_string(&table->default_route->gateway, gw_str);
    printf("Default route: %s (%u)\n", gw_str, table->default_route->interface_index);
  }
  else
  {
    printf("Default route not found.\n");
  }
}
#endif
