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
#include <string.h>
#include <lwip/netif.h>
#include <lwip/tcpip.h>
#include "etcpal/private/netint.h"
#include "etcpal/private/util.h"

/*********************** Private function prototypes *************************/

// These functions must be called inside a LOCK_TCPIP_CORE() block. All operations on a given state
// of the lwIP network interface array need to be inside the same such block, e.g.:
//
// calculate_size_needed(buf, &sizes);
//
// // MUST NOT UNLOCK HERE
//
// copy_all_netint_info(buf, &sizes);

static void calculate_size_needed(const uint8_t* buf_addr, NetintArraySizes* sizes);
static void calculate_size_needed_single_netint(const struct netif* lwip_netif,
                                                const uint8_t*      buf_addr,
                                                NetintArraySizes*   sizes);
static void copy_all_netint_info(uint8_t* buf, const NetintArraySizes* sizes);
static void copy_single_netint_info(const struct netif* lwip_netif, NetintArrayContext* context);

/*************************** Function definitions ****************************/

size_t etcpal_netint_get_num_interfaces(void)
{
  size_t num_interfaces = 0;

  LOCK_TCPIP_CORE();
  struct netif* lwip_netif = NULL;
  NETIF_FOREACH(lwip_netif) { ++num_interfaces; }
  UNLOCK_TCPIP_CORE();

  return num_interfaces;
}

etcpal_error_t etcpal_netint_get_interfaces(uint8_t* buf, size_t* buf_size)
{
  if (!buf || !buf_size)
    return kEtcPalErrInvalid;

  LOCK_TCPIP_CORE();

  NetintArraySizes sizes = NETINT_ARRAY_SIZES_INIT;
  calculate_size_needed(buf, &sizes);
  if (sizes.total_size > *buf_size)
  {
    *buf_size = sizes.total_size;
    UNLOCK_TCPIP_CORE();
    return kEtcPalErrBufSize;
  }

  copy_all_netint_info(buf, &sizes);

  UNLOCK_TCPIP_CORE();

  *buf_size = sizes.total_size;
  return kEtcPalErrOk;
}

etcpal_error_t etcpal_netint_get_interface_by_index(unsigned int index, uint8_t* buf, size_t* buf_size)
{
  if (!buf || !buf_size)
    return kEtcPalErrInvalid;

  LOCK_TCPIP_CORE();

  struct netif* lwip_netif = NULL;
  NETIF_FOREACH(lwip_netif)
  {
    if (netif_get_index(lwip_netif) == index)
    {
      NetintArraySizes sizes = NETINT_ARRAY_SIZES_INIT;
      calculate_size_needed_single_netint(lwip_netif, buf, &sizes);
      if (sizes.total_size > *buf_size)
      {
        *buf_size = sizes.total_size;
        UNLOCK_TCPIP_CORE();
        return kEtcPalErrBufSize;
      }

      NetintArrayContext context = NETINT_ARRAY_CONTEXT_INIT(buf, &sizes);
      copy_single_netint_info(lwip_netif, &context);
      *buf_size = sizes.total_size;
      UNLOCK_TCPIP_CORE();
      return kEtcPalErrOk;
    }
  }

  UNLOCK_TCPIP_CORE();
  return kEtcPalErrNotFound;
}

etcpal_error_t etcpal_netint_get_default_interface_index(etcpal_iptype_t type, unsigned int* netint_index)
{
  if (!netint_index)
    return kEtcPalErrInvalid;

#if !LWIP_IPV4
  if (type != kEtcPalIpTypeV6)
    return kEtcPalErrInvalid;
#endif
#if !LWIP_IPV6
  if (type != kEtcPalIpTypeV4)
    return kEtcPalErrInvalid;
#endif

  unsigned int default_index = 0;

  LOCK_TCPIP_CORE();
  if (netif_default)
    default_index = netif_get_index(netif_default);
  UNLOCK_TCPIP_CORE();

  if (default_index == 0)
    return kEtcPalErrNotFound;

  *netint_index = default_index;
  return kEtcPalErrOk;
}

