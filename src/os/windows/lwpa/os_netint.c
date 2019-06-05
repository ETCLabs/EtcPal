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
  bool have_default_netint_index;
  size_t default_netint_index;
} state;

/*********************** Private function prototypes *************************/

static IP_ADAPTER_ADDRESSES* get_windows_adapters();
static void copy_ipv4_info(IP_ADAPTER_UNICAST_ADDRESS* pip, IP_ADAPTER_GATEWAY_ADDRESS* pgate, LwpaNetintInfo* info);
static void copy_all_netint_info(IP_ADAPTER_ADDRESSES *adapters);
static lwpa_error_t enumerate_netints();
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

bool lwpa_netint_get_default_interface(LwpaNetintInfo* netint)
{
  if (netint && state.have_default_netint_index)
  {
    *netint = state.lwpa_netints[state.default_netint_index];
    return true;
  }
  return false;
}

const LwpaNetintInfo* lwpa_netint_get_iface_for_dest(const LwpaIpAddr* dest, const LwpaNetintInfo* netint_arr,
                                                     size_t netint_arr_size)
{
  const LwpaNetintInfo* res = NULL;
  const LwpaNetintInfo* def = NULL;
  const LwpaNetintInfo* netint;

  if (!dest || !netint_arr || netint_arr_size == 0)
    return false;

  for (netint = netint_arr; netint < netint_arr + netint_arr_size; ++netint)
  {
    if (netint->is_default)
      def = netint;
    if (!lwpa_ip_is_wildcard(&netint->mask) && lwpa_ip_network_portions_equal(&netint->addr, dest, &netint->mask))
    {
      res = netint;
      break;
    }
  }
  if (!res)
    res = def;
  return res;
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
          ++state.num_netints;
          break;
        case AF_INET6:
        default:
          // TODO IPv6
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
  return kLwpaErrOk;
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
  ULONG flags = GAA_FLAG_SKIP_ANYCAST | GAA_FLAG_SKIP_MULTICAST | GAA_FLAG_SKIP_DNS_SERVER | GAA_FLAG_INCLUDE_GATEWAYS;

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

void copy_ipv4_info(IP_ADAPTER_UNICAST_ADDRESS* pip, IP_ADAPTER_GATEWAY_ADDRESS* pgate, LwpaNetintInfo* info)
{
  struct sockaddr_in* sin = (struct sockaddr_in*)pip->Address.lpSockaddr;
  struct sockaddr_in* gw_sin = pgate ? (struct sockaddr_in*)pgate->Address.lpSockaddr : NULL;
  ULONG mask = 0;

  ConvertLengthToIpv4Mask(pip->OnLinkPrefixLength, &mask);
  LWPA_IP_SET_V4_ADDRESS(&info->addr, ntohl(sin->sin_addr.s_addr));
  LWPA_IP_SET_V4_ADDRESS(&info->mask, ntohl((uint32_t)mask));
  if (pgate)
    LWPA_IP_SET_V4_ADDRESS(&info->gate, ntohl(gw_sin->sin_addr.s_addr));
  else
    LWPA_IP_SET_V4_ADDRESS(&info->gate, 0u);
}

void copy_all_netint_info(IP_ADAPTER_ADDRESSES *adapters)
{
  IP_ADAPTER_ADDRESSES *pcur = adapters;
  DWORD def_ifindex;
  bool have_def_index = false;
  if (NO_ERROR == GetBestInterface(0, &def_ifindex))
    have_def_index = true;
  size_t netint_index = 0;

  while (pcur)
  {
    // If this is multihomed, there may be multiple addresses under the same adapter
    IP_ADAPTER_UNICAST_ADDRESS* pip = pcur->FirstUnicastAddress;
    IP_ADAPTER_GATEWAY_ADDRESS* pgate = pcur->FirstGatewayAddress;
    while (pip)
    {
      LwpaNetintInfo* info = &state.lwpa_netints[netint_index];
      switch (pip->Address.lpSockaddr->sa_family)
      {
        case AF_INET:
          copy_ipv4_info(pip, pgate, info);
          break;
        case AF_INET6:
        default:
          /* TODO IPv6 */
          pip = pip->Next;
          if (pgate && pgate->Next)
            pgate = pgate->Next;
          continue;
      }

      info->ifindex = pcur->IfIndex;
      if (have_def_index && pcur->IfIndex == def_ifindex)
      {
        info->is_default = true;
        state.have_default_netint_index = true;
        state.default_netint_index = netint_index;
        have_def_index = false;
      }
      else
      {
        info->is_default = false;
      }
      strncpy_s(info->name, LWPA_NETINTINFO_NAME_LEN, pcur->AdapterName, _TRUNCATE);
      if (pcur->PhysicalAddressLength == LWPA_NETINTINFO_MAC_LEN)
        memcpy(info->mac, pcur->PhysicalAddress, LWPA_NETINTINFO_MAC_LEN);
      else
        memset(info->mac, 0, LWPA_NETINTINFO_MAC_LEN);

      ++netint_index;

      pip = pip->Next;
      // Just in case there's only ever one gateway but more addrs (probably not possible)
      if (pgate && pgate->Next)
        pgate = pgate->Next;
    }
    pcur = pcur->Next;
  }
}
