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

#include "lwpa_socket.h"

#include <string.h>
#include <mqx.h>
#include <bsp.h>
#include <rtcs.h>

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
  PF_INET,
  PF_INET6
};

#define LWPA_NUM_TYPE   2
static const uint32_t stmap[LWPA_NUM_TYPE] =
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
  0,
  0,
  0,
  IPPROTO_TCP,
  IPPROTO_UDP
};

/* clang-format on */

static lwpa_error_t err_os_to_lwpa(uint32_t rtcserr)
{
  switch (rtcserr)
  {
    case RTCS_OK:
      return kLwpaErrOk;
    case RTCSERR_OUT_OF_MEMORY:
    case RTCSERR_OUT_OF_BUFFERS:
    case RTCSERR_OUT_OF_SOCKETS:
      return kLwpaErrNoMem;
    case RTCSERR_TIMEOUT:
      return kLwpaErrTimedOut;
    case RTCSERR_INVALID_ADDRESS:
    case RTCSERR_INVALID_PARAMETER:
    case RTCSERR_SOCK_INVALID:
    case RTCSERR_SOCK_INVALID_AF:
    case RTCSERR_SOCK_SHORT_ADDRESS:
    case RTCSERR_SOCK_NOT_BOUND:
    case RTCSERR_SOCK_NOT_LISTENING:
    case RTCSERR_SOCK_NOT_OWNER:
    case RTCSERR_SOCK_CLOSED:
    case RTCSERR_SOCK_INVALID_OPTION:
    case RTCSERR_SOCK_SHORT_OPTION:
    case RTCSERR_SOCK_ALLOC_GROUP:
    case RTCSERR_SOCK_NOT_JOINED:
    case RTCSERR_SOCK_INVALID_PARAMETER:
    case RTCSERR_SOCK_EBADF:
    case RTCSERR_SOCK_EINVAL:
    case RTCSERR_SOCK_OPTION_IS_READ_ONLY:
      return kLwpaErrInvalid;
    case RTCSERR_FEATURE_NOT_ENABLED:
    case RTCSERR_SOCK_NOT_SUPPORTED:
      return kLwpaErrNotImpl;
    case RTCSERR_SOCK_NOT_CONNECTED:
      return kLwpaErrNotConn;
    case RTCSERR_SOCK_IS_BOUND:
    case RTCSERR_SOCK_IS_LISTENING:
      return kLwpaErrAlready;
    case RTCSERR_SOCK_IS_CONNECTED:
      return kLwpaErrIsConn;
    case RTCSERR_SOCK_ESHUTDOWN:
      return kLwpaErrShutdown;
    default:
      return kLwpaErrSys;
  }
}

lwpa_error_t lwpa_socket_init(void* os_data)
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
  /* TODO */
  return kLwpaErrNotImpl;
}

lwpa_error_t lwpa_bind(lwpa_socket_t id, const LwpaSockaddr* address)
{
  struct sockaddr ss;
  size_t sa_size;

  if (!address)
    return kLwpaErrInvalid;

  sa_size = sockaddr_lwpa_to_os(&ss, address);
  if (sa_size == 0)
    return kLwpaErrInvalid;

  return err_os_to_lwpa(bind(id, &ss, sa_size));
}

lwpa_error_t lwpa_close(lwpa_socket_t id)
{
  return err_os_to_lwpa(closesocket(id));
}

lwpa_error_t lwpa_connect(lwpa_socket_t id, const LwpaSockaddr* address)
{
  /* TODO */
  return kLwpaErrNotImpl;
}

lwpa_error_t lwpa_getpeername(lwpa_socket_t id, LwpaSockaddr* address)
{
  /* TODO */
  return kLwpaErrNotImpl;
}

lwpa_error_t lwpa_getsockname(lwpa_socket_t id, LwpaSockaddr* address)
{
  struct sockaddr ss;
  uint16_t size = sizeof ss;
  uint32_t res;

  if (!address)
    return kLwpaErrInvalid;

  res = getsockname(id, &ss, &size);
  if (res == RTCS_OK)
  {
    if (!sockaddr_os_to_lwpa(address, &ss))
      return kLwpaErrSys;
  }
  return err_os_to_lwpa(res);
}

lwpa_error_t lwpa_getsockopt(lwpa_socket_t id, int level, int option_name, void* option_value, size_t* option_len)
{
  /* TODO */
  return kLwpaErrNotImpl;
}

lwpa_error_t lwpa_listen(lwpa_socket_t id, int backlog)
{
  /* TODO */
  return kLwpaErrNotImpl;
}

