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

#include <mqx.h>
#include <bsp.h>
#include <rtcs.h>

#include "lwpa_int.h"

static bool mask_is_empty(const LwpaIpAddr *mask);
static bool mask_compare(const LwpaIpAddr *ip1, const LwpaIpAddr *ip2, const LwpaIpAddr *mask);

static void copy_interface_info(size_t mqx_index, LwpaNetintInfo *netint)
{
  IPCFG_IP_ADDRESS_DATA ip_data;

  /* This module is quick and dirty and needs some work.
   * - We are ignoring IPv6.
   * - We don't check for loopback interfaces or invalid data.
   */
  netint->ifindex = (int)RTCS_if_get_handle(mqx_index);

  ipcfg_get_ip(mqx_index, &ip_data);
  lwpaip_set_v4_address(&netint->addr, ip_data.ip);
  lwpaip_set_v4_address(&netint->mask, ip_data.mask);
  lwpaip_set_v4_address(&netint->gate, ip_data.gateway);

  ipcfg_get_mac(mqx_index, netint->mac);
  sprintf(netint->name, "en%d", mqx_index);
  if (mqx_index == BSP_DEFAULT_ENET_DEVICE)
    netint->is_default = true;
  else
    netint->is_default = false;
}

size_t netint_get_num_interfaces()
{
  return BSP_ENET_DEVICE_COUNT;
}

size_t netint_get_interfaces(LwpaNetintInfo *netint_arr, size_t netint_arr_size)
{
  size_t i;

  if (!netint_arr || netint_arr_size == 0)
    return 0;

  for (i = 0; i < BSP_ENET_DEVICE_COUNT; ++i)
  {
    copy_interface_info(i, &netint_arr[i]);
    if (i >= netint_arr_size)
      break;
  }
  return i;
}

bool netint_get_default_interface(LwpaNetintInfo *netint)
{
  if (netint)
  {
    copy_interface_info(BSP_DEFAULT_ENET_DEVICE, netint);
    return true;
  }
  return false;
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

    for (i = 0; i < IPV6_BYTES / 4; ++i, ++p1, ++p2, ++pm)
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
    mask_part = lwpaip_v4_address(mask);
  else if (lwpaip_is_v6(mask))
  {
    size_t i;
    const uint32_t *p = (const uint32_t *)lwpaip_v6_address(mask);
    for (i = 0; i < IPV6_BYTES / 4; ++i, ++p)
      mask_part |= *p;
  }
  return (mask_part == 0);
}

const LwpaNetintInfo *netint_get_iface_for_dest(const LwpaIpAddr *dest, const LwpaNetintInfo *netint_arr,
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
