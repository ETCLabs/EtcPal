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
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>

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

bool sockaddr_plat_to_lwpa(LwpaSockaddr *sa, const struct sockaddr *pfsa)
{
  if (pfsa->sa_family == AF_INET)
  {
    struct sockaddr_in *sin = (struct sockaddr_in *)pfsa;
    sa->port = ntohs(sin->sin_port);
    lwpaip_set_v4_address(&sa->ip, ntohl(sin->sin_addr.s_addr));
    return true;
  }
  else if (pfsa->sa_family == AF_INET6)
  {
    struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *)pfsa;
    sa->port = ntohs(sin6->sin6_port);
    lwpaip_set_v6_address(&sa->ip, sin6->sin6_addr.s6_addr);
    return true;
  }
  return false;
}

size_t sockaddr_lwpa_to_plat(struct sockaddr *pfsa, const LwpaSockaddr *sa)
{
  size_t ret = 0;
  if (lwpaip_is_v4(&sa->ip))
  {
    struct sockaddr_in *sin = (struct sockaddr_in *)pfsa;
    sin->sin_family = AF_INET;
    sin->sin_port = htons(sa->port);
    sin->sin_addr.s_addr = htonl(lwpaip_v4_address(&sa->ip));
    ret = sizeof(struct sockaddr_in);
  }
  else if (lwpaip_is_v6(&sa->ip))
  {
    struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *)pfsa;
    sin6->sin6_family = AF_INET6;
    sin6->sin6_port = htons(sa->port);
    memcpy(sin6->sin6_addr.s6_addr, lwpaip_v6_address(&sa->ip), LWPA_IPV6_BYTES);
    ret = sizeof(struct sockaddr_in6);
  }
  return ret;
}

#if !defined(LWPA_BUILDING_MOCK_LIB)

static lwpa_error_t err_plat_to_lwpa(int wsaerror)
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

lwpa_error_t lwpa_accept(lwpa_socket_t id, LwpaSockaddr *address, lwpa_socket_t *conn_sock)
{
  struct sockaddr_storage ss;
  int sa_size = sizeof ss;
  SOCKET res;

  if (!conn_sock)
    return kLwpaErrInvalid;

  res = accept(id, (struct sockaddr *)&ss, &sa_size);

  if (res != INVALID_SOCKET)
  {
    if (address && !sockaddr_plat_to_lwpa(address, (struct sockaddr *)&ss))
    {
      closesocket(res);
      return kLwpaErrSys;
    }
    *conn_sock = res;
    return kLwpaErrOk;
  }
  return err_plat_to_lwpa(WSAGetLastError());
}

lwpa_error_t lwpa_bind(lwpa_socket_t id, const LwpaSockaddr *address)
{
  struct sockaddr_storage ss;
  size_t sa_size;
  int res;

  if (!address)
    return kLwpaErrInvalid;

  sa_size = sockaddr_lwpa_to_plat((struct sockaddr *)&ss, address);
  if (sa_size == 0)
    return kLwpaErrInvalid;

  res = bind(id, (struct sockaddr *)&ss, (int)sa_size);
  return (res == 0 ? kLwpaErrOk : err_plat_to_lwpa(WSAGetLastError()));
}

lwpa_error_t lwpa_close(lwpa_socket_t id)
{
  int res = closesocket(id);
  return (res == 0 ? kLwpaErrOk : err_plat_to_lwpa(WSAGetLastError()));
}

lwpa_error_t lwpa_connect(lwpa_socket_t id, const LwpaSockaddr *address)
{
  struct sockaddr_storage ss;
  size_t sa_size;
  int res;

  if (!address)
    return kLwpaErrInvalid;

  sa_size = sockaddr_lwpa_to_plat((struct sockaddr *)&ss, address);
  if (sa_size == 0)
    return kLwpaErrInvalid;

  res = connect(id, (struct sockaddr *)&ss, (int)sa_size);
  return (res == 0 ? kLwpaErrOk : err_plat_to_lwpa(WSAGetLastError()));
}

lwpa_error_t lwpa_getpeername(lwpa_socket_t id, LwpaSockaddr *address)
{
  /* TODO */
  (void)id;
  (void)address;
  return kLwpaErrNotImpl;
}

