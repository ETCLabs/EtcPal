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

#include <mqx.h>
#include <bsp.h>
#include <rtcs.h>
#include <stdint.h>

#include "etcpal/common.h"
#include "etcpal/private/netint.h"
#include "etcpal/private/util.h"

// This module is quick and dirty and needs some work.
// - We are ignoring IPv6.
// - We don't check for loopback interfaces or invalid data.

// Assumptions we can/are making at this time on MQX:
// - One IPv4 address and 0 IPv6 addresses per interface.
// - We generate names of the form 'enX' for each interface, where X is the 0-based index.
// - MQX index == standard interface index - 1 (0-based vs 1-based)

#define NETINT_NAME_MAX_SIZE 13  // size of 'en4294967295' with nul

/*********************** Private function prototypes *************************/

static void calculate_size_needed(const uint8_t* buf_addr, NetintArraySizes* sizes);
static void calculate_size_needed_single_netint(uint32_t mqx_index, const uint8_t* buf_addr, NetintArraySizes* sizes);
static void copy_all_netint_info(uint8_t* buf, const NetintArraySizes* sizes);
static void copy_single_netint_info(uint32_t mqx_index, NetintArrayContext* context);

static void get_netint_name(uint32_t mqx_index, char* name_buf);

/*************************** Function definitions ****************************/

size_t etcpal_netint_get_num_interfaces(void)
{
  return BSP_ENET_DEVICE_COUNT;
}

etcpal_error_t etcpal_netint_get_interfaces(uint8_t* buf, size_t* buf_size)
{
  if (!buf || !buf_size)
    return kEtcPalErrInvalid;

  NetintArraySizes sizes = NETINT_ARRAY_SIZES_INIT;
  calculate_size_needed(buf, &sizes);
  if (sizes.total_size > *buf_size)
  {
    *buf_size = sizes.total_size;
    return kEtcPalErrBufSize;
  }

  copy_all_netint_info(buf, &sizes);
  *buf_size = sizes.total_size;
  return kEtcPalErrOk;
}

etcpal_error_t etcpal_netint_get_interface_by_index(unsigned int index, uint8_t* buf, size_t* buf_size)
{
  if (!buf || !buf_size || index == 0)
    return kEtcPalErrInvalid;
  if (index > BSP_ENET_DEVICE_COUNT)
    return kEtcPalErrNotFound;

  NetintArraySizes sizes = NETINT_ARRAY_SIZES_INIT;
  calculate_size_needed_single_netint(index - 1, buf, &sizes);
  if (sizes.total_size > *buf_size)
  {
    *buf_size = sizes.total_size;
    return kEtcPalErrBufSize;
  }

  NetintArrayContext context = NETINT_ARRAY_CONTEXT_INIT(buf, &sizes);
  copy_single_netint_info(index - 1, &context);
  *buf_size = sizes.total_size;
  return kEtcPalErrOk;
}

etcpal_error_t etcpal_netint_get_default_interface_index(etcpal_iptype_t type, unsigned int* netint_index)
{
  if (!netint_index || type != kEtcPalIpTypeV4)
    return kEtcPalErrInvalid;

  *netint_index = BSP_DEFAULT_ENET_DEVICE + 1;
  return kEtcPalErrOk;
}

etcpal_error_t etcpal_netint_get_default_interface(etcpal_iptype_t type, uint8_t* buf, size_t* buf_size)
{
  if (!buf || !buf_size)
    return kEtcPalErrInvalid;

  NetintArraySizes sizes = NETINT_ARRAY_SIZES_INIT;
  calculate_size_needed_single_netint(BSP_DEFAULT_ENET_DEVICE, buf, &sizes);
  if (sizes.total_size > *buf_size)
  {
    *buf_size = sizes.total_size;
    return kEtcPalErrBufSize;
  }

  NetintArrayContext context = NETINT_ARRAY_CONTEXT_INIT(buf, &sizes);
  copy_single_netint_info(BSP_DEFAULT_ENET_DEVICE, &context);
  *buf_size = sizes.total_size;
  return kEtcPalErrOk;
}