etcpal_error_t etcpal_netint_get_default_interface(etcpal_iptype_t type, uint8_t* buf, size_t* buf_size)
{
  if (!buf || !buf_size)
    return kEtcPalErrInvalid;

  LOCK_TCPIP_CORE();

  if (!netif_default)
  {
    UNLOCK_TCPIP_CORE();
    return kEtcPalErrNotFound;
  }

  NetintArraySizes sizes = NETINT_ARRAY_SIZES_INIT;
  calculate_size_needed_single_netint(netif_default, buf, &sizes);
  if (sizes.total_size > *buf_size)
  {
    *buf_size = sizes.total_size;
    UNLOCK_TCPIP_CORE();
    return kEtcPalErrBufSize;
  }

  NetintArrayContext context = NETINT_ARRAY_CONTEXT_INIT(buf, &sizes);
  copy_single_netint_info(netif_default, &context);
  UNLOCK_TCPIP_CORE();
  *buf_size = sizes.total_size;
  return kEtcPalErrOk;
}

bool etcpal_netint_is_up(unsigned int index)
{
  bool res = false;
  LOCK_TCPIP_CORE();
  struct netif* netif = netif_get_by_index(index);
  if (netif)
    res = netif_is_up(netif);
  UNLOCK_TCPIP_CORE();
  return res;
}

void calculate_size_needed(const uint8_t* buf_addr, NetintArraySizes* sizes)
{
  size_t size_needed         = 0;
  size_t netintinfo_arr_size = 0;
  size_t ip_addr_arr_size    = 0;

  struct netif* lwip_netif = NULL;
  NETIF_FOREACH(lwip_netif)
  {
    netintinfo_arr_size += sizeof(EtcPalNetintInfo);
    size_needed += sizeof(EtcPalNetintInfo) + NETIF_NAMESIZE;

#if LWIP_IPV4
    if (!ip_addr_isany(netif_ip_addr4(lwip_netif)))
    {
      size_needed += sizeof(EtcPalNetintAddr);
      ip_addr_arr_size += sizeof(EtcPalNetintAddr);
    }
#endif

#if LWIP_IPV6
    for (size_t i = 0; i < LWIP_IPV6_NUM_ADDRESSES; ++i)
    {
      if (ip6_addr_isvalid(lwip_netif->ip6_addr_state[i]))
      {
        size_needed += sizeof(EtcPalNetintAddr);
        ip_addr_arr_size += sizeof(EtcPalNetintAddr);
      }
#endif
    }
  }

  size_t padding = GET_PADDING_FOR_ALIGNMENT(buf_addr, EtcPalNetintInfo);
  size_needed += padding;
  sizes->total_size      = size_needed;
  sizes->ip_addrs_offset = netintinfo_arr_size + padding;
  sizes->names_offset    = netintinfo_arr_size + padding + ip_addr_arr_size;
}

void calculate_size_needed_single_netint(const struct netif* lwip_netif,
                                         const uint8_t*      buf_addr,
                                         NetintArraySizes*   sizes)
{
  size_t size_needed         = sizeof(EtcPalNetintInfo) + NETIF_NAMESIZE;
  size_t netintinfo_arr_size = sizeof(EtcPalNetintInfo);
  size_t ip_addr_arr_size    = 0;

#if LWIP_IPV4
  if (!ip_addr_isany(netif_ip_addr4(lwip_netif)))
  {
    size_needed += sizeof(EtcPalNetintAddr);
    ip_addr_arr_size += sizeof(EtcPalNetintAddr);
  }
#endif

#if LWIP_IPV6
  for (size_t i = 0; i < LWIP_IPV6_NUM_ADDRESSES; ++i)
  {
    if (ip6_addr_isvalid(lwip_netif->ip6_addr_state[i]))
    {
      size_needed += sizeof(EtcPalNetintAddr);
      ip_addr_arr_size += sizeof(EtcPalNetintAddr);
    }
#endif
  }

  size_t padding = GET_PADDING_FOR_ALIGNMENT(buf_addr, EtcPalNetintInfo);
  size_needed += padding;
  sizes->total_size      = size_needed;
  sizes->ip_addrs_offset = netintinfo_arr_size + padding;
  sizes->names_offset    = netintinfo_arr_size + padding + ip_addr_arr_size;
}

