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

#ifndef NOMINMAX
#define NOMINMAX 1 /* Suppress some conflicting definitions in the Windows headers */
#include <winsock2.h>
#include <windows.h>
#undef NOMINMAX
#else
#include <winsock2.h>
#include <windows.h>
#endif

#include "etcpal/inet.h"
#include "etcpal/lock.h"
#include "etcpal/rbtree.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Definitions for the EtcPal socket type */

typedef SOCKET etcpal_socket_t;

#if defined(_WIN64)
#define PRIepsock "I64u"
#else
#define PRIepsock "u"
#endif

#define ETCPAL_SOCKET_INVALID INVALID_SOCKET

#define ETCPAL_SOCKET_MAX_POLL_SIZE FD_SETSIZE

/* Definitions for the etcpal_poll API */

typedef struct EtcPalPollFdSet
{
  fd_set set;
  size_t count;
} EtcPalPollFdSet;

typedef struct EtcPalPollContext
{
  bool           valid;
  etcpal_mutex_t lock;

  EtcPalRbTree sockets;

  EtcPalPollFdSet readfds;
  EtcPalPollFdSet writefds;
  EtcPalPollFdSet exceptfds;
} EtcPalPollContext;

#ifdef __cplusplus
}
#endif

#endif /* ETCPAL_OS_SOCKET_H_ */
