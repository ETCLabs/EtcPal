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
#include "etcpal/inet.h"
#include "etcpal/rbtree.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Definitions for the lwpa socket type */

typedef int lwpa_socket_t;

#define PRIlwpasockt "d"

#define LWPA_SOCKET_INVALID -1

#define LWPA_SOCKET_MAX_POLL_SIZE -1

/* Definitions for lwpa_poll API */

typedef struct LwpaPollContext
{
  bool valid;
  int kq_fd;
  LwpaRbTree sockets;
} LwpaPollContext;

#ifdef __cplusplus
}
#endif

#endif /* _LWPA_OS_SOCKET_H_ */