void copy_all_netint_info(uint8_t* buf, const NetintArraySizes* sizes)
{
  NetintArrayContext context    = NETINT_ARRAY_CONTEXT_INIT(buf, sizes);
  struct netif*      lwip_netif = NULL;

  NETIF_FOREACH(lwip_netif) { copy_single_netint_info(lwip_netif, &context); }

  // Link the EtcPalNetintInfo structures together
  EtcPalNetintInfo* cur_netint = (EtcPalNetintInfo*)buf;
  while (cur_netint + 1 != context.cur_info)
  {
    cur_netint->next = cur_netint + 1;
    ++cur_netint;
  }
}

void copy_single_netint_info(const struct netif* lwip_netif, NetintArrayContext* context)
{
  EtcPalNetintInfo* cur_info = context->cur_info;
  memset(cur_info, 0, sizeof(EtcPalNetintInfo));

  cur_info->index = netif_get_index(lwip_netif);
  memcpy(cur_info->mac.data, lwip_netif->hwaddr, lwip_netif->hwaddr_len);

  char lwip_name[NETIF_NAMESIZE];
  if (NULL != netif_index_to_name((u8_t)cur_info->index, lwip_name))
  {
    strcpy(context->cur_name, lwip_name);
    cur_info->id            = context->cur_name;
    cur_info->friendly_name = context->cur_name;
    context->cur_name += strlen(lwip_name) + 1;
  }

#if LWIP_IPV4
  if (!ip_addr_isany(netif_ip_addr4(lwip_netif)))
  {
    memset(context->cur_addr, 0, sizeof(EtcPalNetintAddr));
    ETCPAL_IP_SET_V4_ADDRESS(&context->cur_addr->addr, ntohl(netif_ip4_addr(lwip_netif)->addr));

    EtcPalIpAddr mask;
    ETCPAL_IP_SET_V4_ADDRESS(&mask, ntohl(netif_ip4_netmask(lwip_netif)->addr));
    context->cur_addr->mask_length = etcpal_ip_mask_length(&mask);

    cur_info->addrs = context->cur_addr;
    ++context->cur_addr;
  }

  if (lwip_netif == netif_default)
    cur_info->flags |= ETCPAL_NETINT_FLAG_DEFAULT_V4;
#endif

#if LWIP_IPV6
  EtcPalNetintAddr* last_addr = (EtcPalNetintAddr*)cur_info->addrs;
  for (size_t i = 0; i < LWIP_IPV6_NUM_ADDRESSES; ++i)
  {
    if (ip6_addr_isvalid(lwip_netif->ip6_addr_state[i]))
    {
      EtcPalNetintAddr* cur_addr = context->cur_addr;
      memset(cur_addr, 0, sizeof(EtcPalNetintAddr));

#if LWIP_IPV6_SCOPES
      ETCPAL_IP_SET_V6_ADDRESS_WITH_SCOPE_ID(&cur_addr->addr, &(ip_2_ip6(&lwip_netif->ip6_addr[i])->addr),
                                             ip_2_ip6(&lwip_netif->ip6_addr[i])->zone);
#else
      ETCPAL_IP_SET_V6_ADDRESS(&cur_addr->addr, &(ip_2_ip6(&lwip_netif->ip6_addr[i])->addr));
#endif
      // TODO revisit
      cur_addr->mask_length = 128;

      if (last_addr)
        last_addr->next = cur_addr;
      else
        cur_info->addrs = cur_addr;
      ++context->cur_addr;
    }
  }

  if (lwip_netif == netif_default)
    cur_info->flags |= ETCPAL_NETINT_FLAG_DEFAULT_V6;
#endif

  ++context->cur_info;
}
