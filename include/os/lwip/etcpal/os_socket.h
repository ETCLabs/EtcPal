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

#include <lwip/opt.h>

/* Avoid header clashes caused by the definition of the macro bind() in lwIP headers when the
 * LWIP_COMPAT_SOCKETS option is set to 1. Restores the value of LWIP_COMPAT_SOCKETS for future
 * inclusion of lwIP headers.
 *
 * Note: header clashes could still occur if library users include lwIP headers before other C++
 * EtcPal headers which use <functional>.
 */
#if (LWIP_COMPAT_SOCKETS == 1)
#undef LWIP_COMPAT_SOCKETS
#define LWIP_COMPAT_SOCKETS 0
#include <lwip/sockets.h>
#undef LWIP_COMPAT_SOCKETS
#define LWIP_COMPAT_SOCKETS 1
#endif /* (LWIP_COMPAT_SOCKETS == 1) */

#ifdef __cplusplus
extern "C" {
#endif

/* Definitions for the EtcPal socket type */

typedef int etcpal_socket_t;

#define PRIepsock "d"

#define ETCPAL_SOCKET_INVALID -1

#define ETCPAL_SOCKET_MAX_POLL_SIZE FD_SETSIZE

/* Definitions for etcpal_poll API */

typedef struct EtcPalPollSocket
{
  etcpal_socket_t      sock;
  etcpal_poll_events_t events;
  void*                user_data;
} EtcPalPollSocket;

typedef struct EtcPalPollFdSet
{
  fd_set set;
  int    count;
} EtcPalPollFdSet;

typedef struct EtcPalPollContext
{
  bool valid;

  EtcPalPollSocket sockets[ETCPAL_SOCKET_MAX_POLL_SIZE];
  size_t           num_valid_sockets;
  int              max_fd;

  EtcPalPollFdSet readfds;
  EtcPalPollFdSet writefds;
  EtcPalPollFdSet exceptfds;
} EtcPalPollContext;

#ifdef __cplusplus
}
#endif

#endif /* ETCPAL_OS_SOCKET_H_ */
