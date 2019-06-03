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

#include "lwpa/socket.h"

#include <string.h>
#include <stdlib.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>

/*************************** Private constants *******************************/

#define POLL_CONTEXT_ARR_CHUNK_SIZE 10

#define LWPA_FD_ZERO(setptr) \
  FD_ZERO(&(setptr)->set);   \
  (setptr)->count = 0

#define LWPA_FD_SET(sock, setptr) \
  FD_SET(sock, &(setptr)->set);   \
  (setptr)->count++

#define LWPA_FD_CLEAR(sock, setptr) \
  FD_CLR(sock, &(setptr)->set);     \
  (setptr)->count--

#define LWPA_FD_ISSET(sock, setptr) FD_ISSET(sock, &(setptr)->set)

/**************************** Private variables ******************************/

/* clang-format off */

#define LWPA_NUM_SHUT 3
static const int shutmap[LWPA_NUM_SHUT] =
{
  SD_RECEIVE,
  SD_SEND,
  SD_BOTH
};

#define LWPA_NUM_AF 3
static const int sfmap[LWPA_NUM_AF] =
{
  AF_UNSPEC,
  AF_INET,
  AF_INET6
};

#define LWPA_NUM_TYPE 2
static const int stmap[LWPA_NUM_TYPE] =
{
  SOCK_STREAM,
  SOCK_DGRAM
};

#define LWPA_NUM_AIF 8
static const int aiflagmap[LWPA_NUM_AIF] =
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

static const int aifammap[LWPA_NUM_AF] =
{
  AF_UNSPEC,
  AF_INET,
  AF_INET6
};

#define LWPA_NUM_IPPROTO 6
static const int aiprotmap[LWPA_NUM_IPPROTO] =
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

// Convert Windows sockets errors to lwpa_error_t values.
static lwpa_error_t err_os_to_lwpa(int wsaerror);

// Helper functions for the lwpa_poll API
static void init_socket_chunk(LwpaPollCtxSocket* chunk);
static LwpaPollCtxSocket* find_socket(LwpaPollContext* context, lwpa_socket_t socket);
static LwpaPollCtxSocket* find_hole(LwpaPollContext* context);
static void set_in_fd_sets(LwpaPollContext* context, const LwpaPollCtxSocket* sock);
static void clear_in_fd_sets(LwpaPollContext* context, const LwpaPollCtxSocket* sock);
static lwpa_error_t handle_select_result(LwpaPollContext* context, LwpaPollEvent* event, const LwpaPollFdSet* readfds,
                                         const LwpaPollFdSet* writefds, const LwpaPollFdSet* exceptfds);

/*************************** Function definitions ****************************/

bool sockaddr_os_to_lwpa(LwpaSockaddr* sa, const struct sockaddr* pfsa)
{
  if (pfsa->sa_family == AF_INET)
  {
    struct sockaddr_in* sin = (struct sockaddr_in*)pfsa;
    sa->port = ntohs(sin->sin_port);
    lwpaip_set_v4_address(&sa->ip, ntohl(sin->sin_addr.s_addr));
    return true;
  }
  else if (pfsa->sa_family == AF_INET6)
  {
    struct sockaddr_in6* sin6 = (struct sockaddr_in6*)pfsa;
    sa->port = ntohs(sin6->sin6_port);
    lwpaip_set_v6_address(&sa->ip, sin6->sin6_addr.s6_addr);
    return true;
  }
  return false;
}

size_t sockaddr_lwpa_to_os(struct sockaddr* pfsa, const LwpaSockaddr* sa)
{
  size_t ret = 0;
  if (lwpaip_is_v4(&sa->ip))
  {
    struct sockaddr_in* sin = (struct sockaddr_in*)pfsa;
    memset(sin, 0, sizeof(struct sockaddr_in));
    sin->sin_family = AF_INET;
    sin->sin_port = htons(sa->port);
    sin->sin_addr.s_addr = htonl(lwpaip_v4_address(&sa->ip));
    ret = sizeof(struct sockaddr_in);
  }
  else if (lwpaip_is_v6(&sa->ip))
  {
    struct sockaddr_in6* sin6 = (struct sockaddr_in6*)pfsa;
    memset(sin6, 0, sizeof(struct sockaddr_in6));
    sin6->sin6_family = AF_INET6;
    sin6->sin6_port = htons(sa->port);
    memcpy(sin6->sin6_addr.s6_addr, lwpaip_v6_address(&sa->ip), LWPA_IPV6_BYTES);
    ret = sizeof(struct sockaddr_in6);
    in6addr_any;
  }
  return ret;
}

#if !defined(LWPA_BUILDING_MOCK_LIB)

