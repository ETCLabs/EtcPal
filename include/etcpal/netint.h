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

/* lwpa_netint.h: Platform-neutral method for enumerating network interfaces. */
#ifndef _LWPA_NETINT_H_
#define _LWPA_NETINT_H_

#include <stddef.h>
#include "etcpal/bool.h"
#include "etcpal/common.h"
#include "etcpal/inet.h"

/*! \defgroup lwpa_netint lwpa_netint
 *  \ingroup lwpa
 *  \brief A platform-neutral method for enumerating network interfaces.
 *
 *  \#include "etcpal/netint.h"
 *
 *  @{
 */

#ifdef __cplusplus
extern "C" {
#endif

size_t lwpa_netint_get_num_interfaces();
const LwpaNetintInfo* lwpa_netint_get_interfaces();
size_t lwpa_netint_copy_interfaces(LwpaNetintInfo* netint_arr, size_t netint_arr_size);
lwpa_error_t lwpa_netint_get_interfaces_by_index(unsigned int index, const LwpaNetintInfo** netint_arr,
                                                 size_t* netint_arr_size);

lwpa_error_t lwpa_netint_get_default_interface(lwpa_iptype_t type, unsigned int* netint_index);
lwpa_error_t lwpa_netint_get_interface_for_dest(const LwpaIpAddr* dest, unsigned int* netint_index);

#ifdef __cplusplus
}
#endif

/*! @} */

#endif /* _LWPA_NETINT_H_ */
