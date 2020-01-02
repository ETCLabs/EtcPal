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
 * This file is a part of EtcPal. For more information, go to:
 * https://github.com/ETCLabs/EtcPal
 ******************************************************************************/

#include "etcpal/netint.h"
#include <string.h>
#include <lwip/netif.h>
#include "etcpal/private/opts.h"
#include "etcpal/private/netint.h"

#if ETCPAL_EMBOS_USE_MALLOC
#include <stdlib.h>
static EtcPalNetintInfo* static_netints;
#else
static EtcPalNetintInfo static_netints[ETCPAL_EMBOS_MAX_NETINTS];
#endif
size_t num_static_netints;
size_t default_index;

static void copy_common_interface_info(const struct netif* lwip_netif, EtcPalNetintInfo* netint)
{
  netint->index = netif_get_index(lwip_netif);
  memset(netint->mac.data, 0, ETCPAL_MAC_BYTES);
  memcpy(netint->mac.data, lwip_netif->hwaddr, lwip_netif->hwaddr_len);

  char lwip_name[NETIF_NAMESIZE];
  if (NULL != netif_index_to_name((u8_t)netint->index, lwip_name))
  {
    strncpy(netint->name, lwip_name, ETCPAL_NETINTINFO_NAME_LEN);
    strncpy(netint->friendly_name, lwip_name, ETCPAL_NETINTINFO_FRIENDLY_NAME_LEN);
  }
}

static void copy_interface_info_v4(const struct netif* lwip_netif, EtcPalNetintInfo* netint)
{
  copy_common_interface_info(lwip_netif, netint);

  if (!ip_addr_isany(netif_ip_addr4(lwip_netif)))
  {
    ETCPAL_IP_SET_V4_ADDRESS(&netint->addr, ntohl(netif_ip4_addr(lwip_netif)->addr));
    ETCPAL_IP_SET_V4_ADDRESS(&netint->mask, ntohl(netif_ip4_netmask(lwip_netif)->addr));
  }

  if (lwip_netif == netif_default)
    netint->is_default = true;
  else
    netint->is_default = false;
}

#if LWIP_IPV6
static bool copy_interface_info_v6(const struct netif* lwip_netif, size_t v6_addr_index, EtcPalNetintInfo* netint)
{
  copy_common_interface_info(lwip_netif, netint);

  /* Finish filling in a netintinfo for a single IPv6 address. */

  if (ip6_addr_isvalid(lwip_netif->ip6_addr_state[v6_addr_index]))
  {
#if LWIP_IPV6_SCOPES
    ETCPAL_IP_SET_V6_ADDRESS_WITH_SCOPE_ID(&netint->addr, &(ip_2_ip6(&lwip_netif->ip6_addr[v6_addr_index])->addr),
                                           ip_2_ip6(&lwip_netif->ip6_addr[v6_addr_index])->zone);
#else
    ETCPAL_IP_SET_V6_ADDRESS(&netint->addr, &(ip_2_ip6(&lwip_netif->ip6_addr[v6_addr_index])->addr));
    // TODO revisit
    netint->mask = etcpal_ip_mask_from_length(kEtcPalIpTypeV6, 128);
#endif

    if (lwip_netif == netif_default)
      netint->is_default = true;
    else
      netint->is_default = false;

    return true;
  }
  return false;
}
#endif

etcpal_error_t os_enumerate_interfaces(CachedNetintInfo* cache)
{
  struct netif* lwip_netif;

#if ETCPAL_EMBOS_USE_MALLOC
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
  cache->netints = (EtcPalNetintInfo*)calloc(sizeof(EtcPalNetintInfo), num_lwip_netints);
  if (!cache->netints)
    return kEtcPalErrNoMem;
#endif

  num_static_netints = 0;

  // Make sure the default netint is included
#if LWIP_IPV4
  if (netif_default)
  {
    cache->def.v4_valid = true;
    cache->def.v4_index = netif_get_index(netif_default);
    copy_interface_info_v4(netif_default, &static_netints[num_static_netints++]);
  }
#endif
#if LWIP_IPV6
  if (netif_default)
  {
    cache->def.v6_valid = true;
    cache->def.v6_index = netif_get_index(netif_default);
    for (size_t i = 0; i < LWIP_IPV6_NUM_ADDRESSES; ++i)
    {
#if !ETCPAL_EMBOS_USE_MALLOC
      if (num_static_netints >= ETCPAL_EMBOS_MAX_NETINTS)
        break;
#endif
      if (copy_interface_info_v6(netif_default, i, &static_netints[num_static_netints]))
        ++num_static_netints;
    }
  }
#endif

  NETIF_FOREACH(lwip_netif)
  {
    if (lwip_netif == netif_default)
      continue;

#if !ETCPAL_EMBOS_USE_MALLOC
    if (num_static_netints >= ETCPAL_EMBOS_MAX_NETINTS)
      break;
#endif

#if LWIP_IPV4
    copy_interface_info_v4(lwip_netif, &static_netints[num_static_netints++]);
#endif
#if LWIP_IPV6
    for (size_t i = 0; i < LWIP_IPV6_NUM_ADDRESSES; ++i)
    {
#if !ETCPAL_EMBOS_USE_MALLOC
      if (num_static_netints >= ETCPAL_EMBOS_MAX_NETINTS)
        break;
#endif
      if (copy_interface_info_v6(lwip_netif, i, &static_netints[num_static_netints]))
        ++num_static_netints;
    }
#endif
  }

  cache->netints = static_netints;
  cache->num_netints = num_static_netints;
  return kEtcPalErrOk;
}

void os_free_interfaces(CachedNetintInfo* cache)
{
#if ETCPAL_EMBOS_USE_MALLOC
  if (cache->netints)
  {
    free(cache->netints);
    cache->netints = NULL;
  }
#endif
}

etcpal_error_t os_resolve_route(const EtcPalIpAddr* dest, const CachedNetintInfo* cache, unsigned int* index)
{
  unsigned int index_found = 0;

  for (const EtcPalNetintInfo* netint = static_netints; netint < static_netints + num_static_netints; ++netint)
  {
    if (!etcpal_ip_is_wildcard(&netint->mask) && etcpal_ip_network_portions_equal(&netint->addr, dest, &netint->mask))
    {
      index_found = netint->index;
      break;
    }
  }

  if (index_found == 0)
  {
    if (ETCPAL_IP_IS_V4(dest) && cache->def.v4_valid)
      index_found = cache->def.v4_index;
    else if (ETCPAL_IP_IS_V6(dest) && cache->def.v6_valid)
      index_found = cache->def.v6_index;
  }

  if (index_found == 0)
  {
    return kEtcPalErrNotFound;
  }
  else
  {
    *index = index_found;
    return kEtcPalErrOk;
  }
}
