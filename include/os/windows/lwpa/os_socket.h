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

#ifdef __cplusplus
extern "C" {
#endif

/* Definitions for the lwpa socket type */

typedef SOCKET lwpa_socket_t;

#if defined(_WIN64)
#define PRIlwpasockt "I64u"
#else
#define PRIlwpasockt "u"
#endif

#define LWPA_SOCKET_INVALID INVALID_SOCKET

#define LWPA_SOCKET_MAX_POLL_SIZE FD_SETSIZE

/* Definitions for the lwpa_poll API */

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
}
#endif

#endif /* _LWPA_OS_SOCKET_H_ */