lwpa_error_t err_os_to_lwpa(int wsaerror)
{
  /* The Winsock error codes are not even close to contiguous in defined value,
   * so a giant switch statement is the only solution here... */
  switch (wsaerror)
  {
    case WSAEBADF:
    case WSAENOTSOCK:
    case WSATYPE_NOT_FOUND:
    case WSAHOST_NOT_FOUND:
    case WSAESTALE:
      return kLwpaErrNotFound;
    case WSA_INVALID_HANDLE:
    case WSA_INVALID_PARAMETER:
    case WSAEFAULT:
    case WSAEINVAL:
    case WSAEDESTADDRREQ:
    case WSAENOPROTOOPT:
      return kLwpaErrInvalid;
    case WSA_NOT_ENOUGH_MEMORY:
    case WSAEMFILE:
    case WSAETOOMANYREFS:
    case WSAEPROCLIM:
    case WSAEUSERS:
      return kLwpaErrNoMem;
    case WSA_IO_PENDING:
    case WSAEINPROGRESS:
      return kLwpaErrInProgress;
    case WSA_IO_INCOMPLETE:
    case WSAEALREADY:
      return kLwpaErrAlready;
    case WSAEWOULDBLOCK:
      return kLwpaErrWouldBlock;
    case WSAEMSGSIZE:
      return kLwpaErrMsgSize;
    case WSAEADDRINUSE:
      return kLwpaErrAddrInUse;
    case WSAEADDRNOTAVAIL:
      return kLwpaErrAddrNotAvail;
    case WSAENETDOWN:
    case WSAENETUNREACH:
    case WSAENETRESET:
    case WSAEHOSTDOWN:
    case WSAEHOSTUNREACH:
      return kLwpaErrNetwork;
    case WSAECONNRESET:
    case WSAECONNABORTED:
      return kLwpaErrConnReset;
    case WSAEISCONN:
      return kLwpaErrIsConn;
    case WSAENOTCONN:
      return kLwpaErrNotConn;
    case WSAESHUTDOWN:
      return kLwpaErrShutdown;
    case WSAETIMEDOUT:
      return kLwpaErrTimedOut;
    case WSAECONNREFUSED:
      return kLwpaErrConnRefused;
    case WSAENOBUFS:
      return kLwpaErrBufSize;
    case WSANOTINITIALISED:
    case WSASYSNOTREADY:
      return kLwpaErrNotInit;
    case WSAEACCES:
    case WSA_OPERATION_ABORTED:
    case WSAEPROTOTYPE:
    case WSAEPROTONOSUPPORT:
    case WSAESOCKTNOSUPPORT:
    case WSAEOPNOTSUPP:
    case WSAEPFNOSUPPORT:
    case WSAEAFNOSUPPORT:
    case WSASYSCALLFAILURE:
    default:
      return kLwpaErrSys;
  }
}

lwpa_error_t lwpa_accept(lwpa_socket_t id, LwpaSockaddr* address, lwpa_socket_t* conn_sock)
{
  struct sockaddr_storage ss;
  int sa_size = sizeof ss;
  SOCKET res;

  if (!conn_sock)
    return kLwpaErrInvalid;

  res = accept(id, (struct sockaddr*)&ss, &sa_size);

  if (res != INVALID_SOCKET)
  {
    if (address && !sockaddr_os_to_lwpa(address, (struct sockaddr*)&ss))
    {
      closesocket(res);
      return kLwpaErrSys;
    }
    *conn_sock = res;
    return kLwpaErrOk;
  }
  return err_os_to_lwpa(WSAGetLastError());
}

lwpa_error_t lwpa_bind(lwpa_socket_t id, const LwpaSockaddr* address)
{
  struct sockaddr_storage ss;
  size_t sa_size;
  int res;

  if (!address)
    return kLwpaErrInvalid;

  sa_size = sockaddr_lwpa_to_os((struct sockaddr*)&ss, address);
  if (sa_size == 0)
    return kLwpaErrInvalid;

  res = bind(id, (struct sockaddr*)&ss, (int)sa_size);
  return (res == 0 ? kLwpaErrOk : err_os_to_lwpa(WSAGetLastError()));
}

lwpa_error_t lwpa_close(lwpa_socket_t id)
{
  int res = closesocket(id);
  return (res == 0 ? kLwpaErrOk : err_os_to_lwpa(WSAGetLastError()));
}

lwpa_error_t lwpa_connect(lwpa_socket_t id, const LwpaSockaddr* address)
{
  struct sockaddr_storage ss;
  size_t sa_size;
  int res;

  if (!address)
    return kLwpaErrInvalid;

  sa_size = sockaddr_lwpa_to_os((struct sockaddr*)&ss, address);
  if (sa_size == 0)
    return kLwpaErrInvalid;

  res = connect(id, (struct sockaddr*)&ss, (int)sa_size);
  return (res == 0 ? kLwpaErrOk : err_os_to_lwpa(WSAGetLastError()));
}

