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
#include <lwip/sockets.h>
#include <lwip/netdb.h>
#include "os_error.h"

/***************************** Private macros ********************************/

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

#define LWPA_NUM_SHUT   3
static const int shutmap[LWPA_NUM_SHUT] =
{
  SHUT_RD,
  SHUT_WR,
  SHUT_RDWR
};

#define LWPA_NUM_AF     3
static const int sfmap[LWPA_NUM_AF] =
{
  AF_UNSPEC,
  AF_INET,
  AF_INET6
};

#define LWPA_NUM_TYPE   2
static const int stmap[LWPA_NUM_TYPE] =
{
  SOCK_STREAM,
  SOCK_DGRAM
};

#if LWIP_DNS

#define LWPA_NUM_AIF    8
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

#define LWPA_NUM_IPPROTO    6
static const int aiprotmap[LWPA_NUM_IPPROTO] =
{
  0,
  IPPROTO_IP,
#if LWIP_IPV6
  IPPROTO_ICMPV6,
  IPPROTO_IPV6,
#else
  0,
  0,
#endif
  IPPROTO_TCP,
  IPPROTO_UDP
};

#endif // LWIP_DNS

/* clang-format on */

/*********************** Private function prototypes *************************/

// Helpers for lwpa_setsockopt()
static void ms_to_timeval(int ms, struct timeval* tv);
static int setsockopt_socket(lwpa_socket_t id, int option_name, const void* option_value, size_t option_len);
#if LWIP_IPV4
static int setsockopt_ip(lwpa_socket_t id, int option_name, const void* option_value, size_t option_len);
#endif
#if LWIP_IPV6
static int setsockopt_ip6(lwpa_socket_t id, int option_name, const void* option_value, size_t option_len);
#endif

// Helper functions for the lwpa_poll API
static void init_context_socket_array(LwpaPollContext* context);
static LwpaPollSocket* find_socket(LwpaPollContext* context, lwpa_socket_t sock);
static LwpaPollSocket* find_hole(LwpaPollContext* context);
static void set_in_fd_sets(LwpaPollContext* context, const LwpaPollSocket* sock);
static void clear_in_fd_sets(LwpaPollContext* context, const LwpaPollSocket* sock);
static lwpa_error_t handle_select_result(LwpaPollContext* context, LwpaPollEvent* event);

/*************************** Function definitions ****************************/

lwpa_error_t lwpa_socket_init(lwpa_socket_t os_data)
{
  /* No initialization is necessary on this os. */
  (void)os_data;
  return kLwpaErrOk;
}

void lwpa_socket_deinit()
{
  /* No deinitialization is necessary on this os. */
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
  return errno_lwip_to_lwpa(errno);
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
  return (res == 0 ? kLwpaErrOk : errno_lwip_to_lwpa(errno));
}

lwpa_error_t lwpa_close(lwpa_socket_t id)
{
  int res = close(id);
  return (res == 0 ? kLwpaErrOk : errno_lwip_to_lwpa(errno));
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
  return (res == 0 ? kLwpaErrOk : errno_lwip_to_lwpa(errno));
}

lwpa_error_t lwpa_getpeername(lwpa_socket_t id, LwpaSockaddr* address)
{
  // TODO
  (void)id;
  (void)address;
  return kLwpaErrNotImpl;
}

lwpa_error_t lwpa_getsockname(lwpa_socket_t id, LwpaSockaddr* address)
{
  if (!address)
    return kLwpaErrInvalid;

  struct sockaddr_storage ss;
  socklen_t size = (socklen_t)sizeof ss;
  int res = getsockname(id, (struct sockaddr*)&ss, &size);
  if (res == 0)
  {
    if (!sockaddr_os_to_lwpa((lwpa_os_sockaddr_t*)&ss, address))
      return kLwpaErrSys;
    return kLwpaErrOk;
  }
  return errno_lwip_to_lwpa(errno);
}

lwpa_error_t lwpa_getsockopt(lwpa_socket_t id, int level, int option_name, void* option_value, size_t* option_len)
{
  // TODO
  return -1;
}

lwpa_error_t lwpa_listen(lwpa_socket_t id, int backlog)
{
  int res = listen(id, backlog);
  return (res == 0 ? kLwpaErrOk : errno_lwip_to_lwpa(errno));
}

