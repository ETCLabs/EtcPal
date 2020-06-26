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

#include "etcpal/socket.h"

#include <string.h>
#include <stdlib.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>

#include "etcpal/common.h"
#include "etcpal/private/socket.h"
#include "os_error.h"

/*************************** Private constants *******************************/

#define POLL_CONTEXT_ARR_CHUNK_SIZE 10

/****************************** Private types ********************************/

/* A struct to track sockets being polled by the etcpal_poll() API */
typedef struct EtcPalPollSocket
{
  // 'sock' must always remain as the first member in the struct to facilitate an EtcPalRbTree lookup
  // shortcut
  etcpal_socket_t      sock;
  etcpal_poll_events_t events;
  void*                user_data;
} EtcPalPollSocket;

/***************************** Private macros ********************************/

#define ETCPAL_FD_ZERO(setptr) \
  FD_ZERO(&(setptr)->set);     \
  (setptr)->count = 0

#define ETCPAL_FD_SET(sock, setptr) \
  FD_SET(sock, &(setptr)->set);     \
  (setptr)->count++

#define ETCPAL_FD_CLEAR(sock, setptr) \
  FD_CLR(sock, &(setptr)->set);       \
  (setptr)->count--

#define ETCPAL_FD_ISSET(sock, setptr) FD_ISSET(sock, &(setptr)->set)

/**************************** Private variables ******************************/

#if !defined(ETCPAL_BUILDING_MOCK_LIB)

/* clang-format off */

#define ETCPAL_NUM_SHUT 3
static const int shutmap[ETCPAL_NUM_SHUT] =
{
  SD_RECEIVE,
  SD_SEND,
  SD_BOTH
};

#define ETCPAL_NUM_AF 3
static const int sfmap[ETCPAL_NUM_AF] =
{
  AF_UNSPEC,
  AF_INET,
  AF_INET6
};

#define ETCPAL_NUM_TYPE 2
static const int stmap[ETCPAL_NUM_TYPE] =
{
  SOCK_STREAM,
  SOCK_DGRAM
};

#define ETCPAL_NUM_AIF 8
static const int aiflagmap[ETCPAL_NUM_AIF] =
{
  0,
  AI_PASSIVE,
  AI_CANONNAME,
  AI_PASSIVE | AI_CANONNAME,
  AI_NUMERICHOST,
  AI_PASSIVE | AI_NUMERICHOST,
  AI_CANONNAME | AI_NUMERICHOST,
  AI_PASSIVE | AI_CANONNAME | AI_NUMERICHOST
};

static const int aifammap[ETCPAL_NUM_AF] =
{
  AF_UNSPEC,
  AF_INET,
  AF_INET6
};

#define ETCPAL_NUM_IPPROTO 6
static const int aiprotmap[ETCPAL_NUM_IPPROTO] =
{
  0,
  IPPROTO_IP,
  IPPROTO_ICMPV6,
  IPPROTO_IPV6,
  IPPROTO_TCP,
  IPPROTO_UDP
};

/* clang-format on */

/*********************** Private function prototypes *************************/

static int setsockopt_socket(etcpal_socket_t id, int option_name, const void* option_value, size_t option_len);
static int setsockopt_ip(etcpal_socket_t id, int option_name, const void* option_value, size_t option_len);
static int setsockopt_ip6(etcpal_socket_t id, int option_name, const void* option_value, size_t option_len);

// Helper functions for the etcpal_poll API
static void           set_in_fd_sets(EtcPalPollContext* context, const EtcPalPollSocket* sock);
static void           clear_in_fd_sets(EtcPalPollContext* context, const EtcPalPollSocket* sock);
static etcpal_error_t handle_select_result(EtcPalPollContext*     context,
                                           EtcPalPollEvent*       event,
                                           const EtcPalPollFdSet* readfds,
                                           const EtcPalPollFdSet* writefds,
                                           const EtcPalPollFdSet* exceptfds);

static int           poll_socket_compare(const EtcPalRbTree* tree, const void* value_a, const void* value_b);
static EtcPalRbNode* poll_socket_alloc(void);
static void          poll_socket_free(EtcPalRbNode* node);

/*************************** Function definitions ****************************/

etcpal_error_t etcpal_socket_init(void)
{
  WSADATA wsdata;
  WORD    wsver = MAKEWORD(2, 2);
  int     startup_res = WSAStartup(wsver, &wsdata);
  if (startup_res != 0)
  {
    return err_winsock_to_etcpal(startup_res);
  }
  return kEtcPalErrOk;
}

void etcpal_socket_deinit(void)
{
  WSACleanup();
}