lwpa_error_t lwpa_getpeername(lwpa_socket_t id, LwpaSockaddr* address)
{
  /* TODO */
  (void)id;
  (void)address;
  return kLwpaErrNotImpl;
}

lwpa_error_t lwpa_getsockname(lwpa_socket_t id, LwpaSockaddr* address)
{
  int res;
  struct sockaddr_storage ss;
  socklen_t size = sizeof ss;

  if (!address)
    return kLwpaErrInvalid;

  res = getsockname(id, (struct sockaddr*)&ss, &size);
  if (res == 0)
  {
    if (!sockaddr_os_to_lwpa(address, (struct sockaddr*)&ss))
      return kLwpaErrSys;
    return kLwpaErrOk;
  }
  return err_os_to_lwpa(WSAGetLastError());
}

lwpa_error_t lwpa_getsockopt(lwpa_socket_t id, int level, int option_name, void* option_value, size_t* option_len)
{
  /* TODO */
  (void)id;
  (void)level;
  (void)option_name;
  (void)option_value;
  (void)option_len;
  return kLwpaErrNotImpl;
}

lwpa_error_t lwpa_listen(lwpa_socket_t id, int backlog)
{
  int res = listen(id, backlog);
  return (res == 0 ? kLwpaErrOk : err_os_to_lwpa(WSAGetLastError()));
}

int lwpa_recv(lwpa_socket_t id, void* buffer, size_t length, int flags)
{
  int res;
  int impl_flags = (flags & LWPA_MSG_PEEK) ? MSG_PEEK : 0;

  if (!buffer)
    return kLwpaErrInvalid;

  res = recv(id, buffer, (int)length, impl_flags);
  return (res >= 0 ? res : (int)err_os_to_lwpa(WSAGetLastError()));
}

int lwpa_recvfrom(lwpa_socket_t id, void* buffer, size_t length, int flags, LwpaSockaddr* address)
{
  int res;
  int impl_flags = (flags & LWPA_MSG_PEEK) ? MSG_PEEK : 0;
  struct sockaddr_storage fromaddr;
  socklen_t fromlen = sizeof fromaddr;

  if (!buffer)
    return (int)kLwpaErrInvalid;

  res = recvfrom(id, buffer, (int)length, impl_flags, (struct sockaddr*)&fromaddr, &fromlen);

  if (res >= 0)
  {
    if (address && fromlen > 0)
    {
      if (!sockaddr_os_to_lwpa(address, (struct sockaddr*)&fromaddr))
        return kLwpaErrSys;
    }
    return res;
  }
  return (int)err_os_to_lwpa(WSAGetLastError());
}

int lwpa_send(lwpa_socket_t id, const void* message, size_t length, int flags)
{
  int res;
  (void)flags;

  if (!message)
    return kLwpaErrInvalid;

  res = send(id, message, (int)length, 0);
  return (res >= 0 ? res : (int)err_os_to_lwpa(WSAGetLastError()));
}

int lwpa_sendto(lwpa_socket_t id, const void* message, size_t length, int flags, const LwpaSockaddr* dest_addr)
{
  int res = -1;
  size_t ss_size;
  struct sockaddr_storage ss;
  (void)flags;

  if (!dest_addr || !message)
    return (int)kLwpaErrInvalid;

  if ((ss_size = sockaddr_lwpa_to_os((struct sockaddr*)&ss, dest_addr)) > 0)
    res = sendto(id, message, (int)length, 0, (struct sockaddr*)&ss, (int)ss_size);

  return (res >= 0 ? res : (int)err_os_to_lwpa(WSAGetLastError()));
}