int lwpa_recv(lwpa_socket_t id, void* buffer, size_t length, int flags)
{
  if (!buffer)
    return (int)kLwpaErrInvalid;

  int impl_flags = (flags & LWPA_MSG_PEEK) ? MSG_PEEK : 0;
  int res = (int)recv(id, buffer, length, impl_flags);
  return (res >= 0 ? res : (int)errno_lwip_to_lwpa(errno));
}

int lwpa_recvfrom(lwpa_socket_t id, void* buffer, size_t length, int flags, LwpaSockaddr* address)
{
  if (!buffer)
    return (int)kLwpaErrInvalid;

  struct sockaddr_storage fromaddr;
  socklen_t fromlen = (socklen_t)sizeof fromaddr;
  int impl_flags = (flags & LWPA_MSG_PEEK) ? MSG_PEEK : 0;
  int res = (int)recvfrom(id, buffer, length, impl_flags, (struct sockaddr*)&fromaddr, &fromlen);

  if (res >= 0)
  {
    if (address && fromlen > 0)
    {
      if (!sockaddr_os_to_lwpa((const lwpa_os_sockaddr_t*)&fromaddr, address))
        return (int)kLwpaErrSys;
    }
    return res;
  }
  return (int)errno_lwip_to_lwpa(errno);
}

int lwpa_send(lwpa_socket_t id, const void* message, size_t length, int flags)
{
  (void)flags;

  if (!message)
    return (int)kLwpaErrInvalid;

  int res = (int)send(id, message, length, 0);
  return (res >= 0 ? res : (int)errno_lwip_to_lwpa(errno));
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
  return (res >= 0 ? res : (int)errno_lwip_to_lwpa(errno));
}

lwpa_error_t lwpa_setsockopt(lwpa_socket_t id, int level, int option_name, const void* option_value, size_t option_len)
{
  int res = -1;

  if (!option_value)
    return kLwpaErrInvalid;

  switch (level)
  {
    case LWPA_SOL_SOCKET:
      res = setsockopt_socket(id, option_name, option_value, option_len);
      break;
    case LWPA_IPPROTO_IP:
#if LWIP_IPV4
      res = setsockopt_ip(id, option_name, option_value, option_len);
#else
      errno = EAFNOSUPPORT;
#endif
      break;
    case LWPA_IPPROTO_IPV6:
#if LWIP_IPV6
      res = setsockopt_ip6(id, option_name, option_value, option_len);
#else
      errno = EAFNOSUPPORT;
#endif
      break;
    default:
      break;
  }

  return (res == 0 ? kLwpaErrOk : errno_lwip_to_lwpa(errno));
}

int setsockopt_socket(lwpa_socket_t id, int option_name, const void* option_value, size_t option_len)
{
  switch (option_name)
  {
    case LWPA_SO_RCVBUF:
      return setsockopt(id, SOL_SOCKET, SO_RCVBUF, option_value, option_len);
    case LWPA_SO_SNDBUF:
      return setsockopt(id, SOL_SOCKET, SO_SNDBUF, option_value, option_len);
    case LWPA_SO_RCVTIMEO:
      if (option_len == sizeof(int))
      {
        int ms = *(int*)option_value;
        struct timeval val;
        ms_to_timeval(ms, &val);
        return setsockopt(id, SOL_SOCKET, SO_RCVTIMEO, &val, sizeof val);
      }
      break;
    case LWPA_SO_SNDTIMEO:
      if (option_len == sizeof(int))
      {
        int ms = *(int*)option_value;
        struct timeval val;
        ms_to_timeval(ms, &val);
        return setsockopt(id, SOL_SOCKET, SO_SNDTIMEO, &val, sizeof val);
      }
      break;
    case LWPA_SO_REUSEADDR:
      return setsockopt(id, SOL_SOCKET, SO_REUSEADDR, option_value, option_len);
    case LWPA_SO_REUSEPORT:
      return setsockopt(id, SOL_SOCKET, SO_REUSEPORT, option_value, option_len);
    case LWPA_SO_BROADCAST:
      return setsockopt(id, SOL_SOCKET, SO_BROADCAST, option_value, option_len);
    case LWPA_SO_KEEPALIVE:
      return setsockopt(id, SOL_SOCKET, SO_KEEPALIVE, option_value, option_len);
    case LWPA_SO_LINGER:
      if (option_len == sizeof(LwpaLinger))
      {
        LwpaLinger* ll = (LwpaLinger*)option_value;
        struct linger val;
        val.l_onoff = ll->onoff;
        val.l_linger = ll->linger;
        return setsockopt(id, SOL_SOCKET, SO_LINGER, &val, sizeof val);
      }
      break;
    case LWPA_SO_ERROR:  // Set not supported
    case LWPA_SO_TYPE:   // Set not supported
    default:
      break;
  }
  // If we got here, something was invalid. Set errno accordingly.
  errno = EINVAL;
  return -1;
}

