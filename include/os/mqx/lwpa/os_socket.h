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

#ifndef _LWPA_OS_SOCKET_H_
#define _LWPA_OS_SOCKET_H_

#include <rtcs.h>
#include "lwpa/inet.h"

typedef uint32_t lwpa_socket_t;

/*! An identifier for an invalid socket handle. The ONLY invalid socket descriptor is LWPA_SOCKET_INVALID. */
#define LWPA_SOCKET_INVALID RTCS_SOCKET_ERROR

#define LWPA_SOCKET_MAX_POLL_SIZE RTCSCFG_FD_SETSIZE

#ifdef __cplusplus
extern "C" {
#endif

#define ip_os_to_lwpa_v4(lwpaipptr, pfipptr) lwpaip_set_v4_address((lwpaipptr), (pfipptr)->s_addr)
#define ip_lwpa_to_os_v4(pfipptr, lwpaipptr) ((pfipptr)->s_addr = lwpaip_v4_address(lwpaipptr))
#define ip_os_to_lwpa_v6(lwpaipptr, pfipptr) lwpaip_set_v6_address((lwpaipptr), (pfipptr)->s6_addr)
#define ip_lwpa_to_os_v6(pfipptr, lwpaipptr) memcpy((pfipptr)->s6_addr, lwpaip_v6_address(lwpaipptr), IPV6_BYTES)

bool sockaddr_os_to_lwpa(LwpaSockaddr* sa, const struct sockaddr* pfsa);
size_t sockaddr_lwpa_to_os(struct sockaddr* pfsa, const LwpaSockaddr* sa);

#ifdef __cplusplus
}
#endif

#endif /* _LWPA_OS_SOCKET_H_ */
