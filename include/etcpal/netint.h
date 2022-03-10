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

/* etcpal_netint.h: Platform-neutral method for enumerating network interfaces. */

#ifndef ETCPAL_NETINT_H_
#define ETCPAL_NETINT_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "etcpal/common.h"
#include "etcpal/inet.h"
#include "etcpal/os_netint.h"

/**
 * @defgroup etcpal_netint netint (Network Interfaces)
 * @ingroup etcpal_net
 * @brief A platform-neutral method for enumerating network interfaces.
 *
 * ```c
 * #include "etcpal/netint.h"
 * ```
 *
 * The primary function in this module is etcpal_netint_get_interfaces(), which is used to
 * enumerate the current network interfaces on the system.
 *
 * @code
 * // Use ETCPAL_NETINT_BUF_SIZE_ESTIMATE(num_netints, num_addrs_per_netint) to allocate a buffer
 * // estimated to be large enough to hold information on a system with num_netints interfaces, and
 * // num_addrs_per_netint IP addresses per interface.
 * size_t netint_buf_size = ETCPAL_NETINT_BUF_SIZE_ESTIMATE(5, 3);
 * uint8_t* netint_buf = (uint8_t*) malloc(netint_buf_size);
 *
 * // Get the interfaces
 * etcpal_error_t res = etcpal_netint_get_interfaces(netint_buf, &netint_buf_size);
 *
 * // Grow the buffer if necessary
 * while (res == kEtcPalErrBufSize)
 * {
 *   // netint_buf_size is an in/out parameter which is set to the new needed size after calling
 *   // etcpal_netint_get_interfaces()
 *   netint_buf = (uint8_t*) realloc(netint_buf, netint_buf_size);
 *   res = etcpal_netint_get_interfaces(netint_buf, &netint_buf_size);
 * }
 *
 * if (res == kEtcPalErrOk)
 * {
 *   for (const EtcPalNetintInfo* netint = (const EtcPalNetintInfo*)netint_buf; netint; netint = netint->next)
 *   {
 *     // Record information or do something with each network interface in turn
 *
 *     for (const EtcPalNetintAddr* addr = netint->addrs; addr; addr = addr->next)
 *     {
 *       // Iterate through the IP addresses assigned to this interface
 *     }
 *   }
 * }
 *
 * free(netint_buf);
 * @endcode
 *
 * Network interface information is read into a buffer allocated by the user. This buffer can also
 * be statically allocated; in this case, it's recommended to use C11's
 * [`alignas()`](https://en.cppreference.com/w/c/language/_Alignas) specifier to ensure that the
 * static buffer has the proper alignment to hold an array of EtcPalNetintInfo (if the buffer is
 * allocated using `malloc()`, this is already a given).
 *
 * @code
 * #include <stdalign.h>
 *
 * alignas(EtcPalNetintInfo) uint8_t netint_buf[ETCPAL_NETINT_BUF_SIZE_ESTIMATE(1, 3)];
 *
 * // Then get the interfaces as above, failing hard if the buffer is not big enough
 * @endcode
 *
 * Each EtcPalNetintInfo structure represents a single physical network interface on the system.
 * Network interfaces may have multiple IP addresses, including a mixture of IPv4 and IPv6
 * addresses.
 *
 * The module also attempts to determine the interface used for the system's default route (the
 * route used to get to an arbitrary internet destination) and calls that the "default interface"
 * for IPv4 and IPv6. This information is stored in the `flags` field of EtcPalNetintInfo.
 *
 * @code
 * EtcPalNetintInfo* netint; // Assuming 'netint' is retrieved using the method above
 *
 * if (netint->flags & ETCPAL_NETINT_FLAG_DEFAULT_V4)
 * {
 *   // This network interface is chosen for the default IPv4 route
 * }
 * if (netint->flags & ETCPAL_NETINT_FLAG_DEFAULT_V6)
 * {
 *   // This network interface is chosen for the default IPv6 route
 * }
 * @endcode
 *
 * These default interfaces can also be retrieved using etcpal_netint_get_default_interface().
 *
 * Network interface information is retrieved from the system every time an etcpal_netint_get_*()
 * is called. This can be a costly operation.
 *
 * @{
 */

/* Padding is only needed if alignof(EtcPalNetintAddr) > alignof(EtcPalNetintInfo). In that case
 * the padding needed will be at most alignof(EtcPalNetintAddr) - alignof(EtcPalNetintInfo). */