bool etcpal_netint_is_up(unsigned int index)
{
  if (index == 0)
    return false;

  return (ipcfg_get_state(index - 1) != IPCFG_STATE_INIT);
}

void calculate_size_needed(const uint8_t* buf_addr, NetintArraySizes* sizes)
{
  sizes->ip_addrs_offset =
      (sizeof(EtcPalNetintInfo) * BSP_ENET_DEVICE_COUNT) + GET_PADDING_FOR_ALIGNMENT(buf_addr, EtcPalNetintInfo);
  sizes->names_offset = sizes->ip_addrs_offset + (sizeof(EtcPalNetintAddr) * BSP_ENET_DEVICE_COUNT);
  sizes->total_size   = sizes->names_offset;

  // Fill out the names sizes
  for (uint32_t i = 0; i < BSP_ENET_DEVICE_COUNT; ++i)
  {
    char name[NETINT_NAME_MAX_SIZE];
    get_netint_name(i, name);
    sizes->total_size += strlen(name) + 1;
  }
}

void calculate_size_needed_single_netint(uint32_t mqx_index, const uint8_t* buf_addr, NetintArraySizes* sizes)
{
  sizes->ip_addrs_offset = sizeof(EtcPalNetintInfo) + GET_PADDING_FOR_ALIGNMENT(buf_addr, EtcPalNetintInfo);
  sizes->names_offset    = sizes->ip_addrs_offset + sizeof(EtcPalNetintAddr);
  sizes->total_size      = sizes->names_offset;

  // Fill out the names size
  char name[NETINT_NAME_MAX_SIZE];
  get_netint_name(mqx_index, name);
  sizes->total_size += strlen(name) + 1;
}

void copy_all_netint_info(uint8_t* buf, const NetintArraySizes* sizes)
{
  NetintArrayContext context = NETINT_ARRAY_CONTEXT_INIT(buf, sizes);

  for (uint32_t i = 0; i < BSP_ENET_DEVICE_COUNT; ++i)
  {
    copy_single_netint_info(i, &context);
  }

  // Link the EtcPalNetintInfo structures together
  EtcPalNetintInfo* cur_netint = (EtcPalNetintInfo*)buf;
  while (cur_netint + 1 != context.cur_info)
  {
    cur_netint->next = cur_netint + 1;
    ++cur_netint;
  }
}

void copy_single_netint_info(uint32_t mqx_index, NetintArrayContext* context)
{
  EtcPalNetintInfo* cur_info = context->cur_info;
  memset(cur_info, 0, sizeof(EtcPalNetintInfo));

  // In MQX, interfaces are kept in a linked list and retrieved using an index from 0.
  // To provide an RFC-3493-compliant index, we'll just offset that index by 1.
  cur_info->index = mqx_index + 1;

  IPCFG_IP_ADDRESS_DATA ip_data;
  ipcfg_get_ip(mqx_index, &ip_data);
  ETCPAL_IP_SET_V4_ADDRESS(&context->cur_addr->addr, ip_data.ip);
  EtcPalIpAddr mask;
  ETCPAL_IP_SET_V4_ADDRESS(&mask, ip_data.mask);
  context->cur_addr->mask_length = etcpal_ip_mask_length(&mask);

  cur_info->addrs = context->cur_addr;
  ++context->cur_addr;

  ipcfg_get_mac(mqx_index, cur_info->mac.data);
  get_netint_name(mqx_index, context->cur_name);
  cur_info->id            = context->cur_name;
  cur_info->friendly_name = context->cur_name;
  context->cur_name += strlen(context->cur_name) + 1;

  if (mqx_index == BSP_DEFAULT_ENET_DEVICE)
    cur_info->flags |= ETCPAL_NETINT_FLAG_DEFAULT_V4;

  ++context->cur_info;
}

void get_netint_name(uint32_t mqx_index, char* name_buf)
{
  sprintf(name_buf, "en%d", mqx_index);
}
