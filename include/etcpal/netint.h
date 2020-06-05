/******************************************************************************
 * Copyright 2020 ETC Inc.
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
#include "etcpal/common.h"
#include "etcpal/inet.h"

/**
 * @defgroup etcpal_netint netint (Network Interfaces)
 * @ingroup etcpal
 * @brief A platform-neutral method for enumerating network interfaces.
 *
 * ```c
 * #include "etcpal/netint.h"
 * ```
 *
 * **WARNING:** This module must be explicitly initialized before use. Initialize the module by
 * calling etcpal_init() with the relevant feature mask:
 * @code
 * etcpal_init(ETCPAL_FEATURE_NETINTS);
 * @endcode
 *
 * After initialization, an array of the set of network interfaces which were present on the system
 * at initialization time is kept internally. This array can be retrieved using
 * etcpal_netint_get_num_interfaces() and etcpal_netint_get_interfaces():
 *
 * @code
 * size_t num_netints = etcpal_netint_get_num_interfaces();
 * const EtcPalNetintInfo* netint_array = etcpal_netint_get_interfaces();
 * for (const EtcPalNetintInfo* netint = netint_array; netint < netint_array + num_netints; ++netint)
 * {
 *   // Record information or do something with each network interface in turn
 * }
 * @endcode
 *
 * The network interface array is sorted by interface index (see @ref interface_indexes); multiple
 * IP addresses assigned to the same physical interface are separated into different entries. This
 * means multiple entries in the array can have the same index.
 *
 * The module also attempts to determine the interface used for the system's default route (the
 * route used to get to an arbitrary internet destination) and calls that the "default interface",
 * which can be retrieved using etcpal_netint_get_default_interface().
 *
 * The routing information can also be used to determine which network interface will be used for a
 * given IP address destination, which can be handy for multicasting.
 *
 * @code
 * EtcPalIpAddr dest;
 * etcpal_string_to_ip(kEtcPalIpTypeV4, "192.168.200.35", &dest);
 *
 * unsigned int index;
 * etcpal_netint_get_interface_for_dest(&dest, &index); // Index now holds the interface that will be used
 * @endcode
 *
 * The list of network interfaces is cached and does not change at runtime, unless the
 * etcpal_netint_refresh_interfaces() function is called.
 *
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

size_t                  etcpal_netint_get_num_interfaces(void);
const EtcPalNetintInfo* etcpal_netint_get_interfaces(void);
etcpal_error_t          etcpal_netint_get_interfaces_by_index(unsigned int             index,
                                                              const EtcPalNetintInfo** netint_arr,
                                                              size_t*                  netint_arr_size);

etcpal_error_t etcpal_netint_get_default_interface(etcpal_iptype_t type, unsigned int* netint_index);
etcpal_error_t etcpal_netint_get_interface_for_dest(const EtcPalIpAddr* dest, unsigned int* netint_index);

etcpal_error_t etcpal_netint_refresh_interfaces(bool* list_changed);

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif /* ETCPAL_NETINT_H_ */
