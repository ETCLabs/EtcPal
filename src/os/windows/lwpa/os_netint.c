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

#include <WinSock2.h>
#include <Windows.h>
#include <Ws2tcpip.h>
#include <iphlpapi.h>

#include "lwpa/int.h"
#include "lwpa/private/netint.h"

/**************************** Private variables ******************************/

static struct LwpaNetintState
{
  bool initialized;

  size_t num_netints;
  LwpaNetintInfo* lwpa_netints;

  bool have_default_netint_index_v4;
  size_t default_netint_index_v4;

  bool have_default_netint_index_v6;
  size_t default_netint_index_v6;
} state;

/*********************** Private function prototypes *************************/

static IP_ADAPTER_ADDRESSES* get_windows_adapters();
static void copy_ipv4_info(IP_ADAPTER_UNICAST_ADDRESS* pip, LwpaNetintInfo* info);
static void copy_ipv6_info(IP_ADAPTER_UNICAST_ADDRESS* pip, LwpaNetintInfo* info);
static void copy_all_netint_info(IP_ADAPTER_ADDRESSES* adapters);

static lwpa_error_t enumerate_netints();
static int compare_netints(const void* a, const void* b);
static void free_netints();

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

  const LwpaNetintInfo* res = NULL;
  const LwpaNetintInfo* def = NULL;

  for (const LwpaNetintInfo* netint_entry = state.lwpa_netints; netint_entry < state.lwpa_netints + state.num_netints;
       ++netint_entry)
  {
    if (netint_entry->is_default)
      def = netint_entry;
    if (!lwpa_ip_is_wildcard(&netint_entry->mask) &&
        lwpa_ip_network_portions_equal(&netint_entry->addr, dest, &netint_entry->mask))
    {
      res = netint_entry;
      break;
    }
  }
  if (!res)
    res = def;

  if (res)
  {
    *netint = *res;
    return kLwpaErrOk;
  }
  else
  {
    return kLwpaErrNotFound;
  }
}

lwpa_error_t enumerate_netints()
{
  IP_ADAPTER_ADDRESSES *padapters, *pcur;

  padapters = get_windows_adapters();
  if (!padapters)
    return kLwpaErrSys;
  pcur = padapters;

  state.num_netints = 0;
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
          ++state.num_netints;
          break;
        default:
          break;
      }
      pip = pip->Next;
    }
    pcur = pcur->Next;
  }

  if (state.num_netints == 0)
  {
    free(padapters);
    return kLwpaErrNoNetints;
  }

  state.lwpa_netints = calloc(state.num_netints, sizeof(LwpaNetintInfo));
  if (!state.lwpa_netints)
  {
    free(padapters);
    return kLwpaErrNoMem;
  }

  copy_all_netint_info(padapters);
  free(padapters);

  // Sort the interfaces by OS index
  qsort(state.lwpa_netints, state.num_netints, sizeof(LwpaNetintInfo), compare_netints);

  // Store the locations of the default netints for access by the API function
  for (size_t i = 0; i < state.num_netints; ++i)
  {
    LwpaNetintInfo* netint = &state.lwpa_netints[i];
    if (LWPA_IP_IS_V4(&netint->addr) && netint->is_default)
    {
      state.have_default_netint_index_v4 = true;
      state.default_netint_index_v4 = i;
    }
    else if (LWPA_IP_IS_V6(&netint->addr) && netint->is_default)
    {
      state.have_default_netint_index_v6 = true;
      state.default_netint_index_v6 = i;
    }
  }

  return kLwpaErrOk;
}

int compare_netints(const void* a, const void* b)
{
  LwpaNetintInfo* netint1 = (LwpaNetintInfo*)a;
  LwpaNetintInfo* netint2 = (LwpaNetintInfo*)b;

  return (netint1->ifindex > netint2->ifindex) - (netint1->ifindex < netint2->ifindex);
}

void free_netints()
{
  if (state.lwpa_netints)
  {
    free(state.lwpa_netints);
    state.lwpa_netints = NULL;
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

void copy_ipv4_info(IP_ADAPTER_UNICAST_ADDRESS* pip, LwpaNetintInfo* info)
{
  struct sockaddr_in* sin = (struct sockaddr_in*)pip->Address.lpSockaddr;

  LWPA_IP_SET_V4_ADDRESS(&info->addr, ntohl(sin->sin_addr.s_addr));
  info->mask = lwpa_ip_mask_from_length(kLwpaIpTypeV4, pip->OnLinkPrefixLength);
}

void copy_ipv6_info(IP_ADAPTER_UNICAST_ADDRESS *pip, LwpaNetintInfo *info)
{
  struct sockaddr_in6* sin6 = (struct sockaddr_in6*)pip->Address.lpSockaddr;

  LWPA_IP_SET_V6_ADDRESS(&info->addr, sin6->sin6_addr.s6_addr);
  info->mask = lwpa_ip_mask_from_length(kLwpaIpTypeV6, pip->OnLinkPrefixLength);
}

void copy_all_netint_info(IP_ADAPTER_ADDRESSES* adapters)
{
  IP_ADAPTER_ADDRESSES* pcur = adapters;

  // Get the index of the default interface for IPv4
  DWORD def_ifindex_v4;
  bool have_def_index_v4 = false;
  struct sockaddr_in v4_dest;
  memset(&v4_dest, 0, sizeof v4_dest);
  v4_dest.sin_family = AF_INET;
  if (NO_ERROR == GetBestInterfaceEx((struct sockaddr *)&v4_dest, &def_ifindex_v4))
    have_def_index_v4 = true;

  // And the same for IPv6
  DWORD def_ifindex_v6;
  bool have_def_index_v6 = false;
  struct sockaddr_in6 v6_dest;
  memset(&v6_dest, 0, sizeof v6_dest);
  v6_dest.sin6_family = AF_INET6;
  if (NO_ERROR == GetBestInterfaceEx((struct sockaddr *)&v6_dest, &def_ifindex_v6))
    have_def_index_v6 = true;

  size_t netint_index = 0;

  while (pcur)
  {
    // If this is multihomed, there may be multiple addresses under the same adapter
    IP_ADAPTER_UNICAST_ADDRESS* pip = pcur->FirstUnicastAddress;
    while (pip)
    {
      LwpaNetintInfo* info = &state.lwpa_netints[netint_index];
      switch (pip->Address.lpSockaddr->sa_family)
      {
        case AF_INET:
          copy_ipv4_info(pip, info);
          if (have_def_index_v4 && pcur->IfIndex == def_ifindex_v4)
          {
            info->is_default = true;
            have_def_index_v4 = false;
          }
          else
          {
            info->is_default = false;
          }
          info->ifindex = pcur->IfIndex;
          break;
        case AF_INET6:
          copy_ipv6_info(pip, info);
          if (have_def_index_v6 && pcur->IfIndex == def_ifindex_v6)
          {
            info->is_default = true;
            have_def_index_v6 = false;
          }
          else
          {
            info->is_default = false;
          }
          info->ifindex = pcur->Ipv6IfIndex;
          break;
        default:
          pip = pip->Next;
          continue;
      }

      strncpy_s(info->name, LWPA_NETINTINFO_NAME_LEN, pcur->AdapterName, _TRUNCATE);
      if (pcur->PhysicalAddressLength == LWPA_NETINTINFO_MAC_LEN)
        memcpy(info->mac, pcur->PhysicalAddress, LWPA_NETINTINFO_MAC_LEN);
      else
        memset(info->mac, 0, LWPA_NETINTINFO_MAC_LEN);

      ++netint_index;

      pip = pip->Next;
    }
    pcur = pcur->Next;
  }
}