lwpa_error_t lwpa_setsockopt(lwpa_socket_t id, int level, int option_name, const void* option_value, size_t option_len)
{
  int res = -1;

  if (!option_value)
    return kLwpaErrInvalid;

  /* TODO this OS implementation could be simplified by use of socket option lookup arrays. */
  switch (level)
  {
    case LWPA_SOL_SOCKET:
      switch (option_name)
      {
        case LWPA_SO_RCVBUF:
          res = setsockopt(id, SOL_SOCKET, SO_RCVBUF, option_value, (int)option_len);
          break;
        case LWPA_SO_SNDBUF:
          res = setsockopt(id, SOL_SOCKET, SO_SNDBUF, option_value, (int)option_len);
          break;
        case LWPA_SO_RCVTIMEO:
          if (option_len == sizeof(int))
          {
            DWORD val = (DWORD) * (int*)option_value;
            res = setsockopt(id, SOL_SOCKET, SO_RCVTIMEO, (char*)&val, sizeof val);
          }
          break;
        case LWPA_SO_SNDTIMEO:
          if (option_len == sizeof(int))
          {
            DWORD val = (DWORD) * (int*)option_value;
            res = setsockopt(id, SOL_SOCKET, SO_SNDTIMEO, (char*)&val, sizeof val);
          }
          break;
        case LWPA_SO_REUSEADDR:
          if (option_len == sizeof(int))
          {
            BOOL val = (BOOL) * (int*)option_value;
            res = setsockopt(id, SOL_SOCKET, SO_REUSEADDR, (char*)&val, sizeof val);
          }
          break;
        case LWPA_SO_BROADCAST:
          if (option_len == sizeof(int))
          {
            BOOL val = (BOOL) * (int*)option_value;
            res = setsockopt(id, SOL_SOCKET, SO_BROADCAST, (char*)&val, sizeof val);
          }
          break;
        case LWPA_SO_KEEPALIVE:
          if (option_len == sizeof(int))
          {
            BOOL val = (BOOL) * (int*)option_value;
            res = setsockopt(id, SOL_SOCKET, SO_KEEPALIVE, (char*)&val, sizeof val);
          }
          break;
        case LWPA_SO_LINGER:
          if (option_len == sizeof(LwpaLinger))
          {
            LwpaLinger* ll = (LwpaLinger*)option_value;
            struct linger val;
            val.l_onoff = (u_short)ll->onoff;
            val.l_linger = (u_short)ll->linger;
            res = setsockopt(id, SOL_SOCKET, SO_LINGER, (char*)&val, sizeof val);
          }
          break;
        case LWPA_SO_ERROR: /* Set not supported */
        case LWPA_SO_TYPE:  /* Not supported */
        default:
          return kLwpaErrInvalid;
      }
      break;
    case LWPA_IPPROTO_IP:
      switch (option_name)
      {
        case LWPA_IP_TTL:
          if (option_len == sizeof(int))
          {
            DWORD val = (DWORD) * (int*)option_value;
            res = setsockopt(id, IPPROTO_IP, IP_TTL, (char*)&val, sizeof val);
          }
          break;
        case LWPA_MCAST_JOIN_GROUP:
          if (option_len == sizeof(LwpaMreq))
          {
            LwpaMreq* amreq = (LwpaMreq*)option_value;
            if (lwpaip_is_v4(&amreq->group))
            {
              struct ip_mreq val;
              val.imr_multiaddr.s_addr = htonl(lwpaip_v4_address(&amreq->group));
              val.imr_interface.s_addr = htonl(lwpaip_v4_address(&amreq->netint));
              res = setsockopt(id, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&val, sizeof val);
            }
          }
          break;
        case LWPA_MCAST_LEAVE_GROUP:
          if (option_len == sizeof(LwpaMreq))
          {
            LwpaMreq* amreq = (LwpaMreq*)option_value;
            if (lwpaip_is_v4(&amreq->group))
            {
              struct ip_mreq val;
              val.imr_multiaddr.s_addr = htonl(lwpaip_v4_address(&amreq->group));
              val.imr_interface.s_addr = htonl(lwpaip_v4_address(&amreq->netint));
              res = setsockopt(id, IPPROTO_IP, IP_DROP_MEMBERSHIP, (char*)&val, sizeof val);
            }
          }
          break;
        case LWPA_IP_MULTICAST_IF:
          if (option_len == sizeof(LwpaIpAddr))
          {
            LwpaIpAddr* netint = (LwpaIpAddr*)option_value;
            if (lwpaip_is_v4(netint))
            {
              DWORD val = htonl(lwpaip_v4_address(netint));
              res = setsockopt(id, IPPROTO_IP, IP_MULTICAST_IF, (char*)&val, sizeof val);
            }
          }
          break;
        case LWPA_IP_MULTICAST_TTL:
          if (option_len == sizeof(int))
          {
            DWORD val = (DWORD) * (int*)option_value;
            res = setsockopt(id, IPPROTO_IP, IP_MULTICAST_TTL, (char*)&val, sizeof val);
          }
          break;
        case LWPA_IP_MULTICAST_LOOP:
          if (option_len == sizeof(int))
          {
            DWORD val = (DWORD) * (int*)option_value;
            res = setsockopt(id, IPPROTO_IP, IP_MULTICAST_LOOP, (char*)&val, sizeof val);
          }
          break;
        default:
          return kLwpaErrInvalid;
      }
      break;
    case LWPA_IPPROTO_IPV6:
      switch (option_name)
      {
        case LWPA_MCAST_JOIN_GROUP:
          /* TODO on windows.
          if (option_len == sizeof(struct lwpa_mreq))
          {
            LwpaMreq *amreq = (LwpaMreq *)option_value;
            if (lwpaip_is_v6(&amreq->group))
            {
              struct ipv6_mreq val;
              val.ipv6imr_interface = 0;
              memcpy(&val.ipv6imr_multiaddr.s6_addr,
                     lwpaip_v6_address(&amreq->group), LWPA_IPV6_BYTES);
              res = setsockopt(id, IPPROTO_IPV6, MCAST_JOIN_GROUP, &val,
                               sizeof val);
            }
          }
          */
          break;
        case LWPA_MCAST_LEAVE_GROUP:
          /* TODO on windows.
          if (option_len == sizeof(struct lwpa_mreq))
          {
            LwpaMreq *amreq = (LwpaMreq *)option_value;
            if (lwpaip_is_v6(&amreq->group))
            {
              struct ipv6_mreq val;
              val.ipv6imr_interface = 0;
              memcpy(&val.ipv6imr_multiaddr.s6_addr,
                     lwpaip_v6_address(&amreq->group), LWPA_IPV6_BYTES);
              res = setsockopt(id, IPPROTO_IPV6, MCAST_JOIN_GROUP, &val,
                               sizeof val);
            }
          }
          */
          break;
        case LWPA_IPV6_V6ONLY:
          if (option_len == sizeof(int))
          {
            DWORD val = (DWORD) * (int*)option_value;
            res = setsockopt(id, IPPROTO_IPV6, IPV6_V6ONLY, (char*)&val, sizeof val);
          }
          break;
        default: /* Other IPv6 options TODO on windows. */
          return kLwpaErrInvalid;
      }
      break;
    case LWPA_SO_REUSEPORT: /* Not supported on this OS. */
    default:
      return kLwpaErrInvalid;
  }
  return (res == 0 ? kLwpaErrOk : err_os_to_lwpa(WSAGetLastError()));
}

