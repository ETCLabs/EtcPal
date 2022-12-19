/******************************************************************************
 * Copyright 2022 ETC Inc.
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

#include <zephyr/net/socket.h>
#include <zephyr/net/net_ip.h>
#include <zephyr/net/net_if.h>
#include <zephyr/net/net_pkt.h>
#include <zephyr/posix/poll.h>

#include "etcpal/inet.h"
#include "etcpal/rbtree.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Definitions for the EtcPal socket type */

typedef int etcpal_socket_t;

#define PRIepsock "d"

#define ETCPAL_SOCKET_INVALID -1

#define ETCPAL_SOCKET_MAX_POLL_SIZE -1

/* Definitions for etcpal_poll API */

typedef struct EtcPalPollContext
{
  bool         valid;
	struct zsock_pollfd *zpoll_fd;
  EtcPalRbTree sockets;
} EtcPalPollContext;

/* Definitions for the etcpal_recvmsg API */
// NAM: Not sure sockaddr_in and sockaddr_in6 are correct, should be something like
// in_pktinfo and in6_pktinfo 
#define ETCPAL_PLATFORM_IN_PKTINFO_SPACE  CMSG_SPACE(sizeof(struct sockaddr_in))
#define ETCPAL_PLATFORM_IN6_PKTINFO_SPACE CMSG_SPACE(sizeof(struct sockaddr_in6))

#ifdef __cplusplus
}
#endif

#endif /* ETCPAL_OS_SOCKET_H_ */