etcpal_error_t etcpal_accept(etcpal_socket_t id, EtcPalSockAddr* address, etcpal_socket_t* conn_sock)
{
  struct sockaddr_storage ss;
  int                     sa_size = sizeof ss;
  SOCKET                  res;

  if (!conn_sock)
    return kEtcPalErrInvalid;

  res = accept(id, (struct sockaddr*)&ss, &sa_size);

  if (res != INVALID_SOCKET)
  {
    if (address && !sockaddr_os_to_etcpal((etcpal_os_sockaddr_t*)&ss, address))
    {
      closesocket(res);
      return kEtcPalErrSys;
    }
    *conn_sock = res;
    return kEtcPalErrOk;
  }
  return err_winsock_to_etcpal(WSAGetLastError());
}

etcpal_error_t etcpal_bind(etcpal_socket_t id, const EtcPalSockAddr* address)
{
  struct sockaddr_storage ss;
  size_t                  sa_size;
  int                     res;

  if (!address)
    return kEtcPalErrInvalid;

  sa_size = sockaddr_etcpal_to_os(address, (etcpal_os_sockaddr_t*)&ss);
  if (sa_size == 0)
    return kEtcPalErrInvalid;

  res = bind(id, (struct sockaddr*)&ss, (int)sa_size);
  return (res == 0 ? kEtcPalErrOk : err_winsock_to_etcpal(WSAGetLastError()));
}

etcpal_error_t etcpal_close(etcpal_socket_t id)
{
  int res = closesocket(id);
  return (res == 0 ? kEtcPalErrOk : err_winsock_to_etcpal(WSAGetLastError()));
}

etcpal_error_t etcpal_connect(etcpal_socket_t id, const EtcPalSockAddr* address)
{
  struct sockaddr_storage ss;
  size_t                  sa_size;
  int                     res;

  if (!address)
    return kEtcPalErrInvalid;

  sa_size = sockaddr_etcpal_to_os(address, (etcpal_os_sockaddr_t*)&ss);
  if (sa_size == 0)
    return kEtcPalErrInvalid;

  res = connect(id, (struct sockaddr*)&ss, (int)sa_size);
  return (res == 0 ? kEtcPalErrOk : err_winsock_to_etcpal(WSAGetLastError()));
}

etcpal_error_t etcpal_getpeername(etcpal_socket_t id, EtcPalSockAddr* address)
{
  /* TODO */
  ETCPAL_UNUSED_ARG(id);
  ETCPAL_UNUSED_ARG(address);
  return kEtcPalErrNotImpl;
}

etcpal_error_t etcpal_getsockname(etcpal_socket_t id, EtcPalSockAddr* address)
{
  int                     res;
  struct sockaddr_storage ss;
  socklen_t               size = sizeof ss;

  if (!address)
    return kEtcPalErrInvalid;

  res = getsockname(id, (struct sockaddr*)&ss, &size);
  if (res == 0)
  {
    if (!sockaddr_os_to_etcpal((etcpal_os_sockaddr_t*)&ss, address))
      return kEtcPalErrSys;
    return kEtcPalErrOk;
  }
  return err_winsock_to_etcpal(WSAGetLastError());
}

etcpal_error_t etcpal_getsockopt(etcpal_socket_t id, int level, int option_name, void* option_value, size_t* option_len)
{
  /* TODO */
  ETCPAL_UNUSED_ARG(id);
  ETCPAL_UNUSED_ARG(level);
  ETCPAL_UNUSED_ARG(option_name);
  ETCPAL_UNUSED_ARG(option_value);
  ETCPAL_UNUSED_ARG(option_len);
  return kEtcPalErrNotImpl;
}

etcpal_error_t etcpal_listen(etcpal_socket_t id, int backlog)
{
  int res = listen(id, backlog);
  return (res == 0 ? kEtcPalErrOk : err_winsock_to_etcpal(WSAGetLastError()));
}

int etcpal_recv(etcpal_socket_t id, void* buffer, size_t length, int flags)
{
  int res;
  int impl_flags = (flags & ETCPAL_MSG_PEEK) ? MSG_PEEK : 0;

  if (!buffer)
    return kEtcPalErrInvalid;

  res = recv(id, buffer, (int)length, impl_flags);
  return (res >= 0 ? res : (int)err_winsock_to_etcpal(WSAGetLastError()));
}

int etcpal_recvfrom(etcpal_socket_t id, void* buffer, size_t length, int flags, EtcPalSockAddr* address)
{
  int                     res;
  int                     impl_flags = (flags & ETCPAL_MSG_PEEK) ? MSG_PEEK : 0;
  struct sockaddr_storage fromaddr;
  socklen_t               fromlen = sizeof fromaddr;

  if (!buffer)
    return (int)kEtcPalErrInvalid;

  res = recvfrom(id, buffer, (int)length, impl_flags, (struct sockaddr*)&fromaddr, &fromlen);

  if (res >= 0)
  {
    if (address && fromlen > 0)
    {
      if (!sockaddr_os_to_etcpal((etcpal_os_sockaddr_t*)&fromaddr, address))
        return kEtcPalErrSys;
    }
    return res;
  }
  return (int)err_winsock_to_etcpal(WSAGetLastError());
}

