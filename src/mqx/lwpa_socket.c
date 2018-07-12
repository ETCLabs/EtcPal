/******************************************************************************
 * Copyright 2018 ETC Inc.
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
  0,
  0,
  0,
  IPPROTO_TCP,
  IPPROTO_UDP
};

/* clang-format on */

static bool sockaddr_plat_to_lwpa(LwpaSockaddr *sa, const struct sockaddr *pfsa)
{
  if (pfsa->sa_family == AF_INET)
  {
    struct sockaddr_in *sin = (struct sockaddr_in *)pfsa;
    sa->port = sin->sin_port;
    lwpaip_set_v4_address(&sa->ip, sin->sin_addr.s_addr);
    return true;
  }
  else if (pfsa->sa_family == AF_INET6)
  {
    struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *)pfsa;
    sa->port = sin6->sin6_port;
    lwpaip_set_v6_address(&sa->ip, sin6->sin6_addr.s6_addr);
    return true;
  }
  return false;
}

static size_t sockaddr_lwpa_to_plat(struct sockaddr *pfsa, const LwpaSockaddr *sa)
{
  size_t ret = 0;
  if (lwpaip_is_v4(&sa->ip))
  {
    struct sockaddr_in *sin = (struct sockaddr_in *)pfsa;
    sin->sin_family = AF_INET;
    sin->sin_port = sa->port;
    sin->sin_addr.s_addr = lwpaip_v4_address(&sa->ip);
    ret = sizeof(struct sockaddr_in);
  }
  else if (lwpaip_is_v6(&sa->ip))
  {
    struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *)pfsa;
    sin6->sin6_family = AF_INET6;
    sin6->sin6_port = sa->port;
    memcpy(sin6->sin6_addr.s6_addr, lwpaip_v6_address(&sa->ip), IPV6_BYTES);
    ret = sizeof(struct sockaddr_in6);
  }
  return ret;
}

lwpa_error_t lwpa_socket_init(void *platform_data)
{
  /* No initialization is necessary on this platform. */
  (void)platform_data;
  return LWPA_OK;
}

void lwpa_socket_deinit()
{
  /* No deinitialization is necessary on this platform. */
}

int lwpa_accept(void *id, LwpaSockaddr *address)
{
  /* TODO */
  return -1;
}

int lwpa_bind(void *id, const LwpaSockaddr *address)
{
  int res = -1;
  if (address)
  {
    struct sockaddr ss;
    size_t sa_size = sockaddr_lwpa_to_plat(&ss, address);
    if (sa_size > 0)
      res = (RTCS_OK == bind((uint32_t)id, &ss, sa_size)) ? 0 : -1;
  }
  return res;
}

int lwpa_close(void *id)
{
  return (RTCS_OK == closesocket((uint32_t)id)) ? 0 : -1;
}

int lwpa_connect(void *id, const LwpaSockaddr *address)
{
  /* TODO */
  return -1;
}

int lwpa_getpeername(void *id, LwpaSockaddr *address)
{
  /* TODO */
  return -1;
}

int lwpa_getsockname(void *id, LwpaSockaddr *address)
{
  int res = -1;
  if (address)
  {
    struct sockaddr ss;
    uint16_t size = sizeof ss;
    if (RTCS_OK == getsockname((uint32_t)id, &ss, &size))
    {
      if (sockaddr_plat_to_lwpa(address, &ss))
      {
        res = 0;
      }
    }
  }
  return res;
}

int lwpa_getsockopt(void *id, int level, int option_name, void *option_value, size_t *option_len)
{
  /* TODO */
  return -1;
}

int lwpa_listen(void *id, int backlog)
{
  /* TODO */
  return -1;
}

int lwpa_recv(void *id, void *buffer, size_t length, int flags)
{
  /* TODO */
  return -1;
}

int lwpa_recvfrom(void *id, void *buffer, size_t length, int flags, LwpaSockaddr *address)
{
  int res = -1;
  uint32_t impl_flags = (flags & LWPA_MSG_PEEK) ? RTCS_MSG_PEEK : 0;
  struct sockaddr fromaddr;
  uint16_t fromlen = sizeof fromaddr;

  if (!buffer)
    return res;

  res = recvfrom((uint32_t)id, buffer, length, impl_flags, &fromaddr, &fromlen);

  if (res != RTCS_ERROR)
  {
    if (address && fromlen > 0)
    {
      if (!sockaddr_plat_to_lwpa(address, &fromaddr))
        res = -1;
    }
  }
  else
    res = -1;

  return res;
}