#define ETCPAL_NETINT_PADDING_HEURISTIC                                   \
  ((ETCPAL_ALIGNOF(EtcPalNetintInfo) >= ETCPAL_ALIGNOF(EtcPalNetintAddr)) \
       ? 0                                                                \
       : ETCPAL_ALIGNOF(EtcPalNetintAddr) - ETCPAL_ALIGNOF(EtcPalNetintInfo))

/**
 * @brief Get an estimate of the buffer size needed to enumerate the system network interfaces.
 *
 * This uses heuristics and is not guaranteed to produce a buffer of adequate size. See the
 * documentation for the netint module for more information.
 *
 * If you want to increase the probability that you will get a buffer of adequate size on the first
 * try, provide overestimates for the num_netints and num_addrs_per_netint parameters.
 *
 * @param num_netints The anticipated number of network interfaces on the system.
 * @param num_addrs_per_netint The anticipated average number of IP addresses (both IPv4 and IPv6)
 *                             assigned to each network interface on the system.
 */
#define ETCPAL_NETINT_BUF_SIZE_ESTIMATE(num_netints, num_addrs_per_netint)                                          \
  (((num_netints) * ((sizeof(EtcPalNetintInfo)) + ETCPAL_NETINT_NAME_HEURISTIC +                                    \
                     ETCPAL_NETINT_FRIENDLY_NAME_HEURISTIC + ((num_addrs_per_netint) * sizeof(EtcPalNetintAddr))) + \
    ETCPAL_NETINT_PADDING_HEURISTIC))

typedef struct EtcPalNetintAddr EtcPalNetintAddr;

/** A network address assigned to a network interface. */
struct EtcPalNetintAddr
{
  /** The IP address itself. */
  EtcPalIpAddr addr;
  /** The length in bits of the netmask. Use etcpal_ip_mask_from_length() to get a representation of
   * the mask if necessary. */
  unsigned int mask_length;
  /** A pointer to the next address in the buffer. */
  EtcPalNetintAddr* next;
};

/** Maximum length of the string representation of a network interface address. */
#define ETCPAL_NETINT_ADDR_STRING_BYTES ETCPAL_IP_STRING_BYTES + 4

typedef struct EtcPalNetintInfo EtcPalNetintInfo;

/**
 * @brief A description of a single network interface.
 *
 * A network interface has one name, one MAC address and one index. It can have an arbitrary number
 * of IP addresses of either type.
 */
struct EtcPalNetintInfo
{
  /** The OS-specific network interface number. See @ref interface_indexes for more information. */
  unsigned int index;
  /** Linked list of IP addresses assigned to the interface. */
  const EtcPalNetintAddr* addrs;
  /** The interface MAC address. */
  EtcPalMacAddr mac;
  /** The system name for the interface. This name can be used as a primary key to identify a
   *  single network adapter. It will not change unless the adapter is removed or reconfigured. */
  const char* id;
  /** A user-friendly name for the interface. On some systems, this is the same as the id field.
   *  Others allow users to create and change a friendly name for network interfaces that's
   *  different than the system name. This field should be used when printing the adapter list in a
   *  UI. */
  const char* friendly_name;
  /** A set of flags indicating properties about this interface. */
  uint32_t flags;
  /** A pointer to the next entry in the netint buffer. */
  EtcPalNetintInfo* next;
};

/**
 * @name EtcPalNetintInfo Flags
 * @{
 */

/**
 * Flag indicating that this is the default network interface for IPv4. The default network
 * interface is defined as the network interface chosen for the default IPv4 route on a system.
 */
#define ETCPAL_NETINT_FLAG_DEFAULT_V4 0x00000001u
/**
 * Flag indicating that this is the default network interface for IPv6. The default network
 * interface is defined as the network interface chosen for the default IPv6 route on a system.
 */
#define ETCPAL_NETINT_FLAG_DEFAULT_V6 0x00000002u

/**
 * @}
 */

#ifdef __cplusplus
extern "C" {
#endif

size_t         etcpal_netint_get_num_interfaces(void);
etcpal_error_t etcpal_netint_get_interfaces(uint8_t* buf, size_t* buf_size);
etcpal_error_t etcpal_netint_get_interface_by_index(unsigned int index, uint8_t* buf, size_t* buf_size);

etcpal_error_t etcpal_netint_get_default_interface_index(etcpal_iptype_t type, unsigned int* netint_index);
etcpal_error_t etcpal_netint_get_default_interface(etcpal_iptype_t type, uint8_t* buf, size_t* buf_size);

bool etcpal_netint_is_up(unsigned int netint_index);

etcpal_error_t etcpal_netint_addr_to_string(const EtcPalNetintAddr* addr, char* dest);

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif /* ETCPAL_NETINT_H_ */
