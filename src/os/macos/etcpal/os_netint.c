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

#include <assert.h>
#include <errno.h>
#include <limits.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <ifaddrs.h>
#include <net/if.h>
#include <net/if_dl.h>
#include <sys/ioctl.h>
#include <net/route.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/sysctl.h>
#include <unistd.h>

#include "etcpal/common.h"
#include "etcpal/private/netint.h"
#include "etcpal/private/util.h"
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

#define ROUTING_TABLE_ENTRY_INIT                                                     \
  {                                                                                  \
    ETCPAL_IP_INVALID_INIT, ETCPAL_IP_INVALID_INIT, ETCPAL_IP_INVALID_INIT, 0, false \
  }

typedef struct DarwinNetint
{
  char                name[IFNAMSIZ];
  struct sockaddr_dl* link_addr;
  unsigned int        num_addrs;
} DarwinNetint;

typedef struct DarwinNetints
{
  DarwinNetint* netints;
  size_t        num_netints;
} DarwinNetints;

#define DARWIN_NETINTS_INIT \
  {                         \
    NULL, 0                 \
  }

/*********************** Private function prototypes *************************/

// Getting general network interface information.
static etcpal_error_t get_interface_by_index_internal(unsigned int             index,
                                                      uint8_t*                 buf,
                                                      size_t*                  buf_size,
                                                      const DefaultInterfaces* default_interfaces);

static void calculate_size_needed(const DarwinNetints* darwin_netints,
                                  const uint8_t*       buf_addr,
                                  unsigned int         index,
                                  NetintArraySizes*    sizes);
static void copy_all_netint_info(const struct ifaddrs*    os_addrs,
                                 const DarwinNetints*     darwin_netints,
                                 uint8_t*                 buf,
                                 const NetintArraySizes*  sizes,
                                 const DefaultInterfaces* default_interfaces);
static void copy_single_netint_info(const struct ifaddrs*    os_addr,
                                    const DarwinNetints*     darwin_netints,
                                    EtcPalNetintInfo*        first_info,
                                    NetintArrayContext*      context,
                                    const DefaultInterfaces* default_interfaces);
static bool has_valid_inet_addr(const struct ifaddrs* netint);
static bool populate_darwin_netints(const struct ifaddrs* os_addrs, unsigned int index, DarwinNetints* netints_out);
static void free_darwin_netints(DarwinNetints* netints);
static DarwinNetint* find_darwin_netint(const char* name, DarwinNetint* begin, DarwinNetint* end);

// Functions for getting the default interfaces
static etcpal_error_t get_default_interfaces(DefaultInterfaces* default_interfaces);
static etcpal_error_t get_default_interface_index(int family, unsigned int* default_interface);

// Interacting with the BSD routing stack
static etcpal_error_t get_routing_table_dump(int family, uint8_t** buf, size_t* buf_len);
static etcpal_error_t get_default_interface_from_routing_table_dump(int           family,
                                                                    uint8_t*      buf,
                                                                    size_t        buf_len,
                                                                    unsigned int* default_interface);

static void get_addrs_from_route_entry(int addrs, struct sockaddr* sa, struct sockaddr** rti_info);
static void dest_from_route_entry(int family, const struct sockaddr* os_dst, EtcPalIpAddr* etcpal_dst);
static void netmask_from_route_entry(int family, const struct sockaddr* os_netmask, EtcPalIpAddr* etcpal_netmask);
static void gateway_from_route_entry(const struct sockaddr* os_gw, EtcPalIpAddr* etcpal_gw);

/*************************** Function definitions ****************************/

size_t etcpal_netint_get_num_interfaces(void)
{
  struct ifaddrs* os_addrs = NULL;
  if (getifaddrs(&os_addrs) < 0)
  {
    return 0;
  }

  size_t        num_netints = 0;
  DarwinNetints netints     = DARWIN_NETINTS_INIT;
  if (populate_darwin_netints(os_addrs, 0, &netints))
  {
    for (const DarwinNetint* netint = netints.netints; netint < netints.netints + netints.num_netints; ++netint)
    {
      // Only consider interfaces that have a link addr structure
      if (netint->link_addr)
      {
        ++num_netints;
      }
    }

    free_darwin_netints(&netints);
  }

  freeifaddrs(os_addrs);
  return num_netints;
}

