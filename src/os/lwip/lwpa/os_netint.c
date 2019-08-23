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
#include <string.h>
#include <lwip/netif.h>
#include "lwpa/private/opts.h"
#include "lwpa/private/netint.h"

#if LWPA_EMBOS_USE_MALLOC
#include <stdlib.h>
static LwpaNetintInfo* static_netints;
#else
static LwpaNetintInfo static_netints[LWPA_EMBOS_MAX_NETINTS];
#endif
size_t num_static_netints;
size_t default_index;

static void copy_common_interface_info(const struct netif* lwip_netif, LwpaNetintInfo* netint)
{
  netint->index = netif_get_index(lwip_netif);
  memset(netint->mac, 0, LWPA_NETINTINFO_MAC_LEN);
  memcpy(netint->mac, lwip_netif->hwaddr, lwip_netif->hwaddr_len);

  char lwip_name[NETIF_NAMESIZE];
  if (NULL != netif_index_to_name((u8_t)netint->index, lwip_name))
  {
    strncpy(netint->name, lwip_name, LWPA_NETINTINFO_NAME_LEN);
    strncpy(netint->friendly_name, lwip_name, LWPA_NETINTINFO_FRIENDLY_NAME_LEN);
  }
}

static void copy_interface_info_v4(const struct netif* lwip_netif, LwpaNetintInfo* netint)
{
  copy_common_interface_info(lwip_netif, netint);

  if (!ip_addr_isany(netif_ip_addr4(lwip_netif)))
  {
    LWPA_IP_SET_V4_ADDRESS(&netint->addr, ntohl(netif_ip4_addr(lwip_netif)->addr));
    LWPA_IP_SET_V4_ADDRESS(&netint->mask, ntohl(netif_ip4_netmask(lwip_netif)->addr));
  }

  if (lwip_netif == netif_default)
    netint->is_default = true;
  else
    netint->is_default = false;
}

static void copy_interface_info_v6(const struct netif* lwip_netif, size_t v6_addr_index, LwpaNetintInfo* netint)
{
  copy_common_interface_info(lwip_netif, netint);

  /* Finish filling in a netintinfo for a single IPv6 address. */

  /* NOTE: We are not currently supporting IPv6 in any lwIP products, and this implementation will
   * probably need to be revisited before we do, especially the default interface code. */
  for (size_t i = 0; i < LWIP_IPV6_NUM_ADDRESSES; ++i)
  {
    if (ip6_addr_isvalid(lwip_netif->ip6_addr_state[i]))
    {
#if LWIP_IPV6_SCOPES
      LWPA_IP_SET_V6_ADDRESS_WITH_SCOPE_ID(&netint->addr, &(ip_2_ip6(&lwip_netif->ip6_addr[i])->addr),
                                           ip_2_ip6(&lwip_netif->ip6_addr[i])->zone);
#else
      LWPA_IP_SET_V6_ADDRESS(&netint->addr, &(ip_2_ip6(&lwip_netif->ip6_addr[i])->addr));
      // TODO revisit
      netint->mask = lwpa_ip_mask_from_length(kLwpaIpTypeV6, 128);
#endif

      if (lwip_netif == netif_default && i == 0)
        netint->is_default = true;
      else
        netint->is_default = false;
    }
  }
}

lwpa_error_t os_enumerate_interfaces(CachedNetintInfo* cache)
{
  struct netif* lwip_netif;

#if LWPA_EMBOS_USE_MALLOC
  size_t num_lwip_netints = 0;
  NETIF_FOREACH(lwip_netif)
  {
#if LWIP_IPV4
    ++num_lwip_netints;
#endif
#if LWIP_IPV6
    for (size_t i = 0; i < LWIP_IPV6_NUM_ADDRESSES; ++i)
    {
      if (ip6_addr_isvalid(lwip_netif->ip6_addr_state[i]))
        ++num_lwip_netints;
    }
#endif
  }
  cache->netints = (LwpaNetintInfo*)calloc(sizeof(LwpaNetintInfo), num_lwip_netints);
  if (!cache->netints)
    return kLwpaErrNoMem;
#endif

  num_static_netints = 0;

  // Make sure the default netint is included
#if LWIP_IPV4
  copy_interface_info_v4(netif_default, &static_netints[num_static_netints++]);
#endif
#if LWIP_IPV6
  for (size_t i = 0; i < LWIP_IPV6_NUM_ADDRESSES; ++i)
  {
#if !LWPA_EMBOS_USE_MALLOC
    if (num_static_netints >= LWPA_EMBOS_MAX_NETINTS)
      break;
#endif
    copy_interface_info_v6(netif_default, i, &static_netints[num_static_netints++]);
  }
#endif

  NETIF_FOREACH(lwip_netif)
  {
    if (lwip_netif == netif_default)
      continue;

#if !LWPA_EMBOS_USE_MALLOC
    if (num_static_netints >= LWPA_EMBOS_MAX_NETINTS)
      break;
#endif

#if LWIP_IPV4
    copy_interface_info_v4(lwip_netif, &static_netints[num_static_netints++]);
#endif
#if LWIP_IPV6
    for (size_t i = 0; i < LWIP_IPV6_NUM_ADDRESSES; ++i)
    {
#if !LWPA_EMBOS_USE_MALLOC
      if (num_static_netints >= LWPA_EMBOS_MAX_NETINTS)
        break;
#endif
      copy_interface_info_v6(lwip_netif, i, &static_netints[num_static_netints++]);
    }
#endif
  }

  cache->netints = static_netints;
  cache->num_netints = num_static_netints;
  return kLwpaErrOk;
}

void os_free_interfaces(CachedNetintInfo* cache)
{
#if LWPA_EMBOS_USE_MALLOC
  if (cache->netints)
  {
    free(cache->netints);
    cache->netints = NULL;
  }
#endif
}

lwpa_error_t os_resolve_route(const LwpaIpAddr* dest, unsigned int* index)
{
  unsigned int index_found = 0;

  for (const LwpaNetintInfo* netint = static_netints; netint < static_netints + num_static_netints; ++netint)
  {
    if (!lwpa_ip_is_wildcard(&netint->mask) && lwpa_ip_network_portions_equal(&netint->addr, dest, &netint->mask))
    {
      index_found = netint->index;
      break;
    }
  }
  if (index_found == 0)
    index_found = static_netints[default_index].index;

  *index = index_found;
  return kLwpaErrOk;
}
