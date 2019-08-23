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

/**************************** Private constants ******************************/

/****************************** Private types ********************************/

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

/* clang-format on */

/*********************** Private function prototypes *************************/

// Helpers for lwpa_setsockopt()
static void ms_to_timeval(int ms, struct timeval* tv);
static int setsockopt_socket(lwpa_socket_t id, int option_name, const void* option_value, size_t option_len);
static int setsockopt_ip(lwpa_socket_t id, int option_name, const void* option_value, size_t option_len);
static int setsockopt_ip6(lwpa_socket_t id, int option_name, const void* option_value, size_t option_len);

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

lwpa_error_t lwpa_getpeername(lwpa_error_t id, LwpaSockaddr* address)
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
    if (!sockaddr_os_to_lwpa((lwpa_os_sockadr_t*)&ss, address))
      return kLwpaErrSys;
    return kLwpaErrOk;
  }
  return errno_lwip_to_lwpa(errno);
}

int lwpa_getsockopt(lwpa_socket_t id, int level, int option_name, lwpa_socket_t option_value, size_t* option_len)
{
  // TODO
  return -1;
}

int lwpa_listen(lwpa_socket_t id, int backlog)
{
  int res = listen(id, backlog);
  return (res == 0 ? kLwpaErrOk : errno_lwip_to_lwpa(errno));
}

int lwpa_recv(lwpa_socket_t id, void* buffer, size_t length, int flags)
{
  if (!buffer)
    return (int)kLwpaErrInvalid;

  struct sockaddr_storage from_addr;
  socklen_t from_len = (socklen_t)sizeof from_addr;
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
      if (!sockaddr_os_to_lwpa(address, (struct sockaddr*)&fromaddr))
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
  return (res >= 0 ? res : (int)errno_os_to_lwpa(errno));
}

int lwpa_setsockopt(lwpa_socket_t id, int level, int option_name, const void* option_value, size_t option_len)
{
  int res = -1;

  if (!option_value)
    return (int)kLwpaErrInvalid;

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
      break;
  }
  return res;
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

static bool netif_index_to_ipv4_addr(unsigned int netif_index, struct in_addr* addr)
{
}

int setsockopt_ip(lwpa_socket_t id, int option_name, const void* option_value, size_t option_len)
{
  switch (option_name)
  {
    case LWPA_IP_TTL:
      return setsockopt(id, IPPROTO_IP, IP_TTL, option_value, option_len);
      break;
#if LWIP_IGMP || 1
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
        LwpaIpAddr* netint = (LwpaIpAddr*)option_value;
        if (LWPA_IP_IS_V4(netint))
        {
          struct in_addr val;
          val.s_addr = htonl(LWPA_IP_V4_ADDRESS(netint));
          return setsockopt(id, IPPROTO_IP, IP_MULTICAST_IF, &val, sizeof val);
        }
      }
      break;
    case LWPA_IP_MULTICAST_TTL:
    {
      unsigned char val = (unsigned char)*(int*)option_value;
      return setsockopt(id, IPPROTO_IP, IP_MULTICAST_TTL, &val, sizeof val);
    }
    break;
    case LWPA_IP_MULTICAST_LOOP:
    {
      unsigned char val = (unsigned char)*(int*)option_value;
      return setsockopt(id, IPPROTO_IP, IP_MULTICAST_LOOP, &val, sizeof val);
    }
    break;
#endif  // LWIP_MULTICAST_TX_OPTIONS
    default:
      break;
  }
  // If we got here, something was invalid. Set errno accordingly.
  errno = EINVAL;
  return -1;
}