etcpal_error_t etcpal_netint_get_interfaces(uint8_t* buf, size_t* buf_size)
{
  if (!buf || !buf_size)
    return kEtcPalErrInvalid;

  DefaultInterfaces default_interfaces = DEFAULT_INTERFACES_INIT;
  get_default_interfaces(&default_interfaces);

  struct ifaddrs* os_addrs = NULL;
  if (getifaddrs(&os_addrs) < 0)
    return errno_os_to_etcpal(errno);

  DarwinNetints darwin_netints = DARWIN_NETINTS_INIT;
  if (!populate_darwin_netints(os_addrs, 0, &darwin_netints))
  {
    freeifaddrs(os_addrs);
    return kEtcPalErrNoNetints;
  }

  NetintArraySizes sizes = NETINT_ARRAY_SIZES_INIT;
  calculate_size_needed(&darwin_netints, buf, 0, &sizes);
  if (sizes.total_size > *buf_size)
  {
    *buf_size = sizes.total_size;
    free_darwin_netints(&darwin_netints);
    freeifaddrs(os_addrs);
    return kEtcPalErrBufSize;
  }

  copy_all_netint_info(os_addrs, &darwin_netints, buf, &sizes, &default_interfaces);
  *buf_size = sizes.total_size;
  free_darwin_netints(&darwin_netints);
  freeifaddrs(os_addrs);
  return kEtcPalErrOk;
}

etcpal_error_t etcpal_netint_get_interface_by_index(unsigned int index, uint8_t* buf, size_t* buf_size)
{
  if (!buf || !buf_size)
    return kEtcPalErrInvalid;

  DefaultInterfaces default_interfaces = DEFAULT_INTERFACES_INIT;
  get_default_interfaces(&default_interfaces);

  return get_interface_by_index_internal(index, buf, buf_size, &default_interfaces);
}

etcpal_error_t etcpal_netint_get_default_interface_index(etcpal_iptype_t type, unsigned int* netint_index)
{
  if (!netint_index)
    return kEtcPalErrInvalid;

  etcpal_error_t res       = kEtcPalErrInvalid;
  unsigned int   def_index = 0;

  switch (type)
  {
    case kEtcPalIpTypeV4:
      res = get_default_interface_index(AF_INET, &def_index);
      break;
    case kEtcPalIpTypeV6:
      res = get_default_interface_index(AF_INET6, &def_index);
      break;
    default:
      break;
  }

  if (res != kEtcPalErrOk)
    return res;
  if (def_index == 0)
    return kEtcPalErrNotFound;

  *netint_index = def_index;
  return kEtcPalErrOk;
}

etcpal_error_t etcpal_netint_get_default_interface(etcpal_iptype_t type, uint8_t* buf, size_t* buf_size)
{
  if (type != kEtcPalIpTypeV4 && type != kEtcPalIpTypeV6)
    return kEtcPalErrInvalid;

  // We will eventually need both defaults to populate the flags correctly, even though we are only
  // getting the default for one IP type.
  DefaultInterfaces default_interfaces = DEFAULT_INTERFACES_INIT;
  etcpal_error_t    res                = get_default_interfaces(&default_interfaces);
  if (res != kEtcPalErrOk)
    return res;

  if ((type == kEtcPalIpTypeV4 && default_interfaces.default_index_v4 == 0) ||
      (type == kEtcPalIpTypeV6 && default_interfaces.default_index_v6 == 0))
  {
    return kEtcPalErrNotFound;
  }

  return get_interface_by_index_internal(
      type == kEtcPalIpTypeV6 ? default_interfaces.default_index_v6 : default_interfaces.default_index_v4, buf,
      buf_size, &default_interfaces);
}

