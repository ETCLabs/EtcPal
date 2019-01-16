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
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include "lwpa/int.h"

static bool mask_is_empty(const LwpaIpAddr *mask);
static bool mask_compare(const LwpaIpAddr *ip1, const LwpaIpAddr *ip2, const LwpaIpAddr *mask);

static IP_ADAPTER_ADDRESSES *get_windows_adapters()
{
  ULONG buflen = 0;
  ULONG flags = GAA_FLAG_SKIP_ANYCAST | GAA_FLAG_SKIP_MULTICAST | GAA_FLAG_SKIP_DNS_SERVER | GAA_FLAG_INCLUDE_GATEWAYS;

  /* Preallocating a buffer specifically this size is expressly recommended by the Microsoft usage
   * page. */
  uint8_t *buffer = malloc(15000);
  if (!buffer)
    return NULL;

  if (ERROR_BUFFER_OVERFLOW == GetAdaptersAddresses(AF_UNSPEC, flags, NULL, (IP_ADAPTER_ADDRESSES *)buffer, &buflen))
  {
    free(buffer);
    buffer = malloc(buflen);
    if (buffer &&
        (ERROR_SUCCESS == GetAdaptersAddresses(AF_UNSPEC, flags, NULL, (IP_ADAPTER_ADDRESSES *)buffer, &buflen)))
    {
      return (IP_ADAPTER_ADDRESSES *)buffer;
    }
  }
  return NULL;
}

static void copy_ipv4_info(IP_ADAPTER_UNICAST_ADDRESS *pip, IP_ADAPTER_GATEWAY_ADDRESS *pgate, LwpaNetintInfo *info)
{
  struct sockaddr_in *sin = (struct sockaddr_in *)pip->Address.lpSockaddr;
  struct sockaddr_in *gw_sin = pgate ? (struct sockaddr_in *)pgate->Address.lpSockaddr : NULL;
  ULONG mask = 0;

  ConvertLengthToIpv4Mask(pip->OnLinkPrefixLength, &mask);
  lwpaip_set_v4_address(&info->addr, ntohl(sin->sin_addr.s_addr));
  lwpaip_set_v4_address(&info->mask, ntohl((uint32_t)mask));
  if (pgate)
    lwpaip_set_v4_address(&info->gate, ntohl(gw_sin->sin_addr.s_addr));
  else
    lwpaip_set_v4_address(&info->gate, 0u);
}

size_t lwpa_netint_get_num_interfaces()
{
  IP_ADAPTER_ADDRESSES *padapters, *pcur;
  size_t n_ifaces = 0;

  padapters = get_windows_adapters();
  if (!padapters)
    return 0;
  pcur = padapters;

  while (pcur)
  {
    /* If this is multihomed, there may be multiple addresses under the same adapter */
    IP_ADAPTER_UNICAST_ADDRESS *pip = pcur->FirstUnicastAddress;
    while (pip)
    {
      switch (pip->Address.lpSockaddr->sa_family)
      {
        case AF_INET:
          ++n_ifaces;
          break;
        case AF_INET6:
        default:
          /* TODO IPv6 */
          break;
      }
      pip = pip->Next;
    }
    pcur = pcur->Next;
  }
  free(padapters);
  return n_ifaces;
}

size_t lwpa_netint_get_interfaces(LwpaNetintInfo *netint_arr, size_t netint_arr_size)
{
  size_t n_ifaces = 0;
  IP_ADAPTER_ADDRESSES *padapters, *pcur;
  DWORD def_ifindex;
  bool have_def_index = false;

  if (!netint_arr || netint_arr_size == 0)
    return 0;

  padapters = get_windows_adapters();
  if (!padapters)
    return 0;
  pcur = padapters;

  if (NO_ERROR == GetBestInterface(0, &def_ifindex))
    have_def_index = true;

  while (pcur)
  {
    /* If this is multihomed, there may be multiple addresses under the same adapter */
    IP_ADAPTER_UNICAST_ADDRESS *pip = pcur->FirstUnicastAddress;
    IP_ADAPTER_GATEWAY_ADDRESS *pgate = pcur->FirstGatewayAddress;
    while (pip)
    {
      LwpaNetintInfo *info = &netint_arr[n_ifaces];
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

      if (++n_ifaces >= netint_arr_size)
        break;

      pip = pip->Next;
      /* Just in case there's only ever one gateway but more addrs (probably not possible) */
      if (pgate && pgate->Next)
        pgate = pgate->Next;
    }
    if (n_ifaces >= netint_arr_size)
      break;
    pcur = pcur->Next;
  }
  free(padapters);
  return n_ifaces;
}

