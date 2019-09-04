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
#include "lwpa/private/socket.h"

#include <errno.h>
#include <stdlib.h>

#include <arpa/inet.h>
#include <fcntl.h>
#include <net/if.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/event.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "os_error.h"

/**************************** Private constants ******************************/

/* The maximum number of kevents that can be added in one call to an lwpa_poll API function. */
#define LWPA_SOCKET_MAX_KEVENTS 3

/****************************** Private types ********************************/

/* A struct to track sockets being polled by the lwpa_poll() API */
typedef struct LwpaPollSocket
{
  // 'sock' must always remain as the first member in the struct to facilitate an LwpaRbTree lookup
  // shortcut
  lwpa_socket_t sock;
  lwpa_poll_events_t events;
} LwpaPollSocket;

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

// Helpers for lwpa_setsockopt()
static void ms_to_timeval(int ms, struct timeval* tv);
static int setsockopt_socket(lwpa_socket_t id, int option_name, const void* option_value, size_t option_len);
static int setsockopt_ip(lwpa_socket_t id, int option_name, const void* option_value, size_t option_len);
static int setsockopt_ip6(lwpa_socket_t id, int option_name, const void* option_value, size_t option_len);

// Helpers for lwpa_poll API
static int events_lwpa_to_kqueue(lwpa_socket_t socket, lwpa_poll_events_t prev_events, lwpa_poll_events_t new_events,
                                 void* user_data, struct kevent* events);
static lwpa_poll_events_t events_kqueue_to_lwpa(const struct kevent* kevent, const LwpaPollSocket* sock_desc);

static int poll_socket_compare(const LwpaRbTree* tree, const LwpaRbNode* node_a, const LwpaRbNode* node_b);
static LwpaRbNode* poll_socket_alloc();
static void poll_socket_free(LwpaRbNode* node);

/*************************** Function definitions ****************************/

lwpa_error_t lwpa_socket_init()
{
  // No initialization necessary on this platform
  return kLwpaErrOk;
}

