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

#ifndef ETCPAL_OS_SOCKET_H_
#define ETCPAL_OS_SOCKET_H_

#include <rtcs.h>
#include <stdint.h>
#include "etcpal/inet.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Definitions for the EtcPal socket type */

typedef uint32_t etcpal_socket_t;

#ifdef PRIu32
#define PRIepsock PRIu32
#else
#define PRIepsock "u"
#endif

#define ETCPAL_SOCKET_INVALID RTCS_SOCKET_ERROR

#define ETCPAL_SOCKET_MAX_POLL_SIZE RTCSCFG_FD_SETSIZE

typedef struct EtcPalPollCtxSocket
{
  etcpal_socket_t      socket;
  etcpal_poll_events_t events;
  void*                user_data;
} EtcPalPollCtxSocket;

typedef struct EtcPalPollFdSet
{
  rtcs_fd_set set;
  size_t      count;
} EtcPalPollFdSet;

typedef struct EtcPalPollContext
{
  bool valid;

  EtcPalPollCtxSocket sockets[ETCPAL_SOCKET_MAX_POLL_SIZE];
  size_t              num_valid_sockets;

  EtcPalPollFdSet readfds;
  EtcPalPollFdSet writefds;
} EtcPalPollContext;

#ifdef __cplusplus
}
#endif

#endif /* ETCPAL_OS_SOCKET_H_ */
