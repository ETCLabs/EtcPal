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

#include "etcpal/netint.h"

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include <WinSock2.h>
#include <Windows.h>
#include <Ws2tcpip.h>
#include <iphlpapi.h>

#include "etcpal/common.h"
#include "etcpal/socket.h"
#include "etcpal/private/netint.h"
#include "etcpal/private/util.h"
#include "os_error.h"

#ifdef _MSC_VER
#pragma warning(disable : 4996)
#endif

/****************************** Private types ********************************/

/*********************** Private function prototypes *************************/

static IP_ADAPTER_ADDRESSES* get_windows_adapters();
static void                  calculate_size_needed(const IP_ADAPTER_ADDRESSES* padapters,
                                                   const uint8_t*              buf_addr,
                                                   unsigned int                index,
                                                   NetintArraySizes*           sizes);
static void copy_all_netint_info(const IP_ADAPTER_ADDRESSES* adapters, uint8_t* buf, const NetintArraySizes* sizes);
static void copy_single_netint_info(const IP_ADAPTER_ADDRESSES* adapter,
                                    EtcPalNetintInfo*           info,
                                    EtcPalNetintAddr**          addr_ptr,
                                    char**                      name_ptr,
                                    uint8_t*                    buf_end,
                                    unsigned int                def_ifindex_v4,
                                    unsigned int                def_ifindex_v6);
static void copy_ip_address_info(const IP_ADAPTER_ADDRESSES* adapter,
                                 EtcPalNetintInfo*           info,
                                 EtcPalNetintAddr**          addr_ptr);
static void copy_ipv4_info(const IP_ADAPTER_UNICAST_ADDRESS* pip, EtcPalNetintAddr* addr);
static void copy_ipv6_info(const IP_ADAPTER_UNICAST_ADDRESS* pip, EtcPalNetintAddr* addr);
static unsigned int get_default_interface_index(etcpal_iptype_t ip_type);

/*************************** Function definitions ****************************/

///////////////////////////////////////////////////////////////////////////////////////////////////
// API functions
///////////////////////////////////////////////////////////////////////////////////////////////////

size_t etcpal_netint_get_num_interfaces(void)
{
  IP_ADAPTER_ADDRESSES* padapters = get_windows_adapters();
  if (!padapters)
    return 0;

  size_t                num_interfaces = 0;
  IP_ADAPTER_ADDRESSES* pcur           = padapters;
  while (pcur)
  {
    ++num_interfaces;
    pcur = pcur->Next;
  }

  free(padapters);
  return num_interfaces;
}

etcpal_error_t etcpal_netint_get_interfaces(uint8_t* buf, size_t* buf_size)
{
  if (!buf || !buf_size)
    return kEtcPalErrInvalid;

  IP_ADAPTER_ADDRESSES* adapters = get_windows_adapters();
  if (!adapters)
    return kEtcPalErrSys;

  NetintArraySizes sizes = {0};
  calculate_size_needed(adapters, buf, 0, &sizes);
  if (sizes.total_size > *buf_size)
  {
    *buf_size = sizes.total_size;
    free(adapters);
    return kEtcPalErrBufSize;
  }

  copy_all_netint_info(adapters, buf, &sizes);
  *buf_size = sizes.total_size;
  free(adapters);
  return kEtcPalErrOk;
}

