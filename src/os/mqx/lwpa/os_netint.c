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

#include <mqx.h>
#include <bsp.h>
#include <rtcs.h>

#include "lwpa/int.h"
#include "lwpa/private/netint.h"

static LwpaNetintInfo netints[BSP_ENET_DEVICE_COUNT];

static void copy_interface_info(uint32_t mqx_index, LwpaNetintInfo* netint)
{
  IPCFG_IP_ADDRESS_DATA ip_data;

  // This module is quick and dirty and needs some work.
  // - We are ignoring IPv6.
  // - We don't check for loopback interfaces or invalid data.

  // In MQX, interfaces are kept in a linked list and retrieved using an index from 0.
  // To provide an RFC-3493-compliant index, we'll just offset that index by 1.
  netint->index = mqx_index + 1;

  ipcfg_get_ip(mqx_index, &ip_data);
  LWPA_IP_SET_V4_ADDRESS(&netint->addr, ip_data.ip);
  LWPA_IP_SET_V4_ADDRESS(&netint->mask, ip_data.mask);

  ipcfg_get_mac(mqx_index, netint->mac);
  sprintf(netint->name, "en%d", mqx_index);
  sprintf(netint->friendly_name, "en%d", mqx_index);
  if (mqx_index == BSP_DEFAULT_ENET_DEVICE)
    netint->is_default = true;
  else
    netint->is_default = false;
}

lwpa_error_t os_enumerate_interfaces(CachedNetintInfo* cache)
{
  for (uint32_t i = 0; i < BSP_ENET_DEVICE_COUNT; ++i)
  {
    copy_interface_info(i, &netints[i]);
  }

  cache->netints = netints;
  cache->num_netints = BSP_ENET_DEVICE_COUNT;

  cache->def.v4_valid = true;
  cache->def.v4_index = BSP_DEFAULT_ENET_DEVICE + 1;

  return kLwpaErrOk;
}

void os_free_interfaces(CachedNetintInfo* cache)
{
  cache->netints = NULL;
}

lwpa_error_t os_resolve_route(const LwpaIpAddr* dest, const CachedNetintInfo* cache, unsigned int* index)
{
  (void)cache;  // unused

  unsigned int index_found = 0;

  for (const LwpaNetintInfo* netint = netints; netint < netints + BSP_ENET_DEVICE_COUNT; ++netint)
  {
    if (!lwpa_ip_is_wildcard(&netint->mask) && lwpa_ip_network_portions_equal(&netint->addr, dest, &netint->mask))
    {
      index_found = netint->index;
      break;
    }
  }
  if (index_found == 0)
    index_found = netints[BSP_DEFAULT_ENET_DEVICE].index;

  *index = index_found;
  return kLwpaErrOk;
}
