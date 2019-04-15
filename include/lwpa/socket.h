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

/* lwpa/socket.h: Platform-neutral BSD-modeled network socket implementation. */
#ifndef _LWPA_SOCKET_H_
#define _LWPA_SOCKET_H_

#include <stddef.h>
#include "lwpa/common.h"
#include "lwpa/bool.h"
#include "lwpa/error.h"
#include "lwpa/inet.h"

/*! \defgroup lwpa_socket lwpa_socket
 *  \ingroup lwpa
 *  \brief Platform-neutral BSD-modeled network socket implementation.
 *
 *  \#include "lwpa/socket.h"
 *
 *  This module attempts to abstract the network socket interface from various platforms into one
 *  platform-neutral interface. The functions and flags are modeled after the BSD sockets
 *  interface. <em>UNIX Network Programming: The Sockets Networking API</em> by Stevens, Fenner,
 *  and Rudoff is highly recommended reading for those unfamiliar with this interface.
 *
 *  @{
 */

/*! Event flags for the lwpa_poll_*() API functions. */
typedef uint32_t lwpa_poll_events_t;

#include "lwpa/plat_socket.h" /* The platform-specific socket definitions */

/* clang-format off */

/*! \name Flags for lwpa_recvfrom()
 *  @{ */
#define LWPA_MSG_PEEK 0x1
/*! @} */

/* Note: no flags are currently implemented for lwpa_sendto() */

/*! \name Level values for lwpa_setsockopt() and lwpa_getsockopt()
 *  Refer to the group of option names for each level.
 *  @{ */
#define LWPA_SOL_SOCKET     0 /*!< lwpa_socket API level. */
#define LWPA_IPPROTO_IP     1 /*!< IPv4 protocol level. */
#define LWPA_IPPROTO_ICMPV6 2 /*!< ICMPv6 protocol level. */
#define LWPA_IPPROTO_IPV6   3 /*!< IPv6 protocol level. */
#define LWPA_IPPROTO_TCP    4 /*!< TCP protocol level. */
#define LWPA_IPPROTO_UDP    5 /*!< UDP protocol level. */
/*! @} */

/*! \name Options for level LWPA_SOL_SOCKET
 *  Used in the option parameter to lwpa_setsockopt() and lwpa_getsockopt().
 *  Refer to the similarly-named option on your favorite man page for more details.
 *  @{ */
#define LWPA_SO_BROADCAST 0  /*!< Get/Set, value is boolean int */
#define LWPA_SO_ERROR     1  /*!< Get only, value is int representing platform error value */
#define LWPA_SO_KEEPALIVE 2  /*!< Get/Set, value is boolean int */
#define LWPA_SO_LINGER    3  /*!< Get/Set, value is LwpaLinger */
#define LWPA_SO_RCVBUF    4  /*!< Get/Set, value is int representing byte size */
#define LWPA_SO_SNDBUF    5  /*!< Get/Set, value is int representing byte size */
#define LWPA_SO_RCVTIMEO  6  /*!< Get/Set, value is int representing ms */
#define LWPA_SO_SNDTIMEO  7  /*!< Get/Set, value is int representing ms */
#define LWPA_SO_REUSEADDR 8  /*!< Get/Set, value is boolean int */
#define LWPA_SO_REUSEPORT 9  /*!< Get/Set, value is boolean int */
#define LWPA_SO_TYPE      10 /*!< Get only, value is int */
/*! @} */

/*! \name Options for level LWPA_IPPROTO_IP or LWPA_IPPROTO_IPV6
 *  Used in the option parameter to lwpa_setsockopt() and lwpa_getsockopt().
 *  Refer to the similarly-named option on your favorite man page for more details.
 *  @{ */
