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

#include <errno.h>

#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "os_error.h"

/**************************** Private variables ******************************/

/* clang-format off */

#define LWPA_NUM_SHUT 3
static const int shutmap[LWPA_NUM_SHUT] =
{
  SHUT_RD,
  SHUT_WR,
  SHUT_RDWR
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

static void ms_to_timeval(int ms, struct timeval* tv);
static int setsockopt_socket(lwpa_socket_t id, int option_name, const void* option_value, size_t option_len);
static int setsockopt_ip(lwpa_socket_t id, int option_name, const void* option_value, size_t option_len);
static int setsockopt_ip6(lwpa_socket_t id, int option_name, const void* option_value, size_t option_len);

/*************************** Function definitions ****************************/

bool sockaddr_os_to_lwpa(LwpaSockaddr* sa, const struct sockaddr* pfsa)
{
  if (pfsa->sa_family == AF_INET)
  {
    struct sockaddr_in* sin = (struct sockaddr_in*)pfsa;
    sa->port = ntohs(sin->sin_port);
    LWPA_IP_SET_V4_ADDRESS(&sa->ip, ntohl(sin->sin_addr.s_addr));
    return true;
  }
  else if (pfsa->sa_family == AF_INET6)
  {
    struct sockaddr_in6* sin6 = (struct sockaddr_in6*)pfsa;
    sa->port = ntohs(sin6->sin6_port);
    LWPA_IP_SET_V6_ADDRESS(&sa->ip, sin6->sin6_addr.s6_addr);
    return true;
  }
  return false;
}

size_t sockaddr_lwpa_to_os(struct sockaddr* pfsa, const LwpaSockaddr* sa)
{
  size_t ret = 0;
  if (LWPA_IP_IS_V4(&sa->ip))
  {
    struct sockaddr_in* sin = (struct sockaddr_in*)pfsa;
    sin->sin_family = AF_INET;
    sin->sin_port = htons(sa->port);
    sin->sin_addr.s_addr = htonl(LWPA_IP_V4_ADDRESS(&sa->ip));
    ret = sizeof(struct sockaddr_in);
  }
  else if (LWPA_IP_IS_V6(&sa->ip))
  {
    struct sockaddr_in6* sin6 = (struct sockaddr_in6*)pfsa;
    sin6->sin6_family = AF_INET6;
    sin6->sin6_port = htons(sa->port);
    memcpy(sin6->sin6_addr.s6_addr, LWPA_IP_V6_ADDRESS(&sa->ip), LWPA_IPV6_BYTES);
    ret = sizeof(struct sockaddr_in6);
  }
  return ret;
}

lwpa_error_t lwpa_accept(lwpa_socket_t id, LwpaSockaddr* address, lwpa_socket_t* conn_sock)
{
  if (!conn_sock)
    return kLwpaErrInvalid;

  struct sockaddr_storage ss;
  socklen_t sa_size = sizeof ss;
  int res = accept(id, (struct sockaddr*)&ss, &sa_size);

  if (res != -1)
  {
    if (address && !sockaddr_os_to_lwpa(address, (struct sockaddr*)&ss))
    {
      close(res);
      return kLwpaErrSys;
    }
    *conn_sock = res;
    return kLwpaErrOk;
  }
  return errno_os_to_lwpa(errno);
}

lwpa_error_t lwpa_bind(lwpa_socket_t id, const LwpaSockaddr* address)
{
  if (!address)
    return kLwpaErrInvalid;

  struct sockaddr_storage ss;
  socklen_t sa_size = (socklen_t)sockaddr_lwpa_to_os((struct sockaddr*)&ss, address);
  if (sa_size == 0)
    return kLwpaErrInvalid;

  int res = bind(id, (struct sockaddr*)&ss, sa_size);
  return (res == 0 ? kLwpaErrOk : errno_os_to_lwpa(errno));
}

lwpa_error_t lwpa_close(lwpa_socket_t id)
{
  int res = close(id);
  return (res == 0 ? kLwpaErrOk : errno_os_to_lwpa(errno));
}

lwpa_error_t lwpa_connect(lwpa_socket_t id, const LwpaSockaddr* address)
{
  if (!address)
    return kLwpaErrInvalid;

  struct sockaddr_storage ss;
  socklen_t sa_size = (socklen_t)sockaddr_lwpa_to_os((struct sockaddr*)&ss, address);
  if (sa_size == 0)
    return kLwpaErrInvalid;

  int res = connect(id, (struct sockaddr*)&ss, sa_size);
  return (res == 0 ? kLwpaErrOk : errno_os_to_lwpa(errno));
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
  if (!address)
    return kLwpaErrInvalid;

  struct sockaddr_storage ss;
  socklen_t size = sizeof ss;
  int res = getsockname(id, (struct sockaddr*)&ss, &size);
  if (res == 0)
  {
    if (!sockaddr_os_to_lwpa(address, (struct sockaddr*)&ss))
      return kLwpaErrSys;
    return kLwpaErrOk;
  }
  return errno_os_to_lwpa(errno);
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
  return (res == 0 ? kLwpaErrOk : errno_os_to_lwpa(errno));
}

int lwpa_recv(lwpa_socket_t id, void* buffer, size_t length, int flags)
{
  if (!buffer)
    return kLwpaErrInvalid;

  int impl_flags = (flags & LWPA_MSG_PEEK) ? MSG_PEEK : 0;
  int res = (int)recv(id, buffer, length, impl_flags);
  return (res >= 0 ? res : (int)errno_os_to_lwpa(errno));
}

int lwpa_recvfrom(lwpa_socket_t id, void* buffer, size_t length, int flags, LwpaSockaddr* address)
{
  if (!buffer)
    return (int)kLwpaErrInvalid;

  struct sockaddr_storage fromaddr;
  socklen_t fromlen = sizeof fromaddr;
  int impl_flags = (flags & LWPA_MSG_PEEK) ? MSG_PEEK : 0;
  int res = (int)recvfrom(id, buffer, length, impl_flags, (struct sockaddr*)&fromaddr, &fromlen);

  if (res >= 0)
  {
    if (address && fromlen > 0)
    {
      if (!sockaddr_os_to_lwpa(address, (struct sockaddr*)&fromaddr))
        return kLwpaErrSys;
    }
    return res;
  }
  return (int)errno_os_to_lwpa(errno);
}

int lwpa_send(lwpa_socket_t id, const void* message, size_t length, int flags)
{
  (void)flags;

  if (!message)
    return (int)kLwpaErrInvalid;

  int res = (int)send(id, message, length, 0);
  return (res >= 0 ? res : (int)errno_os_to_lwpa(errno));
}

int lwpa_sendto(lwpa_socket_t id, const void* message, size_t length, int flags, const LwpaSockaddr* dest_addr)
{
  (void)flags;

  if (!dest_addr || !message)
    return (int)kLwpaErrInvalid;

  struct sockaddr_storage ss;
  socklen_t ss_size = (socklen_t)sockaddr_lwpa_to_os((struct sockaddr*)&ss, dest_addr);
  if (ss_size == 0)
    return (int)kLwpaErrSys;

  int res = (int)sendto(id, message, length, 0, (struct sockaddr*)&ss, ss_size);

  return (res >= 0 ? res : (int)errno_os_to_lwpa(errno));
}

lwpa_error_t lwpa_setsockopt(lwpa_socket_t id, int level, int option_name, const void* option_value, size_t option_len)
{
  int res = -1;

  if (!option_value)
    return kLwpaErrInvalid;

  // TODO this OS implementation could be simplified by use of socket option lookup arrays.
  switch (level)
  {
    case LWPA_SOL_SOCKET:
      res = setsockopt_socket(id, option_name, option_value, option_len);
      break;
    case LWPA_IPPROTO_IP:
      res = setsockopt_ip(id, option_name, option_value, option_len);
      break;
    case LWPA_IPPROTO_IPV6:
      res = setsockopt_ip6(id, option_name, option_value, option_len);
      break;
    default:
      return kLwpaErrInvalid;
  }
  return (res == 0 ? kLwpaErrOk : errno_os_to_lwpa(errno));
}

int setsockopt_socket(lwpa_socket_t id, int option_name, const void* option_value, size_t option_len)
{
  switch (option_name)
  {
    case LWPA_SO_RCVBUF:
      return setsockopt(id, SOL_SOCKET, SO_RCVBUF, option_value, (socklen_t)option_len);
    case LWPA_SO_SNDBUF:
      return setsockopt(id, SOL_SOCKET, SO_SNDBUF, option_value, (socklen_t)option_len);
    case LWPA_SO_RCVTIMEO:
      if (option_len == sizeof(int))
      {
        int val = *(int*)option_value;
        struct timeval sys_val;
        ms_to_timeval(val, &sys_val);
        return setsockopt(id, SOL_SOCKET, SO_RCVTIMEO, &sys_val, sizeof sys_val);
      }
      break;
    case LWPA_SO_SNDTIMEO:
      if (option_len == sizeof(int))
      {
        int val = *(int*)option_value;
        struct timeval sys_val;
        ms_to_timeval(val, &sys_val);
        return setsockopt(id, SOL_SOCKET, SO_SNDTIMEO, &sys_val, sizeof sys_val);
      }
      break;
    case LWPA_SO_REUSEADDR:
      return setsockopt(id, SOL_SOCKET, SO_REUSEADDR, option_value, (socklen_t)option_len);
    case LWPA_SO_REUSEPORT:
      return setsockopt(id, SOL_SOCKET, SO_REUSEPORT, option_value, (socklen_t)option_len);
    case LWPA_SO_BROADCAST:
      return setsockopt(id, SOL_SOCKET, SO_BROADCAST, option_value, (socklen_t)option_len);
    case LWPA_SO_KEEPALIVE:
      return setsockopt(id, SOL_SOCKET, SO_KEEPALIVE, option_value, (socklen_t)option_len);
    case LWPA_SO_LINGER:
      if (option_len == sizeof(LwpaLinger))
      {
        LwpaLinger* ll = (LwpaLinger*)option_value;
        struct linger val;
        val.l_onoff = (u_short)ll->onoff;
        val.l_linger = (u_short)ll->linger;
        return setsockopt(id, SOL_SOCKET, SO_LINGER, &val, sizeof val);
      }
      break;
    case LWPA_SO_ERROR:  // Set not supported
    case LWPA_SO_TYPE:   // Set not supported
    default:
      break;
  }
  // If we got here, something was invalid. Set errno accordingly
  errno = EINVAL;
  return -1;
}

int setsockopt_ip(lwpa_socket_t id, int option_name, const void* option_value, size_t option_len)
{
  switch (option_name)
  {
    case LWPA_IP_TTL:
      return setsockopt(id, IPPROTO_IP, IP_TTL, option_value, (socklen_t)option_len);
    case LWPA_IP_ADD_MEMBERSHIP:
      if (option_len == sizeof(LwpaMreq))
      {
        LwpaMreq* amreq = (LwpaMreq*)option_value;
        if (LWPA_IP_IS_V4(&amreq->group))
        {
          struct ip_mreq val;
          val.imr_multiaddr.s_addr = htonl(LWPA_IP_V4_ADDRESS(&amreq->group));
          val.imr_interface.s_addr = htonl(LWPA_IP_V4_ADDRESS(&amreq->netint));
          return setsockopt(id, IPPROTO_IP, IP_ADD_MEMBERSHIP, &val, sizeof val);
        }
      }
      break;
    case LWPA_IP_DROP_MEMBERSHIP:
      if (option_len == sizeof(LwpaMreq))
      {
        LwpaMreq* amreq = (LwpaMreq*)option_value;
        if (LWPA_IP_IS_V4(&amreq->group))
        {
          struct ip_mreq val;
          val.imr_multiaddr.s_addr = htonl(LWPA_IP_V4_ADDRESS(&amreq->group));
          val.imr_interface.s_addr = htonl(LWPA_IP_V4_ADDRESS(&amreq->netint));
          return setsockopt(id, IPPROTO_IP, IP_DROP_MEMBERSHIP, &val, sizeof val);
        }
      }
      break;
    case LWPA_MCAST_JOIN_GROUP:
      if (option_len == sizeof(LwpaGroupReq))
      {
        LwpaGroupReq* greq = (LwpaGroupReq*)option_value;
        if (LWPA_IP_IS_V4(&greq->group) && greq->interface >= 0)
        {
          struct group_req val;
          val.gr_interface = (uint32_t)greq->interface;
          ((struct sockaddr_in*)&val.gr_group)->sin_addr.s_addr = htonl(LWPA_IP_V4_ADDRESS(&greq->group));
          return setsockopt(id, IPPROTO_IP, MCAST_JOIN_GROUP, &val, sizeof val);
        }
      }
      break;
    case LWPA_MCAST_LEAVE_GROUP:
      if (option_len == sizeof(LwpaGroupReq))
      {
        LwpaGroupReq* greq = (LwpaGroupReq*)option_value;
        if (LWPA_IP_IS_V6(&greq->group) && greq->interface >= 0)
        {
          struct group_req val;
          val.gr_interface = (uint32_t)greq->interface;
          ((struct sockaddr_in*)&val.gr_group)->sin_addr.s_addr = htonl(LWPA_IP_V4_ADDRESS(&greq->group));
          return setsockopt(id, IPPROTO_IP, MCAST_LEAVE_GROUP, &val, sizeof val);
        }
      }
      break;
    case LWPA_IP_MULTICAST_IF:
      if (option_len == sizeof(LwpaIpAddr))
      {
        LwpaIpAddr* netint = (LwpaIpAddr*)option_value;
        if (LWPA_IP_IS_V4(netint))
        {
          struct in_addr val;
          val.s_addr = LWPA_IP_V4_ADDRESS(netint);
          return setsockopt(id, IPPROTO_IP, IP_MULTICAST_IF, &val, sizeof val);
        }
      }
      break;
    case LWPA_IP_MULTICAST_TTL:
      return setsockopt(id, IPPROTO_IP, IP_MULTICAST_TTL, option_value, (socklen_t)option_len);
    case LWPA_IP_MULTICAST_LOOP:
      return setsockopt(id, IPPROTO_IP, IP_MULTICAST_LOOP, option_value, (socklen_t)option_len);
    default:
      break;
  }
  // If we got here, something was invalid. Set errno accordingly
  errno = EINVAL;
  return -1;
}

int setsockopt_ip6(lwpa_socket_t id, int option_name, const void* option_value, size_t option_len)
{
  switch (option_name)
  {
    case LWPA_MCAST_JOIN_GROUP:
      if (option_len == sizeof(LwpaGroupReq))
      {
        LwpaGroupReq* greq = (LwpaGroupReq*)option_value;
        if (LWPA_IP_IS_V6(&greq->group) && greq->interface >= 0)
        {
          struct group_req val;
          val.gr_interface = (uint32_t)greq->interface;
          memcpy(&((struct sockaddr_in6*)&val.gr_group)->sin6_addr.s6_addr, LWPA_IP_V6_ADDRESS(&greq->group),
                 LWPA_IPV6_BYTES);
          return setsockopt(id, IPPROTO_IPV6, MCAST_JOIN_GROUP, &val, sizeof val);
        }
      }
      break;
    case LWPA_MCAST_LEAVE_GROUP:
      if (option_len == sizeof(LwpaGroupReq))
      {
        LwpaGroupReq* greq = (LwpaGroupReq*)option_value;
        if (LWPA_IP_IS_V6(&greq->group) && greq->interface >= 0)
        {
          struct group_req val;
          val.gr_interface = (uint32_t)greq->interface;
          memcpy(&((struct sockaddr_in6*)&val.gr_group)->sin6_addr.s6_addr, LWPA_IP_V6_ADDRESS(&greq->group),
                 LWPA_IPV6_BYTES);
          return setsockopt(id, IPPROTO_IPV6, MCAST_LEAVE_GROUP, &val, sizeof val);
        }
      }
      break;
    default: /* Other IPv6 options TODO on linux. */
      break;
  }
  // If we got here, something was invalid. Set errno accordingly
  errno = EINVAL;
  return -1;
}

void ms_to_timeval(int ms, struct timeval* tv)
{
  tv->tv_sec = ms / 1000;
  tv->tv_usec = (ms % 1000) * 1000;
}

lwpa_error_t lwpa_shutdown(lwpa_socket_t id, int how)
{
  if (how >= 0 && how < LWPA_NUM_SHUT)
  {
    int res = shutdown(id, shutmap[how]);
    return (res == 0 ? kLwpaErrOk : errno_os_to_lwpa(errno));
  }
  return kLwpaErrInvalid;
}

lwpa_error_t lwpa_socket(unsigned int family, unsigned int type, lwpa_socket_t* id)
{
  if (id)
  {
    if (family < LWPA_NUM_AF && type < LWPA_NUM_TYPE)
    {
      int sock = socket(sfmap[family], stmap[type], 0);
      if (sock != -1)
      {
        *id = sock;
        return kLwpaErrOk;
      }
      else
      {
        *id = LWPA_SOCKET_INVALID;
        return errno_os_to_lwpa(errno);
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
  int val = fcntl(id, F_GETFL, 0);
  if (val >= 0)
  {
    val = fcntl(id, F_SETFL, val | O_NONBLOCK);
  }
  return (val >= 0 ? kLwpaErrOk : errno_os_to_lwpa(errno));
}

lwpa_error_t lwpa_poll_context_init(LwpaPollContext* context)
{
  return kLwpaErrNotImpl;
}

void lwpa_poll_context_deinit(LwpaPollContext* context)
{
}

lwpa_error_t lwpa_poll_add_socket(LwpaPollContext* context, lwpa_socket_t socket, lwpa_poll_events_t events,
                                  void* user_data)
{
  return kLwpaErrNotImpl;
}

lwpa_error_t lwpa_poll_modify_socket(LwpaPollContext* context, lwpa_socket_t socket, lwpa_poll_events_t new_events,
                                     void* new_user_data)
{
  return kLwpaErrNotImpl;
}

void lwpa_poll_remove_socket(LwpaPollContext* context, lwpa_socket_t socket)
{
}

lwpa_error_t lwpa_poll_wait(LwpaPollContext* context, LwpaPollEvent* event, int timeout_ms)
{
  //  fd_set readfds, writefds, exceptfds;
  //  LwpaPollfd *fd;
  //  int nosfds;
  //  int sel_res;
  //  int nreadfds = 0, nwritefds = 0, nexceptfds = 0;
  //  struct timeval os_timeout;
  //
  //  if (fds && nfds > 0)
  //  {
  //    FD_ZERO(&readfds);
  //    FD_ZERO(&writefds);
  //    FD_ZERO(&exceptfds);
  //
  //    for (fd = fds; fd < fds + nfds; ++fd)
  //    {
  //      if (fd->events & LWPA_POLLIN)
  //      {
  //        FD_SET(fd->fd, &readfds);
  //        nreadfds++;
  //      }
  //      if (fd->events & LWPA_POLLOUT)
  //      {
  //        FD_SET(fd->fd, &writefds);
  //        nwritefds++;
  //      }
  //      if (fd->events & LWPA_POLLPRI)
  //      {
  //        FD_SET(fd->fd, &exceptfds);
  //        nexceptfds++;
  //      }
  //    }
  //  }
  //
  //  if (timeout_ms == 0)
  //  {
  //    os_timeout.tv_sec = 0;
  //    os_timeout.tv_usec = 0;
  //  }
  //  else if (timeout_ms != LWPA_WAIT_FOREVER)
  //  {
  //    os_timeout.tv_sec = timeout_ms / 1000;
  //    os_timeout.tv_usec = (timeout_ms % 1000) * 1000;
  //  }
  //  nosfds = (nreadfds > nwritefds) ? nreadfds : nwritefds;
  //  nosfds = (nexceptfds > nosfds) ? nexceptfds : nosfds;
  //  sel_res = select(nosfds, nreadfds ? &readfds : NULL, nwritefds ? &writefds : NULL, nexceptfds ? &exceptfds :
  //  NULL,
  //                   timeout_ms == LWPA_WAIT_FOREVER ? NULL : &os_timeout);
  //
  //  if (sel_res < 0)
  //  {
  //    return err_os_to_lwpa(WSAGetLastError());
  //  }
  //  else if (sel_res == 0)
  //  {
  //    return kLwpaErrTimedOut;
  //  }
  //  else if (fds && nfds > 0)
  //  {
  //    for (fd = fds; fd < fds + nfds; ++fd)
  //    {
  //      fd->revents = 0;
  //
  //      if (FD_ISSET(fd->fd, &readfds) || FD_ISSET(fd->fd, &writefds) || FD_ISSET(fd->fd, &exceptfds))
  //      {
  //        /* Check for errors */
  //        int error;
  //        int error_size = sizeof(error);
  //        if (getsockopt(fd->fd, SOL_SOCKET, SO_ERROR, (char *)&error, &error_size) == 0)
  //        {
  //          if (error != 0)
  //          {
  //            fd->revents |= LWPA_POLLERR;
  //            fd->err = err_os_to_lwpa(error);
  //          }
  //        }
  //        else
  //          return (int)err_os_to_lwpa(WSAGetLastError());
  //      }
  //      if (nreadfds && (fd->events & LWPA_POLLIN) && FD_ISSET(fd->fd, &readfds))
  //      {
  //        fd->revents |= LWPA_POLLIN;
  //      }
  //      if (nwritefds && (fd->events & LWPA_POLLOUT) && FD_ISSET(fd->fd, &writefds))
  //      {
  //        fd->revents |= LWPA_POLLOUT;
  //      }
  //      if (nexceptfds && (fd->events & LWPA_POLLPRI) && FD_ISSET(fd->fd, &exceptfds))
  //      {
  //        fd->revents |= LWPA_POLLPRI;
  //      }
  //    }
  //  }
  //  return sel_res;
  return kLwpaErrNotImpl;
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
  return (res == 0 ? kLwpaErrOk : errno_os_to_lwpa(res));
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

lwpa_error_t lwpa_inet_ntop(const LwpaIpAddr* src, char* dest, size_t size)
{
  if (!src || !dest)
    return kLwpaErrInvalid;

  switch (src->type)
  {
    case kLwpaIpTypeV4:
    {
      struct in_addr addr;
      addr.s_addr = htonl(LWPA_IP_V4_ADDRESS(src));
      if (NULL != inet_ntop(AF_INET, &addr, dest, (socklen_t)size))
        return kLwpaErrOk;
      return kLwpaErrSys;
    }
    case kLwpaIpTypeV6:
    {
      struct in6_addr addr;
      memcpy(addr.s6_addr, LWPA_IP_V6_ADDRESS(src), LWPA_IPV6_BYTES);
      if (NULL != inet_ntop(AF_INET6, &addr, dest, (socklen_t)size))
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
      LWPA_IP_SET_V4_ADDRESS(dest, ntohl(addr.s_addr));
      return kLwpaErrOk;
    }
    case kLwpaIpTypeV6:
    {
      struct in6_addr addr;
      if (1 != inet_pton(AF_INET6, src, &addr))
        return kLwpaErrSys;
      LWPA_IP_SET_V6_ADDRESS(dest, addr.s6_addr);
      return kLwpaErrOk;
    }
    default:
      return kLwpaErrInvalid;
  }
}
