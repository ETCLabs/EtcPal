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

#include <WinSock2.h>
#include <Windows.h>
#include <Ws2tcpip.h>
#include <iphlpapi.h>

#include "etcpal/common.h"
#include "etcpal/socket.h"
#include "etcpal/private/common.h"
#include "etcpal/private/netint.h"
#include "os_error.h"

/*********************** Private function prototypes *************************/

static IP_ADAPTER_ADDRESSES* get_windows_adapters();
static void                  copy_ipv4_info(const IP_ADAPTER_UNICAST_ADDRESS* pip, EtcPalNetintInfo* info);
static void                  copy_ipv6_info(const IP_ADAPTER_UNICAST_ADDRESS* pip, EtcPalNetintInfo* info);
static etcpal_error_t        copy_all_netint_info(const IP_ADAPTER_ADDRESSES* adapters, CachedNetintInfo* cache);

/*************************** Function definitions ****************************/

etcpal_error_t os_enumerate_interfaces(CachedNetintInfo* cache)
{
  if (!ETCPAL_ASSERT_VERIFY(cache))
    return kEtcPalErrSys;

  IP_ADAPTER_ADDRESSES* padapters = get_windows_adapters();
  if (!padapters)
    return kEtcPalErrSys;
  IP_ADAPTER_ADDRESSES* pcur = padapters;

  cache->num_netints = 0;
  while (pcur)
  {
    // If this is multihomed, there may be multiple addresses under the same adapter
    IP_ADAPTER_UNICAST_ADDRESS* pip = pcur->FirstUnicastAddress;
    while (pip)
    {
      if (pip->Address.lpSockaddr)
      {
        switch (pip->Address.lpSockaddr->sa_family)
        {
          case AF_INET:
          case AF_INET6:
            ++cache->num_netints;
            break;
          default:
            break;
        }
      }
      pip = pip->Next;
    }
    pcur = pcur->Next;
  }

  if (cache->num_netints == 0)
  {
    free(padapters);
    return kEtcPalErrNoNetints;
  }

  cache->netints = calloc(cache->num_netints, sizeof(EtcPalNetintInfo));
  if (!cache->netints)
  {
    free(padapters);
    cache->num_netints = 0;
    return kEtcPalErrNoMem;
  }

  etcpal_error_t res = copy_all_netint_info(padapters, cache);
  free(padapters);

  if (res != kEtcPalErrOk)
  {
    if (cache->netints)
      free(cache->netints);

    cache->num_netints  = 0;
    cache->def.v4_valid = false;
    cache->def.v6_valid = false;
  }

  return res;
}

void os_free_interfaces(CachedNetintInfo* cache)
{
  if (!ETCPAL_ASSERT_VERIFY(cache))
    return;

  if (cache->netints)
  {
    free(cache->netints);
    cache->netints = NULL;
  }
}

etcpal_error_t os_resolve_route(const EtcPalIpAddr* dest, const CachedNetintInfo* cache, unsigned int* index)
{
  ETCPAL_UNUSED_ARG(cache);  // unused

  if (!ETCPAL_ASSERT_VERIFY(dest) || !ETCPAL_ASSERT_VERIFY(index))
    return kEtcPalErrSys;

  struct sockaddr_storage os_addr = {0};
  if (ip_etcpal_to_os(dest, (etcpal_os_ipaddr_t*)&os_addr))
  {
    DWORD resolved_index = 0;
    DWORD res            = GetBestInterfaceEx((struct sockaddr*)&os_addr, &resolved_index);
    if (res == NO_ERROR)
    {
      *index = resolved_index;
      return kEtcPalErrOk;
    }

    if (res == ERROR_INVALID_PARAMETER)
      return kEtcPalErrInvalid;

    return kEtcPalErrNotFound;
  }

  return kEtcPalErrInvalid;
}

bool os_netint_is_up(unsigned int index, const CachedNetintInfo* cache)
{
  if (!ETCPAL_ASSERT_VERIFY(cache))
    return false;

  // Note: I have not found a way to dynamically get whether an adapter is enabled based on its
  // index. Trial and error shows a lot of false positives, like NT is reserving indexes and giving
  // them names even though they do not correspond to adapters on the system. For this reason, the
  // Windows implementation uses the cached netint list and interprets an index being present as
  // that interface being up.

  for (const EtcPalNetintInfo* netint = cache->netints; netint && (netint < (cache->netints + cache->num_netints));
       ++netint)
  {
    if (netint->index == index)
      return true;
  }

  return false;
}

IP_ADAPTER_ADDRESSES* get_windows_adapters()
{
  ULONG flags = GAA_FLAG_SKIP_ANYCAST | GAA_FLAG_SKIP_MULTICAST | GAA_FLAG_SKIP_DNS_SERVER;

  // Preallocating a buffer specifically this size is expressly recommended by the Microsoft usage
  // page.
  ULONG    buflen = 15000;
  uint8_t* buffer = NULL;
  for (int i = 0; i < 3; ++i)  // Maximum of 3 attempts
  {
    buffer = malloc(buflen);
    if (!buffer)
      break;

    DWORD result = GetAdaptersAddresses(AF_UNSPEC, flags, NULL, (IP_ADAPTER_ADDRESSES*)buffer, &buflen);
    if (result != NO_ERROR)
    {
      free(buffer);
      buffer = NULL;
    }

    if (result != ERROR_BUFFER_OVERFLOW)
      break;
  }

  return (IP_ADAPTER_ADDRESSES*)buffer;
}