bool lwpa_netint_get_default_interface(LwpaNetintInfo *netint)
{
  DWORD def_ifindex;
  bool res = false;

  if (NO_ERROR == GetBestInterface(0, &def_ifindex))
  {
    IP_ADAPTER_ADDRESSES *pcur;
    IP_ADAPTER_ADDRESSES *padapters = get_windows_adapters();
    if (!padapters)
      return res;
    pcur = padapters;

    while (pcur)
    {
      /* If this is multihomed, there may be multiple addresses under the same adapter */
      IP_ADAPTER_UNICAST_ADDRESS *pip = pcur->FirstUnicastAddress;
      IP_ADAPTER_GATEWAY_ADDRESS *pgate = pcur->FirstGatewayAddress;
      while (pip)
      {
        if (pcur->IfIndex == def_ifindex && pip->Address.lpSockaddr->sa_family == AF_INET)
        {
          copy_ipv4_info(pip, pgate, netint);
          netint->ifindex = def_ifindex;
          netint->is_default = true;
          strncpy_s(netint->name, LWPA_NETINTINFO_NAME_LEN, pcur->AdapterName, _TRUNCATE);
          if (pcur->PhysicalAddressLength == LWPA_NETINTINFO_MAC_LEN)
            memcpy(netint->mac, pcur->PhysicalAddress, LWPA_NETINTINFO_MAC_LEN);
          else
            memset(netint->mac, 0, LWPA_NETINTINFO_MAC_LEN);
          res = true;
          break;
        }
        pip = pip->Next;
        /* Just in case there's only ever one gateway but more addrs (probably not possible) */
        if (pgate && pgate->Next)
          pgate = pgate->Next;
      }
      if (res == true)
        break;
      pcur = pcur->Next;
    }
    free(padapters);
  }
  return res;
}

bool mask_compare(const LwpaIpAddr *ip1, const LwpaIpAddr *ip2, const LwpaIpAddr *mask)
{
  if (lwpaip_is_v4(ip1) && lwpaip_is_v4(ip2) && lwpaip_is_v4(mask))
  {
    return ((lwpaip_v4_address(ip1) & lwpaip_v4_address(mask)) == (lwpaip_v4_address(ip2) & lwpaip_v4_address(mask)));
  }
  else if (lwpaip_is_v6(ip1) && lwpaip_is_v6(ip2) && lwpaip_is_v6(mask))
  {
    size_t i;
    const uint32_t *p1 = (const uint32_t *)lwpaip_v6_address(ip1);
    const uint32_t *p2 = (const uint32_t *)lwpaip_v6_address(ip2);
    const uint32_t *pm = (const uint32_t *)lwpaip_v6_address(mask);

    for (i = 0; i < LWPA_IPV6_BYTES / 4; ++i, ++p1, ++p2, ++pm)
    {
      if ((*p1 & *pm) != (*p2 & *pm))
        return false;
    }
    return true;
  }
  return false;
}

bool mask_is_empty(const LwpaIpAddr *mask)
{
  uint32_t mask_part = 0;

  if (lwpaip_is_v4(mask))
  {
    mask_part = lwpaip_v4_address(mask);
  }
  else if (lwpaip_is_v6(mask))
  {
    size_t i;
    const uint32_t *p = (const uint32_t *)lwpaip_v6_address(mask);
    for (i = 0; i < LWPA_IPV6_BYTES / 4; ++i, ++p)
      mask_part |= *p;
  }
  return (mask_part == 0);
}

const LwpaNetintInfo *lwpa_netint_get_iface_for_dest(const LwpaIpAddr *dest, const LwpaNetintInfo *netint_arr,
                                                     size_t netint_arr_size)
{
  const LwpaNetintInfo *res = NULL;
  const LwpaNetintInfo *def = NULL;
  const LwpaNetintInfo *netint;

  if (!dest || !netint_arr || netint_arr_size == 0)
    return false;

  for (netint = netint_arr; netint < netint_arr + netint_arr_size; ++netint)
  {
    if (netint->is_default)
      def = netint;
    if (!mask_is_empty(&netint->mask) && mask_compare(&netint->addr, dest, &netint->mask))
    {
      res = netint;
      break;
    }
  }
  if (!res)
    res = def;
  return res;
}

// static struct change_cb
//{
//  netint_change_notification fn;
//  void *context;
//  int handle;
//  struct change_cb *next;
//} *change_cb_head;
// int next_handle;

// int
// netint_register_change_cb(netint_change_notification fn, void *context)
//{
//  return -1;
//}

// void netint_unregister_change_cb(int handle)
//{

//}
