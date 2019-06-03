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

#ifndef _LWPA_WINDOWS_SOCKET_H_
#define _LWPA_WINDOWS_SOCKET_H_

#ifndef NOMINMAX
#define NOMINMAX 1 /* Suppress some conflicting definitions in the Windows headers */
#include <winsock2.h>
#include <windows.h>
#undef NOMINMAX
#else
#include <winsock2.h>
#include <windows.h>
#endif

#include "lwpa/inet.h"
#include "lwpa/lock.h"

typedef SOCKET lwpa_socket_t;

/*! An identifier for an invalid socket handle. The ONLY invalid socket descriptor is
 *  LWPA_SOCKET_INVALID. */
#define LWPA_SOCKET_INVALID INVALID_SOCKET

#define LWPA_SOCKET_MAX_POLL_SIZE FD_SETSIZE

typedef struct LwpaPollCtxSocket
{
  lwpa_socket_t socket;
  lwpa_poll_events_t events;
  void* user_data;
} LwpaPollCtxSocket;

typedef struct LwpaPollFdSet
{
  fd_set set;
  size_t count;
} LwpaPollFdSet;

typedef struct LwpaPollContext
{
  bool valid;
  lwpa_mutex_t lock;

  LwpaPollCtxSocket* sockets;
  size_t socket_arr_size;
  size_t num_valid_sockets;

  LwpaPollFdSet readfds;
  LwpaPollFdSet writefds;
  LwpaPollFdSet exceptfds;

} LwpaPollContext;

#ifdef __cplusplus
extern "C" {
#endif

#define ip_plat_to_lwpa_v4(lwpaipptr, pfipptr) lwpaip_set_v4_address((lwpaipptr), ntohl((pfipptr)->s_addr))
#define ip_lwpa_to_plat_v4(pfipptr, lwpaipptr) ((pfipptr)->s_addr = htonl(lwpaip_v4_address(lwpaipptr)))
#define ip_plat_to_lwpa_v6(lwpaipptr, pfipptr) lwpaip_set_v6_address((lwpaipptr), (pfipptr)->s6_addr)
#define ip_lwpa_to_plat_v6(pfipptr, lwpaipptr) memcpy((pfipptr)->s6_addr, lwpaip_v6_address(lwpaipptr), IPV6_BYTES)

bool sockaddr_plat_to_lwpa(LwpaSockaddr* sa, const struct sockaddr* pfsa);
size_t sockaddr_lwpa_to_plat(struct sockaddr* pfsa, const LwpaSockaddr* sa);

#ifdef __cplusplus
}
#endif

#endif /* _LWPA_WINDOWS_SOCKET_H_ */