int lwpa_recv(lwpa_socket_t id, void* buffer, size_t length, int flags)
{
  /* TODO */
  return kLwpaErrNotImpl;
}

int lwpa_recvfrom(lwpa_socket_t id, void* buffer, size_t length, int flags, LwpaSockaddr* address)
{
  int32_t res;
  uint32_t impl_flags = (flags & LWPA_MSG_PEEK) ? RTCS_MSG_PEEK : 0;
  struct sockaddr fromaddr;
  uint16_t fromlen = sizeof fromaddr;

  if (!buffer)
    return kLwpaErrInvalid;

  res = recvfrom(id, buffer, length, impl_flags, &fromaddr, &fromlen);

  if (res != RTCS_ERROR)
  {
    if (address && fromlen > 0)
    {
      if (!sockaddr_os_to_lwpa(address, &fromaddr))
        return kLwpaErrInvalid;
    }
  }
  return (res == RTCS_ERROR ? err_os_to_lwpa(RTCS_geterror(id)) : res);
}

int lwpa_send(lwpa_socket_t id, const void* message, size_t length, int flags)
{
  /* TODO */
  return kLwpaErrNotImpl;
}

int lwpa_sendto(lwpa_socket_t id, const void* message, size_t length, int flags, const LwpaSockaddr* dest_addr)
{
  int32_t res;
  size_t ss_size;
  struct sockaddr ss;

  if (!dest_addr || !message)
    return kLwpaErrInvalid;

  if ((ss_size = sockaddr_lwpa_to_os(&ss, dest_addr)) == 0)
    return kLwpaErrInvalid;

  res = sendto(id, (char*)message, (uint32_t)length, 0, &ss, ss_size);
  return (res == RTCS_ERROR ? err_os_to_lwpa(RTCS_geterror(id)) : res);
}