void lwpa_socket_deinit()
{
  // No deinitialization necessary on this platform
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
    if (address && !sockaddr_os_to_lwpa((lwpa_os_sockaddr_t*)&ss, address))
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
  socklen_t sa_size = (socklen_t)sockaddr_lwpa_to_os(address, (lwpa_os_sockaddr_t*)&ss);
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
  socklen_t sa_size = (socklen_t)sockaddr_lwpa_to_os(address, (lwpa_os_sockaddr_t*)&ss);
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
    if (!sockaddr_os_to_lwpa((lwpa_os_sockaddr_t*)&ss, address))
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
      if (!sockaddr_os_to_lwpa((lwpa_os_sockaddr_t*)&fromaddr, address))
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
  socklen_t ss_size = (socklen_t)sockaddr_lwpa_to_os(dest_addr, (lwpa_os_sockaddr_t*)&ss);
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

/* On Darwin, MCAST_JOIN_GROUP/MCAST_LEAVE_GROUP APIs do not seem to be supported. So we need to
 * translate interface indexes to addresses for IPv4 MCAST_JOIN_GROUP sockopts. */
static int ip4_ifindex_to_addr(unsigned int ifindex, struct in_addr* addr)
{
  struct ifreq req;
  if (if_indextoname(ifindex, req.ifr_name) != NULL)
  {
    int ioctl_sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (ioctl_sock != -1)
    {
      int ioctl_res = ioctl(ioctl_sock, SIOCGIFADDR, &req);
      if (ioctl_res != -1)
      {
        *addr = ((struct sockaddr_in*)&req.ifr_addr)->sin_addr;
        close(ioctl_sock);
        return 0;
      }
      close(ioctl_sock);
    }
  }
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
        if (LWPA_IP_IS_V4(&greq->group) && greq->ifindex >= 0)
        {
          struct ip_mreq val;
          val.imr_multiaddr.s_addr = htonl(LWPA_IP_V4_ADDRESS(&greq->group));
          if (0 != ip4_ifindex_to_addr(greq->ifindex, &val.imr_interface))
            return -1;
          return setsockopt(id, IPPROTO_IP, IP_ADD_MEMBERSHIP, &val, sizeof val);
        }
      }
      break;
    case LWPA_MCAST_LEAVE_GROUP:
      if (option_len == sizeof(LwpaGroupReq))
      {
        LwpaGroupReq* greq = (LwpaGroupReq*)option_value;
        if (LWPA_IP_IS_V4(&greq->group) && greq->ifindex >= 0)
        {
          struct ip_mreq val;
          val.imr_multiaddr.s_addr = htonl(LWPA_IP_V4_ADDRESS(&greq->group));
          if (0 != ip4_ifindex_to_addr(greq->ifindex, &val.imr_interface))
            return -1;
          return setsockopt(id, IPPROTO_IP, IP_DROP_MEMBERSHIP, &val, sizeof val);
        }
      }
      break;
    case LWPA_IP_MULTICAST_IF:
      if (option_len == sizeof(unsigned int))
      {
        struct in_addr val;
        if (0 != ip4_ifindex_to_addr(*(unsigned int*)option_value, &val))
          return -1;
        return setsockopt(id, IPPROTO_IP, IP_MULTICAST_IF, &val, sizeof val);
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
        if (LWPA_IP_IS_V6(&greq->group) && greq->ifindex >= 0)
        {
          struct ipv6_mreq val;
          val.ipv6mr_interface = (uint32_t)greq->ifindex;
          memcpy(val.ipv6mr_multiaddr.s6_addr, LWPA_IP_V6_ADDRESS(&greq->group), LWPA_IPV6_BYTES);
          return setsockopt(id, IPPROTO_IPV6, IPV6_JOIN_GROUP, &val, sizeof val);
        }
      }
      break;
    case LWPA_MCAST_LEAVE_GROUP:
      if (option_len == sizeof(LwpaGroupReq))
      {
        LwpaGroupReq* greq = (LwpaGroupReq*)option_value;
        if (LWPA_IP_IS_V6(&greq->group) && greq->ifindex >= 0)
        {
          struct ipv6_mreq val;
          val.ipv6mr_interface = (uint32_t)greq->ifindex;
          memcpy(val.ipv6mr_multiaddr.s6_addr, LWPA_IP_V6_ADDRESS(&greq->group), LWPA_IPV6_BYTES);
          return setsockopt(id, IPPROTO_IPV6, IPV6_LEAVE_GROUP, &val, sizeof val);
        }
      }
      break;
    case LWPA_IP_MULTICAST_IF:
      return setsockopt(id, IPPROTO_IPV6, IPV6_MULTICAST_IF, option_value, (socklen_t)option_len);
    case LWPA_IP_MULTICAST_TTL:
      return setsockopt(id, IPPROTO_IPV6, IPV6_MULTICAST_HOPS, option_value, (socklen_t)option_len);
    case LWPA_IP_MULTICAST_LOOP:
      return setsockopt(id, IPPROTO_IPV6, IPV6_MULTICAST_LOOP, option_value, (socklen_t)option_len);
    case LWPA_IPV6_V6ONLY:
      return setsockopt(id, IPPROTO_IPV6, IPV6_V6ONLY, option_value, (socklen_t)option_len);
    default: /* Other IPv6 options TODO on macOS. */
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
    val = fcntl(id, F_SETFL, (blocking ? (val & (int)(~O_NONBLOCK)) : (val | O_NONBLOCK)));
  }
  return (val >= 0 ? kLwpaErrOk : errno_os_to_lwpa(errno));
}

lwpa_error_t lwpa_getblocking(lwpa_socket_t id, bool* blocking)
{
  if (blocking)
  {
    int val = fcntl(id, F_GETFL, 0);
    if (val >= 0)
    {
      *blocking = ((val & O_NONBLOCK) == 0);
      return kLwpaErrOk;
    }
    return errno_os_to_lwpa(errno);
  }
  return kLwpaErrInvalid;
}

lwpa_error_t lwpa_poll_context_init(LwpaPollContext* context)
{
  if (!context)
    return kLwpaErrInvalid;

  context->kq_fd = kqueue();
  if (context->kq_fd >= 0)
  {
    lwpa_rbtree_init(&context->sockets, poll_socket_compare, poll_socket_alloc, poll_socket_free);
    context->valid = true;
    return kLwpaErrOk;
  }
  else
  {
    return errno_os_to_lwpa(errno);
  }
}

void lwpa_poll_context_deinit(LwpaPollContext* context)
{
  if (context && context->valid)
  {
    lwpa_rbtree_clear(&context->sockets);
    close(context->kq_fd);
    context->valid = false;
  }
}