etcpal_error_t etcpal_netint_get_interface_by_index(unsigned int index, uint8_t* buf, size_t* buf_size)
{
  if (!buf || !buf_size)
    return kEtcPalErrInvalid;

  IP_ADAPTER_ADDRESSES* adapters = get_windows_adapters();
  if (!adapters)
    return kEtcPalErrSys;

  NetintArraySizes sizes = {0};
  calculate_size_needed(adapters, buf, index, &sizes);
  if (sizes.total_size > *buf_size)
  {
    *buf_size = sizes.total_size;
    free(adapters);
    return kEtcPalErrBufSize;
  }

  EtcPalNetintInfo* info         = (EtcPalNetintInfo*)buf;
  EtcPalNetintAddr* cur_ip       = (EtcPalNetintAddr*)&buf[sizes.ip_addrs_offset];
  char*             cur_name_ptr = (char*)&buf[sizes.names_offset];

  for (IP_ADAPTER_ADDRESSES* pcur = adapters; pcur; pcur = pcur->Next)
  {
    if (pcur->IfIndex == index)
    {
      memset(info, 0, sizeof(EtcPalNetintInfo));
      copy_single_netint_info(pcur, info, &cur_ip, &cur_name_ptr, &buf[sizes.total_size],
                              get_default_interface_index(kEtcPalIpTypeV4),
                              get_default_interface_index(kEtcPalIpTypeV6));
      *buf_size = sizes.total_size;
      free(adapters);
      return kEtcPalErrOk;
    }
  }

  free(adapters);
  return kEtcPalErrNotFound;
}

etcpal_error_t etcpal_netint_get_default_interface_index(etcpal_iptype_t type, unsigned int* netint_index)
{
  if (!netint_index)
    return kEtcPalErrInvalid;

  unsigned int def_index = get_default_interface_index(type);
  if (def_index == 0)
    return kEtcPalErrNotFound;

  *netint_index = def_index;
  return kEtcPalErrOk;
}

etcpal_error_t etcpal_netint_get_default_interface(etcpal_iptype_t type, uint8_t* buf, size_t* buf_size)
{
  if (!buf || !buf_size)
    return kEtcPalErrInvalid;

  unsigned int def_index = get_default_interface_index(type);
  if (def_index == 0)
    return kEtcPalErrNotFound;

  return etcpal_netint_get_interface_by_index(def_index, buf, buf_size);
}