lwpa_error_t lwpa_getsockname(lwpa_socket_t id, LwpaSockaddr *address)
{
  int res;
  struct sockaddr_storage ss;
  socklen_t size = sizeof ss;

  if (!address)
    return kLwpaErrInvalid;

  res = getsockname(id, (struct sockaddr *)&ss, &size);
  if (res == 0)
  {
    if (!sockaddr_plat_to_lwpa(address, (struct sockaddr *)&ss))
      return kLwpaErrSys;
    return kLwpaErrOk;
  }
  return err_plat_to_lwpa(WSAGetLastError());
}

lwpa_error_t lwpa_getsockopt(lwpa_socket_t id, int level, int option_name, void *option_value, size_t *option_len)
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
  return (res == 0 ? kLwpaErrOk : err_plat_to_lwpa(WSAGetLastError()));
}

int lwpa_recv(lwpa_socket_t id, void *buffer, size_t length, int flags)
{
  int res;
  int impl_flags = (flags & LWPA_MSG_PEEK) ? MSG_PEEK : 0;

  if (!buffer)
    return kLwpaErrInvalid;

  res = recv(id, buffer, (int)length, impl_flags);
  return (res >= 0 ? res : (int)err_plat_to_lwpa(WSAGetLastError()));
}

int lwpa_recvfrom(lwpa_socket_t id, void *buffer, size_t length, int flags, LwpaSockaddr *address)
{
  int res;
  int impl_flags = (flags & LWPA_MSG_PEEK) ? MSG_PEEK : 0;
  struct sockaddr_storage fromaddr;
  socklen_t fromlen = sizeof fromaddr;

  if (!buffer)
    return (int)kLwpaErrInvalid;

  res = recvfrom(id, buffer, (int)length, impl_flags, (struct sockaddr *)&fromaddr, &fromlen);

  if (res >= 0)
  {
    if (address && fromlen > 0)
    {
      if (!sockaddr_plat_to_lwpa(address, (struct sockaddr *)&fromaddr))
        return kLwpaErrSys;
    }
    return res;
  }
  return (int)err_plat_to_lwpa(WSAGetLastError());
}

int lwpa_send(lwpa_socket_t id, const void *message, size_t length, int flags)
{
  int res;
  (void)flags;

  if (!message)
    return kLwpaErrInvalid;

  res = send(id, message, (int)length, 0);
  return (res >= 0 ? res : (int)err_plat_to_lwpa(WSAGetLastError()));
}

int lwpa_sendto(lwpa_socket_t id, const void *message, size_t length, int flags, const LwpaSockaddr *dest_addr)
{
  int res = -1;
  size_t ss_size;
  struct sockaddr_storage ss;
  (void)flags;

  if (!dest_addr || !message)
    return (int)kLwpaErrInvalid;

  if ((ss_size = sockaddr_lwpa_to_plat((struct sockaddr *)&ss, dest_addr)) > 0)
    res = sendto(id, message, (int)length, 0, (struct sockaddr *)&ss, (int)ss_size);

  return (res >= 0 ? res : (int)err_plat_to_lwpa(WSAGetLastError()));
}

