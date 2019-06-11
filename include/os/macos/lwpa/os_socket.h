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

#include <netinet/in.h>
#include <sys/select.h>
#include "lwpa/inet.h"
#include "lwpa/rbtree.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef int lwpa_socket_t;

#define LWPA_SOCKET_INVALID -1

#define LWPA_SOCKET_MAX_POLL_SIZE -1

typedef struct LwpaPollContext
{
  bool valid;
  int epoll_fd;
  LwpaRbTree sockets;
} LwpaPollContext;

#define IP_OS_TO_LWPA_V4(lwpaipptr, pfipptr) LWPA_IP_SET_V4_ADDRESS((lwpaipptr), ntohl((pfipptr)->s_addr))
#define IP_LWPA_TO_OS_V4(pfipptr, lwpaipptr) ((pfipptr)->s_addr = htonl(LWPA_IP_V4_ADDRESS(lwpaipptr)))
#define IP_OS_TO_LWPA_V6(lwpaipptr, pfipptr) LWPA_IP_SET_V6_ADDRESS((lwpaipptr), (pfipptr)->s6_addr)
#define IP_LWPA_TO_OS_V6(pfipptr, lwpaipptr) memcpy((pfipptr)->s6_addr, LWPA_IP_V6_ADDRESS(lwpaipptr), IPV6_BYTES)

bool sockaddr_os_to_lwpa(LwpaSockaddr* sa, const struct sockaddr* pfsa);
size_t sockaddr_lwpa_to_os(struct sockaddr* pfsa, const LwpaSockaddr* sa);

#ifdef __cplusplus
}
#endif

#endif /* _LWPA_OS_SOCKET_H_ */
