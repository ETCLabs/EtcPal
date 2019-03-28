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
#include "lwip/sockets.h"
#include "lwip/netdb.h"

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

static bool sockaddr_plat_to_lwpa(LwpaSockaddr *sa, const struct sockaddr *pfsa)
{
  if (pfsa->sa_family == AF_INET)
  {
#if LWIP_IPV4
    struct sockaddr_in *sin = (struct sockaddr_in *)pfsa;
    sa->port = ntohs(sin->sin_port);
    lwpaip_set_v4_address(&sa->ip, ntohl(sin->sin_addr.s_addr));
    return true;
#else
    return false;
#endif
  }
  else if (pfsa->sa_family == AF_INET6)
  {
#if LWIP_IPV6
    struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *)pfsa;
    sa->port = ntohs(sin6->sin6_port);
    lwpaip_set_v6_address(&sa->ip, sin6->sin6_addr.s6_addr);
    return true;
#else
    return false;
#endif
  }
  return false;
}

static socklen_t sockaddr_lwpa_to_plat(struct sockaddr *pfsa, const LwpaSockaddr *sa)
{
  socklen_t ret = 0;
  if (lwpaip_is_v4(&sa->ip))
  {
#if LWIP_IPV4
    struct sockaddr_in *sin = (struct sockaddr_in *)pfsa;
    sin->sin_family = AF_INET;
    sin->sin_port = htons(sa->port);
    sin->sin_addr.s_addr = htonl(lwpaip_v4_address(&sa->ip));
    ret = sizeof(struct sockaddr_in);
#endif
  }
  else if (lwpaip_is_v6(&sa->ip))
  {
#if LWIP_IPV6
    struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *)pfsa;
    sin6->sin6_family = AF_INET6;
    sin6->sin6_port = htons(sa->port);
    memcpy(sin6->sin6_addr.s6_addr, lwpaip_v6_address(&sa->ip), IPV6_BYTES);
    ret = sizeof(struct sockaddr_in6);
#endif
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
    struct sockaddr_storage ss;
    socklen_t sa_size = sockaddr_lwpa_to_plat((struct sockaddr *)&ss, address);
    if (sa_size > 0)
      res = bind((int)id, (struct sockaddr *)&ss, sa_size);
  }
  return res;
}