bool etcpal_netint_is_up(unsigned int index)
{
  int ioctl_sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (ioctl_sock == -1)
    return false;

  // Translate the index to a name
  struct ifreq if_req;
  if (!if_indextoname(index, if_req.ifr_name))
  {
    close(ioctl_sock);
    return false;
  }

  // Get the flags for the interface with the given name
  int ioctl_res = ioctl(ioctl_sock, SIOCGIFFLAGS, &if_req);
  close(ioctl_sock);
  if (ioctl_res == 0)
  {
    return (if_req.ifr_flags & IFF_UP) != 0;
  }
  else
  {
    return false;
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Internal functions
///////////////////////////////////////////////////////////////////////////////////////////////////

etcpal_error_t get_interface_by_index_internal(unsigned int             index,
                                               uint8_t*                 buf,
                                               size_t*                  buf_size,
                                               const DefaultInterfaces* default_interfaces)
{
  if (index == 0)
    return kEtcPalErrInvalid;

  struct ifaddrs* os_addrs = NULL;
  if (getifaddrs(&os_addrs) < 0)
    return errno_os_to_etcpal(errno);

  DarwinNetints darwin_netints = DARWIN_NETINTS_INIT;
  if (!populate_darwin_netints(os_addrs, index, &darwin_netints))
  {
    freeifaddrs(os_addrs);
    return kEtcPalErrNoNetints;
  }

  NetintArraySizes sizes = NETINT_ARRAY_SIZES_INIT;
  calculate_size_needed(&darwin_netints, buf, index, &sizes);
  if (sizes.total_size > *buf_size)
  {
    *buf_size = sizes.total_size;
    free_darwin_netints(&darwin_netints);
    freeifaddrs(os_addrs);
    return kEtcPalErrBufSize;
  }

  NetintArrayContext context = NETINT_ARRAY_CONTEXT_INIT(buf, &sizes);

  bool found_interface = false;
  for (const struct ifaddrs* ifaddr = os_addrs; ifaddr; ifaddr = ifaddr->ifa_next)
  {
    if (if_nametoindex(ifaddr->ifa_name) == index)
    {
      found_interface = true;
      copy_single_netint_info(ifaddr, &darwin_netints, (EtcPalNetintInfo*)buf, &context, default_interfaces);
    }
  }

  free_darwin_netints(&darwin_netints);
  freeifaddrs(os_addrs);
  if (found_interface)
  {
    *buf_size = sizes.total_size;
    return kEtcPalErrOk;
  }
  return kEtcPalErrNotFound;
}

void calculate_size_needed(const DarwinNetints* darwin_netints,
                           const uint8_t*       buf_addr,
                           unsigned int         index,
                           NetintArraySizes*    sizes)
{
  size_t size_needed         = 0;
  size_t netintinfo_arr_size = 0;
  size_t ip_addr_arr_size    = 0;

  for (const DarwinNetint* netint = darwin_netints->netints;
       netint < darwin_netints->netints + darwin_netints->num_netints; ++netint)
  {
    // We only count network interfaces that have a sockaddr_dl/AF_LINK structure
    if (netint->link_addr)
    {
      netintinfo_arr_size += sizeof(EtcPalNetintInfo);
      size_needed += sizeof(EtcPalNetintInfo) + strlen(netint->name) + 1;
      ip_addr_arr_size += (sizeof(EtcPalNetintAddr) * netint->num_addrs);
      size_needed += (sizeof(EtcPalNetintAddr) * netint->num_addrs);
    }
  }

  size_t padding = GET_PADDING_FOR_ALIGNMENT(buf_addr, EtcPalNetintInfo);
  size_needed += padding;
  sizes->total_size      = size_needed;
  sizes->ip_addrs_offset = netintinfo_arr_size + padding;
  sizes->names_offset    = netintinfo_arr_size + padding + ip_addr_arr_size;
}

void copy_all_netint_info(const struct ifaddrs*    os_addrs,
                          const DarwinNetints*     darwin_netints,
                          uint8_t*                 buf,
                          const NetintArraySizes*  sizes,
                          const DefaultInterfaces* default_interfaces)
{
  NetintArrayContext context = NETINT_ARRAY_CONTEXT_INIT(buf, sizes);

  for (const struct ifaddrs* ifaddr = os_addrs; ifaddr; ifaddr = ifaddr->ifa_next)
  {
    DarwinNetint* netint = find_darwin_netint(ifaddr->ifa_name, darwin_netints->netints,
                                              darwin_netints->netints + darwin_netints->num_netints);
    if (!netint->link_addr)
      continue;

    copy_single_netint_info(ifaddr, darwin_netints, (EtcPalNetintInfo*)buf, &context, default_interfaces);
  }

  // Link the EtcPalNetintInfo structures together
  EtcPalNetintInfo* cur_netint = (EtcPalNetintInfo*)buf;
  while (cur_netint + 1 != context.cur_info)
  {
    cur_netint->next = cur_netint + 1;
    ++cur_netint;
  }
}

void copy_single_netint_info(const struct ifaddrs*    os_addr,
                             const DarwinNetints*     darwin_netints,
                             EtcPalNetintInfo*        first_info,
                             NetintArrayContext*      context,
                             const DefaultInterfaces* default_interfaces)
{
  bool              new_netint         = false;
  EtcPalNetintAddr* last_existing_addr = NULL;
  EtcPalNetintInfo* cur_info           = find_existing_netint(os_addr->ifa_name, first_info, context->cur_info);
  if (cur_info)
  {
    if (cur_info->addrs)
    {
      last_existing_addr = (EtcPalNetintAddr*)cur_info->addrs;
      while (last_existing_addr->next)
        last_existing_addr = last_existing_addr->next;
    }
  }
  else
  {
    new_netint = true;
    cur_info   = context->cur_info;
    memset(cur_info, 0, sizeof(EtcPalNetintInfo));
  }

  if (new_netint)
  {
    // Interface name
    strcpy(context->cur_name, os_addr->ifa_name);
    cur_info->id            = context->cur_name;
    cur_info->friendly_name = context->cur_name;
    context->cur_name += strlen(os_addr->ifa_name) + 1;
  }

  if (has_valid_inet_addr(os_addr))
  {
    // Add an IP address
    memset(context->cur_addr, 0, sizeof(EtcPalNetintAddr));

    ip_os_to_etcpal(os_addr->ifa_addr, &context->cur_addr->addr);
    EtcPalIpAddr mask;
    ip_os_to_etcpal(os_addr->ifa_netmask, &mask);
    context->cur_addr->mask_length = etcpal_ip_mask_length(&mask);

    if (last_existing_addr)
      last_existing_addr->next = context->cur_addr;
    else
      cur_info->addrs = context->cur_addr;

    ++context->cur_addr;
  }
  else if (os_addr->ifa_addr->sa_family == AF_LINK)
  {
    // Add link information
    struct sockaddr_dl* link_addr = (struct sockaddr_dl*)os_addr->ifa_addr;

    cur_info->index = link_addr->sdl_index;
    if (link_addr->sdl_alen == ETCPAL_MAC_BYTES)
      memcpy(cur_info->mac.data, link_addr->sdl_data + link_addr->sdl_nlen, ETCPAL_MAC_BYTES);

    if (cur_info->index == default_interfaces->default_index_v4)
      cur_info->flags |= ETCPAL_NETINT_FLAG_DEFAULT_V4;
    if (cur_info->index == default_interfaces->default_index_v6)
      cur_info->flags |= ETCPAL_NETINT_FLAG_DEFAULT_V6;
  }

  if (new_netint)
    ++context->cur_info;
}

bool has_valid_inet_addr(const struct ifaddrs* netint)
{
  return netint->ifa_addr && netint->ifa_netmask &&
         (netint->ifa_addr->sa_family == AF_INET || netint->ifa_addr->sa_family == AF_INET6);
}

bool populate_darwin_netints(const struct ifaddrs* os_addrs, unsigned int index, DarwinNetints* netints_out)
{
  DarwinNetint* netints     = NULL;
  size_t        num_netints = 0;

  for (const struct ifaddrs* ifaddr = os_addrs; ifaddr; ifaddr = ifaddr->ifa_next)
  {
    if (index != 0)
    {
      // Determine whether to include this interface, using its index
      if (if_nametoindex(ifaddr->ifa_name) != index)
        continue;
    }

    DarwinNetint* netint = NULL;

    if (!netints)
    {
      netints = (DarwinNetint*)malloc(sizeof(DarwinNetint));
      assert(netints);
      memset(netints, 0, sizeof(DarwinNetint));
      strcpy(netints[0].name, ifaddr->ifa_name);
      ++num_netints;
      netint = netints;
    }
    else
    {
      netint = find_darwin_netint(ifaddr->ifa_name, netints, netints + num_netints);
      if (!netint)
      {
        netints = (DarwinNetint*)realloc(netints, sizeof(DarwinNetint) * (num_netints + 1));
        assert(netints);
        memset(&netints[num_netints], 0, sizeof(DarwinNetint));
        strcpy(netints[num_netints].name, ifaddr->ifa_name);
        netint = &netints[num_netints];
        ++num_netints;
      }
    }

    // Invariant at this point: netint != NULL
    if (has_valid_inet_addr(ifaddr))
    {
      ++netint->num_addrs;
    }
    else if (ifaddr->ifa_addr && ifaddr->ifa_addr->sa_family == AF_LINK)
    {
      // We have a link address
      netint->link_addr = (struct sockaddr_dl*)ifaddr->ifa_addr;
    }
  }

  if (netints && num_netints)
  {
    netints_out->netints     = netints;
    netints_out->num_netints = num_netints;
    return true;
  }
  return false;
}

void free_darwin_netints(DarwinNetints* netints)
{
  if (netints->netints)
    free(netints->netints);
}

DarwinNetint* find_darwin_netint(const char* name, DarwinNetint* begin, DarwinNetint* end)
{
  for (DarwinNetint* cur_iface = begin; cur_iface < end; ++cur_iface)
  {
    if (strcmp(cur_iface->name, name) == 0)
    {
      return cur_iface;
    }
  }
  return NULL;
}

etcpal_error_t get_default_interfaces(DefaultInterfaces* default_interfaces)
{
  etcpal_error_t res = get_default_interface_index(AF_INET, &default_interfaces->default_index_v4);
  if (res == kEtcPalErrOk)
  {
    res = get_default_interface_index(AF_INET6, &default_interfaces->default_index_v6);
  }

  return res;
}

etcpal_error_t get_default_interface_index(int family, unsigned int* default_interface)
{
  uint8_t* buf     = NULL;
  size_t   buf_len = 0;

  etcpal_error_t res = get_routing_table_dump(family, &buf, &buf_len);
  if (res == kEtcPalErrOk)
  {
    res = get_default_interface_from_routing_table_dump(family, buf, buf_len, default_interface);
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

#ifdef RTF_LLDATA
#define RTF_LINK_FLAG RTF_LLDATA
#else
#define RTF_LINK_FLAG RTF_LLINFO
#endif

/*
 * Use the system routing table from the data buffer that sysctl() gives us to determine the
 * default route for the given IP family. Relies on helper functions above. Much trial and error
 * was involved in making this function work correctly.
 */
etcpal_error_t get_default_interface_from_routing_table_dump(int           family,
                                                             uint8_t*      buf,
                                                             size_t        buf_len,
                                                             unsigned int* default_interface)
{
  // Note: There is a bit more code here than necessary. We don't have to parse out the addresses
  // of all the routing table entries to extract a default route. The helper functions and
  // RoutingTableEntry struct are left over from old code which relied on constructing the entire
  // system routing table to serve removed API functions which attempted to predict the network
  // interface that a packet to a given destination address would use.
  //
  // The helpers are left in here because they required quite a bit of investment of effort to
  // create, and represent a nontrivial amount of undocumented knowledge of how to get routing
  // information from the Darwin kernel. This information may be needed again in the future. This
  // code is non-performance-critical, and only runs when enumerating network interfaces.

  size_t            buf_pos           = 0;
  RoutingTableEntry default_candidate = ROUTING_TABLE_ENTRY_INIT;

  // Parse the dump
  // Loop through all routing table entries returned in the buffer
  while (buf_pos < buf_len)
  {
    RoutingTableEntry entry = ROUTING_TABLE_ENTRY_INIT;

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
        dest_from_route_entry(family, rti_info[RTAX_DST], &entry.addr);
      }
      if (rti_info[RTAX_GATEWAY] != NULL)
      {
        gateway_from_route_entry(rti_info[RTAX_GATEWAY], &entry.gateway);
      }

      // If this is a host route, it indicates that it has an address with a full netmask (and the
      // netmask field is not included)
      if (rmsg->rtm_flags & RTF_HOST)
      {
        if (family == AF_INET6)
          entry.mask = etcpal_ip_mask_from_length(kEtcPalIpTypeV6, 128);
        else
          entry.mask = etcpal_ip_mask_from_length(kEtcPalIpTypeV4, 32);
      }
      else if (rti_info[RTAX_NETMASK] != NULL)
      {
        netmask_from_route_entry(family, rti_info[RTAX_NETMASK], &entry.mask);
      }

      if (rmsg->rtm_flags & RTF_IFSCOPE)
      {
        entry.interface_scope = true;
      }

      entry.interface_index = rmsg->rtm_index;

      // Default route candidates have invalid or wildcard addresses for the address and mask, and
      // a valid default gateway. Among these candidates, routes with the "interface scope" flag
      // unset have priority. If more than one route fits all of these criteria, the first one is
      // chosen.
      if ((ETCPAL_IP_IS_INVALID(&entry.addr) || etcpal_ip_is_wildcard(&entry.addr)) &&
          (ETCPAL_IP_IS_INVALID(&entry.mask) || etcpal_ip_is_wildcard(&entry.mask)) &&
          !ETCPAL_IP_IS_INVALID(&entry.gateway))
      {
        if (ETCPAL_IP_IS_INVALID(&default_candidate.gateway) ||
            (!entry.interface_scope && default_candidate.interface_scope))
        {
          default_candidate = entry;
        }
      }
    }

    buf_pos += rmsg->rtm_msglen;
  }

  if (!ETCPAL_IP_IS_INVALID(&default_candidate.gateway))
    *default_interface = default_candidate.interface_index;
  else
    *default_interface = 0;

  return kEtcPalErrOk;
}

/* These two macros and the following function were taken from the Unix Network Programming book
 * (sect. 18.3) but they required some modification, because they mistakenly assert that the
 * sockaddr structures following a route message header are aligned to sizeof(unsigned long), when
 * in fact on 64-bit macOS platforms they still seem to be aligned on 4-byte boundaries.
 * (sizeof(unsigned long) is 8 on 64-bit platforms)
 */
#define SOCKADDR_ALIGN 4  // not sizeof(unsigned long)
#define SA_SIZE(sa)    ((!(sa) || (sa)->sa_len == 0) ? SOCKADDR_ALIGN : (1 + (((sa)->sa_len - 1) | (SOCKADDR_ALIGN - 1))))

void get_addrs_from_route_entry(int addrs, struct sockaddr* sa, struct sockaddr** rti_info)
{
  for (int i = 0; i < RTAX_MAX; ++i)
  {
    if (addrs & (1 << i))
    {
      rti_info[i] = sa;
      sa          = (struct sockaddr*)((char*)sa + SA_SIZE(sa));
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
void dest_from_route_entry(int family, const struct sockaddr* os_dst, EtcPalIpAddr* etcpal_dst)
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
void netmask_from_route_entry(int family, const struct sockaddr* os_netmask, EtcPalIpAddr* etcpal_netmask)
{
  if (family == AF_INET)
  {
    size_t      mask_offset = offsetof(struct sockaddr_in, sin_addr);
    const char* mask_ptr    = &((char*)os_netmask)[mask_offset];

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
    const char* mask_ptr    = &((char*)os_netmask)[mask_offset];

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
void gateway_from_route_entry(const struct sockaddr* os_gw, EtcPalIpAddr* etcpal_gw)
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
