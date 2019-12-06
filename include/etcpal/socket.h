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

/* etcpal/socket.h: Platform-neutral BSD-modeled network socket implementation. */

#ifndef ETCPAL_SOCKET_H_
#define ETCPAL_SOCKET_H_

#include <stddef.h>
#include "etcpal/common.h"
#include "etcpal/bool.h"
#include "etcpal/error.h"
#include "etcpal/inet.h"

/*!
 * \defgroup etcpal_socket Network Socket Interface (socket)
 * \ingroup etcpal
 * \brief Platform-neutral BSD-modeled network socket implementation.
 *
 * ```c
 * #include "etcpal/socket.h"
 * ```
 *
 * This module attempts to abstract the network socket interface from various platforms into one
 * platform-neutral interface. The functions and flags are modeled after the BSD sockets
 * interface. <em>UNIX Network Programming: The Sockets Networking API</em> by Stevens, Fenner,
 * and Rudoff is highly recommended reading for those unfamiliar with this interface.
 *
 * @{
 */

/*! Event flags for the etcpal_poll_*() API functions. */
typedef uint32_t etcpal_poll_events_t;

#include "etcpal/os_socket.h" /* .he os-specific socket definitions */

/* clang-format off */

/*!
 * \name Flags for etcpal_recvfrom()
 * @{
 */
#define ETCPAL_MSG_PEEK 0x1
/*!
 * @}
 */

/* Note: no flags are currently implemented for etcpal_sendto() */

/*!
 * \name Level values for etcpal_setsockopt() and etcpal_getsockopt()
 * Refer to the group of option names for each level.
 * @{
 */
#define ETCPAL_SOL_SOCKET     0 /*!< etcpal_socket API level. */
#define ETCPAL_IPPROTO_IP     1 /*!< IPv4 protocol level. */
#define ETCPAL_IPPROTO_ICMPV6 2 /*!< ICMPv6 protocol level. */
#define ETCPAL_IPPROTO_IPV6   3 /*!< IPv6 protocol level. */
#define ETCPAL_IPPROTO_TCP    4 /*!< TCP protocol level. */
#define ETCPAL_IPPROTO_UDP    5 /*!< UDP protocol level. */
/*!
 * @}
 */

/*!
 * \name Options for level ETCPAL_SOL_SOCKET
 * Used in the option parameter to etcpal_setsockopt() and etcpal_getsockopt().
 * Refer to the similarly-named option on your favorite man page for more details.
 * @{
 */
#define ETCPAL_SO_BROADCAST 0  /*!< Get/Set, value is boolean int */
#define ETCPAL_SO_ERROR     1  /*!< Get only, value is int representing platform error value */
#define ETCPAL_SO_KEEPALIVE 2  /*!< Get/Set, value is boolean int */
#define ETCPAL_SO_LINGER    3  /*!< Get/Set, value is EtcPalLinger */
#define ETCPAL_SO_RCVBUF    4  /*!< Get/Set, value is int representing byte size */
#define ETCPAL_SO_SNDBUF    5  /*!< Get/Set, value is int representing byte size */
#define ETCPAL_SO_RCVTIMEO  6  /*!< Get/Set, value is int representing ms */
#define ETCPAL_SO_SNDTIMEO  7  /*!< Get/Set, value is int representing ms */
#define ETCPAL_SO_REUSEADDR 8  /*!< Get/Set, value is boolean int */
#define ETCPAL_SO_REUSEPORT 9  /*!< Get/Set, value is boolean int */
#define ETCPAL_SO_TYPE      10 /*!< Get only, value is int */
/*!
 * @}
 */

/*!
 * \name Options for level ETCPAL_IPPROTO_IP or ETCPAL_IPPROTO_IPV6
 * Used in the option parameter to etcpal_setsockopt() and etcpal_getsockopt().
 * Refer to the similarly-named option on your favorite man page for more details.
 * @{
 */

/*! Get/Set, value is int. Value indicates TTL for IPv4 or hop limit for IPv6. */
#define ETCPAL_IP_TTL             11
/*! Get/Set, value is unsigned int representing interface index (see \ref interface_indexes). */
#define ETCPAL_IP_MULTICAST_IF    12
/*! Get/Set, value is int. Value indicates TTL for IPv4 or hop limit for IPv6. */
#define ETCPAL_IP_MULTICAST_TTL   13
#define ETCPAL_IP_MULTICAST_LOOP  14 /*!< Get/Set, value is boolean int */
/*! [Legacy IPv4-only option, use of #ETCPAL_MCAST_JOIN_GROUP is preferred] Set only, value is
 *  EtcPalMreq. */