lwpa_error_t lwpa_setsockopt(lwpa_socket_t id, int level, int option_name, const void *option_value, size_t option_len)
{
  int res = -1;

  if (!option_value)
    return kLwpaErrInvalid;

  /* TODO this platform implementation could be simplified by use of socket option lookup arrays. */
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
            DWORD val = (DWORD) * (int *)option_value;
            res = setsockopt(id, SOL_SOCKET, SO_RCVTIMEO, (char *)&val, sizeof val);
          }
          break;
        case LWPA_SO_SNDTIMEO:
          if (option_len == sizeof(int))
          {
            DWORD val = (DWORD) * (int *)option_value;
            res = setsockopt(id, SOL_SOCKET, SO_SNDTIMEO, (char *)&val, sizeof val);
          }
          break;
        case LWPA_SO_REUSEADDR:
          if (option_len == sizeof(int))
          {
            BOOL val = (BOOL) * (int *)option_value;
            res = setsockopt(id, SOL_SOCKET, SO_REUSEADDR, (char *)&val, sizeof val);
          }
          break;
        case LWPA_SO_BROADCAST:
          if (option_len == sizeof(int))
          {
            BOOL val = (BOOL) * (int *)option_value;
            res = setsockopt(id, SOL_SOCKET, SO_BROADCAST, (char *)&val, sizeof val);
          }
          break;
        case LWPA_SO_KEEPALIVE:
          if (option_len == sizeof(int))
          {
            BOOL val = (BOOL) * (int *)option_value;
            res = setsockopt(id, SOL_SOCKET, SO_KEEPALIVE, (char *)&val, sizeof val);
          }
          break;
        case LWPA_SO_LINGER:
          if (option_len == sizeof(LwpaLinger))
          {
            LwpaLinger *ll = (LwpaLinger *)option_value;
            struct linger val;
            val.l_onoff = (u_short)ll->onoff;
            val.l_linger = (u_short)ll->linger;
            res = setsockopt(id, SOL_SOCKET, SO_LINGER, (char *)&val, sizeof val);
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
            DWORD val = (DWORD) * (int *)option_value;
            res = setsockopt(id, IPPROTO_IP, IP_TTL, (char *)&val, sizeof val);
          }
          break;
        case LWPA_MCAST_JOIN_GROUP:
          if (option_len == sizeof(LwpaMreq))
          {
            LwpaMreq *amreq = (LwpaMreq *)option_value;
            if (lwpaip_is_v4(&amreq->group))
            {
              struct ip_mreq val;
              val.imr_multiaddr.s_addr = htonl(lwpaip_v4_address(&amreq->group));
              val.imr_interface.s_addr = htonl(lwpaip_v4_address(&amreq->netint));
              res = setsockopt(id, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&val, sizeof val);
            }
          }
          break;
        case LWPA_MCAST_LEAVE_GROUP:
          if (option_len == sizeof(LwpaMreq))
          {
            LwpaMreq *amreq = (LwpaMreq *)option_value;
            if (lwpaip_is_v4(&amreq->group))
            {
              struct ip_mreq val;
              val.imr_multiaddr.s_addr = htonl(lwpaip_v4_address(&amreq->group));
              val.imr_interface.s_addr = htonl(lwpaip_v4_address(&amreq->netint));
              res = setsockopt(id, IPPROTO_IP, IP_DROP_MEMBERSHIP, (char *)&val, sizeof val);
            }
          }
          break;
        case LWPA_IP_MULTICAST_IF:
          if (option_len == sizeof(LwpaIpAddr))
          {
            LwpaIpAddr *netint = (LwpaIpAddr *)option_value;
            if (lwpaip_is_v4(netint))
            {
              DWORD val = htonl(lwpaip_v4_address(netint));
              res = setsockopt(id, IPPROTO_IP, IP_MULTICAST_IF, (char *)&val, sizeof val);
            }
          }
          break;
        case LWPA_IP_MULTICAST_TTL:
          if (option_len == sizeof(int))
          {
            DWORD val = (DWORD) * (int *)option_value;
            res = setsockopt(id, IPPROTO_IP, IP_MULTICAST_TTL, (char *)&val, sizeof val);
          }
          break;
        case LWPA_IP_MULTICAST_LOOP:
          if (option_len == sizeof(int))
          {
            DWORD val = (DWORD) * (int *)option_value;
            res = setsockopt(id, IPPROTO_IP, IP_MULTICAST_LOOP, (char *)&val, sizeof val);
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
        default: /* Other IPv6 options TODO on windows. */
          return kLwpaErrInvalid;
      }
      break;
    case LWPA_SO_REUSEPORT: /* Not supported on this platform. */
    default:
      return kLwpaErrInvalid;
  }
  return (res == 0 ? kLwpaErrOk : err_plat_to_lwpa(WSAGetLastError()));
}

lwpa_error_t lwpa_shutdown(lwpa_socket_t id, int how)
{
  if (how >= 0 && how < LWPA_NUM_SHUT)
  {
    int res = shutdown(id, shutmap[how]);
    return (res == 0 ? kLwpaErrOk : err_plat_to_lwpa(WSAGetLastError()));
  }
  return kLwpaErrInvalid;
}

lwpa_error_t lwpa_socket(unsigned int family, unsigned int type, lwpa_socket_t *id)
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
        return err_plat_to_lwpa(WSAGetLastError());
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
  return (res == 0 ? kLwpaErrOk : err_plat_to_lwpa(WSAGetLastError()));
}