int lwpa_close(void *id)
{
  return close((int)id);
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
    struct sockaddr_storage ss;
    socklen_t size = sizeof ss;
    if (0 == getsockname((int)id, (struct sockaddr *)&ss, &size))
    {
      if (sockaddr_plat_to_lwpa(address, (struct sockaddr *)&ss))
        res = 0;
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
  int impl_flags = (flags & LWPA_MSG_PEEK) ? MSG_PEEK : 0;
  struct sockaddr_storage fromaddr;
  socklen_t fromlen = sizeof fromaddr;

  if (!buffer)
    return res;

  res = recvfrom((int)id, buffer, length, impl_flags, (struct sockaddr *)&fromaddr, &fromlen);

  if (res >= 0)
  {
    if (address && fromlen > 0)
    {
      if (!sockaddr_plat_to_lwpa(address, (struct sockaddr *)&fromaddr))
        res = -1;
    }
  }

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
  socklen_t ss_size;
  struct sockaddr_storage ss;

  if (!dest_addr || !message)
    return res;

  if ((ss_size = sockaddr_lwpa_to_plat((struct sockaddr *)&ss, dest_addr)) > 0)
    res = sendto((int)id, message, length, 0, (struct sockaddr *)&ss, ss_size);
  return res;
}

int lwpa_setsockopt(void *id, int level, int option_name, const void *option_value, size_t option_len)
{
  int res = -1;

  if (!option_value)
    return res;

  /* TODO this platform implementation could be simplified by use of socket
   * option lookup arrays. */
  switch (level)
  {
    case LWPA_SOL_SOCKET:
      switch (option_name)
      {
        case LWPA_SO_RCVBUF:
          res = setsockopt((int)id, SOL_SOCKET, SO_RCVBUF, option_value, option_len);
          break;
        case LWPA_SO_SNDBUF:
          res = setsockopt((int)id, SOL_SOCKET, SO_SNDBUF, option_value, option_len);
          break;
        case LWPA_SO_RCVTIMEO:
          if (option_len == sizeof(int))
          {
            int ms = *(int *)option_value;
            struct timeval val = {ms / 1000, (ms % 1000) * 1000};
            res = setsockopt((int)id, SOL_SOCKET, SO_RCVTIMEO, &val, sizeof val);
          }
          break;
        case LWPA_SO_SNDTIMEO:
          if (option_len == sizeof(int))
          {
            int ms = *(int *)option_value;
            struct timeval val = {ms / 1000, (ms % 1000) * 1000};
            res = setsockopt((int)id, SOL_SOCKET, SO_SNDTIMEO, &val, sizeof val);
          }
          break;
        case LWPA_SO_REUSEADDR:
          res = setsockopt((int)id, SOL_SOCKET, SO_REUSEADDR, option_value, option_len);
          break;
        case LWPA_SO_REUSEPORT:
          res = setsockopt((int)id, SOL_SOCKET, SO_REUSEPORT, option_value, option_len);
          break;
        case LWPA_SO_BROADCAST:
          res = setsockopt((int)id, SOL_SOCKET, SO_BROADCAST, option_value, option_len);
          break;
        case LWPA_SO_KEEPALIVE:
          res = setsockopt((int)id, SOL_SOCKET, SO_KEEPALIVE, option_value, option_len);
          break;
        case LWPA_SO_ERROR:  /* Set not supported */
        case LWPA_SO_LINGER: /* TODO */
        case LWPA_SO_TYPE:   /* Not supported */
        default:
          break;
      }
      break;
    case LWPA_IPPROTO_IP:
      switch (option_name)
      {
        case LWPA_IP_TTL:
          res = setsockopt((int)id, IPPROTO_IP, IP_TTL, option_value, option_len);
          break;
        case LWPA_MCAST_JOIN_GROUP:
          if (option_len == sizeof(struct lwpa_mreq))
          {
            LwpaMreq *amreq = (LwpaMreq *)option_value;
            if (lwpaip_is_v4(&amreq->group))
            {
              struct ip_mreq val;
              val.imr_multiaddr.s_addr = htonl(lwpaip_v4_address(&amreq->group));
              val.imr_interface.s_addr = htonl(lwpaip_v4_address(&amreq->netint));
              res = setsockopt((int)id, IPPROTO_IP, IP_ADD_MEMBERSHIP, &val, sizeof val);
            }
          }
          break;
        case LWPA_MCAST_LEAVE_GROUP:
          if (option_len == sizeof(struct lwpa_mreq))
          {
            LwpaMreq *amreq = (LwpaMreq *)option_value;
            if (lwpaip_is_v4(&amreq->group))
            {
              struct ip_mreq val;
              val.imr_multiaddr.s_addr = htonl(lwpaip_v4_address(&amreq->group));
              val.imr_interface.s_addr = htonl(lwpaip_v4_address(&amreq->netint));
              res = setsockopt((int)id, IPPROTO_IP, IP_DROP_MEMBERSHIP, &val, sizeof val);
            }
          }
          break;
        case LWPA_IP_MULTICAST_IF:
          if (option_len == sizeof(struct lwpa_ip))
          {
            LwpaIpAddr *netint = (LwpaIpAddr *)option_value;
            if (lwpaip_is_v4(netint))
            {
              struct in_addr val;
              val.s_addr = htonl(lwpaip_v4_address(netint));
              res = setsockopt((int)id, IPPROTO_IP, IP_MULTICAST_IF, &val, sizeof val);
            }
          }
          break;
        case LWPA_IP_MULTICAST_TTL:
        {
          unsigned char val = (unsigned char)*(int *)option_value;
          res = setsockopt((int)id, IPPROTO_IP, IP_MULTICAST_TTL, &val, sizeof val);
        }
        break;
        case LWPA_IP_MULTICAST_LOOP:
        {
          unsigned char val = (unsigned char)*(int *)option_value;
          res = setsockopt((int)id, IPPROTO_IP, IP_MULTICAST_LOOP, &val, sizeof val);
        }
        break;
        default:
          break;
      }
      break;
    case LWPA_IPPROTO_IPV6:
      switch (option_name)
      {
        case LWPA_MCAST_JOIN_GROUP:
          /* This is not supported in lwip yet.
          if (option_len == sizeof(struct lwpa_mreq))
          {
            LwpaMreq *amreq = (LwpaMreq *)option_value;
            if (lwpaip_is_v6(&amreq->group))
            {
              struct ipv6_mreq val;
              val.ipv6imr_interface = 0;
              memcpy(&val.ipv6imr_multiaddr.s6_addr,
                     lwpaip_v6_address(&amreq->group), IPV6_BYTES);
              res = setsockopt((int)id, IPPROTO_IPV6, MCAST_JOIN_GROUP, &val,
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
            if (lwpaip_is_v6(&amreq->group))
            {
              struct ipv6_mreq val;
              val.ipv6imr_interface = 0;
              memcpy(&val.ipv6imr_multiaddr.s6_addr,
                     lwpaip_v6_address(&amreq->group), IPV6_BYTES);
              res = setsockopt((int)id, IPPROTO_IPV6, MCAST_JOIN_GROUP, &val,
                               sizeof val);
            }
          }
          */
          break;
        default: /* lwip does not have a lot of IPV6 socket option support
                  * yet. */
          break;
      }
      break;
    default:
      break;
  }
  return res;
}

int lwpa_shutdown(void *id, int how)
{
  if (how >= 0 && how < LWPA_NUM_SHUT)
    return shutdown((int)id, shutmap[how]);
  return -1;
}

lwpa_error_t lwpa_socket(unsigned int family, unsigned int type, lwpa_socket_t *id)
{
  if (id)
  {
    if (family < LWPA_NUM_AF && type < LWPA_NUM_TYPE)
    {
      int sock = socket(sfmap[family], stmap[type], 0);
      if (sock >= 0)
      {
        *id = sock;
        return LWPA_OK;
      }
      else
      {
        *id = LWPA_SOCKET_INVALID;
        return err_plat_to_lwpa(errno);
      }
    }
    else
    {
      *id = LWPA_SOCKET_INVALID;
    }
  }
  return LWPA_INVALID;
}

int lwpa_poll(LwpaPollfd *fds, size_t nfds, int timeout_ms)
{
  fd_set readfds;
  fd_set writefds;
  LwpaPollfd *fd;
  int nplatfds;
  int sel_res;
  int nreadfds = 0, nwritefds = 0;
  struct timeval plat_timeout;

  if (fds && nfds > 0)
  {
    FD_ZERO(&readfds);
    FD_ZERO(&writefds);

    for (fd = fds; fd < fds + nfds; ++fd)
    {
      if (fd->events & LWPA_POLLIN)
      {
        FD_SET((int)fd->fd, &readfds);
        nreadfds++;
      }
      if (fd->events & LWPA_POLLOUT)
      {
        FD_SET((int)fd->fd, &writefds);
        nwritefds++;
      }
      /* LWPA_POLLPRI/exceptfds is not handled properly on this platform */
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
  sel_res = select(nplatfds, nreadfds ? &readfds : NULL, nwritefds ? &writefds : NULL, NULL,
                   timeout_ms == LWPA_WAIT_FOREVER ? NULL : &plat_timeout);

  if (sel_res < 0)
    return -1;
  else if (sel_res > 0 && fds && nfds > 0)
  {
    for (fd = fds; fd < fds + nfds; ++fd)
    {
      fd->revents = 0;
      if (nreadfds && (fd->events & LWPA_POLLIN) && FD_ISSET((int)fd->fd, &readfds))
      {
        fd->revents |= LWPA_POLLIN;
      }
      if (nwritefds && (fd->events & LWPA_POLLOUT) && FD_ISSET((int)fd->fd, &writefds))
      {
        fd->revents |= LWPA_POLLOUT;
      }
      /* LWPA_POLLPRI/exceptfds is not handled properly on this
       * platform */
    }
  }
  return sel_res;
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
      addr.s_addr = htonl(lwpaip_v4_address(src));
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
      if (1 != inet_pton(AF_INET, src, &addr))
        return LWPA_SYSERR;
      lwpaip_set_v4_address(dest, ntohl(addr.s_addr));
      return LWPA_OK;
    }
    case LWPA_IPV6:
    {
      struct in6_addr addr;
      if (1 != inet_pton(AF_INET6, src, &addr))
        return LWPA_SYSERR;
      lwpaip_set_v6_address(dest, addr.s6_addr);
      return LWPA_OK;
    }
    default:
      return LWPA_INVALID;
  }
}