#define ETCPAL_IP_ADD_MEMBERSHIP  15
/*! [Legacy IPv4-only option, use of #ETCPAL_MCAST_LEAVE_GROUP is preferred] Set only, value is
 *  EtcPalMreq. */
#define ETCPAL_IP_DROP_MEMBERSHIP 16
#define ETCPAL_MCAST_JOIN_GROUP   17 /*!< Set only, value is EtcPalGroupReq */
#define ETCPAL_MCAST_LEAVE_GROUP  18 /*!< Set only, value is EtcPalGroupReq */
#define ETCPAL_IPV6_V6ONLY        19 /*!< Get/Set, value is boolean int */

/*!
 * @}
 */

/* clang-format on */

/* ETCPAL_IPPROTO_TCP: TODO */

/*! Option value for #ETCPAL_SO_LINGER. */
typedef struct EtcPalLinger
{
  int onoff;  /*!< 0 = off, nonzero = on */
  int linger; /*!< Linger time in seconds */
} EtcPalLinger;

/*! Option value for #ETCPAL_IP_ADD_MEMBERSHIP and #ETCPAL_IP_DROP_MEMBERSHIP. */
typedef struct EtcPalMreq
{
  /*! Address of network interface on which to join the multicast group. */
  EtcPalIpAddr netint;
  /*! Multicast group to join. */
  EtcPalIpAddr group;
} EtcPalMreq;

/*! Option value for #ETCPAL_MCAST_JOIN_GROUP and #ETCPAL_MCAST_LEAVE_GROUP. */
typedef struct EtcPalGroupReq
{
  /*! Index of network interface on which to join the multicast group (see
   *  \ref interface_indexes). */
  unsigned int ifindex;
  /*! Multicast group to join. */
  EtcPalIpAddr group;
} EtcPalGroupReq;

/*!
 * \name 'how' values for etcpal_shutdown()
 * @{
 */
#define ETCPAL_SHUT_RD 0
#define ETCPAL_SHUT_WR 1
#define ETCPAL_SHUT_RDWR 2
/*!
 * @}
 */

/*!
 * \name 'family' values for etcpal_socket() and EtcPalAddrinfo
 * @{
 */
#define ETCPAL_AF_UNSPEC 0u
#define ETCPAL_AF_INET 1u
#define ETCPAL_AF_INET6 2u
/*!
 * @}
 */

/*!
 * \name 'type' values for etcpal_socket() and etcpal_getsockopt()
 * @{
 */
#define ETCPAL_STREAM 0u
#define ETCPAL_DGRAM 1u
/*!
 * @}
 */

/********************** Mimic sys/socket.h functions *************************/