#if LWIP_IPV4
static bool netif_index_to_ipv4_addr(unsigned int netif_index, struct in_addr* addr)
{
  struct netif* lwip_netif = netif_get_by_index((u8_t)netif_index);
  if (lwip_netif)
  {
    const ip4_addr_t* lwip_addr = netif_ip4_addr(lwip_netif);
    addr->s_addr = lwip_addr->addr;
    return true;
  }
  return false;
}

int setsockopt_ip(lwpa_socket_t id, int option_name, const void* option_value, size_t option_len)
{
  switch (option_name)
  {
    case LWPA_IP_TTL:
      return setsockopt(id, IPPROTO_IP, IP_TTL, option_value, option_len);
#if LWIP_IGMP
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
        struct ip_mreq val;
        val.imr_multiaddr.s_addr = htonl(LWPA_IP_V4_ADDRESS(&greq->group));
        if (!netif_index_to_ipv4_addr(greq->ifindex, &val.imr_interface))
          return -1;
        return setsockopt(id, IPPROTO_IP, IP_ADD_MEMBERSHIP, &val, sizeof val);
      }
      break;
    case LWPA_MCAST_LEAVE_GROUP:
      if (option_len == sizeof(LwpaGroupReq))
      {
        LwpaGroupReq* greq = (LwpaGroupReq*)option_value;
        struct ip_mreq val;
        val.imr_multiaddr.s_addr = htonl(LWPA_IP_V4_ADDRESS(&greq->group));
        if (!netif_index_to_ipv4_addr(greq->ifindex, &val.imr_interface))
          return -1;
        return setsockopt(id, IPPROTO_IP, IP_DROP_MEMBERSHIP, &val, sizeof val);
      }
      break;
#endif  // LWIP_IGMP
#if LWIP_MULTICAST_TX_OPTIONS
    case LWPA_IP_MULTICAST_IF:
      if (option_len == sizeof(unsigned int))
      {
        unsigned int netint_index = *((unsigned int*)option_value);
        struct in_addr val;
        if (!netif_index_to_ipv4_addr(netint_index, &val))
          return -1;
        return setsockopt(id, IPPROTO_IP, IP_MULTICAST_IF, &val, sizeof val);
      }
      break;
    case LWPA_IP_MULTICAST_TTL:
    {
      unsigned char val = (unsigned char)*(int*)option_value;
      return setsockopt(id, IPPROTO_IP, IP_MULTICAST_TTL, &val, sizeof val);
    }
    case LWPA_IP_MULTICAST_LOOP:
    {
      unsigned char val = (unsigned char)*(int*)option_value;
      return setsockopt(id, IPPROTO_IP, IP_MULTICAST_LOOP, &val, sizeof val);
    }
#endif  // LWIP_MULTICAST_TX_OPTIONS
    default:
      break;
  }
  // If we got here, something was invalid. Set errno accordingly.
  errno = EINVAL;
  return -1;
}
#endif  // LWIP_IPV4

