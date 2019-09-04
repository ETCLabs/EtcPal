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

#ifndef _LWPA_OS_SOCKET_H_
#define _LWPA_OS_SOCKET_H_

#include <rtcs.h>
#include "etcpal/inet.h"
#include "etcpal/int.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Definitions for the lwpa socket type */

typedef uint32_t etcpal_socket_t;

#ifdef PRIu32
#define PRIlwpasockt PRIu32
#else
#define PRIlwpasockt "u"
#endif

#define LWPA_SOCKET_INVALID RTCS_SOCKET_ERROR

#define LWPA_SOCKET_MAX_POLL_SIZE RTCSCFG_FD_SETSIZE

typedef struct LwpaPollCtxSocket
{
  etcpal_socket_t socket;
  etcpal_poll_events_t events;
  void* user_data;
} LwpaPollCtxSocket;

typedef struct LwpaPollFdSet
{
  rtcs_fd_set set;
  size_t count;
} LwpaPollFdSet;

typedef struct LwpaPollContext
{
  bool valid;

  LwpaPollCtxSocket sockets[LWPA_SOCKET_MAX_POLL_SIZE];
  size_t num_valid_sockets;

  LwpaPollFdSet readfds;
  LwpaPollFdSet writefds;
} LwpaPollContext;

#ifdef __cplusplus
}
#endif

#endif /* _LWPA_OS_SOCKET_H_ */