int lwpa_send(void *id, const void *message, size_t length, int flags)
{
  /* TODO */
  return -1;
}

int lwpa_sendto(void *id, const void *message, size_t length, int flags, const LwpaSockaddr *dest_addr)
{
  int res = -1;
  size_t ss_size;
  struct sockaddr ss;

  if (!dest_addr || !message)
    return res;

  if ((ss_size = sockaddr_lwpa_to_plat(&ss, dest_addr)) > 0)
  {
    res = sendto((uint32_t)id, (char *)message, (uint32_t)length, 0, &ss, ss_size);
    if (res == RTCS_ERROR)
      res = -1;
  }
  return res;
}

int lwpa_setsockopt(void *id, int level, int option_name, const void *option_value, size_t option_len)
{
  int res = -1;

  if (!option_value)
    return res;

  switch (option_name)
  {
    case LWPA_SO_RCVBUF:
      if (level == LWPA_SOL_SOCKET && option_len >= sizeof(uint32_t))
      {
        uint32_t sock_type;
        uint32_t size = sizeof sock_type;
        if (RTCS_OK == getsockopt((uint32_t)id, SOL_SOCKET, OPT_SOCKET_TYPE, &sock_type, &size))
        {
          uint32_t val = (uint32_t)(*(int *)option_value);
          if (sock_type == SOCK_STREAM)
          {
            res = (RTCS_OK == setsockopt((uint32_t)id, SOL_TCP, OPT_RBSIZE, &val, sizeof val)) ? 0 : -1;
          }
          else if (sock_type == SOCK_DGRAM)
          {
            res = (RTCS_OK == setsockopt((uint32_t)id, SOL_UDP, OPT_RBSIZE, &val, sizeof val)) ? 0 : -1;
          }
        }
      }
      break;
    case LWPA_SO_SNDBUF:
      if (level == LWPA_SOL_SOCKET && option_len >= sizeof(uint32_t))
      {
        uint32_t val = *(int *)option_value;
        res = (RTCS_OK == setsockopt((uint32_t)id, SOL_TCP, OPT_TBSIZE, &val, sizeof val)) ? 0 : -1;
      }
      break;
    case LWPA_SO_RCVTIMEO:
      if (level == LWPA_SOL_SOCKET && option_len >= sizeof(uint32_t))
      {
        uint32_t val = (uint32_t)(*(int *)option_value);
        res = (RTCS_OK == setsockopt((uint32_t)id, SOL_SOCKET, SO_RCVTIMEO, &val, sizeof val)) ? 0 : -1;
      }
      break;
    case LWPA_SO_SNDTIMEO:
      if (level == LWPA_SOL_SOCKET && option_len >= sizeof(uint32_t))
      {
        uint32_t val = (uint32_t)(*(int *)option_value);
        res = (RTCS_OK == setsockopt((uint32_t)id, SOL_SOCKET, SO_SNDTIMEO, &val, sizeof val)) ? 0 : -1;
      }
      break;
    case LWPA_IP_TTL:
      if (level == LWPA_IPPROTO_IP && option_len >= sizeof(unsigned char))
      {
        unsigned char val = (unsigned char)(*(int *)option_value);
        res = (RTCS_OK == setsockopt((uint32_t)id, SOL_IP, RTCS_SO_IP_TX_TTL, &val, sizeof val)) ? 0 : -1;
      }
      break;
    case LWPA_MCAST_JOIN_GROUP:
      if (option_len == sizeof(struct lwpa_mreq))
      {
        LwpaMreq *amreq = (LwpaMreq *)option_value;
        if (lwpaip_is_v4(&amreq->group) && level == LWPA_IPPROTO_IP)
        {
          /* We don't need htonl -- MQX/RTCS stores IPv4s in host order. */
          struct ip_mreq val;
          val.imr_multiaddr.s_addr = lwpaip_v4_address(&amreq->group);
          val.imr_interface.s_addr = lwpaip_v4_address(&amreq->netint);
          res = (RTCS_OK == setsockopt((uint32_t)id, SOL_IGMP, RTCS_SO_IGMP_ADD_MEMBERSHIP, &val, sizeof val)) ? 0 : -1;
          if (res == 0)
          {
            /* Voodoo to allow locally-sent multicast traffic to be received by
             * local sockets. We don't check the result. */
            val.imr_interface.s_addr = 0x7f000001;
            setsockopt((uint32_t)id, SOL_IGMP, RTCS_SO_IGMP_ADD_MEMBERSHIP, &val, sizeof val);
          }
        }
        else if (lwpaip_is_v6(&amreq->group) && level == LWPA_IPPROTO_IPV6)
        {
          struct ipv6_mreq val;
          val.ipv6imr_interface = 0; /* TODO find out what MQX does with
                                      * interface indexes */
          memcpy(&val.ipv6imr_multiaddr.s6_addr, lwpaip_v6_address(&amreq->group), IPV6_BYTES);
          res = (RTCS_OK == setsockopt((uint32_t)id, SOL_IP6, RTCS_SO_IP6_JOIN_GROUP, &val, sizeof val)) ? 0 : -1;
        }
      }
      break;
    case LWPA_MCAST_LEAVE_GROUP:
      if (option_len == sizeof(struct lwpa_mreq))
      {
        LwpaMreq *amreq = (LwpaMreq *)option_value;
        if (lwpaip_is_v4(&amreq->group) && level == LWPA_IPPROTO_IP)
        {
          /* We don't need htonl -- MQX/RTCS stores IPv4s in host order. */
          struct ip_mreq val;
          val.imr_multiaddr.s_addr = lwpaip_v4_address(&amreq->group);
          val.imr_interface.s_addr = lwpaip_v4_address(&amreq->netint);
          res =
              (RTCS_OK == setsockopt((uint32_t)id, SOL_IGMP, RTCS_SO_IGMP_DROP_MEMBERSHIP, &val, sizeof val)) ? 0 : -1;
        }
        else if (lwpaip_is_v6(&amreq->group) && level == LWPA_IPPROTO_IPV6)
        {
          struct ipv6_mreq val;
          val.ipv6imr_interface = 0; /* TODO find out what MQX does with
                                      * interface indexes */
          memcpy(&val.ipv6imr_multiaddr.s6_addr, lwpaip_v6_address(&amreq->group), IPV6_BYTES);
          res = (RTCS_OK == setsockopt((uint32_t)id, SOL_IP6, RTCS_SO_IP6_LEAVE_GROUP, &val, sizeof val)) ? 0 : -1;
        }
      }
      break;
    case LWPA_SO_REUSEADDR:
    case LWPA_SO_REUSEPORT:
      /* Option not supported, but seems to be allowed by default */
      res = 0;
      break;
    case LWPA_SO_BROADCAST:      /* Not supported */
    case LWPA_SO_ERROR:          /* Set not supported */
    case LWPA_SO_KEEPALIVE:      /* TODO */
    case LWPA_SO_LINGER:         /* TODO */
    case LWPA_SO_TYPE:           /* Not supported */
    case LWPA_IP_MULTICAST_IF:   /* Not supported */
    case LWPA_IP_MULTICAST_TTL:  /* Not supported */
    case LWPA_IP_MULTICAST_LOOP: /* TODO */
    default:
      break;
  }

  return res;
}