#if LWIP_IPV6
int setsockopt_ip6(lwpa_socket_t id, int option_name, const void* option_value, size_t option_len)
{
  switch (option_name)
  {
    case LWPA_MCAST_JOIN_GROUP:
      if (option_len == sizeof(LwpaGroupReq))
      {
        LwpaGroupReq* greq = (LwpaGroupReq*)option_value;
        if (LWPA_IP_IS_V6(&greq->group))
        {
          struct ipv6_mreq val;
          val.ipv6mr_interface = greq->ifindex;
          memcpy(&val.ipv6mr_multiaddr.s6_addr, LWPA_IP_V6_ADDRESS(&greq->group), LWPA_IPV6_BYTES);
          return setsockopt(id, IPPROTO_IPV6, IPV6_JOIN_GROUP, &val, sizeof val);
        }
      }
      break;
    case LWPA_MCAST_LEAVE_GROUP:
      if (option_len == sizeof(LwpaGroupReq))
      {
        LwpaGroupReq* greq = (LwpaGroupReq*)option_value;
        if (LWPA_IP_IS_V6(&greq->group))
        {
          struct ipv6_mreq val;
          val.ipv6mr_interface = greq->ifindex;
          memcpy(&val.ipv6mr_multiaddr.s6_addr, LWPA_IP_V6_ADDRESS(&greq->group), LWPA_IPV6_BYTES);
          return setsockopt(id, IPPROTO_IPV6, IPV6_JOIN_GROUP, &val, sizeof val);
        }
      }
      break;
    case LWPA_IPV6_V6ONLY:
      return setsockopt(id, IPPROTO_IPV6, IPV6_V6ONLY, option_value, option_len);
    default:  // lwIP does not have support for the IPv6 multicast TX options yet.
      break;
  }
  // If we got here, something was invalid. Set errno accordingly.
  errno = EINVAL;
  return -1;
}
#endif  // LWIP_IPV6

void ms_to_timeval(int ms, struct timeval* tv)
{
  tv->tv_usec = ms / 1000;
  tv->tv_sec = (ms % 1000) * 1000;
}

lwpa_error_t lwpa_shutdown(lwpa_socket_t id, int how)
{
  if (how >= 0 && how < LWPA_NUM_SHUT)
  {
    int res = shutdown(id, shutmap[how]);
    return (res == 0 ? kLwpaErrOk : errno_lwip_to_lwpa(errno));
  }
  return -1;
}