int etcpal_send(etcpal_socket_t id, const void* message, size_t length, int flags)
{
  ETCPAL_UNUSED_ARG(flags);

  if (!message)
    return kEtcPalErrInvalid;

  int res = send(id, message, (int)length, 0);
  return (res >= 0 ? res : (int)err_winsock_to_etcpal(WSAGetLastError()));
}

int etcpal_sendto(etcpal_socket_t id, const void* message, size_t length, int flags, const EtcPalSockAddr* dest_addr)
{
  ETCPAL_UNUSED_ARG(flags);

  if (!dest_addr || !message)
    return (int)kEtcPalErrInvalid;

  int                     res = -1;
  struct sockaddr_storage ss;
  size_t                  ss_size = sockaddr_etcpal_to_os(dest_addr, (etcpal_os_sockaddr_t*)&ss);
  if (ss_size > 0)
    res = sendto(id, message, (int)length, 0, (struct sockaddr*)&ss, (int)ss_size);

  return (res >= 0 ? res : (int)err_winsock_to_etcpal(WSAGetLastError()));
}

etcpal_error_t etcpal_setsockopt(etcpal_socket_t id,
                                 int             level,
                                 int             option_name,
                                 const void*     option_value,
                                 size_t          option_len)
{
  int res = -1;

  if (!option_value)
    return kEtcPalErrInvalid;

  // TODO this OS implementation could be simplified by use of socket option lookup arrays.
  switch (level)
  {
    case ETCPAL_SOL_SOCKET:
      res = setsockopt_socket(id, option_name, option_value, option_len);
      break;
    case ETCPAL_IPPROTO_IP:
      res = setsockopt_ip(id, option_name, option_value, option_len);
      break;
    case ETCPAL_IPPROTO_IPV6:
      res = setsockopt_ip6(id, option_name, option_value, option_len);
      break;
    default:
      return kEtcPalErrInvalid;
  }
  return (res == 0 ? kEtcPalErrOk : err_winsock_to_etcpal(WSAGetLastError()));
}

int setsockopt_socket(etcpal_socket_t id, int option_name, const void* option_value, size_t option_len)
{
  switch (option_name)
  {
    case ETCPAL_SO_RCVBUF:
      return setsockopt(id, SOL_SOCKET, SO_RCVBUF, option_value, (int)option_len);
    case ETCPAL_SO_SNDBUF:
      return setsockopt(id, SOL_SOCKET, SO_SNDBUF, option_value, (int)option_len);
    case ETCPAL_SO_RCVTIMEO:
      if (option_len == sizeof(int))
      {
        DWORD val = (DWORD) * (int*)option_value;
        return setsockopt(id, SOL_SOCKET, SO_RCVTIMEO, (char*)&val, sizeof val);
      }
      break;
    case ETCPAL_SO_SNDTIMEO:
      if (option_len == sizeof(int))
      {
        DWORD val = (DWORD) * (int*)option_value;
        return setsockopt(id, SOL_SOCKET, SO_SNDTIMEO, (char*)&val, sizeof val);
      }
      break;
    case ETCPAL_SO_REUSEADDR:
      if (option_len == sizeof(int))
      {
        BOOL val = (BOOL) * (int*)option_value;
        return setsockopt(id, SOL_SOCKET, SO_REUSEADDR, (char*)&val, sizeof val);
      }
      break;
    case ETCPAL_SO_BROADCAST:
      if (option_len == sizeof(int))
      {
        BOOL val = (BOOL) * (int*)option_value;
        return setsockopt(id, SOL_SOCKET, SO_BROADCAST, (char*)&val, sizeof val);
      }
      break;
    case ETCPAL_SO_KEEPALIVE:
      if (option_len == sizeof(int))
      {
        BOOL val = (BOOL) * (int*)option_value;
        return setsockopt(id, SOL_SOCKET, SO_KEEPALIVE, (char*)&val, sizeof val);
      }
      break;
    case ETCPAL_SO_LINGER:
      if (option_len == sizeof(EtcPalLinger))
      {
        EtcPalLinger* ll = (EtcPalLinger*)option_value;
        struct linger val;
        val.l_onoff = (u_short)ll->onoff;
        val.l_linger = (u_short)ll->linger;
        return setsockopt(id, SOL_SOCKET, SO_LINGER, (char*)&val, sizeof val);
      }
      break;
    case ETCPAL_SO_ERROR:     /* Set not supported */
    case ETCPAL_SO_REUSEPORT: /* Not supported on this OS. */
    case ETCPAL_SO_TYPE:      /* Not supported */
    default:
      break;
  }
  // If we got here, something was invalid. Set errno accordingly
  WSASetLastError(WSAEINVAL);
  return -1;
}