lwpa_error_t lwpa_setsockopt(lwpa_socket_t id, int level, int option_name, const void* option_value, size_t option_len)
{
  uint32_t res = RTCSERR_SOCK_INVALID_OPTION;

  if (!option_value)
    return kLwpaErrInvalid;

  switch (option_name)
  {
    case LWPA_SO_RCVBUF:
      if (level == LWPA_SOL_SOCKET && option_len >= sizeof(uint32_t))
      {
        uint32_t sock_type;
        uint32_t size = sizeof sock_type;
        if (RTCS_OK == getsockopt(id, SOL_SOCKET, OPT_SOCKET_TYPE, &sock_type, &size))
        {
          uint32_t val = (uint32_t)(*(int*)option_value);
          if (sock_type == SOCK_STREAM)
          {
            res = setsockopt(id, SOL_TCP, OPT_RBSIZE, &val, sizeof val);
          }
          else if (sock_type == SOCK_DGRAM)
          {
            res = setsockopt(id, SOL_UDP, OPT_RBSIZE, &val, sizeof val);
          }
        }
      }
      break;
    case LWPA_SO_SNDBUF:
      if (level == LWPA_SOL_SOCKET && option_len >= sizeof(uint32_t))
      {
        uint32_t val = *(int*)option_value;
        res = setsockopt(id, SOL_TCP, OPT_TBSIZE, &val, sizeof val);
      }
      break;
    case LWPA_SO_RCVTIMEO:
      if (level == LWPA_SOL_SOCKET && option_len >= sizeof(uint32_t))
      {
        uint32_t val = (uint32_t)(*(int*)option_value);
        res = setsockopt(id, SOL_SOCKET, SO_RCVTIMEO, &val, sizeof val);
      }
      break;
    case LWPA_SO_SNDTIMEO:
      if (level == LWPA_SOL_SOCKET && option_len >= sizeof(uint32_t))
      {
        uint32_t val = (uint32_t)(*(int*)option_value);
        res = setsockopt(id, SOL_SOCKET, SO_SNDTIMEO, &val, sizeof val);
      }
      break;
    case LWPA_IP_TTL:
      if (level == LWPA_IPPROTO_IP && option_len >= sizeof(unsigned char))
      {
        unsigned char val = (unsigned char)(*(int*)option_value);
        res = setsockopt(id, SOL_IP, RTCS_SO_IP_TX_TTL, &val, sizeof val);
      }
      break;
    case LWPA_MCAST_JOIN_GROUP:
      if (option_len == sizeof(LwpaMreq))
      {
        LwpaMreq* amreq = (LwpaMreq*)option_value;
        if (LWPA_IP_IS_V4(&amreq->group) && level == LWPA_IPPROTO_IP)
        {
          /* We don't need htonl -- MQX/RTCS stores IPv4s in host order. */
          struct ip_mreq val;
          val.imr_multiaddr.s_addr = LWPA_IP_V4_ADDRESS(&amreq->group);
          val.imr_interface.s_addr = LWPA_IP_V4_ADDRESS(&amreq->netint);
          res = setsockopt(id, SOL_IGMP, RTCS_SO_IGMP_ADD_MEMBERSHIP, &val, sizeof val);
          if (res == RTCS_OK)
          {
            /* Voodoo to allow locally-sent multicast traffic to be received by local sockets. We
             * don't check the result. */
            val.imr_interface.s_addr = 0x7f000001;
            setsockopt(id, SOL_IGMP, RTCS_SO_IGMP_ADD_MEMBERSHIP, &val, sizeof val);
          }
        }
        else if (LWPA_IP_IS_V6(&amreq->group) && level == LWPA_IPPROTO_IPV6)
        {
          struct ipv6_mreq val;
          val.ipv6imr_interface = 0; /* TODO find out what MQX does with interface indexes */
          memcpy(&val.ipv6imr_multiaddr.s6_addr, LWPA_IP_V6_ADDRESS(&amreq->group), IPV6_BYTES);
          res = setsockopt(id, SOL_IP6, RTCS_SO_IP6_JOIN_GROUP, &val, sizeof val);
        }
      }
      break;
    case LWPA_MCAST_LEAVE_GROUP:
      if (option_len == sizeof(LwpaMreq))
      {
        LwpaMreq* amreq = (LwpaMreq*)option_value;
        if (LWPA_IP_IS_V4(&amreq->group) && level == LWPA_IPPROTO_IP)
        {
          /* We don't need htonl -- MQX/RTCS stores IPv4s in host order. */
          struct ip_mreq val;
          val.imr_multiaddr.s_addr = LWPA_IP_V4_ADDRESS(&amreq->group);
          val.imr_interface.s_addr = LWPA_IP_V4_ADDRESS(&amreq->netint);
          res = setsockopt(id, SOL_IGMP, RTCS_SO_IGMP_DROP_MEMBERSHIP, &val, sizeof val);
        }
        else if (LWPA_IP_IS_V6(&amreq->group) && level == LWPA_IPPROTO_IPV6)
        {
          struct ipv6_mreq val;
          val.ipv6imr_interface = 0; /* TODO find out what MQX does with interface indexes */
          memcpy(&val.ipv6imr_multiaddr.s6_addr, LWPA_IP_V6_ADDRESS(&amreq->group), IPV6_BYTES);
          res = setsockopt(id, SOL_IP6, RTCS_SO_IP6_LEAVE_GROUP, &val, sizeof val);
        }
      }
      break;
    case LWPA_SO_REUSEADDR:
    case LWPA_SO_REUSEPORT:
      /* Option not supported, but seems to be allowed by default */
      res = RTCS_OK;
      break;
    case LWPA_IP_MULTICAST_IF:
      /* This one is a bit tricky. The stack doesn't actually support this functionality, but for
       * systems with only one network interface we want to pretend that it does if called
       * correctly. This makes the behavior for os-neutral calling code more consistent. */
#if BSP_ENET_DEVICE_COUNT == 1
      if (option_len == sizeof(LwpaIpAddr))
      {
        LwpaIpAddr* netint_requested = (LwpaIpAddr*)option_value;
        IPCFG_IP_ADDRESS_DATA ip_data;

        if (ipcfg_get_ip(BSP_DEFAULT_ENET_DEVICE, &ip_data))
        {
          LwpaIpAddr default_netint_ip;

          LWPA_IP_SET_V4_ADDRESS(&default_netint_ip, ip_data.ip);
          if (lwpa_ip_equal(netint_requested, &default_netint_ip))
            res = kLwpaErrOk;
        }
      }
#endif
      break;
    case LWPA_SO_BROADCAST:      /* Not supported */
    case LWPA_SO_ERROR:          /* Set not supported */
    case LWPA_SO_KEEPALIVE:      /* TODO */
    case LWPA_SO_LINGER:         /* TODO */
    case LWPA_SO_TYPE:           /* Not supported */
    case LWPA_IP_MULTICAST_TTL:  /* Not supported */
    case LWPA_IP_MULTICAST_LOOP: /* TODO */
    default:
      break;
  }

  return err_os_to_lwpa(res);
}