int setsockopt_ip6(lwpa_socket_t id, int option_name, const void* option_value, size_t option_len)
{
  switch (option_name)
  {
    case LWPA_MCAST_JOIN_GROUP:
      /* This is not supported in lwip yet.
      if (option_len == sizeof(struct lwpa_mreq))
      {
        LwpaMreq *amreq = (LwpaMreq *)option_value;
        if (LWPA_IP_IS_V6(&amreq->group))
        {
          struct ipv6_mreq val;
          val.ipv6imr_interface = 0;
          memcpy(&val.ipv6imr_multiaddr.s6_addr,
                 LWPA_IP_V6_ADDRESS(&amreq->group), IPV6_BYTES);
          res = setsockopt(id, IPPROTO_IPV6, MCAST_JOIN_GROUP, &val,
                           sizeof val);
        }
      }
      */
      break;
    case LWPA_MCAST_LEAVE_GROUP:
      /* This is not supported in lwip yet.
      if (option_len == sizeof(struct lwpa_mreq))
      {
        LwpaMreq *amreq = (LwpaMreq *)option_value;
        if (LWPA_IP_IS_V6(&amreq->group))
        {
          struct ipv6_mreq val;
          val.ipv6imr_interface = 0;
          memcpy(&val.ipv6imr_multiaddr.s6_addr,
                 LWPA_IP_V6_ADDRESS(&amreq->group), IPV6_BYTES);
          res = setsockopt(id, IPPROTO_IPV6, MCAST_JOIN_GROUP, &val,
                           sizeof val);
        }
      }
      */
      break;
    default:  // lwIP does not have a lot of IPV6 socket option support yet.
      break;
  }
}

void ms_to_timeval(int ms, struct timeval* tv)
{
  tv->tv_usec = ms / 1000;
  tv->tv_sec = (ms % 1000) * 1000;
}

int lwpa_shutdown(lwpa_socket_t id, int how)
{
  if (how >= 0 && how < LWPA_NUM_SHUT)
    return shutdown(id, shutmap[how]);
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
        return err_os_to_lwpa(errno);
      }
    }
    else
    {
      *id = LWPA_SOCKET_INVALID;
    }
  }
  return kLwpaErrInvalid;
}

lwpa_error_t lwpa_poll_context_init(LwpaPollContext* context)
{
  if (!context)
    return kLwpaErrInvalid;

  init_context_socket_array(context);
  LWPA_FD_ZERO(&context->readfds);
  LWPA_FD_ZERO(&context->writefds);
  context->valid = true;
  return kLwpaErrOk;
}

void lwpa_poll_context_deinit(LwpaPollContext* context)
{
  if (!context || !context->valid)
    return;

  context->valid = false;
}

lwpa_error_t lwpa_poll_add_socket(LwpaPollContext* context, lwpa_socket_t socket, lwpa_poll_events_t events,
                                  lwpa_socket_t user_data)
{
  if (!context || !context->valid || socket == LWPA_SOCKET_INVALID || !(events & LWPA_POLL_VALID_INPUT_EVENT_MASK))
    return kLwpaErrInvalid;

  if (context->num_valid_sockets >= LWPA_SOCKET_MAX_POLL_SIZE)
  {
    return kLwpaErrNoMem;
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
      return kLwpaErrOk;
    }
    return kLwpaErrNoMem;
  }
}

lwpa_error_t lwpa_poll_modify_socket(LwpaPollContext* context, lwpa_socket_t socket, lwpa_poll_events_t new_events,
                                     lwpa_socket_t new_user_data)
{
  if (!context || !context->valid || socket == LWPA_SOCKET_INVALID || !(new_events & LWPA_POLL_VALID_INPUT_EVENT_MASK))
    return kLwpaErrInvalid;

  LwpaPollCtxSocket* sock_desc = find_socket(context, socket);
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

  LwpaPollCtxSocket* sock_desc = find_socket(context, socket);
  if (sock_desc)
  {
    clear_in_fd_sets(context, sock_desc);
    sock_desc->socket = LWPA_SOCKET_INVALID;
    context->num_valid_sockets--;
  }
}