int setsockopt_ip(etcpal_socket_t id, int option_name, const void* option_value, size_t option_len)
{
  switch (option_name)
  {
    case ETCPAL_IP_TTL:
      if (option_len == sizeof(int))
      {
        DWORD val = (DWORD) * (int*)option_value;
        return setsockopt(id, IPPROTO_IP, IP_TTL, (char*)&val, sizeof val);
      }
      break;
    case ETCPAL_IP_ADD_MEMBERSHIP:
      if (option_len == sizeof(EtcPalMreq))
      {
        EtcPalMreq* amreq = (EtcPalMreq*)option_value;
        if (ETCPAL_IP_IS_V4(&amreq->group))
        {
          struct ip_mreq val;
          val.imr_multiaddr.s_addr = htonl(ETCPAL_IP_V4_ADDRESS(&amreq->group));
          val.imr_interface.s_addr = htonl(ETCPAL_IP_V4_ADDRESS(&amreq->netint));
          return setsockopt(id, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&val, sizeof val);
        }
      }
      break;
    case ETCPAL_IP_DROP_MEMBERSHIP:
      if (option_len == sizeof(EtcPalMreq))
      {
        EtcPalMreq* amreq = (EtcPalMreq*)option_value;
        if (ETCPAL_IP_IS_V4(&amreq->group))
        {
          struct ip_mreq val;
          val.imr_multiaddr.s_addr = htonl(ETCPAL_IP_V4_ADDRESS(&amreq->group));
          val.imr_interface.s_addr = htonl(ETCPAL_IP_V4_ADDRESS(&amreq->netint));
          return setsockopt(id, IPPROTO_IP, IP_DROP_MEMBERSHIP, (char*)&val, sizeof val);
        }
      }
      break;
    case ETCPAL_MCAST_JOIN_GROUP:
      if (option_len == sizeof(EtcPalGroupReq))
      {
        EtcPalGroupReq* greq = (EtcPalGroupReq*)option_value;
        if (ETCPAL_IP_IS_V4(&greq->group) && greq->ifindex >= 0)
        {
          struct group_req val;
          val.gr_interface = (ULONG)greq->ifindex;

          memset(&val.gr_group, 0, sizeof val.gr_group);
          struct sockaddr_in* sin = (struct sockaddr_in*)&val.gr_group;
          sin->sin_family = AF_INET;
          sin->sin_addr.s_addr = htonl(ETCPAL_IP_V4_ADDRESS(&greq->group));
          return setsockopt(id, IPPROTO_IP, MCAST_JOIN_GROUP, (char*)&val, sizeof val);
        }
      }
      break;
    case ETCPAL_MCAST_LEAVE_GROUP:
      if (option_len == sizeof(EtcPalGroupReq))
      {
        EtcPalGroupReq* greq = (EtcPalGroupReq*)option_value;
        if (ETCPAL_IP_IS_V4(&greq->group) && greq->ifindex >= 0)
        {
          struct group_req val;
          val.gr_interface = (ULONG)greq->ifindex;

          memset(&val.gr_group, 0, sizeof val.gr_group);
          struct sockaddr_in* sin = (struct sockaddr_in*)&val.gr_group;
          sin->sin_family = AF_INET;
          sin->sin_addr.s_addr = htonl(ETCPAL_IP_V4_ADDRESS(&greq->group));
          return setsockopt(id, IPPROTO_IP, MCAST_LEAVE_GROUP, (char*)&val, sizeof val);
        }
      }
      break;
    case ETCPAL_IP_MULTICAST_IF:
      if (option_len == sizeof(unsigned int))
      {
        unsigned int ifindex = *(unsigned int*)option_value;
        // On Windows, this socket option takes either an IPv4 address or an interface index in
        // network byte order. We use the latter option.
        DWORD val = htonl(ifindex);
        return setsockopt(id, IPPROTO_IP, IP_MULTICAST_IF, (char*)&val, sizeof val);
      }
      break;
    case ETCPAL_IP_MULTICAST_TTL:
      if (option_len == sizeof(int))
      {
        DWORD val = (DWORD) * (int*)option_value;
        return setsockopt(id, IPPROTO_IP, IP_MULTICAST_TTL, (char*)&val, sizeof val);
      }
      break;
    case ETCPAL_IP_MULTICAST_LOOP:
      if (option_len == sizeof(int))
      {
        DWORD val = (DWORD) * (int*)option_value;
        return setsockopt(id, IPPROTO_IP, IP_MULTICAST_LOOP, (char*)&val, sizeof val);
      }
      break;
    default:
      break;
  }
  // If we got here, something was invalid. Set errno accordingly
  WSASetLastError(WSAEINVAL);
  return -1;
}