#define LWPA_IP_TTL            11 /*!< Get/Set, value is int */
#define LWPA_IP_MULTICAST_IF   12 /*!< Get/Set, value is LwpaIpAddr */
#define LWPA_IP_MULTICAST_TTL  13 /*!< Get/Set, value is int */
#define LWPA_IP_MULTICAST_LOOP 14 /*!< Get/Set, value is boolean int */
#define LWPA_MCAST_JOIN_GROUP  15 /*!< Set only, value is LwpaMreq */
#define LWPA_MCAST_LEAVE_GROUP 16 /*!< Set only, value is LwpaMreq */
#define LWPA_IPV6_V6ONLY       17 /*!< Get/Set, value is boolean int */
/*! @} */

/* clang-format on */

/* LWPA_IPPROTO_TCP: TODO */

/*! Option value for #LWPA_SO_LINGER. */
typedef struct LwpaLinger
{
  int onoff;  /*!< 0 = off, nonzero = on */
  int linger; /*!< Linger time in seconds */
} LwpaLinger;

/*! Option value for #LWPA_MCAST_JOIN_GROUP and #LWPA_MCAST_LEAVE_GROUP. */
typedef struct LwpaMreq
{
  /*! Address of network interface on which to join the multicast group. */
  LwpaIpAddr netint;
  /*! Multicast group to join. */
  LwpaIpAddr group;
} LwpaMreq;

/*! \name 'how' values for lwpa_shutdown()
 *  @{ */
#define LWPA_SHUT_RD 0
#define LWPA_SHUT_WR 1
#define LWPA_SHUT_RDWR 2
/*! @} */

/*! \name 'family' values for lwpa_socket() and LwpaAddrinfo
 *  @{ */
#define LWPA_AF_UNSPEC 0
#define LWPA_AF_INET 1
#define LWPA_AF_INET6 2
/*! @} */

/*! \name 'type' values for lwpa_socket() and lwpa_getsockopt()
 *  @{ */
#define LWPA_STREAM 0
#define LWPA_DGRAM 1
/*! @} */

/********************** Mimic sys/socket.h functions *************************/