/* TODO move to an overlapped IO implementation on Windows for better performance. */
int lwpa_poll(LwpaPollfd *fds, size_t nfds, int timeout_ms)
{
  fd_set readfds, writefds, exceptfds;
  LwpaPollfd *fd;
  int nplatfds;
  int sel_res;
  int nreadfds = 0, nwritefds = 0, nexceptfds = 0;
  struct timeval plat_timeout;

  if (fds && nfds > 0)
  {
    FD_ZERO(&readfds);
    FD_ZERO(&writefds);
    FD_ZERO(&exceptfds);

    for (fd = fds; fd < fds + nfds; ++fd)
    {
      if (fd->events & LWPA_POLLIN)
      {
        FD_SET(fd->fd, &readfds);
        nreadfds++;
      }
      if (fd->events & LWPA_POLLOUT)
      {
        FD_SET(fd->fd, &writefds);
        nwritefds++;
      }
      if (fd->events & LWPA_POLLPRI)
      {
        FD_SET(fd->fd, &exceptfds);
        nexceptfds++;
      }
    }
  }

  if (timeout_ms == 0)
  {
    plat_timeout.tv_sec = 0;
    plat_timeout.tv_usec = 0;
  }
  else if (timeout_ms != LWPA_WAIT_FOREVER)
  {
    plat_timeout.tv_sec = timeout_ms / 1000;
    plat_timeout.tv_usec = (timeout_ms % 1000) * 1000;
  }
  nplatfds = (nreadfds > nwritefds) ? nreadfds : nwritefds;
  nplatfds = (nexceptfds > nplatfds) ? nexceptfds : nplatfds;
  sel_res = select(nplatfds, nreadfds ? &readfds : NULL, nwritefds ? &writefds : NULL, nexceptfds ? &exceptfds : NULL,
                   timeout_ms == LWPA_WAIT_FOREVER ? NULL : &plat_timeout);

  if (sel_res < 0)
  {
    return err_plat_to_lwpa(WSAGetLastError());
  }
  else if (sel_res == 0)
  {
    return kLwpaErrTimedOut;
  }
  else if (fds && nfds > 0)
  {
    for (fd = fds; fd < fds + nfds; ++fd)
    {
      fd->revents = 0;

      if (FD_ISSET(fd->fd, &readfds) || FD_ISSET(fd->fd, &writefds) || FD_ISSET(fd->fd, &exceptfds))
      {
        /* Check for errors */
        int error;
        int error_size = sizeof(error);
        if (getsockopt(fd->fd, SOL_SOCKET, SO_ERROR, (char *)&error, &error_size) == 0)
        {
          if (error != 0)
          {
            fd->revents |= LWPA_POLLERR;
            fd->err = err_plat_to_lwpa(error);
          }
        }
        else
        {
          return (int)err_plat_to_lwpa(WSAGetLastError());
        }
      }
      if (nreadfds && (fd->events & LWPA_POLLIN) && FD_ISSET(fd->fd, &readfds))
      {
        fd->revents |= LWPA_POLLIN;
      }
      if (nwritefds && (fd->events & LWPA_POLLOUT) && FD_ISSET(fd->fd, &writefds))
      {
        fd->revents |= LWPA_POLLOUT;
      }
      if (nexceptfds && (fd->events & LWPA_POLLPRI) && FD_ISSET(fd->fd, &exceptfds))
      {
        fd->revents |= LWPA_POLLPRI;
      }
    }
  }
  return sel_res;
}

lwpa_error_t lwpa_getaddrinfo(const char *hostname, const char *service, const LwpaAddrinfo *hints,
                              LwpaAddrinfo *result)
{
  int res;
  struct addrinfo *pf_res;
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
  return (res == 0 ? kLwpaErrOk : err_plat_to_lwpa(res));
}

bool lwpa_nextaddr(LwpaAddrinfo *ai)
{
  if (ai && ai->pd[1])
  {
    struct addrinfo *pf_ai = (struct addrinfo *)ai->pd[1];
    ai->ai_flags = 0;
    if (!sockaddr_plat_to_lwpa(&ai->ai_addr, pf_ai->ai_addr))
      return false;

    /* Can't use reverse maps, because we have no guarantee of the numeric values of the platform
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

void lwpa_freeaddrinfo(LwpaAddrinfo *ai)
{
  if (ai)
    freeaddrinfo((struct addrinfo *)ai->pd[0]);
}

#endif /* !defined(LWPA_BUILDING_MOCK_LIB) */

lwpa_error_t lwpa_inet_ntop(const LwpaIpAddr *src, char *dest, size_t size)
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

lwpa_error_t lwpa_inet_pton(lwpa_iptype_t type, const char *src, LwpaIpAddr *dest)
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