int setsockopt_ip6(etcpal_socket_t id, int option_name, const void* option_value, size_t option_len)
{
  switch (option_name)
  {
    case ETCPAL_MCAST_JOIN_GROUP:
      if (option_len == sizeof(EtcPalGroupReq))
      {
        EtcPalGroupReq* greq = (EtcPalGroupReq*)option_value;
        if (ETCPAL_IP_IS_V6(&greq->group) && greq->ifindex >= 0)
        {
          struct group_req val;
          val.gr_interface = (ULONG)greq->ifindex;

          memset(&val.gr_group, 0, sizeof val.gr_group);
          struct sockaddr_in6* sin6 = (struct sockaddr_in6*)&val.gr_group;
          sin6->sin6_family = AF_INET6;
          memcpy(sin6->sin6_addr.s6_addr, ETCPAL_IP_V6_ADDRESS(&greq->group), ETCPAL_IPV6_BYTES);
          return setsockopt(id, IPPROTO_IPV6, MCAST_JOIN_GROUP, (char*)&val, sizeof val);
        }
      }
      break;
    case ETCPAL_MCAST_LEAVE_GROUP:
      if (option_len == sizeof(EtcPalGroupReq))
      {
        EtcPalGroupReq* greq = (EtcPalGroupReq*)option_value;
        if (ETCPAL_IP_IS_V6(&greq->group) && greq->ifindex >= 0)
        {
          struct group_req val;
          val.gr_interface = (ULONG)greq->ifindex;

          memset(&val.gr_group, 0, sizeof val.gr_group);
          struct sockaddr_in6* sin6 = (struct sockaddr_in6*)&val.gr_group;
          sin6->sin6_family = AF_INET6;
          memcpy(sin6->sin6_addr.s6_addr, ETCPAL_IP_V6_ADDRESS(&greq->group), ETCPAL_IPV6_BYTES);
          return setsockopt(id, IPPROTO_IPV6, MCAST_LEAVE_GROUP, (char*)&val, sizeof val);
        }
      }
      break;
    case ETCPAL_IP_MULTICAST_IF:
      if (option_len == sizeof(unsigned int))
      {
        DWORD val = (DWORD) * (unsigned int*)option_value;
        return setsockopt(id, IPPROTO_IPV6, IPV6_MULTICAST_IF, (char*)&val, sizeof val);
      }
      break;
    case ETCPAL_IP_MULTICAST_TTL:
      if (option_len == sizeof(int))
      {
        DWORD val = (DWORD) * (int*)option_value;
        return setsockopt(id, IPPROTO_IPV6, IPV6_MULTICAST_HOPS, (char*)&val, sizeof val);
      }
      break;
    case ETCPAL_IP_MULTICAST_LOOP:
      if (option_len == sizeof(int))
      {
        DWORD val = (DWORD) * (int*)option_value;
        return setsockopt(id, IPPROTO_IPV6, IPV6_MULTICAST_LOOP, (char*)&val, sizeof val);
      }
      break;
    case ETCPAL_IPV6_V6ONLY:
      if (option_len == sizeof(int))
      {
        DWORD val = (DWORD) * (int*)option_value;
        return setsockopt(id, IPPROTO_IPV6, IPV6_V6ONLY, (char*)&val, sizeof val);
      }
      break;
    default: /* Other IPv6 options TODO on windows. */
      break;
  }
  // If we got here, something was invalid. Set errno accordingly
  WSASetLastError(WSAEINVAL);
  return -1;
}

etcpal_error_t etcpal_shutdown(etcpal_socket_t id, int how)
{
  if (how >= 0 && how < ETCPAL_NUM_SHUT)
  {
    int res = shutdown(id, shutmap[how]);
    return (res == 0 ? kEtcPalErrOk : err_winsock_to_etcpal(WSAGetLastError()));
  }
  return kEtcPalErrInvalid;
}

etcpal_error_t etcpal_socket(unsigned int family, unsigned int type, etcpal_socket_t* id)
{
  if (id)
  {
    if (family < ETCPAL_NUM_AF && type < ETCPAL_NUM_TYPE)
    {
      SOCKET sock = socket(sfmap[family], stmap[type], 0);
      if (sock != INVALID_SOCKET)
      {
        *id = sock;
        return kEtcPalErrOk;
      }
      else
      {
        *id = ETCPAL_SOCKET_INVALID;
        return err_winsock_to_etcpal(WSAGetLastError());
      }
    }
    else
    {
      *id = ETCPAL_SOCKET_INVALID;
    }
  }
  return kEtcPalErrInvalid;
}

etcpal_error_t etcpal_setblocking(etcpal_socket_t id, bool blocking)
{
  unsigned long val = (blocking ? 0 : 1);
  int           res = ioctlsocket(id, FIONBIO, &val);
  return (res == 0 ? kEtcPalErrOk : err_winsock_to_etcpal(WSAGetLastError()));
}

etcpal_error_t etcpal_getblocking(etcpal_socket_t id, bool* blocking)
{
  ETCPAL_UNUSED_ARG(id);
  ETCPAL_UNUSED_ARG(blocking);
  return kEtcPalErrNotImpl;
}