#ifdef __cplusplus
extern "C" {
#endif

lwpa_error_t lwpa_accept(lwpa_socket_t id, LwpaSockaddr *address, lwpa_socket_t *conn_sock);
lwpa_error_t lwpa_bind(lwpa_socket_t id, const LwpaSockaddr *address);
lwpa_error_t lwpa_close(lwpa_socket_t id);
lwpa_error_t lwpa_connect(lwpa_socket_t id, const LwpaSockaddr *address);
lwpa_error_t lwpa_getpeername(lwpa_socket_t id, LwpaSockaddr *address);
lwpa_error_t lwpa_getsockname(lwpa_socket_t id, LwpaSockaddr *address);
lwpa_error_t lwpa_getsockopt(lwpa_socket_t id, int level, int option_name, void *option_value, size_t *option_len);
lwpa_error_t lwpa_listen(lwpa_socket_t id, int backlog);
int lwpa_recv(lwpa_socket_t id, void *buffer, size_t length, int flags);
int lwpa_recvfrom(lwpa_socket_t id, void *buffer, size_t length, int flags, LwpaSockaddr *address);
/* recvmsg - not implemented */
int lwpa_send(lwpa_socket_t id, const void *message, size_t length, int flags);
/* sendmsg - not implemented */
int lwpa_sendto(lwpa_socket_t id, const void *message, size_t length, int flags, const LwpaSockaddr *dest_addr);
lwpa_error_t lwpa_setsockopt(lwpa_socket_t id, int level, int option_name, const void *option_value, size_t option_len);
lwpa_error_t lwpa_shutdown(lwpa_socket_t id, int how);
lwpa_error_t lwpa_socket(unsigned int family, unsigned int type, lwpa_socket_t *id);
/* int protocol - not necessary */
/* socketpair - not implemented */

/**************************** Mimic fcntl() API ******************************/

lwpa_error_t lwpa_setblocking(lwpa_socket_t id, bool blocking);

/**************************** Mimic poll() API *******************************/

/*! \name Flag values to use with lwpa_poll_events_t.
 *
 *  @{ */
#define LWPA_POLL_IN 0x1u      /*!< Notify when data is available for reading on the socket. */
#define LWPA_POLL_OUT 0x2u     /*!< Notify when the socket has space available to write data. */
#define LWPA_POLL_CONNECT 0x4u /*!< Notify when a non-blocking connect operation has completed. */
#define LWPA_POLL_OOB 0x8u     /*!< Notify when there is out-of-band data on a TCP socket. */
#define LWPA_POLL_ERR 0x10u    /*!< An error has occurred on the socket (output only). */
/*! @} */

/* Mask of valid events for use with lwpa_poll_add_socket(). */
#define LWPA_POLL_VALID_INPUT_EVENT_MASK 0x0fu

/*! A description of an event that occurred on a socket, for usage with lwpa_poll_wait(). */
typedef struct LwpaPollEvent
{
  lwpa_socket_t socket;      /*!< Socket which had activity. */
  lwpa_poll_events_t events; /*!< Event(s) that occurred on the socket. */
  lwpa_error_t err;          /*!< More information about an error that occurred on the socket. */
} LwpaPollEvent;

lwpa_error_t lwpa_poll_context_init(LwpaPollContext *context);
lwpa_error_t lwpa_poll_context_deinit(LwpaPollContext *context);
lwpa_error_t lwpa_poll_add_socket(LwpaPollContext *context, lwpa_socket_t socket, lwpa_poll_events_t events);
lwpa_error_t lwpa_poll_remove_socket(LwpaPollContext *context, lwpa_socket_t socket);
lwpa_error_t lwpa_poll_wait(LwpaPollContext *context, LwpaPollEvent *event, int timeout_ms);

/************************ Mimic getaddrinfo() API ****************************/

/*! \name 'flags' values for LwpaAddrinfo
 *
 *  Refer to the similarly-named flags in your favorite getaddrinfo() man page for more details.
 *  @{ */
#define LWPA_AI_PASSIVE 0x01
#define LWPA_AI_CANONNAME 0x02
#define LWPA_AI_NUMERICHOST 0x04
/*! @} */

/*! A structure containing name and address information about an internet host. Returned by
 *  lwpa_getaddrinfo(). */
typedef struct LwpaAddrinfo
{
  int ai_flags;         /*!< i.e. LWPA_AI_xxx */
  int ai_family;        /*!< i.e. LWPA_AF_xxx */
  int ai_socktype;      /*!< i.e. LWPA_STREAM or LWPA_DGRAM */
  int ai_protocol;      /*!< i.e. LWPA_IPPROTO_xxx */
  char *ai_canonname;   /*!< Canonical name for host */
  LwpaSockaddr ai_addr; /*!< Address of host */
  void *pd[2];          /*!< Used by internal platform logic; don't touch */
} LwpaAddrinfo;

lwpa_error_t lwpa_getaddrinfo(const char *hostname, const char *service, const LwpaAddrinfo *hints,
                              LwpaAddrinfo *result);

bool lwpa_nextaddr(LwpaAddrinfo *ai);

/* Call with any of the LwpaAddrinfos in the list to free the whole list */
void lwpa_freeaddrinfo(LwpaAddrinfo *ai);

/************************* Mimic inet_xtox() API *****************************/

/*! Maximum length of the string representation of an IPv4 address. */
#define LWPA_INET_ADDRSTRLEN 16
/*! Maximum length of the string representation of an IPv6 address. */
#define LWPA_INET6_ADDRSTRLEN 46

lwpa_error_t lwpa_inet_ntop(const LwpaIpAddr *src, char *dest, size_t size);
lwpa_error_t lwpa_inet_pton(lwpa_iptype_t type, const char *src, LwpaIpAddr *dest);

#ifdef __cplusplus
}
#endif

/*! @} */

#endif /* _LWPA_SOCKET_H_ */