int lwpa_shutdown(void *id, int how)
{
  if (how >= 0 && how < LWPA_NUM_SHUT)
  {
    return (RTCS_OK == shutdownsocket((uint32_t)id, (int32_t)shutmap[how])) ? 0 : -1;
  }
  return -1;
}

void *lwpa_socket(unsigned int family, unsigned int type)
{
  if (family < LWPA_NUM_AF && type < LWPA_NUM_TYPE)
  {
    uint32_t sock = socket(sfmap[family], stmap[type], 0);
    if (sock != RTCS_SOCKET_ERROR)
      return (void *)sock;
  }
  return LWPA_SOCKET_INVALID;
}

int lwpa_poll(LwpaPollfd *fds, size_t nfds, int timeout_ms)
{
  rtcs_fd_set readfds;
  rtcs_fd_set writefds;
  LwpaPollfd *fd;
  int32_t nplatfds;
  int sel_res;
  int32_t nreadfds = 0, nwritefds = 0;
  uint32_t plat_timeout;

  if (fds && nfds > 0)
  {
    RTCS_FD_ZERO(&readfds);
    RTCS_FD_ZERO(&writefds);

    for (fd = fds; fd < fds + nfds; ++fd)
    {
      if (fd->events & LWPA_POLLIN)
      {
        RTCS_FD_SET((uint32_t)fd->fd, &readfds);
        nreadfds++;
      }
      if (fd->events & LWPA_POLLOUT)
      {
        RTCS_FD_SET((uint32_t)fd->fd, &writefds);
        nwritefds++;
      }
      /* LWPA_POLLPRI/exceptfds is not handled properly on this platform */
    }
  }

  if (timeout_ms == LWPA_WAIT_FOREVER)
    plat_timeout = 0;
  else if (timeout_ms == 0)
    plat_timeout = 0xffffffff;
  else
    plat_timeout = (uint32_t)timeout_ms;
  nplatfds = (nreadfds > nwritefds) ? nreadfds : nwritefds;
  sel_res = select(nplatfds, nreadfds ? &readfds : NULL, nwritefds ? &writefds : NULL, NULL, plat_timeout);

  if (sel_res == RTCS_ERROR)
    return -1;
  else
  {
    if (fds && nfds > 0)
    {
      for (fd = fds; fd < fds + nfds; ++fd)
      {
        fd->revents = 0;
        if (nreadfds && (fd->events & LWPA_POLLIN) && RTCS_FD_ISSET((uint32_t)fd->fd, &readfds))
        {
          fd->revents |= LWPA_POLLIN;
        }
        if (nwritefds && (fd->events & LWPA_POLLOUT) && RTCS_FD_ISSET((uint32_t)fd->fd, &writefds))
        {
          fd->revents |= LWPA_POLLOUT;
        }
        /* LWPA_POLLPRI/exceptfds is not handled properly on this
         * platform */
      }
    }
    return (int)sel_res;
  }
}