lwpa_error_t lwpa_shutdown(lwpa_socket_t id, int how)
{
  if (how >= 0 && how < LWPA_NUM_SHUT)
  {
    return err_os_to_lwpa(shutdownsocket(id, (int32_t)shutmap[how]));
  }
  return kLwpaErrInvalid;
}

lwpa_error_t lwpa_socket(unsigned int family, unsigned int type, lwpa_socket_t* id)
{
  if (id)
  {
    if (family < LWPA_NUM_AF && type < LWPA_NUM_TYPE)
    {
      uint32_t sock = socket(sfmap[family], stmap[type], 0);
      if (sock != RTCS_SOCKET_ERROR)
      {
        *id = sock;
        return kLwpaErrOk;
      }
      else
      {
        *id = LWPA_SOCKET_INVALID;
        /* RTCS does not provide error codes on socket failure */
        return kLwpaErrSys;
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
  uint32_t sock_type;
  socklen_t uint32_size = sizeof(uint32_t);
  if (RTCS_OK == getsockopt(id, SOL_SOCKET, OPT_SOCKET_TYPE, &sock_type, &uint32_size))
  {
    uint32_t res;
    uint32_t opt_value = (blocking ? FALSE : TRUE);
    if (sock_type == SOCK_STREAM)
    {
      res = setsockopt(id, SOL_TCP, OPT_RECEIVE_NOWAIT, &opt_value, uint32_size);
      if (res == RTCS_OK)
        res = setsockopt(id, SOL_TCP, OPT_SEND_NOWAIT, &opt_value, uint32_size);
      return err_os_to_lwpa(res);
    }
    else if (sock_type == SOCK_DGRAM)
    {
      res = setsockopt(id, SOL_UDP, OPT_RECEIVE_NOWAIT, &opt_value, uint32_size);
      if (res == RTCS_OK)
        res = setsockopt(id, SOL_UDP, OPT_SEND_NOWAIT, &opt_value, uint32_size);
      return err_os_to_lwpa(res);
    }
    else
    {
      return kLwpaErrInvalid;
    }
  }
  return kLwpaErrSys;
}

int lwpa_poll(LwpaPollfd* fds, size_t nfds, int timeout_ms)
{
  rtcs_fd_set readfds;
  rtcs_fd_set writefds;
  LwpaPollfd* fd;
  int32_t nosfds;
  int sel_res;
  int32_t nreadfds = 0, nwritefds = 0;
  uint32_t os_timeout;

  if (fds && nfds > 0)
  {
    RTCS_FD_ZERO(&readfds);
    RTCS_FD_ZERO(&writefds);

    for (fd = fds; fd < fds + nfds; ++fd)
    {
      if (fd->events & LWPA_POLLIN)
      {
        RTCS_FD_SET(fd->fd, &readfds);
        nreadfds++;
      }
      if (fd->events & LWPA_POLLOUT)
      {
        RTCS_FD_SET(fd->fd, &writefds);
        nwritefds++;
      }
      /* LWPA_POLLPRI/exceptfds is not handled properly on this OS */
    }
  }

  if (timeout_ms == LWPA_WAIT_FOREVER)
    os_timeout = 0;
  else if (timeout_ms == 0)
    os_timeout = 0xffffffff;
  else
    os_timeout = (uint32_t)timeout_ms;
  nosfds = (nreadfds > nwritefds) ? nreadfds : nwritefds;
  sel_res = select(nosfds, nreadfds ? &readfds : NULL, nwritefds ? &writefds : NULL, NULL, os_timeout);

  if (sel_res == RTCS_ERROR)
  {
    return err_os_to_lwpa(RTCS_get_errno());
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
      if (nreadfds && (fd->events & LWPA_POLLIN) && RTCS_FD_ISSET(fd->fd, &readfds))
      {
        fd->revents |= LWPA_POLLIN;
      }
      if (nwritefds && (fd->events & LWPA_POLLOUT) && RTCS_FD_ISSET(fd->fd, &writefds))
      {
        fd->revents |= LWPA_POLLOUT;
      }
      /* LWPA_POLLPRI/exceptfds is not handled properly on this OS */
    }
  }
  return (int)sel_res;
}

lwpa_error_t lwpa_getaddrinfo(const char* hostname, const char* service, const LwpaAddrinfo* hints,
                              LwpaAddrinfo* result)
{
  int32_t res;
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
      return kLwpaErrSys;
  }
  return err_os_to_lwpa(res);
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