#ifdef __cplusplus
extern "C" {
#endif

etcpal_error_t etcpal_accept(etcpal_socket_t id, EtcPalSockAddr* address, etcpal_socket_t* conn_sock);
etcpal_error_t etcpal_bind(etcpal_socket_t id, const EtcPalSockAddr* address);
etcpal_error_t etcpal_close(etcpal_socket_t id);
etcpal_error_t etcpal_connect(etcpal_socket_t id, const EtcPalSockAddr* address);
etcpal_error_t etcpal_getpeername(etcpal_socket_t id, EtcPalSockAddr* address);
etcpal_error_t etcpal_getsockname(etcpal_socket_t id, EtcPalSockAddr* address);
etcpal_error_t etcpal_getsockopt(etcpal_socket_t id, int level, int option_name, void* option_value,
                                 size_t* option_len);
etcpal_error_t etcpal_listen(etcpal_socket_t id, int backlog);
int etcpal_recv(etcpal_socket_t id, void* buffer, size_t length, int flags);
int etcpal_recvfrom(etcpal_socket_t id, void* buffer, size_t length, int flags, EtcPalSockAddr* address);
/* recvmsg - not implemented */
int etcpal_send(etcpal_socket_t id, const void* message, size_t length, int flags);
/* sendmsg - not implemented */
int etcpal_sendto(etcpal_socket_t id, const void* message, size_t length, int flags, const EtcPalSockAddr* dest_addr);
etcpal_error_t etcpal_setsockopt(etcpal_socket_t id, int level, int option_name, const void* option_value,
                                 size_t option_len);
etcpal_error_t etcpal_shutdown(etcpal_socket_t id, int how);
etcpal_error_t etcpal_socket(unsigned int family, unsigned int type, etcpal_socket_t* id);
/* socketpair - not implemented */

/**************************** Mimic fcntl() API ******************************/

etcpal_error_t etcpal_setblocking(etcpal_socket_t id, bool blocking);
etcpal_error_t etcpal_getblocking(etcpal_socket_t id, bool* blocking);

/**************************** Mimic poll() API *******************************/

/*!
 * \name Flag values to use with etcpal_poll_events_t.
 * @{
 */
#define ETCPAL_POLL_IN 0x1u      /*!< Notify when data is available for reading on the socket. */
#define ETCPAL_POLL_OUT 0x2u     /*!< Notify when the socket has space available to write data. */
#define ETCPAL_POLL_CONNECT 0x4u /*!< Notify when a non-blocking connect operation has completed. */
#define ETCPAL_POLL_OOB 0x8u     /*!< Notify when there is out-of-band data on a TCP socket. */
#define ETCPAL_POLL_ERR 0x10u    /*!< An error has occurred on the socket (output only). */
/*!
 * @}
 */

/*! Mask of valid events for use with etcpal_poll_add_socket(). */
#define ETCPAL_POLL_VALID_INPUT_EVENT_MASK 0x0fu

/*! A description of an event that occurred on a socket, for usage with etcpal_poll_wait(). */
typedef struct EtcPalPollEvent
{
  etcpal_socket_t socket;      /*!< Socket which had activity. */
  etcpal_poll_events_t events; /*!< Event(s) that occurred on the socket. */
  etcpal_error_t err;          /*!< More information about an error that occurred on the socket. */
  void* user_data;             /*!< The user data that was given when this socket was added. */
} EtcPalPollEvent;

etcpal_error_t etcpal_poll_context_init(EtcPalPollContext* context);
void etcpal_poll_context_deinit(EtcPalPollContext* context);
etcpal_error_t etcpal_poll_add_socket(EtcPalPollContext* context, etcpal_socket_t socket, etcpal_poll_events_t events,
                                      void* user_data);
etcpal_error_t etcpal_poll_modify_socket(EtcPalPollContext* context, etcpal_socket_t socket,
                                         etcpal_poll_events_t new_events, void* new_user_data);
void etcpal_poll_remove_socket(EtcPalPollContext* context, etcpal_socket_t socket);
etcpal_error_t etcpal_poll_wait(EtcPalPollContext* context, EtcPalPollEvent* event, int timeout_ms);

/************************ Mimic getaddrinfo() API ****************************/

/*!
 * \name 'flags' values for EtcPalAddrinfo
 * Refer to the similarly-named flags in your favorite getaddrinfo() man page for more details.
 * @{
 */
#define ETCPAL_AI_PASSIVE 0x01
#define ETCPAL_AI_CANONNAME 0x02
#define ETCPAL_AI_NUMERICHOST 0x04
/*!
 * @}
 */

/*! A structure containing name and address information about an internet host. Returned by
 *  etcpal_getaddrinfo(). */
typedef struct EtcPalAddrinfo
{
  int ai_flags;           /*!< i.e. ETCPAL_AI_xxx */
  int ai_family;          /*!< i.e. ETCPAL_AF_xxx */
  int ai_socktype;        /*!< i.e. ETCPAL_STREAM or ETCPAL_DGRAM */
  int ai_protocol;        /*!< i.e. ETCPAL_IPPROTO_xxx */
  char* ai_canonname;     /*!< Canonical name for host */
  EtcPalSockAddr ai_addr; /*!< Address of host */
  void* pd[2];            /*!< Used by internal platform logic; don't touch */
} EtcPalAddrinfo;

etcpal_error_t etcpal_getaddrinfo(const char* hostname, const char* service, const EtcPalAddrinfo* hints,
                                  EtcPalAddrinfo* result);

bool etcpal_nextaddr(EtcPalAddrinfo* ai);

/* Call with any of the EtcPalAddrinfos in the list to free the whole list */
void etcpal_freeaddrinfo(EtcPalAddrinfo* ai);

#ifdef __cplusplus
}
#endif

/*!
 * @}
 */

#endif /* ETCPAL_SOCKET_H_ */
