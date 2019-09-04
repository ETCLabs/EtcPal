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

#include <lwip/sockets.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Definitions for the lwpa socket type */

typedef int lwpa_socket_t;

#define PRIlwpasockt "d"

#define LWPA_SOCKET_INVALID -1

#define LWPA_SOCKET_MAX_POLL_SIZE FD_SETSIZE

/* Definitions for lwpa_poll API */

typedef struct LwpaPollSocket
{
  lwpa_socket_t sock;
  lwpa_poll_events_t events;
  void* user_data;
} LwpaPollSocket;

typedef struct LwpaPollFdSet
{
  fd_set set;
  int count;
} LwpaPollFdSet;

typedef struct LwpaPollContext
{
  bool valid;

  LwpaPollSocket sockets[LWPA_SOCKET_MAX_POLL_SIZE];
  size_t num_valid_sockets;
  int max_fd;

  LwpaPollFdSet readfds;
  LwpaPollFdSet writefds;
  LwpaPollFdSet exceptfds;
} LwpaPollContext;

#ifdef __cplusplus
}
#endif

#endif /* _LWPA_OS_SOCKET_H_ */