lwpa_error_t lwpa_shutdown(lwpa_socket_t id, int how)
{
  if (how >= 0 && how < LWPA_NUM_SHUT)
  {
    int res = shutdown(id, shutmap[how]);
    return (res == 0 ? kLwpaErrOk : err_os_to_lwpa(WSAGetLastError()));
  }
  return kLwpaErrInvalid;
}

lwpa_error_t lwpa_socket(unsigned int family, unsigned int type, lwpa_socket_t* id)
{
  if (id)
  {
    if (family < LWPA_NUM_AF && type < LWPA_NUM_TYPE)
    {
      SOCKET sock = socket(sfmap[family], stmap[type], 0);
      if (sock != INVALID_SOCKET)
      {
        *id = sock;
        return kLwpaErrOk;
      }
      else
      {
        *id = LWPA_SOCKET_INVALID;
        return err_os_to_lwpa(WSAGetLastError());
      }
    }
    else
    {
      *id = LWPA_SOCKET_INVALID;
    }
  }
  return kLwpaErrInvalid;
}

lwpa_error_t lwpa_setblocking(lwpa_socket_t id, bool blocking)
{
  unsigned long val = (blocking ? 0 : 1);
  int res = ioctlsocket(id, FIONBIO, &val);
  return (res == 0 ? kLwpaErrOk : err_os_to_lwpa(WSAGetLastError()));
}

lwpa_error_t lwpa_poll_context_init(LwpaPollContext* context)
{
  if (!context)
    return kLwpaErrInvalid;

  if (!lwpa_mutex_create(&context->lock))
    return kLwpaErrSys;

  LWPA_FD_ZERO(&context->readfds);
  LWPA_FD_ZERO(&context->writefds);
  LWPA_FD_ZERO(&context->exceptfds);
  context->sockets = NULL;
  context->socket_arr_size = 0;
  context->num_valid_sockets = 0;
  context->valid = true;
  return kLwpaErrOk;
}

void lwpa_poll_context_deinit(LwpaPollContext* context)
{
  if (!context || !context->valid)
    return;

  if (context->sockets)
  {
    free(context->sockets);
  }
  lwpa_mutex_destroy(&context->lock);
  context->valid = false;
}

LwpaPollCtxSocket* find_socket(LwpaPollContext* context, lwpa_socket_t socket)
{
  for (LwpaPollCtxSocket* cur = context->sockets; cur < context->sockets + context->socket_arr_size; ++cur)
  {
    if (cur->socket == socket)
      return cur;
  }
  return NULL;
}

void init_socket_chunk(LwpaPollCtxSocket* chunk)
{
  for (LwpaPollCtxSocket* cur = chunk; cur < chunk + POLL_CONTEXT_ARR_CHUNK_SIZE; ++cur)
  {
    cur->socket = LWPA_SOCKET_INVALID;
  }
}

