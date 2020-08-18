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

#include "etcpal/netint.h"

#include <stdint.h>
#include <stdlib.h>

#include <WinSock2.h>
#include <Windows.h>
#include <Ws2tcpip.h>
#include <iphlpapi.h>

#include "etcpal/common.h"
#include "etcpal/socket.h"
#include "etcpal/private/netint.h"
#include "os_error.h"

/*********************** Private function prototypes *************************/

static IP_ADAPTER_ADDRESSES* get_windows_adapters();
static void                  copy_ipv4_info(const IP_ADAPTER_UNICAST_ADDRESS* pip, EtcPalNetintInfo* info);
static void                  copy_ipv6_info(const IP_ADAPTER_UNICAST_ADDRESS* pip, EtcPalNetintInfo* info);
static void                  copy_all_netint_info(const IP_ADAPTER_ADDRESSES* adapters, CachedNetintInfo* cache);

/*************************** Function definitions ****************************/

etcpal_error_t os_resolve_route(const EtcPalIpAddr* dest, const CachedNetintInfo* cache, unsigned int* index)
{
  ETCPAL_UNUSED_ARG(cache);  // unused

  struct sockaddr_storage os_addr;
  if (ip_etcpal_to_os(dest, (etcpal_os_ipaddr_t*)&os_addr))
  {
    DWORD resolved_index;
    DWORD res = GetBestInterfaceEx((struct sockaddr*)&os_addr, &resolved_index);
    if (res == NO_ERROR)
    {
      *index = resolved_index;
      return kEtcPalErrOk;
    }
    else if (res == ERROR_INVALID_PARAMETER)
    {
      return kEtcPalErrInvalid;
    }
    else
    {
      return kEtcPalErrNotFound;
    }
  }
  else
  {
    return kEtcPalErrInvalid;
  }
}

etcpal_error_t os_enumerate_interfaces(CachedNetintInfo* cache)
{
  IP_ADAPTER_ADDRESSES *padapters, *pcur;

  padapters = get_windows_adapters();
  if (!padapters)
    return kEtcPalErrSys;
  pcur = padapters;

  cache->num_netints = 0;
  while (pcur)
  {
    // If this is multihomed, there may be multiple addresses under the same adapter
    IP_ADAPTER_UNICAST_ADDRESS* pip = pcur->FirstUnicastAddress;
    while (pip)
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
    return kEtcPalErrNoMem;
  }

  copy_all_netint_info(padapters, cache);
  free(padapters);
  return kEtcPalErrOk;
}

void os_free_interfaces(CachedNetintInfo* cache)
{
  if (cache->netints)
  {
    free(cache->netints);
    cache->netints = NULL;
  }
}

IP_ADAPTER_ADDRESSES* get_windows_adapters()
{
  ULONG buflen = 0;
  ULONG flags = GAA_FLAG_SKIP_ANYCAST | GAA_FLAG_SKIP_MULTICAST | GAA_FLAG_SKIP_DNS_SERVER;

  // Preallocating a buffer specifically this size is expressly recommended by the Microsoft usage
  // page.
  uint8_t* buffer = malloc(15000);
  if (!buffer)
    return NULL;

  if (ERROR_BUFFER_OVERFLOW == GetAdaptersAddresses(AF_UNSPEC, flags, NULL, (IP_ADAPTER_ADDRESSES*)buffer, &buflen))
  {
    free(buffer);
    buffer = malloc(buflen);
    if (buffer &&
        (ERROR_SUCCESS == GetAdaptersAddresses(AF_UNSPEC, flags, NULL, (IP_ADAPTER_ADDRESSES*)buffer, &buflen)))
    {
      return (IP_ADAPTER_ADDRESSES*)buffer;
    }
  }
  return NULL;
}

void copy_ipv4_info(const IP_ADAPTER_UNICAST_ADDRESS* pip, EtcPalNetintInfo* info)
{
  const struct sockaddr_in* sin = (const struct sockaddr_in*)pip->Address.lpSockaddr;

  ETCPAL_IP_SET_V4_ADDRESS(&info->addr, ntohl(sin->sin_addr.s_addr));
  info->mask = etcpal_ip_mask_from_length(kEtcPalIpTypeV4, pip->OnLinkPrefixLength);
}

void copy_ipv6_info(const IP_ADAPTER_UNICAST_ADDRESS* pip, EtcPalNetintInfo* info)
{
  const struct sockaddr_in6* sin6 = (const struct sockaddr_in6*)pip->Address.lpSockaddr;

  ETCPAL_IP_SET_V6_ADDRESS(&info->addr, sin6->sin6_addr.s6_addr);
  info->mask = etcpal_ip_mask_from_length(kEtcPalIpTypeV6, pip->OnLinkPrefixLength);
}

void copy_all_netint_info(const IP_ADAPTER_ADDRESSES* adapters, CachedNetintInfo* cache)
{
  const IP_ADAPTER_ADDRESSES* pcur = adapters;

  // Get the index of the default interface for IPv4
  DWORD              def_ifindex_v4;
  struct sockaddr_in v4_dest;
  memset(&v4_dest, 0, sizeof v4_dest);
  v4_dest.sin_family = AF_INET;
  if (NO_ERROR == GetBestInterfaceEx((struct sockaddr*)&v4_dest, &def_ifindex_v4))
  {
    cache->def.v4_valid = true;
    cache->def.v4_index = def_ifindex_v4;
  }

  // And the same for IPv6
  DWORD               def_ifindex_v6;
  struct sockaddr_in6 v6_dest;
  memset(&v6_dest, 0, sizeof v6_dest);
  v6_dest.sin6_family = AF_INET6;
  if (NO_ERROR == GetBestInterfaceEx((struct sockaddr*)&v6_dest, &def_ifindex_v6))
  {
    cache->def.v6_valid = true;
    cache->def.v6_index = def_ifindex_v6;
  }

  size_t netint_index = 0;

  while (pcur)
  {
    // If this is multihomed, there may be multiple addresses under the same adapter
    IP_ADAPTER_UNICAST_ADDRESS* pip = pcur->FirstUnicastAddress;
    while (pip)
    {
      EtcPalNetintInfo* info = &cache->netints[netint_index];
      switch (pip->Address.lpSockaddr->sa_family)
      {
        case AF_INET:
          copy_ipv4_info(pip, info);
          if (cache->def.v4_valid && pcur->IfIndex == def_ifindex_v4)
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
          copy_ipv6_info(pip, info);
          if (cache->def.v6_valid && pcur->IfIndex == def_ifindex_v6)
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
          pip = pip->Next;
          continue;
      }

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

      pip = pip->Next;
    }
    pcur = pcur->Next;
  }
}