lwpa_error_t lwpa_poll_wait(LwpaPollContext* context, LwpaPollEvent* event, int timeout_ms)
{
  if (!context || !context->valid || !event)
    return kLwpaErrInvalid;

  if (context->readfds.count == 0 && context->writefds.count == 0)
  {
    // No valid sockets are currently added to the context.
    return kLwpaErrNoSockets;
  }

  uint32_t os_timeout;
  if (timeout_ms == LWPA_WAIT_FOREVER)
    os_timeout = 0;
  else if (timeout_ms == 0)
    os_timeout = 0xffffffff;
  else
    os_timeout = (uint32_t)timeout_ms;

  int32_t nfds =
      (int32_t)((context->readfds.count > context->writefds.count) ? context->readfds.count : context->writefds.count);
  int32_t sel_res = select(nfds, context->readfds.count ? &context->readfds.set : NULL,
                           context->writefds.count ? &context->writefds.set : NULL, NULL, os_timeout);

  if (sel_res == RTCS_ERROR)
  {
    // RTCS handles some socket errors by returning them from select().
    uint32_t rtcs_err = RTCS_get_errno();
    if (rtcs_err == RTCSERR_SOCK_ESHUTDOWN)
      return handle_select_result(context, event, kLwpaErrConnClosed);
    else if (rtcs_err == RTCSERR_SOCK_CLOSED)
      return handle_select_result(context, event, kLwpaErrNotFound);
    else
      return err_os_to_lwpa(rtcs_err);
  }
  else if (sel_res == 0)
  {
    return kLwpaErrTimedOut;
  }
  else
  {
    return handle_select_result(context, event, kLwpaErrOk);
  }
}

lwpa_error_t handle_select_result(LwpaPollContext* context, LwpaPollEvent* event, lwpa_error_t socket_error)
{
  // Init the event data.
  event->socket = LWPA_SOCKET_INVALID;
  event->events = 0;
  event->err = kLwpaErrOk;

  // The default return; if we don't find any sockets set that we passed to select(), something has
  // gone wrong.
  lwpa_error_t res = kLwpaErrSys;

  for (LwpaPollCtxSocket* sock_desc = context->sockets; sock_desc < context->sockets + LWPA_SOCKET_MAX_POLL_SIZE;
       ++sock_desc)
  {
    if (sock_desc->socket == LWPA_SOCKET_INVALID)
      continue;

    if (LWPA_FD_ISSET(sock_desc->socket, &context->readfds) || LWPA_FD_ISSET(sock_desc->socket, &context->writefds))
    {
      res = kLwpaErrOk;
      event->socket = sock_desc->socket;
      event->user_data = sock_desc->user_data;

      /* Check for errors */
      if (socket_error != kLwpaErrOk)
      {
        event->events |= LWPA_POLL_ERR;
        event->err = socket_error;
      }

      if (LWPA_FD_ISSET(sock_desc->socket, &context->readfds))
      {
        if (sock_desc->events & LWPA_POLL_IN)
          event->events |= LWPA_POLL_IN;
      }
      if (LWPA_FD_ISSET(sock_desc->socket, &context->writefds))
      {
        if (sock_desc->events & LWPA_POLL_CONNECT)
          event->events |= LWPA_POLL_CONNECT;
        else if (sock_desc->events & LWPA_POLL_OUT)
          event->events |= LWPA_POLL_OUT;
      }
      // LWPA_POLL_OOB/exceptfds is not handled properly on this OS

      break;  // We handle one event at a time.
    }
  }
  return res;
}

void init_context_socket_array(LwpaPollContext* context)
{
  context->num_valid_sockets = 0;
  for (LwpaPollCtxSocket* ctx_socket = context->sockets; ctx_socket < context->sockets + LWPA_SOCKET_MAX_POLL_SIZE;
       ++ctx_socket)
  {
    ctx_socket->socket = LWPA_SOCKET_INVALID;
  }
}

LwpaPollCtxSocket* find_socket(LwpaPollContext* context, lwpa_socket_t socket)
{
  for (LwpaPollCtxSocket* cur = context->sockets; cur < context->sockets + LWPA_SOCKET_MAX_POLL_SIZE; ++cur)
  {
    if (cur->socket == socket)
      return cur;
  }
  return NULL;
}

LwpaPollCtxSocket* find_hole(LwpaPollContext* context)
{
  return find_socket(context, LWPA_SOCKET_INVALID);
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
}

int lwpa_getaddrinfo(const char* hostname, const char* service, const LwpaAddrinfo* hints, LwpaAddrinfo* result)
{
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
}

bool lwpa_nextaddr(LwpaAddrinfo* ai)
{
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
  return false;
}

void lwpa_freeaddrinfo(LwpaAddrinfo* ai)
{
  if (ai)
    freeaddrinfo((struct addrinfo*)ai->pd[0]);
}