lwpa_error_t lwpa_poll_add_socket(LwpaPollContext* context, lwpa_socket_t socket, lwpa_poll_events_t events,
                                  void* user_data)
{
  if (context && context->valid && socket != LWPA_SOCKET_INVALID && (events & LWPA_POLL_VALID_INPUT_EVENT_MASK))
  {
    LwpaPollSocket* sock_desc = (LwpaPollSocket*)malloc(sizeof(LwpaPollSocket));
    if (sock_desc)
    {
      sock_desc->sock = socket;
      sock_desc->events = events;
      lwpa_error_t insert_res = lwpa_rbtree_insert(&context->sockets, sock_desc);
      if (insert_res == kLwpaErrOk)
      {
        struct kevent os_events[LWPA_SOCKET_MAX_KEVENTS];
        int num_events = events_lwpa_to_kqueue(socket, 0, events, user_data, os_events);

        int res = kevent(context->kq_fd, os_events, num_events, NULL, 0, NULL);
        if (res == 0)
        {
          return kLwpaErrOk;
        }
        else
        {
          // Our node dealloc function also deallocates sock_desc, so no need to free it here.
          lwpa_rbtree_remove(&context->sockets, sock_desc);
          return errno_os_to_lwpa(errno);
        }
      }
      else
      {
        free(sock_desc);
        return insert_res;
      }
    }
    else
    {
      return kLwpaErrNoMem;
    }
  }
  else
  {
    return kLwpaErrInvalid;
  }
}

lwpa_error_t lwpa_poll_modify_socket(LwpaPollContext* context, lwpa_socket_t socket, lwpa_poll_events_t new_events,
                                     void* new_user_data)
{
  if (context && context->valid && socket != LWPA_SOCKET_INVALID && (new_events & LWPA_POLL_VALID_INPUT_EVENT_MASK))
  {
    LwpaPollSocket* sock_desc = (LwpaPollSocket*)lwpa_rbtree_find(&context->sockets, &socket);
    if (sock_desc)
    {
      struct kevent os_events[LWPA_SOCKET_MAX_KEVENTS];
      int num_events = events_lwpa_to_kqueue(socket, sock_desc->events, new_events, new_user_data, os_events);

      int res = kevent(context->kq_fd, os_events, num_events, NULL, 0, NULL);
      if (res == 0)
      {
        sock_desc->events = new_events;
        return kLwpaErrOk;
      }
      else
      {
        return errno_os_to_lwpa(errno);
      }
    }
    else
    {
      return kLwpaErrNotFound;
    }
  }
  else
  {
    return kLwpaErrInvalid;
  }
}

void lwpa_poll_remove_socket(LwpaPollContext* context, lwpa_socket_t socket)
{
  if (context && context->valid)
  {
    LwpaPollSocket* sock_desc = (LwpaPollSocket*)lwpa_rbtree_find(&context->sockets, &socket);
    if (sock_desc)
    {
      struct kevent os_events[LWPA_SOCKET_MAX_KEVENTS];
      int num_events = events_lwpa_to_kqueue(socket, sock_desc->events, 0, NULL, os_events);

      kevent(context->kq_fd, os_events, num_events, NULL, 0, NULL);
      lwpa_rbtree_remove(&context->sockets, &sock_desc);
    }
  }
}

lwpa_error_t lwpa_poll_wait(LwpaPollContext* context, LwpaPollEvent* event, int timeout_ms)
{
  if (context && context->valid && event)
  {
    if (lwpa_rbtree_size(&context->sockets) > 0)
    {
      struct timespec os_timeout;
      struct timespec* os_timeout_ptr;
      if (timeout_ms == LWPA_WAIT_FOREVER)
      {
        os_timeout_ptr = NULL;
      }
      else
      {
        os_timeout.tv_sec = timeout_ms / 1000;
        os_timeout.tv_nsec = (timeout_ms % 1000) * 1000000;
        os_timeout_ptr = &os_timeout;
      }

      struct kevent kevt;
      int wait_res = kevent(context->kq_fd, NULL, 0, &kevt, 1, os_timeout_ptr);
      if (wait_res > 0)
      {
        lwpa_socket_t sock = (lwpa_socket_t)kevt.ident;
        LwpaPollSocket* sock_desc = (LwpaPollSocket*)lwpa_rbtree_find(&context->sockets, &sock);
        if (sock_desc)
        {
          event->socket = sock_desc->sock;
          event->events = events_kqueue_to_lwpa(&kevt, sock_desc);
          event->err = kLwpaErrOk;
          event->user_data = kevt.udata;

          // Check for errors
          int error;
          socklen_t error_size = sizeof error;
          if (getsockopt(sock_desc->sock, SOL_SOCKET, SO_ERROR, &error, &error_size) == 0)
          {
            if (error != 0)
            {
              event->events |= LWPA_POLL_ERR;
              event->err = errno_os_to_lwpa(error);
            }
          }

          return kLwpaErrOk;
        }
        else
        {
          return kLwpaErrSys;
        }
      }
      else if (wait_res == 0)
      {
        return kLwpaErrTimedOut;
      }
      else
      {
        return errno_os_to_lwpa(errno);
      }
    }
    else
    {
      return kLwpaErrNoSockets;
    }
  }
  else
  {
    return kLwpaErrInvalid;
  }
}