void copy_ipv4_info(const IP_ADAPTER_UNICAST_ADDRESS* pip, EtcPalNetintInfo* info)
{
  if (!ETCPAL_ASSERT_VERIFY(pip) || !ETCPAL_ASSERT_VERIFY(pip->Address.lpSockaddr) || !ETCPAL_ASSERT_VERIFY(info))
    return;

  const struct sockaddr_in* sin = (const struct sockaddr_in*)pip->Address.lpSockaddr;

  ETCPAL_IP_SET_V4_ADDRESS(&info->addr, ntohl(sin->sin_addr.s_addr));
  info->mask = etcpal_ip_mask_from_length(kEtcPalIpTypeV4, pip->OnLinkPrefixLength);
}

void copy_ipv6_info(const IP_ADAPTER_UNICAST_ADDRESS* pip, EtcPalNetintInfo* info)
{
  if (!ETCPAL_ASSERT_VERIFY(pip) || !ETCPAL_ASSERT_VERIFY(pip->Address.lpSockaddr) || !ETCPAL_ASSERT_VERIFY(info))
    return;

  const struct sockaddr_in6* sin6 = (const struct sockaddr_in6*)pip->Address.lpSockaddr;

  ETCPAL_IP_SET_V6_ADDRESS(&info->addr, sin6->sin6_addr.s6_addr);
  info->mask = etcpal_ip_mask_from_length(kEtcPalIpTypeV6, pip->OnLinkPrefixLength);
}

etcpal_error_t copy_all_netint_info(const IP_ADAPTER_ADDRESSES* adapters, CachedNetintInfo* cache)
{
  if (!ETCPAL_ASSERT_VERIFY(adapters) || !ETCPAL_ASSERT_VERIFY(cache))
    return kEtcPalErrSys;

  // Get the index of the default interface for IPv4
  DWORD              def_ifindex_v4 = 0;
  struct sockaddr_in v4_dest;
  memset(&v4_dest, 0, sizeof v4_dest);
  v4_dest.sin_family = AF_INET;
  if (NO_ERROR == GetBestInterfaceEx((struct sockaddr*)&v4_dest, &def_ifindex_v4))
  {
    cache->def.v4_valid = true;
    cache->def.v4_index = (unsigned int)def_ifindex_v4;
  }

  // And the same for IPv6
  DWORD               def_ifindex_v6 = 0;
  struct sockaddr_in6 v6_dest;
  memset(&v6_dest, 0, sizeof v6_dest);
  v6_dest.sin6_family = AF_INET6;
  if (NO_ERROR == GetBestInterfaceEx((struct sockaddr*)&v6_dest, &def_ifindex_v6))
  {
    cache->def.v6_valid = true;
    cache->def.v6_index = (unsigned int)def_ifindex_v6;
  }

  size_t                      netint_index = 0;
  const IP_ADAPTER_ADDRESSES* pcur         = adapters;

  while (pcur)
  {
    // If this is multihomed, there may be multiple addresses under the same adapter
    IP_ADAPTER_UNICAST_ADDRESS* pip = pcur->FirstUnicastAddress;
    while (pip)
    {
      if (!ETCPAL_ASSERT_VERIFY(netint_index < cache->num_netints))
        return kEtcPalErrSys;

      EtcPalNetintInfo* info = &cache->netints[netint_index];

      bool adding_to_cache_netints = false;
      if (pip->Address.lpSockaddr)
      {
        switch (pip->Address.lpSockaddr->sa_family)
        {
          case AF_INET:
            adding_to_cache_netints = true;
            copy_ipv4_info(pip, info);
            if (cache->def.v4_valid && (pcur->IfIndex == def_ifindex_v4))
            {
              info->is_default = true;
            }
            else
            {
              info->is_default = false;
            }
            info->index = pcur->IfIndex;
            break;
          case AF_INET6:
            adding_to_cache_netints = true;
            copy_ipv6_info(pip, info);
            if (cache->def.v6_valid && (pcur->IfIndex == def_ifindex_v6))
            {
              info->is_default = true;
            }
            else
            {
              info->is_default = false;
            }
            info->index = pcur->Ipv6IfIndex;
            break;
          default:
            break;
        }
      }

      if (adding_to_cache_netints)
      {
        strncpy_s(info->id, ETCPAL_NETINTINFO_ID_LEN, pcur->AdapterName, _TRUNCATE);

        // The friendly name requires special handling because it must be converted to UTF-8
        memset(info->friendly_name, 0, ETCPAL_NETINTINFO_FRIENDLY_NAME_LEN);
        WideCharToMultiByte(CP_UTF8, 0, pcur->FriendlyName, -1, info->friendly_name,
                            ETCPAL_NETINTINFO_FRIENDLY_NAME_LEN - 1, NULL, NULL);

        if (pcur->PhysicalAddressLength == ETCPAL_MAC_BYTES)
          memcpy(info->mac.data, pcur->PhysicalAddress, ETCPAL_MAC_BYTES);
        else
          memset(info->mac.data, 0, ETCPAL_MAC_BYTES);

        ++netint_index;
      }

      pip = pip->Next;
    }

    pcur = pcur->Next;
  }

  // At this point, the number of netints written should exactly match the amount allocated for.
  ETCPAL_ASSERT_VERIFY(netint_index == cache->num_netints);

  return kEtcPalErrOk;
}
