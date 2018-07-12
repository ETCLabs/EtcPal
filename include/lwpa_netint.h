/******************************************************************************
 * Copyright 2018 ETC Inc.
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

/* lwpa_netint.h: Platform-neutral method for enumerating network interfaces.
 */
#ifndef _LWPA_NETINT_H_
#define _LWPA_NETINT_H_

#include <stddef.h>
#include "lwpa_bool.h"
#include "lwpa_inet.h"

/*! \defgroup lwpa_netint lwpa_netint
 *  \ingroup lwpa
 *  \brief A platform-neutral method for enumerating network interfaces.
 *
 *  \#include "lwpa_netint.h"
 *
 *  @{
 */

#ifdef __cplusplus
extern "C" {
#endif

size_t netint_get_num_interfaces();
size_t netint_get_interfaces(LwpaNetintInfo *netint_arr, size_t netint_arr_size);
bool netint_get_default_interface(LwpaNetintInfo *netint);
const LwpaNetintInfo *netint_get_iface_for_dest(const LwpaIpAddr *dest, const LwpaNetintInfo *netint_arr,
                                                size_t netint_arr_size);

// typedef void (*netint_change_notification)(void *context);

// int    netint_register_change_cb(netint_change_notification fn, void
// *context); void   netint_unregister_change_cb(int handle);

#ifdef __cplusplus
}
#endif

/*!@}*/

#endif /* _LWPA_NETINT_H_ */