LwpaPollCtxSocket* find_hole(LwpaPollContext* context)
{
  LwpaPollCtxSocket* hole = NULL;

  if (!context->sockets)
  {
    // No sockets have been added yet. Create the first chunk.
    context->sockets = (LwpaPollCtxSocket*)calloc(POLL_CONTEXT_ARR_CHUNK_SIZE, sizeof(LwpaPollCtxSocket));
    if (context->sockets)
    {
      init_socket_chunk(context->sockets);
      context->socket_arr_size = POLL_CONTEXT_ARR_CHUNK_SIZE;
      hole = &context->sockets[0];
    }
  }
  else
  {
    // Find an invalid socket in the list
    for (LwpaPollCtxSocket* cur = context->sockets; cur < context->sockets + context->socket_arr_size; ++cur)
    {
      if (cur->socket == LWPA_SOCKET_INVALID)
      {
        hole = cur;
        break;
      }
    }
    // No hole found; need to do a realloc
    if (!hole)
    {
      // Expand the socket array by another chunk.
      size_t new_size = context->socket_arr_size + POLL_CONTEXT_ARR_CHUNK_SIZE;
      context->sockets = (LwpaPollCtxSocket*)realloc(context->sockets, new_size * sizeof(LwpaPollCtxSocket));
      if (context->sockets)
      {
        init_socket_chunk(&context->sockets[context->socket_arr_size]);
        hole = &context->sockets[context->socket_arr_size];
        context->socket_arr_size += POLL_CONTEXT_ARR_CHUNK_SIZE;
      }
    }
  }

  return hole;
}

void set_in_fd_sets(LwpaPollContext* context, const LwpaPollCtxSocket* sock)
{
  if (sock->events & LWPA_POLL_IN)
  {
    LWPA_FD_SET(sock->socket, &context->readfds);
  }
  if (sock->events & (LWPA_POLL_OUT | LWPA_POLL_CONNECT))
  {
    LWPA_FD_SET(sock->socket, &context->writefds);
  }
  if (sock->events & (LWPA_POLL_OOB | LWPA_POLL_CONNECT))
  {
    LWPA_FD_SET(sock->socket, &context->exceptfds);
  }
}

void clear_in_fd_sets(LwpaPollContext* context, const LwpaPollCtxSocket* sock)
{
  if (sock->events & LWPA_POLL_IN)
  {
    LWPA_FD_CLEAR(sock->socket, &context->readfds);
  }
  if (sock->events & (LWPA_POLL_OUT | LWPA_POLL_CONNECT))
  {
    LWPA_FD_CLEAR(sock->socket, &context->writefds);
  }
  if (sock->events & (LWPA_POLL_OOB | LWPA_POLL_CONNECT))
  {
    LWPA_FD_CLEAR(sock->socket, &context->exceptfds);
  }
}

lwpa_error_t lwpa_poll_add_socket(LwpaPollContext* context, lwpa_socket_t socket, lwpa_poll_events_t events,
                                  void* user_data)
{
  if (!context || !context->valid || socket == LWPA_SOCKET_INVALID || !(events & LWPA_POLL_VALID_INPUT_EVENT_MASK))
    return kLwpaErrInvalid;

  lwpa_error_t res = kLwpaErrSys;
  if (lwpa_mutex_take(&context->lock, LWPA_WAIT_FOREVER))
  {
    if (context->num_valid_sockets >= LWPA_SOCKET_MAX_POLL_SIZE)
    {
      res = kLwpaErrNoMem;
    }
    else
    {
      LwpaPollCtxSocket* new_sock = find_hole(context);
      if (new_sock)
      {
        new_sock->socket = socket;
        new_sock->events = events;
        new_sock->user_data = user_data;
        set_in_fd_sets(context, new_sock);
        context->num_valid_sockets++;
        res = kLwpaErrOk;
      }
      else
      {
        res = kLwpaErrNoMem;
      }
    }
    lwpa_mutex_give(&context->lock);
  }
  return res;
}

lwpa_error_t lwpa_poll_modify_socket(LwpaPollContext* context, lwpa_socket_t socket, lwpa_poll_events_t new_events,
                                     void* new_user_data)
{
  if (!context || !context->valid || socket == LWPA_SOCKET_INVALID || !(new_events & LWPA_POLL_VALID_INPUT_EVENT_MASK))
    return kLwpaErrInvalid;

  lwpa_error_t res = kLwpaErrSys;
  if (lwpa_mutex_take(&context->lock, LWPA_WAIT_FOREVER))
  {
    LwpaPollCtxSocket* sock_desc = find_socket(context, socket);
    if (sock_desc)
    {
      clear_in_fd_sets(context, sock_desc);
      sock_desc->events = new_events;
      sock_desc->user_data = new_user_data;
      set_in_fd_sets(context, sock_desc);
      res = kLwpaErrOk;
    }
    else
    {
      res = kLwpaErrNotFound;
    }
    lwpa_mutex_give(&context->lock);
  }
  return res;
}

void lwpa_poll_remove_socket(LwpaPollContext* context, lwpa_socket_t socket)
{
  if (!context || !context->valid || socket == LWPA_SOCKET_INVALID)
    return;

  if (lwpa_mutex_take(&context->lock, LWPA_WAIT_FOREVER))
  {
    LwpaPollCtxSocket* sock_desc = find_socket(context, socket);
    if (sock_desc)
    {
      clear_in_fd_sets(context, sock_desc);
      sock_desc->socket = LWPA_SOCKET_INVALID;
      context->num_valid_sockets--;
    }
    lwpa_mutex_give(&context->lock);
  }
}

