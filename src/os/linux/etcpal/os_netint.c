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

#include "etcpal/netint.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>

#include <ifaddrs.h>
#include <errno.h>
#include <asm/types.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "etcpal/common.h"
#include "etcpal/socket.h"
#include "etcpal/private/netint.h"
#include "etcpal/private/util.h"
#include "os_error.h"

/***************************** Private types *********************************/

/* A composite struct representing an RT_NETLINK request sent over a netlink socket. */
typedef struct RtNetlinkRequest
{
  struct nlmsghdr nl_header;
  struct rtmsg    rt_msg;
} RtNetlinkRequest;

typedef struct OsResources
{
  int             ioctl_sock;
  struct ifaddrs* os_addrs;
} OsResources;

#define OS_RESOURCES_INIT \
  {                       \
    -1, NULL              \
  }

typedef struct UnixInterfaceName
{
  char name[IFNAMSIZ];
} UnixInterfaceName;

/*********************** Private function prototypes *************************/

// Getting general network interface information.
static etcpal_error_t get_interface_by_index_internal(unsigned int             index,
                                                      uint8_t*                 buf,
                                                      size_t*                  buf_size,
                                                      const DefaultInterfaces* default_interfaces);

static etcpal_error_t get_os_resources(OsResources* os_resources);
static void           free_os_resources(OsResources* os_resources);

static void calculate_size_needed(const OsResources* os_resources,
                                  const uint8_t*     buf_addr,
                                  unsigned int       index,
                                  NetintArraySizes*  sizes);
static void copy_all_netint_info(const OsResources*       os_resources,
                                 uint8_t*                 buf,
                                 const NetintArraySizes*  sizes,
                                 const DefaultInterfaces* default_interfaces);
static void copy_single_netint_info(const OsResources*       os_resources,
                                    const struct ifaddrs*    addr,
                                    EtcPalNetintInfo*        first_info,
                                    NetintArrayContext*      context,
                                    const DefaultInterfaces* default_interfaces);
static bool has_valid_addr(const struct ifaddrs* netint);

// Functions for getting the default interfaces
static etcpal_error_t get_default_interfaces(DefaultInterfaces* default_interfaces);
static etcpal_error_t get_default_interface_index(int family, unsigned int* default_interface);

// Interacting with RTNETLINK
static etcpal_error_t send_netlink_route_request(int socket, int family);
static etcpal_error_t receive_netlink_route_reply(int           sock,
                                                  int           family,
                                                  size_t        buf_size,
                                                  unsigned int* default_interface);
static unsigned int   get_default_index_from_netlink_reply(int family, const char* buffer, size_t nl_msg_size);

static bool unix_interface_name_exists(const char* name, UnixInterfaceName* begin, UnixInterfaceName* end);

/*************************** Function definitions ****************************/

size_t etcpal_netint_get_num_interfaces(void)
{
  struct ifaddrs* os_addrs = NULL;
  if (getifaddrs(&os_addrs) < 0)
  {
    return 0;
  }

  UnixInterfaceName* names     = NULL;
  size_t             num_names = 0;

  for (const struct ifaddrs* ifaddr = os_addrs; ifaddr; ifaddr = ifaddr->ifa_next)
  {
    if (!names)
    {
      names = (UnixInterfaceName*)malloc(sizeof(UnixInterfaceName));
      assert(names);
      strcpy(names[0].name, ifaddr->ifa_name);
      ++num_names;
    }
    else if (!unix_interface_name_exists(ifaddr->ifa_name, names, names + num_names))
    {
      names = (UnixInterfaceName*)realloc(names, sizeof(UnixInterfaceName) * (num_names + 1));
      assert(names);
      strcpy(names[num_names].name, ifaddr->ifa_name);
      ++num_names;
    }
  }

  if (names)
    free(names);
  freeifaddrs(os_addrs);
  return num_names;
}