int lwpa_getaddrinfo(const char *hostname, const char *service, const LwpaAddrinfo *hints, LwpaAddrinfo *result)
{
  int res;
  struct addrinfo *pf_res;
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

  res = (0 == getaddrinfo(hostname, service, hints ? &pf_hints : NULL, &pf_res)) ? 0 : -1;
  if (res == 0)
  {
    result->pd[0] = pf_res;
    result->pd[1] = pf_res;
    if (!lwpa_nextaddr(result))
      res = -1;
  }
  return res;
}

bool lwpa_nextaddr(LwpaAddrinfo *ai)
{
  if (ai && ai->pd[1])
  {
    struct addrinfo *pf_ai = (struct addrinfo *)ai->pd[1];
    ai->ai_flags = 0;
    if (!sockaddr_plat_to_lwpa(&ai->ai_addr, pf_ai->ai_addr))
      return false;
    /* Can't use reverse maps, because we have no guarantee of the numeric
     * values of the platform constants. Ugh. */
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

lwpa_error_t lwpa_inet_ntop(const LwpaIpAddr *src, char *dest, size_t size)
{
  if (!src || !dest)
    return LWPA_INVALID;

  switch (src->type)
  {
    case LWPA_IPV4:
    {
      struct in_addr addr;
      /* RTCS expects host byte order in their in_addrs. Thus no htonl is
       * needed. */
      addr.s_addr = lwpaip_v4_address(src);
      if (NULL != inet_ntop(AF_INET, &addr, dest, size))
        return LWPA_OK;
      return LWPA_SYSERR;
    }
    case LWPA_IPV6:
    {
      struct in6_addr addr;
      memcpy(addr.s6_addr, lwpaip_v6_address(src), IPV6_BYTES);
      if (NULL != inet_ntop(AF_INET6, &addr, dest, size))
        return LWPA_OK;
      return LWPA_SYSERR;
    }
    default:
      return LWPA_INVALID;
  }
}

lwpa_error_t lwpa_inet_pton(lwpa_iptype_t type, const char *src, LwpaIpAddr *dest)
{
  if (!src || !dest)
    return LWPA_INVALID;

  switch (type)
  {
    case LWPA_IPV4:
    {
      struct in_addr addr;
      if (RTCS_OK != inet_pton(AF_INET, src, &addr, sizeof addr))
        return LWPA_SYSERR;
      /* RTCS gives us host byte order in their in_addrs. Thus no htonl is
       * needed. */
      lwpaip_set_v4_address(dest, addr.s_addr);
      return LWPA_OK;
    }
    case LWPA_IPV6:
    {
      struct in6_addr addr;
      if (RTCS_OK != inet_pton(AF_INET6, src, &addr, sizeof addr))
        return LWPA_SYSERR;
      lwpaip_set_v6_address(dest, addr.s6_addr);
      return LWPA_OK;
    }
    default:
      return LWPA_INVALID;
  }
}