etcpal_error_t etcpal_poll_context_init(EtcPalPollContext* context)
{
  if (!context)
    return kEtcPalErrInvalid;

  if (!etcpal_mutex_create(&context->lock))
    return kEtcPalErrSys;

  etcpal_rbtree_init(&context->sockets, poll_socket_compare, poll_socket_alloc, poll_socket_free);
  ETCPAL_FD_ZERO(&context->readfds);
  ETCPAL_FD_ZERO(&context->writefds);
  ETCPAL_FD_ZERO(&context->exceptfds);
  context->valid = true;
  return kEtcPalErrOk;
}

void etcpal_poll_context_deinit(EtcPalPollContext* context)
{
  if (!context || !context->valid)
    return;

  etcpal_rbtree_clear(&context->sockets);
  etcpal_mutex_destroy(&context->lock);
  context->valid = false;
}

void set_in_fd_sets(EtcPalPollContext* context, const EtcPalPollSocket* sock_desc)
{
  if (sock_desc->events & ETCPAL_POLL_IN)
  {
    ETCPAL_FD_SET(sock_desc->sock, &context->readfds);
  }
  if (sock_desc->events & (ETCPAL_POLL_OUT | ETCPAL_POLL_CONNECT))
  {
    ETCPAL_FD_SET(sock_desc->sock, &context->writefds);
  }
  if (sock_desc->events & (ETCPAL_POLL_OOB | ETCPAL_POLL_CONNECT))
  {
    ETCPAL_FD_SET(sock_desc->sock, &context->exceptfds);
  }
}

void clear_in_fd_sets(EtcPalPollContext* context, const EtcPalPollSocket* sock_desc)
{
  if (sock_desc->events & ETCPAL_POLL_IN)
  {
    ETCPAL_FD_CLEAR(sock_desc->sock, &context->readfds);
  }
  if (sock_desc->events & (ETCPAL_POLL_OUT | ETCPAL_POLL_CONNECT))
  {
    ETCPAL_FD_CLEAR(sock_desc->sock, &context->writefds);
  }
  if (sock_desc->events & (ETCPAL_POLL_OOB | ETCPAL_POLL_CONNECT))
  {
    ETCPAL_FD_CLEAR(sock_desc->sock, &context->exceptfds);
  }
}

etcpal_error_t etcpal_poll_add_socket(EtcPalPollContext*   context,
                                      etcpal_socket_t      socket,
                                      etcpal_poll_events_t events,
                                      void*                user_data)
{
  if (!context || !context->valid || socket == ETCPAL_SOCKET_INVALID || !(events & ETCPAL_POLL_VALID_INPUT_EVENT_MASK))
    return kEtcPalErrInvalid;

  etcpal_error_t res = kEtcPalErrSys;
  if (etcpal_mutex_lock(&context->lock))
  {
    if (etcpal_rbtree_size(&context->sockets) >= ETCPAL_SOCKET_MAX_POLL_SIZE)
    {
      res = kEtcPalErrNoMem;
    }
    else
    {
      EtcPalPollSocket* new_sock = (EtcPalPollSocket*)malloc(sizeof(EtcPalPollSocket));
      if (new_sock)
      {
        new_sock->sock = socket;
        new_sock->events = events;
        new_sock->user_data = user_data;
        res = etcpal_rbtree_insert(&context->sockets, new_sock);
        if (res == kEtcPalErrOk)
        {
          set_in_fd_sets(context, new_sock);
        }
        else
        {
          free(new_sock);
        }
      }
      else
      {
        res = kEtcPalErrNoMem;
      }
    }
    etcpal_mutex_unlock(&context->lock);
  }
  return res;
}

etcpal_error_t etcpal_poll_modify_socket(EtcPalPollContext*   context,
                                         etcpal_socket_t      socket,
                                         etcpal_poll_events_t new_events,
                                         void*                new_user_data)
{
  if (!context || !context->valid || socket == ETCPAL_SOCKET_INVALID ||
      !(new_events & ETCPAL_POLL_VALID_INPUT_EVENT_MASK))
    return kEtcPalErrInvalid;

  etcpal_error_t res = kEtcPalErrSys;
  if (etcpal_mutex_lock(&context->lock))
  {
    EtcPalPollSocket* sock_desc = (EtcPalPollSocket*)etcpal_rbtree_find(&context->sockets, &socket);
    if (sock_desc)
    {
      clear_in_fd_sets(context, sock_desc);
      sock_desc->events = new_events;
      sock_desc->user_data = new_user_data;
      set_in_fd_sets(context, sock_desc);
      res = kEtcPalErrOk;
    }
    else
    {
      res = kEtcPalErrNotFound;
    }
    etcpal_mutex_unlock(&context->lock);
  }
  return res;
}