lwpa_error_t lwpa_poll_wait(LwpaPollContext* context, LwpaPollEvent* event, int timeout_ms)
{
  if (!context || !context->valid || !event)
    return kLwpaErrInvalid;

  // Get the sets of sockets that we will select on.
  LwpaPollFdSet readfds, writefds, exceptfds;
  LWPA_FD_ZERO(&readfds);
  LWPA_FD_ZERO(&writefds);
  LWPA_FD_ZERO(&exceptfds);
  if (lwpa_mutex_take(&context->lock, LWPA_WAIT_FOREVER))
  {
    if (context->num_valid_sockets != 0)
    {
      readfds = context->readfds;
      writefds = context->writefds;
      exceptfds = context->exceptfds;
    }
    lwpa_mutex_give(&context->lock);
  }

  // No valid sockets are currently added to the context.
  if (!readfds.count && !writefds.count && !exceptfds.count)
    return kLwpaErrNoSockets;

  struct timeval os_timeout;
  if (timeout_ms == 0)
  {
    os_timeout.tv_sec = 0;
    os_timeout.tv_usec = 0;
  }
  else if (timeout_ms != LWPA_WAIT_FOREVER)
  {
    os_timeout.tv_sec = timeout_ms / 1000;
    os_timeout.tv_usec = (timeout_ms % 1000) * 1000;
  }

  int sel_res = select(0, readfds.count ? &readfds.set : NULL, writefds.count ? &writefds.set : NULL,
                       exceptfds.count ? &exceptfds.set : NULL, timeout_ms == LWPA_WAIT_FOREVER ? NULL : &os_timeout);

  if (sel_res < 0)
  {
    return err_os_to_lwpa(WSAGetLastError());
  }
  else if (sel_res == 0)
  {
    return kLwpaErrTimedOut;
  }
  else
  {
    lwpa_error_t res = kLwpaErrSys;
    if (context->valid && lwpa_mutex_take(&context->lock, LWPA_WAIT_FOREVER))
    {
      res = handle_select_result(context, event, &readfds, &writefds, &exceptfds);
      lwpa_mutex_give(&context->lock);
    }
    return res;
  }
}

lwpa_error_t handle_select_result(LwpaPollContext* context, LwpaPollEvent* event, const LwpaPollFdSet* readfds,
                                  const LwpaPollFdSet* writefds, const LwpaPollFdSet* exceptfds)
{
  // Init the event data.
  event->socket = LWPA_SOCKET_INVALID;
  event->events = 0;
  event->err = kLwpaErrOk;

  // The default return; if we don't find any sockets set that we passed to select(), something has
  // gone wrong.
  lwpa_error_t res = kLwpaErrSys;

  for (LwpaPollCtxSocket* sock_desc = context->sockets; sock_desc < context->sockets + context->socket_arr_size;
       ++sock_desc)
  {
    if (sock_desc->socket == LWPA_SOCKET_INVALID)
      continue;

    if (LWPA_FD_ISSET(sock_desc->socket, readfds) || LWPA_FD_ISSET(sock_desc->socket, writefds) ||
        LWPA_FD_ISSET(sock_desc->socket, exceptfds))
    {
      res = kLwpaErrOk;
      event->socket = sock_desc->socket;
      event->user_data = sock_desc->user_data;

      /* Check for errors */
      int error;
      int error_size = sizeof(error);
      if (getsockopt(sock_desc->socket, SOL_SOCKET, SO_ERROR, (char*)&error, &error_size) == 0)
      {
        if (error != 0)
        {
          event->events |= LWPA_POLL_ERR;
          event->err = err_os_to_lwpa(error);
        }
      }
      else
      {
        res = err_os_to_lwpa(WSAGetLastError());
        break;
      }
      if (LWPA_FD_ISSET(sock_desc->socket, readfds))
      {
        if (sock_desc->events & LWPA_POLL_IN)
          event->events |= LWPA_POLL_IN;
      }
      if (LWPA_FD_ISSET(sock_desc->socket, writefds))
      {
        if (sock_desc->events & LWPA_POLL_CONNECT)
          event->events |= LWPA_POLL_CONNECT;
        else if (sock_desc->events & LWPA_POLL_OUT)
          event->events |= LWPA_POLL_OUT;
      }
      if (LWPA_FD_ISSET(sock_desc->socket, exceptfds))
      {
        if (sock_desc->events & LWPA_POLL_CONNECT)
          event->events |= LWPA_POLL_CONNECT;  // Async connect errors are handled above
        else if (sock_desc->events & LWPA_POLL_OOB)
          event->events |= LWPA_POLL_OOB;
      }
      break;  // We handle one event at a time.
    }
  }
  return res;
}