etcpal_error_t etcpal_netint_get_interfaces(uint8_t* buf, size_t* buf_size)
{
  if (!buf || !buf_size)
    return kEtcPalErrInvalid;

  DefaultInterfaces default_interfaces = DEFAULT_INTERFACES_INIT;
  get_default_interfaces(&default_interfaces);

  OsResources    os_resources = OS_RESOURCES_INIT;
  etcpal_error_t res          = get_os_resources(&os_resources);
  if (res != kEtcPalErrOk)
    return res;

  NetintArraySizes sizes = NETINT_ARRAY_SIZES_INIT;
  calculate_size_needed(&os_resources, buf, 0, &sizes);
  if (sizes.total_size > *buf_size)
  {
    *buf_size = sizes.total_size;
    free_os_resources(&os_resources);
    return kEtcPalErrBufSize;
  }

  copy_all_netint_info(&os_resources, buf, &sizes, &default_interfaces);
  *buf_size = sizes.total_size;
  free_os_resources(&os_resources);
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
  if_req.ifr_ifindex = (int)index;
  int ioctl_res      = ioctl(ioctl_sock, SIOCGIFNAME, &if_req);
  if (ioctl_res != 0)
  {
    close(ioctl_sock);
    return false;
  }

  // Get the flags for the interface with the given name
  ioctl_res = ioctl(ioctl_sock, SIOCGIFFLAGS, &if_req);
  close(ioctl_sock);
  if (ioctl_res == 0)
  {
    return (if_req.ifr_flags & IFF_UP) != 0;
  }
  return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Internal functions
///////////////////////////////////////////////////////////////////////////////////////////////////

etcpal_error_t get_interface_by_index_internal(unsigned int             index,
                                               uint8_t*                 buf,
                                               size_t*                  buf_size,
                                               const DefaultInterfaces* default_interfaces)
{
  OsResources    os_resources = OS_RESOURCES_INIT;
  etcpal_error_t res          = get_os_resources(&os_resources);
  if (res != kEtcPalErrOk)
    return res;

  // Pass 1: Total the number of addresses
  NetintArraySizes sizes = NETINT_ARRAY_SIZES_INIT;
  calculate_size_needed(&os_resources, buf, index, &sizes);
  if (sizes.total_size > *buf_size)
  {
    *buf_size = sizes.total_size;
    free_os_resources(&os_resources);
    return kEtcPalErrBufSize;
  }

  NetintArrayContext context = NETINT_ARRAY_CONTEXT_INIT(buf, &sizes);

  bool found_interface = false;
  for (const struct ifaddrs* ifaddr = os_resources.os_addrs; ifaddr; ifaddr = ifaddr->ifa_next)
  {
    struct ifreq if_req;
    strncpy(if_req.ifr_name, ifaddr->ifa_name, IFNAMSIZ);
    int ioctl_res = ioctl(os_resources.ioctl_sock, SIOCGIFINDEX, &if_req);
    if (ioctl_res == 0)
    {
      if (if_req.ifr_ifindex == index)
      {
        found_interface = true;
        copy_single_netint_info(&os_resources, ifaddr, (EtcPalNetintInfo*)buf, &context, default_interfaces);
      }
    }
  }

  free_os_resources(&os_resources);
  if (found_interface)
  {
    *buf_size = sizes.total_size;
    return kEtcPalErrOk;
  }
  return kEtcPalErrNotFound;
}

etcpal_error_t get_os_resources(OsResources* os_resources)
{
  // Create the OS resources necessary to enumerate the interfaces
  os_resources->ioctl_sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (os_resources->ioctl_sock == -1)
    return errno_os_to_etcpal(errno);

  if (getifaddrs(&os_resources->os_addrs) < 0)
  {
    close(os_resources->ioctl_sock);
    return errno_os_to_etcpal(errno);
  }

  return kEtcPalErrOk;
}

void free_os_resources(OsResources* os_resources)
{
  freeifaddrs(os_resources->os_addrs);
  os_resources->os_addrs = NULL;
  close(os_resources->ioctl_sock);
  os_resources->ioctl_sock = -1;
}

void calculate_size_needed(const OsResources* os_resources,
                           const uint8_t*     buf_addr,
                           unsigned int       index,
                           NetintArraySizes*  sizes)
{
  UnixInterfaceName* names               = NULL;
  size_t             num_names           = 0;
  size_t             size_needed         = 0;
  size_t             netintinfo_arr_size = 0;
  size_t             ip_addr_arr_size    = 0;

  for (const struct ifaddrs* ifaddr = os_resources->os_addrs; ifaddr; ifaddr = ifaddr->ifa_next)
  {
    if (index != 0)
    {
      // Determine whether to include this interface, using its index
      struct ifreq if_req;
      strncpy(if_req.ifr_name, ifaddr->ifa_name, IFNAMSIZ);

      // Interface index
      int ioctl_res = ioctl(os_resources->ioctl_sock, SIOCGIFINDEX, &if_req);
      if (ioctl_res != 0 || if_req.ifr_ifindex != index)
        continue;
    }

    bool new_netint = false;

    if (!names)
    {
      names = (UnixInterfaceName*)malloc(sizeof(UnixInterfaceName));
      assert(names);
      strcpy(names[0].name, ifaddr->ifa_name);
      ++num_names;
      new_netint = true;
    }
    else if (!unix_interface_name_exists(ifaddr->ifa_name, names, names + num_names))
    {
      names = (UnixInterfaceName*)realloc(names, sizeof(UnixInterfaceName) * (num_names + 1));
      assert(names);
      strcpy(names[num_names].name, ifaddr->ifa_name);
      ++num_names;
      new_netint = true;
    }

    if (new_netint)
    {
      netintinfo_arr_size += sizeof(EtcPalNetintInfo);
      size_needed += sizeof(EtcPalNetintInfo) + strlen(ifaddr->ifa_name) + 1;
    }

    if (has_valid_addr(ifaddr))
    {
      size_needed += sizeof(EtcPalNetintAddr);
      ip_addr_arr_size += sizeof(EtcPalNetintAddr);
    }
  }

  if (names)
    free(names);

  size_t padding = GET_PADDING_FOR_ALIGNMENT(buf_addr, EtcPalNetintInfo);
  size_needed += padding;
  sizes->total_size      = size_needed;
  sizes->ip_addrs_offset = netintinfo_arr_size + padding;
  sizes->names_offset    = netintinfo_arr_size + padding + ip_addr_arr_size;
}

void copy_all_netint_info(const OsResources*       os_resources,
                          uint8_t*                 buf,
                          const NetintArraySizes*  sizes,
                          const DefaultInterfaces* default_interfaces)
{
  NetintArrayContext context = NETINT_ARRAY_CONTEXT_INIT(buf, sizes);

  for (const struct ifaddrs* ifaddr = os_resources->os_addrs; ifaddr; ifaddr = ifaddr->ifa_next)
  {
    copy_single_netint_info(os_resources, ifaddr, (EtcPalNetintInfo*)buf, &context, default_interfaces);
  }

  // Link the EtcPalNetintInfo structures together
  EtcPalNetintInfo* cur_netint = (EtcPalNetintInfo*)buf;
  while (cur_netint + 1 != context.cur_info)
  {
    cur_netint->next = cur_netint + 1;
    ++cur_netint;
  }
}

void copy_single_netint_info(const OsResources*       os_resources,
                             const struct ifaddrs*    os_addr,
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

    struct ifreq if_req;
    // Struct ifreq to use with ioctl() calls
    strncpy(if_req.ifr_name, os_addr->ifa_name, IFNAMSIZ);

    // Hardware address
    int ioctl_res = ioctl(os_resources->ioctl_sock, SIOCGIFHWADDR, &if_req);
    // TODO verify ARPHRD_ETHER is the only value for which the MAC address should be copied out
    if (ioctl_res == 0 && if_req.ifr_hwaddr.sa_family == ARPHRD_ETHER)
      memcpy(cur_info->mac.data, if_req.ifr_hwaddr.sa_data, ETCPAL_MAC_BYTES);
    else
      memset(cur_info->mac.data, 0, ETCPAL_MAC_BYTES);

    // Interface index
    ioctl_res = ioctl(os_resources->ioctl_sock, SIOCGIFINDEX, &if_req);
    if (ioctl_res == 0)
      cur_info->index = (unsigned int)if_req.ifr_ifindex;
    else
      cur_info->index = 0;

    // Flags
    if (cur_info->index == default_interfaces->default_index_v4)
      cur_info->flags |= ETCPAL_NETINT_FLAG_DEFAULT_V4;
    if (cur_info->index == default_interfaces->default_index_v6)
      cur_info->flags |= ETCPAL_NETINT_FLAG_DEFAULT_V6;
  }

  // Add the address
  if (has_valid_addr(os_addr))
  {
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

  if (new_netint)
    ++context->cur_info;
}

bool has_valid_addr(const struct ifaddrs* netint)
{
  return netint->ifa_addr && netint->ifa_netmask &&
         (netint->ifa_addr->sa_family == AF_INET || netint->ifa_addr->sa_family == AF_INET6);
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
  // Create a netlink socket, send a netlink request to get the routing table, and receive the
  // reply. If the buffer was not big enough, repeat (cannot reuse the same socket because we've
  // often received partial messages that must be discarded)

  etcpal_error_t result        = kEtcPalErrOk;
  bool           done          = false;
  size_t         recv_buf_size = 2048;  // Tests show this is usually enough for small routing tables
  while (result == kEtcPalErrOk && !done)
  {
    struct sockaddr_nl addr;
    memset(&addr, 0, sizeof(struct sockaddr_nl));
    addr.nl_family = AF_NETLINK;

    int sock = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
    if (sock == -1)
      result = errno_os_to_etcpal(errno);

    if (result == kEtcPalErrOk)
    {
      if (0 != bind(sock, (struct sockaddr*)&addr, sizeof(addr)))
        result = errno_os_to_etcpal(errno);
    }

    if (result == kEtcPalErrOk)
      result = send_netlink_route_request(sock, family);

    if (result == kEtcPalErrOk)
    {
      result = receive_netlink_route_reply(sock, family, recv_buf_size, default_interface);
      switch (result)
      {
        case kEtcPalErrOk:
          done = true;
          break;
        case kEtcPalErrBufSize:
          recv_buf_size *= 2;  // Double the buffer size and try again.
          result = kEtcPalErrOk;
          break;
        default:
          break;
      }
    }

    close(sock);
  }
  return result;
}

etcpal_error_t send_netlink_route_request(int socket, int family)
{
  // Build the request
  RtNetlinkRequest req;
  memset(&req, 0, sizeof(req));
  req.nl_header.nlmsg_len   = NLMSG_LENGTH(sizeof(struct rtmsg));
  req.nl_header.nlmsg_type  = RTM_GETROUTE;
  req.nl_header.nlmsg_flags = (__u16)(NLM_F_REQUEST | NLM_F_DUMP);
  req.nl_header.nlmsg_pid   = (__u32)getpid();
  req.rt_msg.rtm_family     = (unsigned char)family;
  req.rt_msg.rtm_table      = RT_TABLE_MAIN;

  // Send it to the kernel
  struct sockaddr_nl naddr;
  memset(&naddr, 0, sizeof(naddr));
  naddr.nl_family = AF_NETLINK;

  if (sendto(socket, &req.nl_header, req.nl_header.nlmsg_len, 0, (struct sockaddr*)&naddr, sizeof(naddr)) >= 0)
    return kEtcPalErrOk;
  return errno_os_to_etcpal(errno);
}

etcpal_error_t receive_netlink_route_reply(int sock, int family, size_t buf_size, unsigned int* default_interface)
{
  // Allocate slightly larger than buf_size, so we can detect when more room is needed
  size_t real_size = buf_size + 20;
  char*  buffer    = (char*)malloc(real_size);
  if (!buffer)
    return kEtcPalErrNoMem;
  memset(buffer, 0, real_size);

  char*  cur_ptr     = buffer;
  size_t nl_msg_size = 0;

  // Read from the socket until the NLMSG_DONE is returned in the type of the RTNETLINK message
  while (1)
  {
    // While we are receiving with real_size, checking against buf_size will detect when we've
    // passed the limit given by the app
    if (buf_size <= nl_msg_size)
    {
      free(buffer);
      return kEtcPalErrBufSize;
    }

    ssize_t recv_res = recv(sock, cur_ptr, real_size - nl_msg_size, 0);
    if (recv_res == -1)
    {
      free(buffer);
      return errno_os_to_etcpal(errno);
    }

    struct nlmsghdr* nl_header = (struct nlmsghdr*)cur_ptr;

    if (nl_header->nlmsg_type == NLMSG_DONE)
      break;

    // Adjust our position in the buffer and size received
    cur_ptr += recv_res;
    nl_msg_size += (size_t)recv_res;
  }

  *default_interface = get_default_index_from_netlink_reply(family, buffer, nl_msg_size);
  free(buffer);
  return kEtcPalErrOk;
}

unsigned int get_default_index_from_netlink_reply(int family, const char* buffer, size_t nl_msg_size)
{
  int          lowest_metric     = INT_MAX;
  unsigned int default_interface = 0;

  // Parse the result
  // outer loop: loops thru all the NETLINK headers that also include the route entry header
  struct nlmsghdr* nl_header = (struct nlmsghdr*)buffer;
  for (; NLMSG_OK(nl_header, nl_msg_size); nl_header = NLMSG_NEXT(nl_header, nl_msg_size))
  {
    // get route entry header
    struct rtmsg* rt_message = (struct rtmsg*)NLMSG_DATA(nl_header);

    // Filter out entries from the local routing table. Netlink seems to give us those even though
    // we only asked for the main one.
    if (rt_message->rtm_type != RTN_LOCAL && rt_message->rtm_type != RTN_BROADCAST &&
        rt_message->rtm_type != RTN_ANYCAST)
    {
      bool         has_dst        = false;
      bool         has_gateway    = false;
      int          current_metric = INT_MAX;
      unsigned int current_index  = 0;

      // inner loop: loop thru all the attributes of one route entry.
      struct rtattr* rt_attributes = (struct rtattr*)RTM_RTA(rt_message);
      unsigned int   rt_attr_size  = (unsigned int)RTM_PAYLOAD(nl_header);
      for (; RTA_OK(rt_attributes, rt_attr_size); rt_attributes = RTA_NEXT(rt_attributes, rt_attr_size))
      {
        // We only care about the gateway and DST attribute
        if (rt_attributes->rta_type == RTA_DST)
        {
          has_dst = true;
        }
        else if (rt_attributes->rta_type == RTA_GATEWAY)
        {
          has_gateway = true;
        }
        else if (rt_attributes->rta_type == RTA_OIF)
        {
          current_index = *((int*)RTA_DATA(rt_attributes));
        }
        else if (rt_attributes->rta_type == RTA_PRIORITY)
        {
          current_metric = *((int*)RTA_DATA(rt_attributes));
        }
      }

      // This route is a candidate for the default route if it does not have a destination but does
      // have a gateway.
      if (current_index != 0 && !has_dst && has_gateway)
      {
        // Select the default route with the lowest metric. Sometimes a default route does not have
        // a metric associated with it. In that case, we accept it as a default route as long as a
        // default route does not exist that *does* have a valid metric.
        if (current_metric < lowest_metric || lowest_metric == INT_MAX)
        {
          lowest_metric     = current_metric;
          default_interface = current_index;
        }
      }
    }
  }

  return default_interface;
}

bool unix_interface_name_exists(const char* name, UnixInterfaceName* begin, UnixInterfaceName* end)
{
  for (UnixInterfaceName* cur_name = begin; cur_name < end; ++cur_name)
  {
    if (strcmp(cur_name->name, name) == 0)
    {
      return true;
    }
  }
  return false;
}