int events_lwpa_to_kqueue(lwpa_socket_t socket, lwpa_poll_events_t prev_events, lwpa_poll_events_t new_events,
                          void* user_data, struct kevent* kevents)
{
  int num_events = 0;

  // Process EVFILT_READ changes
  if (new_events & LWPA_POLL_IN)
  {
    // Re-add the socket even if it was already added before - user data might be modified.
    EV_SET(&kevents[num_events], socket, EVFILT_READ, EV_ADD, 0, 0, user_data);
    ++num_events;
  }
  else if (prev_events & LWPA_POLL_IN)
  {
    EV_SET(&kevents[num_events], socket, EVFILT_READ, EV_DELETE, 0, 0, user_data);
    ++num_events;
  }

  // Process EVFILT_WRITE changes
  if (new_events & (LWPA_POLL_OUT | LWPA_POLL_CONNECT))
  {
    // Re-add the socket even if it was already added before - user data might be modified.
    EV_SET(&kevents[num_events], socket, EVFILT_WRITE, EV_ADD, 0, 0, user_data);
    ++num_events;
  }
  else if (prev_events & (LWPA_POLL_OUT | LWPA_POLL_CONNECT))
  {
    EV_SET(&kevents[num_events], socket, EVFILT_WRITE, EV_DELETE, 0, 0, user_data);
    ++num_events;
  }

  // EVFILT_EXCEPT is not available on older versions of macOS. It was added somewhere between
  // 10.11 and 10.14.
#ifdef EVFILT_EXCEPT
  // Process EVFILT_EXCEPT changes
  if (new_events & LWPA_POLL_OOB)
  {
    // Re-add the socket even if it was already added before - user data might be modified.
    EV_SET(&kevents[num_events], socket, EVFILT_EXCEPT, EV_ADD, NOTE_OOB, 0, user_data);
    ++num_events;
  }
  else if (prev_events & LWPA_POLL_OOB)
  {
    EV_SET(&kevents[num_events], socket, EVFILT_EXCEPT, EV_DELETE, 0, 0, user_data);
    ++num_events;
  }
#endif

  return num_events;
}

lwpa_poll_events_t events_kqueue_to_lwpa(const struct kevent* kevent, const LwpaPollSocket* sock_desc)
{
  lwpa_poll_events_t events_out = 0;
  if (kevent->filter == EVFILT_READ)
  {
    events_out |= LWPA_POLL_IN;
    if (kevent->flags & EV_EOF)
      events_out |= LWPA_POLL_ERR;
    if (kevent->flags & EV_OOBAND && (sock_desc->events & LWPA_POLL_OOB))
      events_out |= LWPA_POLL_OOB;
  }
  if (kevent->filter == EVFILT_WRITE)
  {
    if (sock_desc->events & LWPA_POLL_OUT)
      events_out |= LWPA_POLL_OUT;
    if (sock_desc->events & LWPA_POLL_CONNECT)
      events_out |= LWPA_POLL_CONNECT;
    if (kevent->flags & EV_EOF)
      events_out |= LWPA_POLL_ERR;
  }
#ifdef EVFILT_EXCEPT
  if (kevent->filter == EVFILT_EXCEPT)
  {
    events_out |= LWPA_POLL_OOB;
    if (kevent->flags & EV_EOF)
      events_out |= LWPA_POLL_ERR;
  }
#endif

  return events_out;
}

int poll_socket_compare(const LwpaRbTree* tree, const LwpaRbNode* node_a, const LwpaRbNode* node_b)
{
  LwpaPollSocket* a = (LwpaPollSocket*)node_a->value;
  LwpaPollSocket* b = (LwpaPollSocket*)node_b->value;

  return (a->sock > b->sock) - (a->sock < b->sock);
}

LwpaRbNode* poll_socket_alloc()
{
  return (LwpaRbNode*)malloc(sizeof(LwpaRbNode));
}

void poll_socket_free(LwpaRbNode* node)
{
  if (node)
  {
    free(node->value);
    free(node);
  }
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
    if (!sockaddr_os_to_lwpa(pf_ai->ai_addr, &ai->ai_addr))
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
