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

/* etcpal_netint.h: Platform-neutral method for enumerating network interfaces. */

#ifndef ETCPAL_NETINT_H_
#define ETCPAL_NETINT_H_

#include <stddef.h>
#include "etcpal/bool.h"
#include "etcpal/common.h"
#include "etcpal/inet.h"

/*!
 * \defgroup etcpal_netint Network Interfaces (netint)
 * \ingroup etcpal
 * \brief A platform-neutral method for enumerating network interfaces.
 *
 * ```c
 * #include "etcpal/netint.h"
 * ```
 *
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

size_t etcpal_netint_get_num_interfaces();
const EtcPalNetintInfo* etcpal_netint_get_interfaces();
size_t etcpal_netint_copy_interfaces(EtcPalNetintInfo* netint_arr, size_t netint_arr_size);
etcpal_error_t etcpal_netint_get_interfaces_by_index(unsigned int index, const EtcPalNetintInfo** netint_arr,
                                                     size_t* netint_arr_size);

etcpal_error_t etcpal_netint_get_default_interface(etcpal_iptype_t type, unsigned int* netint_index);
etcpal_error_t etcpal_netint_get_interface_for_dest(const EtcPalIpAddr* dest, unsigned int* netint_index);

#ifdef __cplusplus
}
#endif

/*!
 * @}
 */

#endif /* ETCPAL_NETINT_H_ */
