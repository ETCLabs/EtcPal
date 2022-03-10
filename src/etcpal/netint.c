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

#include "etcpal/private/netint.h"

#ifdef __MQX__
#define MQX_PROVIDES_STDIO !MQX_SUPPRESS_STDIO_MACROS
#else
#define MQX_PROVIDES_STDIO 0
#endif

#if !MQX_PROVIDES_STDIO
#include <stdio.h>
#endif

/*************************** Function definitions ****************************/

/**
 * @fn void etcpal_netint_get_num_interfaces(void)
 * @brief Get the number of network interfaces present on the system.
 * @return Number of interfaces present.
 */

/**
 * @fn etcpal_error_t etcpal_netint_get_interfaces(uint8_t* buf, size_t* buf_size)
 * @brief Get a list of network interfaces on the system.
 *
 * Network interface information is read into the buffer specified by buf, which has a size
 * specified by buf_size. The contents and ordering of the buffer after being filled in are
 * implementation-defined, except that the beginning of the buffer can be cast to an
 * EtcPalNetintInfo* representing the first interface retrieved, whose next member can in turn be
 * used to iterate through all interfaces on the system as a linked list.
 *
 * If using a statically allocated buffer for buf, it should be given the same alignment
 * requirement as an EtcPalNetintInfo structure. This can be accomplished using the C11 or C++11
 * alignas() specifier, or a complier-specific method.
 *
 * ETCPAL_NETINT_BUF_SIZE_ESTIMATE() can be used to get an estimated buffer size requirement for a
 * given number of network interfaces and IP addresses per interface.
 *
 * @param[out] buf Buffer into which network interface information is read.
 * @param[in,out] buf_size Size of the buffer provided. After returning, this parameter is set to
 *                         the size of the network interface information that was filled in, or to
 *                         the size that would be needed if the function failed due to a short
 *                         buffer.
 * @return #kEtcPalErrOk: Network interface information retrieved successfully.
 * @return #kEtcPalErrInvalid: Invalid argument provided.
 * @return #kEtcPalErrNoNetints: No network interfaces found on system.
 * @return #kEtcPalErrBufSize: The provided buffer was not large enough. buf_size is set to the required size.
 */

/**
 * @fn etcpal_error_t etcpal_netint_get_interface_by_index(unsigned int index, uint8_t* buf, size_t* buf_size)
 * @brief Get the network interface that has the index specified.
 *
 * See @ref interface_indexes for more information about the index parameter.
 *
 * Information about the network interface is read into the provided buffer in the same way as
 * described in etcpal_netint_get_interfaces().
 *
 * @param[in] index Index for which to get the corresponding interface.
 * @param[out] buf Buffer into which network interface information is read.
 * @param[in,out] buf_size Size of the buffer provided. After returning, this parameter is set to
 *                         the size of the network interface information that was filled in, or to
 *                         the size that would be needed if the function failed due to a short
 *                         buffer.
 * @return #kEtcPalErrOk: Network interface information retrieved successfully.
 * @return #kEtcPalErrInvalid: Invalid argument provided.
 * @return #kEtcPalErrNotFound: No interfaces found for this index.
 * @return #kEtcPalErrBufSize: The provided buffer was not large enough. buf_size is set to the required size.
 */

/**
 * @fn etcpal_error_t etcpal_netint_get_default_interface_index(etcpal_iptype_t type, unsigned int* netint_index)
 * @brief Get the index of the default network interface.
 *
 * See @ref interface_indexes for more information about interface indexes.
 *
 * For our purposes, the 'default' network interface is defined as the interface that is chosen
 * for the default IP route.
 *
 * @param[in] type The IP protocol for which to get the default network interface, either
 *                 #kEtcPalIpTypeV4 or #kEtcPalIpTypeV6.
 * @param[out] netint_index Pointer to value to fill with the index of the default interface.
 * @return #kEtcPalErrOk: netint_index was filled in.
 * @return #kEtcPalErrInvalid: Invalid argument provided.
 * @return #kEtcPalErrNotFound: No default interface found for this type.
 */

/**
 * @fn etcpal_error_t etcpal_netint_get_default_interface(etcpal_iptype_t type, uint8_t* buf, size_t* buf_size)
 * @brief Get information about the default network interface.
 *
 * For our purposes, the 'default' network interface is defined as the interface that is chosen
 * for the default IP route.
 *
 * Information about the network interface is read into the provided buffer in the same way as
 * described in etcpal_netint_get_interfaces().
 *
 * @param[in] type The IP protocol for which to get the default network interface, either
 *                 #kEtcPalIpTypeV4 or #kEtcPalIpTypeV6.
 * @param[out] buf Buffer into which network interface information is read.
 * @param[in,out] buf_size Size of the buffer provided. After returning, this parameter is set to
 *                         the size of the network interface information that was filled in, or to
 *                         the size that would be needed if the function failed due to a short
 *                         buffer.
 * @return #kEtcPalErrOk: Default network interface information retrieved successfully.
 * @return #kEtcPalErrInvalid: Invalid argument provided.
 * @return #kEtcPalErrNotFound: No default interface found for this type.
 * @return #kEtcPalErrBufSize: The provided buffer was not large enough. buf_size is set to the required size.
 */

/**
 * @fn bool etcpal_netint_is_up(unsigned int netint_index)
 * @brief Determine whether a network interface is currently up and running.
 * @param netint_index Index of the interface to check.
 * @return true: The interface indicated by netint_index is up.
 * @return false: The interface indicated by netint_index is down, or netint_index is invalid.
 */

/**
 * @brief Convert an EtcPalNetintAddr to a CIDR string representation.
 *
 * CIDR notation is used, e.g. "192.168.1.1/24"
 *
 * @param addr Network interface address to convert to string form.
 * @param dest Filled in on success with the string-represented address. To avoid undefined
 *             behavior, this buffer must be at least of size #ETCPAL_NETINT_ADDR_STRING_BYTES.
 * @return #kEtcPalErrOk: Success.
 * @return #kEtcPalErrInvalid: Invalid argument provided.
 * @return #kEtcPalErrSys: System call failed.
 */
etcpal_error_t etcpal_netint_addr_to_string(const EtcPalNetintAddr* addr, char* dest)
{
  if (!addr || !dest)
    return kEtcPalErrInvalid;

  etcpal_error_t res = etcpal_ip_to_string(&addr->addr, dest);
  if (res != kEtcPalErrOk)
    return res;

  sprintf(&dest[strlen(dest)], "/%u", addr->mask_length);
  return kEtcPalErrOk;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Internal functions
///////////////////////////////////////////////////////////////////////////////////////////////////

EtcPalNetintInfo* find_existing_netint(const char* name, EtcPalNetintInfo* start, EtcPalNetintInfo* end)
{
  for (EtcPalNetintInfo* info = start; info < end; ++info)
  {
    if (strcmp(info->id, name) == 0)
      return info;
  }
  return NULL;
}
