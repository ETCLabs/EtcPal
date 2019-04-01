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

#ifndef _LWPA_PLAT_SOCKET_H_
#define _LWPA_PLAT_SOCKET_H_

#include <netinet/in.h>
#include <sys/select.h>
#include "lwpa/inet.h"

typedef int lwpa_socket_t;

#define LWPA_SOCKET_INVALID -1

#define LWPA_SOCKET_MAX_POLL_SIZE FD_SETSIZE

#define ip_plat_to_lwpa_v4(lwpaipptr, pfipptr) lwpaip_set_v4_address((lwpaipptr), ntohl((pfipptr)->s_addr))
#define ip_lwpa_to_plat_v4(pfipptr, lwpaipptr) ((pfipptr)->s_addr = htonl(lwpaip_v4_address(lwpaipptr)))
#define ip_plat_to_lwpa_v6(lwpaipptr, pfipptr) lwpaip_set_v6_address((lwpaipptr), (pfipptr)->s6_addr)
#define ip_lwpa_to_plat_v6(pfipptr, lwpaipptr) memcpy((pfipptr)->s6_addr, lwpaip_v6_address(lwpaipptr), IPV6_BYTES)

#ifdef __cplusplus
extern "C" {
#endif

bool sockaddr_plat_to_lwpa(LwpaSockaddr *sa, const struct sockaddr *pfsa);
size_t sockaddr_lwpa_to_plat(struct sockaddr *pfsa, const LwpaSockaddr *sa);

#ifdef __cplusplus
}
#endif

#endif /* _LWPA_PLAT_SOCKET_H_ */