void etcpal_poll_remove_socket(EtcPalPollContext* context, etcpal_socket_t socket)
{
  if (!context || !context->valid || socket == ETCPAL_SOCKET_INVALID)
    return;

  if (etcpal_mutex_lock(&context->lock))
  {
    EtcPalPollSocket* sock_desc = (EtcPalPollSocket*)etcpal_rbtree_find(&context->sockets, &socket);
    if (sock_desc)
    {
      clear_in_fd_sets(context, sock_desc);
      etcpal_rbtree_remove(&context->sockets, sock_desc);
    }
    etcpal_mutex_unlock(&context->lock);
  }
}

etcpal_error_t etcpal_poll_wait(EtcPalPollContext* context, EtcPalPollEvent* event, int timeout_ms)
{
  if (!context || !context->valid || !event)
    return kEtcPalErrInvalid;

  // Get the sets of sockets that we will select on.
  EtcPalPollFdSet readfds, writefds, exceptfds;
  ETCPAL_FD_ZERO(&readfds);
  ETCPAL_FD_ZERO(&writefds);
  ETCPAL_FD_ZERO(&exceptfds);
  if (etcpal_mutex_lock(&context->lock))
  {
    if (etcpal_rbtree_size(&context->sockets) > 0)
    {
      readfds = context->readfds;
      writefds = context->writefds;
      exceptfds = context->exceptfds;
    }
    etcpal_mutex_unlock(&context->lock);
  }

  // No valid sockets are currently added to the context.
  if (!readfds.count && !writefds.count && !exceptfds.count)
    return kEtcPalErrNoSockets;

  struct timeval os_timeout;
  if (timeout_ms == 0)
  {
    os_timeout.tv_sec = 0;
    os_timeout.tv_usec = 0;
  }
  else if (timeout_ms != ETCPAL_WAIT_FOREVER)
  {
    os_timeout.tv_sec = timeout_ms / 1000;
    os_timeout.tv_usec = (timeout_ms % 1000) * 1000;
  }

  int sel_res = select(0, readfds.count ? &readfds.set : NULL, writefds.count ? &writefds.set : NULL,
                       exceptfds.count ? &exceptfds.set : NULL, timeout_ms == ETCPAL_WAIT_FOREVER ? NULL : &os_timeout);

  if (sel_res < 0)
  {
    return err_winsock_to_etcpal(WSAGetLastError());
  }
  else if (sel_res == 0)
  {
    return kEtcPalErrTimedOut;
  }
  else
  {
    etcpal_error_t res = kEtcPalErrSys;
    if (context->valid && etcpal_mutex_lock(&context->lock))
    {
      res = handle_select_result(context, event, &readfds, &writefds, &exceptfds);
      etcpal_mutex_unlock(&context->lock);
    }
    return res;
  }
}

etcpal_error_t handle_select_result(EtcPalPollContext*     context,
                                    EtcPalPollEvent*       event,
                                    const EtcPalPollFdSet* readfds,
                                    const EtcPalPollFdSet* writefds,
                                    const EtcPalPollFdSet* exceptfds)
{
  // Init the event data.
  event->socket = ETCPAL_SOCKET_INVALID;
  event->events = 0;
  event->err = kEtcPalErrOk;

  // The default return; if we don't find any sockets set that we passed to select(), something has
  // gone wrong.
  etcpal_error_t res = kEtcPalErrSys;

  EtcPalRbIter iter;
  etcpal_rbiter_init(&iter);
  for (EtcPalPollSocket* sock_desc = (EtcPalPollSocket*)etcpal_rbiter_first(&iter, &context->sockets); sock_desc;
       sock_desc = (EtcPalPollSocket*)etcpal_rbiter_next(&iter))
  {
    if (sock_desc->sock == ETCPAL_SOCKET_INVALID)
      continue;

    if (ETCPAL_FD_ISSET(sock_desc->sock, readfds) || ETCPAL_FD_ISSET(sock_desc->sock, writefds) ||
        ETCPAL_FD_ISSET(sock_desc->sock, exceptfds))
    {
      res = kEtcPalErrOk;
      event->socket = sock_desc->sock;
      event->user_data = sock_desc->user_data;

      /* Check for errors */
      int error;
      int error_size = sizeof(error);
      if (getsockopt(sock_desc->sock, SOL_SOCKET, SO_ERROR, (char*)&error, &error_size) == 0)
      {
        if (error != 0)
        {
          event->events |= ETCPAL_POLL_ERR;
          event->err = err_winsock_to_etcpal(error);
        }
      }
      else
      {
        res = err_winsock_to_etcpal(WSAGetLastError());
        break;
      }
      if (ETCPAL_FD_ISSET(sock_desc->sock, readfds))
      {
        if (sock_desc->events & ETCPAL_POLL_IN)
          event->events |= ETCPAL_POLL_IN;
      }
      if (ETCPAL_FD_ISSET(sock_desc->sock, writefds))
      {
        if (sock_desc->events & ETCPAL_POLL_CONNECT)
          event->events |= ETCPAL_POLL_CONNECT;
        else if (sock_desc->events & ETCPAL_POLL_OUT)
          event->events |= ETCPAL_POLL_OUT;
      }
      if (ETCPAL_FD_ISSET(sock_desc->sock, exceptfds))
      {
        if (sock_desc->events & ETCPAL_POLL_CONNECT)
          event->events |= ETCPAL_POLL_CONNECT;  // Async connect errors are handled above
        else if (sock_desc->events & ETCPAL_POLL_OOB)
          event->events |= ETCPAL_POLL_OOB;
      }
      break;  // We handle one event at a time.
    }
  }
  return res;
}