bool etcpal_netint_is_up(unsigned int netint_index)
{
  // Note: Unless I'm mistaken, Windows (at least through the GetAdaptersAddresses API) doesn't
  // appear to have the notion of a network interface being "down". It is either up, or it doesn't
  // exist. So this just checks if a network interface exists with the given index.
  IP_ADAPTER_ADDRESSES* adapters = get_windows_adapters();
  if (!adapters)
    return false;

  for (IP_ADAPTER_ADDRESSES* pcur = adapters; pcur; pcur = pcur->Next)
  {
    if (pcur->IfIndex == netint_index)
    {
      free(adapters);
      return true;
    }
  }

  free(adapters);
  return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Internal functions
///////////////////////////////////////////////////////////////////////////////////////////////////

IP_ADAPTER_ADDRESSES* get_windows_adapters()
{
  ULONG buflen = 0;
  ULONG flags  = GAA_FLAG_SKIP_ANYCAST | GAA_FLAG_SKIP_MULTICAST | GAA_FLAG_SKIP_DNS_SERVER;

  // Preallocating a buffer specifically this size is expressly recommended by the Microsoft usage
  // page.
  uint8_t* buffer = malloc(15000);
  if (!buffer)
    return NULL;

  DWORD result = GetAdaptersAddresses(AF_UNSPEC, flags, NULL, (IP_ADAPTER_ADDRESSES*)buffer, &buflen);
  if (result == ERROR_BUFFER_OVERFLOW)
  {
    free(buffer);
    buffer = malloc(buflen);
    if (!buffer)
      return NULL;
    result = GetAdaptersAddresses(AF_UNSPEC, flags, NULL, (IP_ADAPTER_ADDRESSES*)buffer, &buflen);
  }

  if (result == NO_ERROR)
  {
    return (IP_ADAPTER_ADDRESSES*)buffer;
  }
  free(buffer);
  return NULL;
}

// index == 0: Calculate size needed for all network interfaces on the system.
// index != 0: Calculate size needed for only that interface index.
void calculate_size_needed(const IP_ADAPTER_ADDRESSES* padapters,
                           const uint8_t*              buf_addr,
                           unsigned int                index,
                           NetintArraySizes*           sizes)
{
  size_t size_needed         = 0;
  size_t netintinfo_arr_size = 0;
  size_t ip_addr_arr_size    = 0;

  const IP_ADAPTER_ADDRESSES* pcur = padapters;

  while (pcur)
  {
    if (index == 0 || index == pcur->IfIndex)
    {
      size_needed += sizeof(EtcPalNetintInfo);
      netintinfo_arr_size += sizeof(EtcPalNetintInfo);
      size_needed += strlen(pcur->AdapterName) + 1;
      size_needed += WideCharToMultiByte(CP_UTF8, 0, pcur->FriendlyName, -1, NULL, 0, NULL, NULL);

      // If this is multihomed, there may be multiple addresses under the same adapter
      IP_ADAPTER_UNICAST_ADDRESS* pip = pcur->FirstUnicastAddress;
      while (pip)
      {
        switch (pip->Address.lpSockaddr->sa_family)
        {
          case AF_INET:
          case AF_INET6:
            size_needed += sizeof(EtcPalNetintAddr);
            ip_addr_arr_size += sizeof(EtcPalNetintAddr);
            break;
          default:
            break;
        }
        pip = pip->Next;
      }
    }

    pcur = pcur->Next;
  }

  size_t padding = GET_PADDING_FOR_ALIGNMENT(buf_addr, EtcPalNetintInfo);
  size_needed += padding;
  sizes->total_size      = size_needed;
  sizes->ip_addrs_offset = netintinfo_arr_size + padding;
  sizes->names_offset    = netintinfo_arr_size + padding + ip_addr_arr_size;
}

// This function does not have bounds checking; calculate_size_needed() must be called first to determine whether it is
// safe to call this function on a buffer.
void copy_all_netint_info(const IP_ADAPTER_ADDRESSES* adapters, uint8_t* buf, const NetintArraySizes* sizes)
{
  const IP_ADAPTER_ADDRESSES* pcur = adapters;

  // Get the indexes of the default interfaces
  unsigned int def_ifindex_v4 = get_default_interface_index(kEtcPalIpTypeV4);
  unsigned int def_ifindex_v6 = get_default_interface_index(kEtcPalIpTypeV6);

  EtcPalNetintInfo* cur_info     = (EtcPalNetintInfo*)buf;
  EtcPalNetintInfo* last_info    = NULL;
  EtcPalNetintAddr* cur_ip       = (EtcPalNetintAddr*)&buf[sizes->ip_addrs_offset];
  char*             cur_name_ptr = (char*)&buf[sizes->names_offset];

  while (pcur)
  {
    memset(cur_info, 0, sizeof(EtcPalNetintInfo));
    if (last_info)
    {
      last_info->next = cur_info;
      ++last_info;
    }
    else
    {
      last_info = cur_info;
    }

    copy_single_netint_info(pcur, cur_info, &cur_ip, &cur_name_ptr, &buf[sizes->total_size], def_ifindex_v4,
                            def_ifindex_v6);

    ++cur_info;
    pcur = pcur->Next;
  }
}

// This function does not have bounds checking; calculate_size_needed() must be called first to determine whether it is
// safe to call this function on a buffer.
void copy_single_netint_info(const IP_ADAPTER_ADDRESSES* adapter,
                             EtcPalNetintInfo*           info,
                             EtcPalNetintAddr**          addr_ptr,
                             char**                      name_ptr,
                             uint8_t*                    buf_end,
                             unsigned int                def_ifindex_v4,
                             unsigned int                def_ifindex_v6)
{
  if (def_ifindex_v4 != 0 && adapter->IfIndex == def_ifindex_v4)
  {
    info->flags |= ETCPAL_NETINT_FLAG_DEFAULT_V4;
  }

  if (def_ifindex_v6 != 0 && adapter->Ipv6IfIndex == def_ifindex_v6)
  {
    info->flags |= ETCPAL_NETINT_FLAG_DEFAULT_V6;
  }

  copy_ip_address_info(adapter, info, addr_ptr);

  info->index = adapter->IfIndex;

  strcpy_s(*name_ptr, (char*)buf_end - *name_ptr, adapter->AdapterName);
  info->id = *name_ptr;
  *name_ptr += strlen(adapter->AdapterName) + 1;

  // The friendly name requires special handling because it must be converted to UTF-8
  int convert_res     = WideCharToMultiByte(CP_UTF8, 0, adapter->FriendlyName, -1, *name_ptr,
                                            (int)((char*)buf_end - *name_ptr), NULL, NULL);
  info->friendly_name = *name_ptr;
  if (convert_res > 0)
    *name_ptr += convert_res;
  else
  {
    **name_ptr = '\0';
    ++(*name_ptr);
  }

  if (adapter->PhysicalAddressLength == ETCPAL_MAC_BYTES)
    memcpy(info->mac.data, adapter->PhysicalAddress, ETCPAL_MAC_BYTES);
  else
    memset(info->mac.data, 0, ETCPAL_MAC_BYTES);
}

void copy_ip_address_info(const IP_ADAPTER_ADDRESSES* adapter, EtcPalNetintInfo* info, EtcPalNetintAddr** addr_ptr)
{
  // If this is multihomed, there may be multiple addresses under the same adapter
  const IP_ADAPTER_UNICAST_ADDRESS* pip = adapter->FirstUnicastAddress;

  EtcPalNetintAddr* last_ip = NULL;
  while (pip)
  {
    switch (pip->Address.lpSockaddr->sa_family)
    {
      case AF_INET:
        copy_ipv4_info(pip, *addr_ptr);
        break;
      case AF_INET6:
        copy_ipv6_info(pip, *addr_ptr);
        break;
      default:
        pip = pip->Next;
        continue;
    }

    // Will only get here if we copied a valid IP address (otherwise 'continue' is hit above)
    if (last_ip)
    {
      last_ip->next = *addr_ptr;
      ++last_ip;
    }
    else
    {
      last_ip = *addr_ptr;
    }
    (*addr_ptr)->next = NULL;
    if (!info->addrs)
      info->addrs = *addr_ptr;
    ++(*addr_ptr);
    pip = pip->Next;
  }
}

void copy_ipv4_info(const IP_ADAPTER_UNICAST_ADDRESS* pip, EtcPalNetintAddr* addr)
{
  const struct sockaddr_in* sin = (const struct sockaddr_in*)pip->Address.lpSockaddr;

  ETCPAL_IP_SET_V4_ADDRESS(&addr->addr, ntohl(sin->sin_addr.s_addr));
  addr->mask_length = pip->OnLinkPrefixLength;
}

void copy_ipv6_info(const IP_ADAPTER_UNICAST_ADDRESS* pip, EtcPalNetintAddr* addr)
{
  const struct sockaddr_in6* sin6 = (const struct sockaddr_in6*)pip->Address.lpSockaddr;

  ETCPAL_IP_SET_V6_ADDRESS(&addr->addr, sin6->sin6_addr.s6_addr);
  addr->mask_length = pip->OnLinkPrefixLength;
}

unsigned int get_default_interface_index(etcpal_iptype_t ip_type)
{
  DWORD def_index = 0;

  if (ip_type == kEtcPalIpTypeV4)
  {
    struct sockaddr_in dest;
    memset(&dest, 0, sizeof dest);
    dest.sin_family = AF_INET;
    GetBestInterfaceEx((struct sockaddr*)&dest, &def_index);
  }
  else if (ip_type == kEtcPalIpTypeV6)
  {
    struct sockaddr_in6 dest;
    memset(&dest, 0, sizeof dest);
    dest.sin6_family = AF_INET6;
    GetBestInterfaceEx((struct sockaddr*)&dest, &def_index);
  }

  return def_index;
}