lwpa_error_t lwpa_socket(unsigned int family, unsigned int type, lwpa_socket_t* id)
{
  if (id)
  {
    if (family < LWPA_NUM_AF && type < LWPA_NUM_TYPE)
    {
      int sock = socket(sfmap[family], stmap[type], 0);
      if (sock >= 0)
      {
        *id = sock;
        return kLwpaErrOk;
      }
      else
      {
        *id = LWPA_SOCKET_INVALID;
        return errno_lwip_to_lwpa(errno);
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
  return (val >= 0 ? kLwpaErrOk : errno_lwip_to_lwpa(errno));
}

lwpa_error_t lwpa_getblocking(lwpa_socket_t id, bool* blocking)
{
  if (blocking)
  {
    int val = fcntl(id, F_GETFL, 0);
    if (val >= 0)
    {
      *blocking = (val & O_NONBLOCK) != 0;
      return kLwpaErrOk;
    }
    return errno_lwip_to_lwpa(errno);
  }
  return kLwpaErrInvalid;
}

lwpa_error_t lwpa_poll_context_init(LwpaPollContext* context)
{
  if (!context)
    return kLwpaErrInvalid;

  init_context_socket_array(context);
  context->max_fd = -1;
  LWPA_FD_ZERO(&context->readfds);
  LWPA_FD_ZERO(&context->writefds);
  LWPA_FD_ZERO(&context->exceptfds);
  context->valid = true;
  return kLwpaErrOk;
}

void lwpa_poll_context_deinit(LwpaPollContext* context)
{
  if (!context)
    return;
  context->valid = false;
}

lwpa_error_t lwpa_poll_add_socket(LwpaPollContext* context, lwpa_socket_t socket, lwpa_poll_events_t events,
                                  void* user_data)
{
  if (!context || !context->valid || socket == LWPA_SOCKET_INVALID || !(events & LWPA_POLL_VALID_INPUT_EVENT_MASK))
    return kLwpaErrInvalid;

  if (context->num_valid_sockets >= LWPA_SOCKET_MAX_POLL_SIZE)
  {
    return kLwpaErrNoMem;
  }
  else
  {
    LwpaPollSocket* new_sock = find_hole(context);
    if (new_sock)
    {
      new_sock->sock = socket;
      new_sock->events = events;
      new_sock->user_data = user_data;
      set_in_fd_sets(context, new_sock);
      context->num_valid_sockets++;
      if (socket > context->max_fd)
        context->max_fd = socket;

      return kLwpaErrOk;
    }
    return kLwpaErrNoMem;
  }
}

lwpa_error_t lwpa_poll_modify_socket(LwpaPollContext* context, lwpa_socket_t socket, lwpa_poll_events_t new_events,
                                     void* new_user_data)
{
  if (!context || !context->valid || socket == LWPA_SOCKET_INVALID || !(new_events & LWPA_POLL_VALID_INPUT_EVENT_MASK))
    return kLwpaErrInvalid;

  LwpaPollSocket* sock_desc = find_socket(context, socket);
  if (sock_desc)
  {
    clear_in_fd_sets(context, sock_desc);
    sock_desc->events = new_events;
    sock_desc->user_data = new_user_data;
    set_in_fd_sets(context, sock_desc);
    return kLwpaErrOk;
  }
  else
  {
    return kLwpaErrNotFound;
  }
}

void lwpa_poll_remove_socket(LwpaPollContext* context, lwpa_socket_t socket)
{
  if (!context || !context->valid || socket == LWPA_SOCKET_INVALID)
    return;

  LwpaPollSocket* sock_desc = find_socket(context, socket);
  if (sock_desc)
  {
    clear_in_fd_sets(context, sock_desc);
    sock_desc->sock = LWPA_SOCKET_INVALID;
    context->num_valid_sockets--;
    if (socket == context->max_fd)
    {
      while (context->max_fd > 0 &&
             (LWPA_FD_ISSET(context->max_fd, &context->readfds) || LWPA_FD_ISSET(context->max_fd, &context->writefds) ||
              LWPA_FD_ISSET(context->max_fd, &context->exceptfds)))
      {
        --context->max_fd;
      }
    }
  }
}

lwpa_error_t lwpa_poll_wait(LwpaPollContext* context, LwpaPollEvent* event, int timeout_ms)
{
  if (!context || !context->valid || !event)
    return kLwpaErrInvalid;

  if (context->readfds.count == 0 && context->writefds.count == 0 && context->exceptfds.count == 0)
  {
    // No valid sockets are currently added to the context.
    return kLwpaErrNoSockets;
  }

  struct timeval os_timeout;
  if (timeout_ms != LWPA_WAIT_FOREVER)
    ms_to_timeval(timeout_ms, &os_timeout);

  int sel_res = select(context->max_fd + 1, context->readfds.count ? &context->readfds.set : NULL,
                       context->writefds.count ? &context->writefds.set : NULL,
                       context->exceptfds.count ? &context->exceptfds.set : NULL,
                       timeout_ms == LWPA_WAIT_FOREVER ? NULL : &os_timeout);

  if (sel_res < 0)
  {
    return errno_lwip_to_lwpa(errno);
  }
  else if (sel_res == 0)
  {
    return kLwpaErrTimedOut;
  }
  else
  {
    return handle_select_result(context, event);
  }
}

lwpa_error_t handle_select_result(LwpaPollContext* context, LwpaPollEvent* event)
{
  // Init the event data.
  event->socket = LWPA_SOCKET_INVALID;
  event->events = 0;
  event->err = kLwpaErrOk;

  // The default return; if we don't find any sockets set that we passed to select(), something has
  // gone wrong.
  lwpa_error_t res = kLwpaErrSys;

  for (LwpaPollSocket* sock_desc = context->sockets; sock_desc < context->sockets + LWPA_SOCKET_MAX_POLL_SIZE;
       ++sock_desc)
  {
    if (sock_desc->sock == LWPA_SOCKET_INVALID)
      continue;

    if (LWPA_FD_ISSET(sock_desc->sock, &context->readfds) || LWPA_FD_ISSET(sock_desc->sock, &context->writefds) ||
        LWPA_FD_ISSET(sock_desc->sock, &context->exceptfds))
    {
      res = kLwpaErrOk;
      event->socket = sock_desc->sock;
      event->user_data = sock_desc->user_data;

      if (LWPA_FD_ISSET(sock_desc->sock, &context->readfds))
      {
        if (sock_desc->events & LWPA_POLL_IN)
          event->events |= LWPA_POLL_IN;
      }
      if (LWPA_FD_ISSET(sock_desc->sock, &context->writefds))
      {
        if (sock_desc->events & LWPA_POLL_CONNECT)
          event->events |= LWPA_POLL_CONNECT;
        else if (sock_desc->events & LWPA_POLL_OUT)
          event->events |= LWPA_POLL_OUT;
      }
      if (LWPA_FD_ISSET(sock_desc->sock, &context->exceptfds))
      {
        event->events |= LWPA_POLL_ERR;
        int err;
        socklen_t err_size = (socklen_t)sizeof err;
        if (getsockopt(sock_desc->sock, SOL_SOCKET, SO_ERROR, &err, &err_size) == 0)
          event->err = errno_lwip_to_lwpa(err);
        else
          event->err = kLwpaErrSys;
      }

      break;  // We handle one event at a time.
    }
  }
  return res;
}

void init_context_socket_array(LwpaPollContext* context)
{
  context->num_valid_sockets = 0;
  for (LwpaPollSocket* poll_sock = context->sockets; poll_sock < context->sockets + LWPA_SOCKET_MAX_POLL_SIZE;
       ++poll_sock)
  {
    poll_sock->sock = LWPA_SOCKET_INVALID;
  }
}

LwpaPollSocket* find_socket(LwpaPollContext* context, lwpa_socket_t sock)
{
  for (LwpaPollSocket* poll_sock = context->sockets; poll_sock < context->sockets + LWPA_SOCKET_MAX_POLL_SIZE;
       ++poll_sock)
  {
    if (poll_sock->sock == sock)
      return poll_sock;
  }
  return NULL;
}

LwpaPollSocket* find_hole(LwpaPollContext* context)
{
  return find_socket(context, LWPA_SOCKET_INVALID);
}

void set_in_fd_sets(LwpaPollContext* context, const LwpaPollSocket* poll_sock)
{
  if (poll_sock->events & LWPA_POLL_IN)
  {
    LWPA_FD_SET(poll_sock->sock, &context->readfds);
  }
  if (poll_sock->events & (LWPA_POLL_OUT | LWPA_POLL_CONNECT))
  {
    LWPA_FD_SET(poll_sock->sock, &context->writefds);
  }
  // exceptfds is used for errors on this platform, so set it regardless
  LWPA_FD_SET(poll_sock->sock, &context->exceptfds);
}

void clear_in_fd_sets(LwpaPollContext* context, const LwpaPollSocket* poll_sock)
{
  if (poll_sock->events & LWPA_POLL_IN)
  {
    LWPA_FD_CLEAR(poll_sock->sock, &context->readfds);
  }
  if (poll_sock->events & (LWPA_POLL_OUT | LWPA_POLL_CONNECT))
  {
    LWPA_FD_CLEAR(poll_sock->sock, &context->writefds);
  }
  LWPA_FD_CLEAR(poll_sock->sock, &context->exceptfds);
}

lwpa_error_t lwpa_getaddrinfo(const char* hostname, const char* service, const LwpaAddrinfo* hints,
                              LwpaAddrinfo* result)
{
#if LWIP_DNS
  int res;
  struct addrinfo* pf_res;
  struct addrinfo pf_hints;

  if ((!hostname && !service) || !result)
    return -1;

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
  return res;
#else   // LWIP_DNS
  return kLwpaErrNotImpl;
#endif  // LWIP_DNS
}

bool lwpa_nextaddr(LwpaAddrinfo* ai)
{
#if LWIP_DNS
  if (ai && ai->pd[1])
  {
    struct addrinfo* pf_ai = (struct addrinfo*)ai->pd[1];
    ai->ai_flags = 0;
    if (!sockaddr_os_to_lwpa(&ai->ai_addr, pf_ai->ai_addr))
      return false;
    /* Can't use reverse maps, because we have no guarantee of the numeric
     * values of the os constants. Ugh. */
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
#endif  // LWIP_DNS
  return false;
}

void lwpa_freeaddrinfo(LwpaAddrinfo* ai)
{
#if LWIP_DNS
  if (ai)
    freeaddrinfo((struct addrinfo*)ai->pd[0]);
#endif  // LWIP_DNS
}