int poll_socket_compare(const EtcPalRbTree* tree, const void* value_a, const void* value_b)
{
  ETCPAL_UNUSED_ARG(tree);

  const EtcPalPollSocket* a = (const EtcPalPollSocket*)value_a;
  const EtcPalPollSocket* b = (const EtcPalPollSocket*)value_b;

  return (a->sock > b->sock) - (a->sock < b->sock);
}

EtcPalRbNode* poll_socket_alloc(void)
{
  return (EtcPalRbNode*)malloc(sizeof(EtcPalRbNode));
}

void poll_socket_free(EtcPalRbNode* node)
{
  if (node)
  {
    free(node->value);
    free(node);
  }
}

etcpal_error_t etcpal_getaddrinfo(const char*           hostname,
                                  const char*           service,
                                  const EtcPalAddrinfo* hints,
                                  EtcPalAddrinfo*       result)
{
  int              res;
  struct addrinfo* pf_res;
  struct addrinfo  pf_hints;

  if ((!hostname && !service) || !result)
    return kEtcPalErrInvalid;

  memset(&pf_hints, 0, sizeof pf_hints);
  if (hints)
  {
    pf_hints.ai_flags = (hints->ai_flags < ETCPAL_NUM_AIF) ? aiflagmap[hints->ai_flags] : 0;
    pf_hints.ai_family = (hints->ai_family < ETCPAL_NUM_AF) ? aifammap[hints->ai_family] : AF_UNSPEC;
    pf_hints.ai_socktype = (hints->ai_socktype < ETCPAL_NUM_TYPE) ? stmap[hints->ai_socktype] : 0;
    pf_hints.ai_protocol = (hints->ai_protocol < ETCPAL_NUM_IPPROTO) ? aiprotmap[hints->ai_protocol] : 0;
  }

  res = getaddrinfo(hostname, service, hints ? &pf_hints : NULL, &pf_res);
  if (res == 0)
  {
    result->pd[0] = pf_res;
    result->pd[1] = pf_res;
    if (!etcpal_nextaddr(result))
      res = -1;
  }
  return (res == 0 ? kEtcPalErrOk : err_winsock_to_etcpal(res));
}

bool etcpal_nextaddr(EtcPalAddrinfo* ai)
{
  if (ai && ai->pd[1])
  {
    struct addrinfo* pf_ai = (struct addrinfo*)ai->pd[1];
    ai->ai_flags = 0;
    if (!sockaddr_os_to_etcpal((etcpal_os_sockaddr_t*)pf_ai->ai_addr, &ai->ai_addr))
      return false;

    /* Can't use reverse maps, because we have no guarantee of the numeric values of the OS
     * constants. Ugh. */
    if (pf_ai->ai_family == AF_INET)
      ai->ai_family = ETCPAL_AF_INET;
    else if (pf_ai->ai_family == AF_INET6)
      ai->ai_family = ETCPAL_AF_INET6;
    else
      ai->ai_family = ETCPAL_AF_UNSPEC;

    if (pf_ai->ai_socktype == SOCK_DGRAM)
      ai->ai_socktype = ETCPAL_SOCK_DGRAM;
    else if (pf_ai->ai_socktype == SOCK_STREAM)
      ai->ai_socktype = ETCPAL_SOCK_STREAM;
    else
      ai->ai_socktype = 0;

    if (pf_ai->ai_protocol == IPPROTO_UDP)
      ai->ai_protocol = ETCPAL_IPPROTO_UDP;
    else if (pf_ai->ai_protocol == IPPROTO_TCP)
      ai->ai_protocol = ETCPAL_IPPROTO_TCP;
    else
      ai->ai_protocol = 0;

    ai->ai_canonname = pf_ai->ai_canonname;
    ai->pd[1] = pf_ai->ai_next;

    return true;
  }
  return false;
}

void etcpal_freeaddrinfo(EtcPalAddrinfo* ai)
{
  if (ai)
    freeaddrinfo((struct addrinfo*)ai->pd[0]);
}

#endif  // !defined(ETCPAL_BUILDING_MOCK_LIB)