lwpa_error_t lwpa_getaddrinfo(const char* hostname, const char* service, const LwpaAddrinfo* hints,
                              LwpaAddrinfo* result)
{
  int res;
  struct addrinfo* pf_res;
  struct addrinfo pf_hints;

  if ((!hostname && !service) || !result)
    return kLwpaErrInvalid;

  memset(&pf_hints, 0, sizeof pf_hints);
  if (hints)
  {
    pf_hints.ai_flags = (hints->ai_flags < LWPA_NUM_AIF) ? aiflagmap[hints->ai_flags] : 0;
    pf_hints.ai_family = (hints->ai_family < LWPA_NUM_AF) ? aifammap[hints->ai_family] : AF_UNSPEC;
    pf_hints.ai_socktype = (hints->ai_socktype < LWPA_NUM_TYPE) ? stmap[hints->ai_socktype] : 0;
    pf_hints.ai_protocol = (hints->ai_protocol < LWPA_NUM_IPPROTO) ? aiprotmap[hints->ai_protocol] : 0;
  }

  res = getaddrinfo(hostname, service, hints ? &pf_hints : NULL, &pf_res);
  if (res == 0)
  {
    result->pd[0] = pf_res;
    result->pd[1] = pf_res;
    if (!lwpa_nextaddr(result))
      res = -1;
  }
  return (res == 0 ? kLwpaErrOk : err_os_to_lwpa(res));
}

bool lwpa_nextaddr(LwpaAddrinfo* ai)
{
  if (ai && ai->pd[1])
  {
    struct addrinfo* pf_ai = (struct addrinfo*)ai->pd[1];
    ai->ai_flags = 0;
    if (!sockaddr_os_to_lwpa(&ai->ai_addr, pf_ai->ai_addr))
      return false;

    /* Can't use reverse maps, because we have no guarantee of the numeric values of the OS
     * constants. Ugh. */
    if (pf_ai->ai_family == AF_INET)
      ai->ai_family = LWPA_AF_INET;
    else if (pf_ai->ai_family == AF_INET6)
      ai->ai_family = LWPA_AF_INET6;
    else
      ai->ai_family = LWPA_AF_UNSPEC;

    if (pf_ai->ai_socktype == SOCK_DGRAM)
      ai->ai_socktype = LWPA_DGRAM;
    else if (pf_ai->ai_socktype == SOCK_STREAM)
      ai->ai_socktype = LWPA_STREAM;
    else
      ai->ai_socktype = 0;

    if (pf_ai->ai_protocol == IPPROTO_UDP)
      ai->ai_protocol = LWPA_IPPROTO_UDP;
    else if (pf_ai->ai_protocol == IPPROTO_TCP)
      ai->ai_protocol = LWPA_IPPROTO_TCP;
    else
      ai->ai_protocol = 0;

    ai->ai_canonname = pf_ai->ai_canonname;
    ai->pd[1] = pf_ai->ai_next;

    return true;
  }
  return false;
}

void lwpa_freeaddrinfo(LwpaAddrinfo* ai)
{
  if (ai)
    freeaddrinfo((struct addrinfo*)ai->pd[0]);
}

#endif /* !defined(LWPA_BUILDING_MOCK_LIB) */

lwpa_error_t lwpa_inet_ntop(const LwpaIpAddr* src, char* dest, size_t size)
{
  if (!src || !dest)
    return kLwpaErrInvalid;

  switch (src->type)
  {
    case kLwpaIpTypeV4:
    {
      struct in_addr addr;
      addr.s_addr = htonl(lwpaip_v4_address(src));
      if (NULL != inet_ntop(AF_INET, &addr, dest, size))
        return kLwpaErrOk;
      return kLwpaErrSys;
    }
    case kLwpaIpTypeV6:
    {
      struct in6_addr addr;
      memcpy(addr.s6_addr, lwpaip_v6_address(src), LWPA_IPV6_BYTES);
      if (NULL != inet_ntop(AF_INET6, &addr, dest, size))
        return kLwpaErrOk;
      return kLwpaErrSys;
    }
    default:
      return kLwpaErrInvalid;
  }
}

lwpa_error_t lwpa_inet_pton(lwpa_iptype_t type, const char* src, LwpaIpAddr* dest)
{
  if (!src || !dest)
    return kLwpaErrInvalid;

  switch (type)
  {
    case kLwpaIpTypeV4:
    {
      struct in_addr addr;
      if (1 != inet_pton(AF_INET, src, &addr))
        return kLwpaErrSys;
      lwpaip_set_v4_address(dest, ntohl(addr.s_addr));
      return kLwpaErrOk;
    }
    case kLwpaIpTypeV6:
    {
      struct in6_addr addr;
      if (1 != inet_pton(AF_INET6, src, &addr))
        return kLwpaErrSys;
      lwpaip_set_v6_address(dest, addr.s6_addr);
      return kLwpaErrOk;
    }
    default:
      return kLwpaErrInvalid;
  }
}
