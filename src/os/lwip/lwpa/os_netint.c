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

#include "lwpa_netint.h"
#include <string.h>
#include "lwip/netif.h"

static bool mask_is_empty(const LwpaIpAddr* mask);
static bool mask_compare(const LwpaIpAddr* ip1, const LwpaIpAddr* ip2, const LwpaIpAddr* mask);

static void copy_interface_info(struct netif* lwip_netint, LwpaNetintInfo* netint)
{
  netint->ifindex = (int)lwip_netint->num;
  if (lwip_netint->hwaddr_len == NETINTINFO_MAC_LEN)
    memcpy(netint->mac, lwip_netint->hwaddr, NETINTINFO_MAC_LEN);
  else
    memset(netint->mac, 0, NETINTINFO_MAC_LEN);
  if (lwip_netint->name[0] != '\0')
  {
    netint->name[0] = lwip_netint->name[0];
    netint->name[1] = lwip_netint->name[1];
    netint->name[2] = (char)(lwip_netint->num + '0');
    netint->name[3] = '\0';
  }
  if (lwip_netint == netif_default)
    netint->is_default = true;
  else
    netint->is_default = false;

#if LWIP_IPV4
  if (!ip_addr_isany(netif_ip_addr4(lwip_netint)))
  {
    LWPA_IP_SET_V4_ADDRESS(&netint->addr, ntohl(netif_ip4_addr(lwip_netint)->addr));
    LWPA_IP_SET_V4_ADDRESS(&netint->mask, ntohl(netif_ip4_netmask(lwip_netint)->addr));
    LWPA_IP_SET_V4_ADDRESS(&netint->gate, ntohl(netif_ip4_gw(lwip_netint)->addr));
  }
#endif
#if 0
  /* TODO port this IPv6 code copied from AsyncSocket */
  /* Add all of the IPv6 addresses that are valid, each gets its own
    * netintinfo */

  /* NOTE: We are not currently supporting IPv6 in any lwIP products, and
    * this implementation will probably need to be revisited before we do,
    * especially the default interface code. */
  for (size_t i = 0; i < LWIP_IPV6_NUM_ADDRESSES; ++i)
  {
    if (ip6_addr_isvalid(iface->ip6_addr_mca_state[i]))
    {
      IAsyncSocketServ::netintinfo v6info = info;
      v6info.addr.SetV6Address(reinterpret_cast<uint8_t*>(
            ip_2_ip6(&iface->ip6_addr[i])->addr));
      v6info.id = m_ifaces.size();
      m_ifaces.push_back(v6info);
      /* This will result in the last valid IPv6 address on the
        * default interface being made the 'default'. Or if there are
        * no IPv6 addresses, the IPv4 address will get it. Good enough
        * for now. */
      if (iface == netif_default)
        m_defaultiface = v6info.id;
    }
  }
#endif
}

size_t netint_get_num_interfaces()
{
  size_t num_interfaces = 0;
  struct netif* netint;
  for (netint = netif_list; netint; netint = netint->next)
  {
    /* Skip loopback interfaces */
    if (!ip4_addr_isloopback(netif_ip4_addr(netint)))
      ++num_interfaces;
  }
  return num_interfaces;
}

size_t netint_get_interfaces(LwpaNetintInfo* netint_arr, size_t netint_arr_size)
{
  size_t num_interfaces = 0;
  struct netif* lwip_netint;

  if (!netint_arr || netint_arr_size == 0)
    return 0;

  for (lwip_netint = netif_list; lwip_netint; lwip_netint = lwip_netint->next)
  {
#if LWIP_IPV4
    /* Skip loopback interfaces */
    if (ip4_addr_isloopback(netif_ip4_addr(lwip_netint)))
      continue;
#endif

    copy_interface_info(lwip_netint, &netint_arr[num_interfaces++]);
    if (num_interfaces >= netint_arr_size)
      break;
  }

  return num_interfaces;
}

bool netint_get_default_interface(LwpaNetintInfo* netint)
{
  if (netint && netif_default)
  {
    copy_interface_info(netif_default, netint);
    return true;
  }
  return false;
}

bool mask_compare(const LwpaIpAddr* ip1, const LwpaIpAddr* ip2, const LwpaIpAddr* mask)
{
  if (LWPA_IP_IS_V4(ip1) && LWPA_IP_IS_V4(ip2) && LWPA_IP_IS_V4(mask))
  {
    return ((LWPA_IP_V4_ADDRESS(ip1) & LWPA_IP_V4_ADDRESS(mask)) == (LWPA_IP_V4_ADDRESS(ip2) & LWPA_IP_V4_ADDRESS(mask)));
  }
  else if (LWPA_IP_IS_V6(ip1) && LWPA_IP_IS_V6(ip2) && LWPA_IP_IS_V6(mask))
  {
    size_t i;
    const uint32_t* p1 = (const uint32_t*)LWPA_IP_V6_ADDRESS(ip1);
    const uint32_t* p2 = (const uint32_t*)LWPA_IP_V6_ADDRESS(ip2);
    const uint32_t* pm = (const uint32_t*)LWPA_IP_V6_ADDRESS(mask);

    for (i = 0; i < IPV6_BYTES / 4; ++i, ++p1, ++p2, ++pm)
    {
      if ((*p1 & *pm) != (*p2 & *pm))
        return false;
    }
    return true;
  }
  return false;
}

bool mask_is_empty(const LwpaIpAddr* mask)
{
  uint32_t mask_part = 0;

  if (LWPA_IP_IS_V4(mask))
    mask_part = LWPA_IP_V4_ADDRESS(mask);
  else if (LWPA_IP_IS_V6(mask))
  {
    size_t i;
    const uint32_t* p = (const uint32_t*)LWPA_IP_V6_ADDRESS(mask);
    for (i = 0; i < IPV6_BYTES / 4; ++i, ++p)
      mask_part |= *p;
  }
  return (mask_part == 0);
}

const LwpaNetintInfo* netint_get_iface_for_dest(const LwpaIpAddr* dest, const LwpaNetintInfo* netint_arr,
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
