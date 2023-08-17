/******************************************************************************
 * Copyright 2022 ETC Inc.
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

// These are defined before the includes to enable IPV6_PKTINFO support.
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#ifndef __APPLE_USE_RFC_3542
#define __APPLE_USE_RFC_3542
#endif

#include "etcpal/socket.h"
#include "etcpal/private/common.h"
#include "etcpal/private/socket.h"

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

#include "etcpal/common.h"
#include "os_error.h"

/**************************** Private constants ******************************/

/* The maximum number of kevents that can be added in one call to an etcpal_poll API function. */
#define ETCPAL_SOCKET_MAX_KEVENTS 3

/****************************** Private types ********************************/

/* A struct to track sockets being polled by the etcpal_poll() API */
typedef struct EtcPalPollSocket
{
  // 'sock' must always remain as the first member in the struct to facilitate an EtcPalRbTree lookup
  // shortcut
  etcpal_socket_t      sock;
  etcpal_poll_events_t events;
} EtcPalPollSocket;

/**************************** Private variables ******************************/

#if !defined(ETCPAL_BUILDING_MOCK_LIB)

/* clang-format off */

#define ETCPAL_NUM_SHUT 3
static const int shutmap[ETCPAL_NUM_SHUT] =
{
  SHUT_RD,
  SHUT_WR,
  SHUT_RDWR
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

// Helpers for etcpal_setsockopt()
static void ms_to_timeval(int ms, struct timeval* tv);
static int  setsockopt_socket(etcpal_socket_t id, int option_name, const void* option_value, size_t option_len);
static int  setsockopt_ip(etcpal_socket_t id, int option_name, const void* option_value, size_t option_len);
static int  setsockopt_ip6(etcpal_socket_t id, int option_name, const void* option_value, size_t option_len);

// Helpers for etcpal_getsockopt()
static int getsockopt_socket(etcpal_socket_t id, int option_name, void* option_value, size_t* option_len);

// Helpers for etcpal_poll API
static int                  events_etcpal_to_kqueue(etcpal_socket_t      socket,
                                                    etcpal_poll_events_t prev_events,
                                                    etcpal_poll_events_t new_events,
                                                    void*                user_data,
                                                    struct kevent*       events);
static etcpal_poll_events_t events_kqueue_to_etcpal(const struct kevent* kevent, const EtcPalPollSocket* sock_desc);

static int           poll_socket_compare(const EtcPalRbTree* tree, const void* value_a, const void* value_b);
static EtcPalRbNode* poll_socket_alloc(void);
static void          poll_socket_free(EtcPalRbNode* node);

// Helpers for etcpal_recvmsg()
static void construct_msghdr(const EtcPalMsgHdr*      in_msg,
                             struct sockaddr_storage* name_store,
                             struct iovec*            buf_store,
                             struct msghdr*           out_msg);
static int  rcvmsg_flags_os_to_etcpal(int os_flags);
static int  rcvmsg_flags_etcpal_to_os(int etcpal_flags);
static bool get_first_compatible_cmsg(struct msghdr* msg, struct cmsghdr* start, EtcPalCMsgHdr* cmsg);

/*************************** Function definitions ****************************/

etcpal_error_t etcpal_socket_init(void)
{
  // No initialization necessary on this platform
  return kEtcPalErrOk;
}

void etcpal_socket_deinit(void)
{
  // No deinitialization necessary on this platform
}

etcpal_error_t etcpal_accept(etcpal_socket_t id, EtcPalSockAddr* address, etcpal_socket_t* conn_sock)
{
  if (!conn_sock)
    return kEtcPalErrInvalid;

  struct sockaddr_storage ss;
  socklen_t               sa_size = sizeof ss;
  int                     res     = accept(id, (struct sockaddr*)&ss, &sa_size);

  if (res != -1)
  {
    if (address && !sockaddr_os_to_etcpal((etcpal_os_sockaddr_t*)&ss, address))
    {
      close(res);
      return kEtcPalErrSys;
    }
    *conn_sock = res;
    return kEtcPalErrOk;
  }
  return errno_os_to_etcpal(errno);
}

etcpal_error_t etcpal_bind(etcpal_socket_t id, const EtcPalSockAddr* address)
{
  if (!address)
    return kEtcPalErrInvalid;

  struct sockaddr_storage ss;
  socklen_t               sa_size = (socklen_t)sockaddr_etcpal_to_os(address, (etcpal_os_sockaddr_t*)&ss);
  if (sa_size == 0)
    return kEtcPalErrInvalid;

  int res = bind(id, (struct sockaddr*)&ss, sa_size);
  return (res == 0 ? kEtcPalErrOk : errno_os_to_etcpal(errno));
}

etcpal_error_t etcpal_close(etcpal_socket_t id)
{
  int res = close(id);
  return (res == 0 ? kEtcPalErrOk : errno_os_to_etcpal(errno));
}

etcpal_error_t etcpal_connect(etcpal_socket_t id, const EtcPalSockAddr* address)
{
  if (!address)
    return kEtcPalErrInvalid;

  struct sockaddr_storage ss;
  socklen_t               sa_size = (socklen_t)sockaddr_etcpal_to_os(address, (etcpal_os_sockaddr_t*)&ss);
  if (sa_size == 0)
    return kEtcPalErrInvalid;

  int res = connect(id, (struct sockaddr*)&ss, sa_size);
  return (res == 0 ? kEtcPalErrOk : errno_os_to_etcpal(errno));
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
  if (!address)
    return kEtcPalErrInvalid;

  struct sockaddr_storage ss;
  socklen_t               size = sizeof ss;
  int                     res  = getsockname(id, (struct sockaddr*)&ss, &size);
  if (res == 0)
  {
    if (!sockaddr_os_to_etcpal((etcpal_os_sockaddr_t*)&ss, address))
      return kEtcPalErrSys;
    return kEtcPalErrOk;
  }
  return errno_os_to_etcpal(errno);
}

etcpal_error_t etcpal_getsockopt(etcpal_socket_t id, int level, int option_name, void* option_value, size_t* option_len)
{
  int res = -1;

  if (!option_value || !option_len)
    return kEtcPalErrInvalid;

  // TODO this OS implementation could be simplified by use of socket option lookup arrays.
  switch (level)
  {
    case ETCPAL_SOL_SOCKET:
      res = getsockopt_socket(id, option_name, option_value, option_len);
      break;
    case ETCPAL_IPPROTO_IP:
      return kEtcPalErrNotImpl;  // TODO
    case ETCPAL_IPPROTO_IPV6:
      return kEtcPalErrNotImpl;  // TODO
    default:
      return kEtcPalErrInvalid;
  }
  return (res == 0 ? kEtcPalErrOk : errno_os_to_etcpal(errno));
}

int getsockopt_socket(etcpal_socket_t id, int option_name, void* option_value, size_t* option_len)
{
  if (!ETCPAL_ASSERT_VERIFY(option_value) || !ETCPAL_ASSERT_VERIFY(option_len))
    return -1;

  switch (option_name)
  {
    case ETCPAL_SO_SNDBUF:
      return getsockopt(id, SOL_SOCKET, SO_SNDBUF, option_value, (socklen_t*)option_len);
    case ETCPAL_SO_RCVBUF:     // TODO
    case ETCPAL_SO_RCVTIMEO:   // TODO
    case ETCPAL_SO_SNDTIMEO:   // TODO
    case ETCPAL_SO_REUSEADDR:  // TODO
    case ETCPAL_SO_REUSEPORT:  // TODO
    case ETCPAL_SO_BROADCAST:  // TODO
    case ETCPAL_SO_KEEPALIVE:  // TODO
    case ETCPAL_SO_LINGER:     // TODO
    case ETCPAL_SO_ERROR:      // Set not supported
    case ETCPAL_SO_TYPE:       // Set not supported
    default:
      break;
  }
  // If we got here, something was invalid. Set errno accordingly
  errno = EINVAL;
  return -1;
}

etcpal_error_t etcpal_listen(etcpal_socket_t id, int backlog)
{
  int res = listen(id, backlog);
  return (res == 0 ? kEtcPalErrOk : errno_os_to_etcpal(errno));
}

int etcpal_recv(etcpal_socket_t id, void* buffer, size_t length, int flags)
{
  if (!buffer)
    return kEtcPalErrInvalid;

  int impl_flags = (flags & ETCPAL_MSG_PEEK) ? MSG_PEEK : 0;
  int res        = (int)recv(id, buffer, length, impl_flags);
  return (res >= 0 ? res : (int)errno_os_to_etcpal(errno));
}

int etcpal_recvfrom(etcpal_socket_t id, void* buffer, size_t length, int flags, EtcPalSockAddr* address)
{
  if (!buffer)
    return (int)kEtcPalErrInvalid;

  struct sockaddr_storage fromaddr;
  socklen_t               fromlen    = sizeof fromaddr;
  int                     impl_flags = (flags & ETCPAL_MSG_PEEK) ? MSG_PEEK : 0;
  int                     res = (int)recvfrom(id, buffer, length, impl_flags, (struct sockaddr*)&fromaddr, &fromlen);

  if (res >= 0)
  {
    if (address && fromlen > 0)
    {
      if (!sockaddr_os_to_etcpal((etcpal_os_sockaddr_t*)&fromaddr, address))
        return kEtcPalErrSys;
    }
    return res;
  }
  return (int)errno_os_to_etcpal(errno);
}

int etcpal_recvmsg(etcpal_socket_t id, EtcPalMsgHdr* msg, int flags)
{
  if (!msg)
    return (int)kEtcPalErrInvalid;

  struct msghdr           impl_msg  = {0};
  struct sockaddr_storage impl_name = {0};
  struct iovec            impl_buf  = {0};
  construct_msghdr(msg, &impl_name, &impl_buf, &impl_msg);

  int res = (int)recvmsg(id, &impl_msg, rcvmsg_flags_etcpal_to_os(flags));

  msg->flags = rcvmsg_flags_os_to_etcpal(impl_msg.msg_flags);

  if (res < 0)
    return (int)errno_os_to_etcpal(errno);

  if (!sockaddr_os_to_etcpal((etcpal_os_sockaddr_t*)&impl_name, &msg->name))
    return kEtcPalErrSys;

  return res;
}

bool etcpal_cmsg_firsthdr(EtcPalMsgHdr* msgh, EtcPalCMsgHdr* firsthdr)
{
  bool result = false;

  if (msgh && firsthdr)
  {
    struct msghdr           impl_msg  = {0};
    struct sockaddr_storage impl_name = {0};
    struct iovec            impl_buf  = {0};
    construct_msghdr(msgh, &impl_name, &impl_buf, &impl_msg);

    result = get_first_compatible_cmsg(&impl_msg, CMSG_FIRSTHDR(&impl_msg), firsthdr);
  }

  return result;
}

bool etcpal_cmsg_nxthdr(EtcPalMsgHdr* msgh, const EtcPalCMsgHdr* cmsg, EtcPalCMsgHdr* nxthdr)
{
  bool result = false;

  if (msgh && cmsg && nxthdr)
  {
    struct msghdr           impl_msg  = {0};
    struct sockaddr_storage impl_name = {0};
    struct iovec            impl_buf  = {0};
    construct_msghdr(msgh, &impl_name, &impl_buf, &impl_msg);

    result = get_first_compatible_cmsg(&impl_msg, CMSG_NXTHDR(&impl_msg, (struct cmsghdr*)cmsg->pd), nxthdr);
  }

  return result;
}

bool etcpal_cmsg_to_pktinfo(const EtcPalCMsgHdr* cmsg, EtcPalPktInfo* pktinfo)
{
  bool result = false;

  if (cmsg && pktinfo)
  {
    struct cmsghdr* impl_cmsg = (struct cmsghdr*)cmsg->pd;

    // Per the cmsg(3) manpage, impl_data "cannot be assumed to be suitably aligned for accessing arbitrary payload data
    // types. Applications should not cast it to a pointer type matching the payload, but should instead use memcpy(3)
    // to copy data to or from a suitably declared object."
    void* impl_data = CMSG_DATA(impl_cmsg);

    if (impl_data)
    {
      if ((cmsg->level == ETCPAL_IPPROTO_IP) && (cmsg->type == ETCPAL_IP_PKTINFO))
      {
        struct in_pktinfo impl_pktinfo;
        memcpy(&impl_pktinfo, impl_data, sizeof(impl_pktinfo));

        struct sockaddr_in impl_addr;
        impl_addr.sin_family = AF_INET;
        impl_addr.sin_addr   = impl_pktinfo.ipi_addr;

        ip_os_to_etcpal((struct sockaddr*)&impl_addr, &pktinfo->addr);
        pktinfo->ifindex = impl_pktinfo.ipi_ifindex;

        result = true;
      }
      else if ((cmsg->level == ETCPAL_IPPROTO_IPV6) && (cmsg->type == ETCPAL_IPV6_PKTINFO))
      {
        struct in6_pktinfo impl_pktinfo;
        memcpy(&impl_pktinfo, impl_data, sizeof(impl_pktinfo));

        struct sockaddr_in6 impl_addr;
        impl_addr.sin6_family = AF_INET6;
        impl_addr.sin6_addr   = impl_pktinfo.ipi6_addr;

        ip_os_to_etcpal((struct sockaddr*)&impl_addr, &pktinfo->addr);
        pktinfo->ifindex = impl_pktinfo.ipi6_ifindex;

        result = true;
      }
    }
  }

  return result;
}

int etcpal_send(etcpal_socket_t id, const void* message, size_t length, int flags)
{
  ETCPAL_UNUSED_ARG(flags);

  if (!message)
    return (int)kEtcPalErrInvalid;

  int res = (int)send(id, message, length, 0);
  return (res >= 0 ? res : (int)errno_os_to_etcpal(errno));
}

int etcpal_sendto(etcpal_socket_t id, const void* message, size_t length, int flags, const EtcPalSockAddr* dest_addr)
{
  ETCPAL_UNUSED_ARG(flags);

  if (!dest_addr || !message)
    return (int)kEtcPalErrInvalid;

  struct sockaddr_storage ss;
  socklen_t               ss_size = (socklen_t)sockaddr_etcpal_to_os(dest_addr, (etcpal_os_sockaddr_t*)&ss);
  if (ss_size == 0)
    return (int)kEtcPalErrSys;

  int res = (int)sendto(id, message, length, 0, (struct sockaddr*)&ss, ss_size);

  return (res >= 0 ? res : (int)errno_os_to_etcpal(errno));
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
  return (res == 0 ? kEtcPalErrOk : errno_os_to_etcpal(errno));
}

int setsockopt_socket(etcpal_socket_t id, int option_name, const void* option_value, size_t option_len)
{
  if (!ETCPAL_ASSERT_VERIFY(option_value))
    return -1;

  switch (option_name)
  {
    case ETCPAL_SO_RCVBUF:
      return setsockopt(id, SOL_SOCKET, SO_RCVBUF, option_value, (socklen_t)option_len);
    case ETCPAL_SO_SNDBUF:
      return setsockopt(id, SOL_SOCKET, SO_SNDBUF, option_value, (socklen_t)option_len);
    case ETCPAL_SO_RCVTIMEO:
      if (option_len == sizeof(int))
      {
        int            val = *(int*)option_value;
        struct timeval sys_val;
        ms_to_timeval(val, &sys_val);
        return setsockopt(id, SOL_SOCKET, SO_RCVTIMEO, &sys_val, sizeof sys_val);
      }
      break;
    case ETCPAL_SO_SNDTIMEO:
      if (option_len == sizeof(int))
      {
        int            val = *(int*)option_value;
        struct timeval sys_val;
        ms_to_timeval(val, &sys_val);
        return setsockopt(id, SOL_SOCKET, SO_SNDTIMEO, &sys_val, sizeof sys_val);
      }
      break;
    case ETCPAL_SO_REUSEADDR:
      return setsockopt(id, SOL_SOCKET, SO_REUSEADDR, option_value, (socklen_t)option_len);
    case ETCPAL_SO_REUSEPORT:
      return setsockopt(id, SOL_SOCKET, SO_REUSEPORT, option_value, (socklen_t)option_len);
    case ETCPAL_SO_BROADCAST:
      return setsockopt(id, SOL_SOCKET, SO_BROADCAST, option_value, (socklen_t)option_len);
    case ETCPAL_SO_KEEPALIVE:
      return setsockopt(id, SOL_SOCKET, SO_KEEPALIVE, option_value, (socklen_t)option_len);
    case ETCPAL_SO_LINGER:
      if (option_len == sizeof(EtcPalLinger))
      {
        EtcPalLinger* ll = (EtcPalLinger*)option_value;
        struct linger val;
        val.l_onoff  = (u_short)ll->onoff;
        val.l_linger = (u_short)ll->linger;
        return setsockopt(id, SOL_SOCKET, SO_LINGER, &val, sizeof val);
      }
      break;
    case ETCPAL_SO_ERROR:  // Set not supported
    case ETCPAL_SO_TYPE:   // Set not supported
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
  if (!ETCPAL_ASSERT_VERIFY(addr))
    return -1;

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

int setsockopt_ip(etcpal_socket_t id, int option_name, const void* option_value, size_t option_len)
{
  if (!ETCPAL_ASSERT_VERIFY(option_value))
    return -1;

  switch (option_name)
  {
    case ETCPAL_IP_TTL:
      return setsockopt(id, IPPROTO_IP, IP_TTL, option_value, (socklen_t)option_len);
    case ETCPAL_IP_ADD_MEMBERSHIP:
      if (option_len == sizeof(EtcPalMreq))
      {
        EtcPalMreq* amreq = (EtcPalMreq*)option_value;
        if (ETCPAL_IP_IS_V4(&amreq->group))
        {
          struct ip_mreq val;
          val.imr_multiaddr.s_addr = htonl(ETCPAL_IP_V4_ADDRESS(&amreq->group));
          val.imr_interface.s_addr = htonl(ETCPAL_IP_V4_ADDRESS(&amreq->netint));
          return setsockopt(id, IPPROTO_IP, IP_ADD_MEMBERSHIP, &val, sizeof val);
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
          return setsockopt(id, IPPROTO_IP, IP_DROP_MEMBERSHIP, &val, sizeof val);
        }
      }
      break;
    case ETCPAL_MCAST_JOIN_GROUP:
      if (option_len == sizeof(EtcPalGroupReq))
      {
        EtcPalGroupReq* greq = (EtcPalGroupReq*)option_value;
        if (ETCPAL_IP_IS_V4(&greq->group) && greq->ifindex >= 0)
        {
          struct ip_mreq val;
          val.imr_multiaddr.s_addr = htonl(ETCPAL_IP_V4_ADDRESS(&greq->group));
          if (0 != ip4_ifindex_to_addr(greq->ifindex, &val.imr_interface))
            return -1;
          return setsockopt(id, IPPROTO_IP, IP_ADD_MEMBERSHIP, &val, sizeof val);
        }
      }
      break;
    case ETCPAL_MCAST_LEAVE_GROUP:
      if (option_len == sizeof(EtcPalGroupReq))
      {
        EtcPalGroupReq* greq = (EtcPalGroupReq*)option_value;
        if (ETCPAL_IP_IS_V4(&greq->group) && greq->ifindex >= 0)
        {
          struct ip_mreq val;
          val.imr_multiaddr.s_addr = htonl(ETCPAL_IP_V4_ADDRESS(&greq->group));
          if (0 != ip4_ifindex_to_addr(greq->ifindex, &val.imr_interface))
            return -1;
          return setsockopt(id, IPPROTO_IP, IP_DROP_MEMBERSHIP, &val, sizeof val);
        }
      }
      break;
    case ETCPAL_IP_MULTICAST_IF:
      if (option_len == sizeof(unsigned int))
      {
        struct in_addr val;
        if (0 != ip4_ifindex_to_addr(*(unsigned int*)option_value, &val))
          return -1;
        return setsockopt(id, IPPROTO_IP, IP_MULTICAST_IF, &val, sizeof val);
      }
      break;
    case ETCPAL_IP_MULTICAST_TTL:
      return setsockopt(id, IPPROTO_IP, IP_MULTICAST_TTL, option_value, (socklen_t)option_len);
    case ETCPAL_IP_MULTICAST_LOOP:
      return setsockopt(id, IPPROTO_IP, IP_MULTICAST_LOOP, option_value, (socklen_t)option_len);
    case ETCPAL_IP_PKTINFO:
      return setsockopt(id, IPPROTO_IP, IP_PKTINFO, option_value, (socklen_t)option_len);
    default:
      break;
  }
  // If we got here, something was invalid. Set errno accordingly
  errno = EINVAL;
  return -1;
}

int setsockopt_ip6(etcpal_socket_t id, int option_name, const void* option_value, size_t option_len)
{
  if (!ETCPAL_ASSERT_VERIFY(option_value))
    return -1;

  switch (option_name)
  {
    case ETCPAL_MCAST_JOIN_GROUP:
      if (option_len == sizeof(EtcPalGroupReq))
      {
        EtcPalGroupReq* greq = (EtcPalGroupReq*)option_value;
        if (ETCPAL_IP_IS_V6(&greq->group) && greq->ifindex >= 0)
        {
          struct ipv6_mreq val;
          val.ipv6mr_interface = (uint32_t)greq->ifindex;
          memcpy(val.ipv6mr_multiaddr.s6_addr, ETCPAL_IP_V6_ADDRESS(&greq->group), ETCPAL_IPV6_BYTES);
          return setsockopt(id, IPPROTO_IPV6, IPV6_JOIN_GROUP, &val, sizeof val);
        }
      }
      break;
    case ETCPAL_MCAST_LEAVE_GROUP:
      if (option_len == sizeof(EtcPalGroupReq))
      {
        EtcPalGroupReq* greq = (EtcPalGroupReq*)option_value;
        if (ETCPAL_IP_IS_V6(&greq->group) && greq->ifindex >= 0)
        {
          struct ipv6_mreq val;
          val.ipv6mr_interface = (uint32_t)greq->ifindex;
          memcpy(val.ipv6mr_multiaddr.s6_addr, ETCPAL_IP_V6_ADDRESS(&greq->group), ETCPAL_IPV6_BYTES);
          return setsockopt(id, IPPROTO_IPV6, IPV6_LEAVE_GROUP, &val, sizeof val);
        }
      }
      break;
    case ETCPAL_IP_MULTICAST_IF:
      return setsockopt(id, IPPROTO_IPV6, IPV6_MULTICAST_IF, option_value, (socklen_t)option_len);
    case ETCPAL_IP_MULTICAST_TTL:
      return setsockopt(id, IPPROTO_IPV6, IPV6_MULTICAST_HOPS, option_value, (socklen_t)option_len);
    case ETCPAL_IP_MULTICAST_LOOP:
      return setsockopt(id, IPPROTO_IPV6, IPV6_MULTICAST_LOOP, option_value, (socklen_t)option_len);
    case ETCPAL_IPV6_V6ONLY:
      return setsockopt(id, IPPROTO_IPV6, IPV6_V6ONLY, option_value, (socklen_t)option_len);
    case ETCPAL_IPV6_PKTINFO:
      return setsockopt(id, IPPROTO_IPV6, IPV6_RECVPKTINFO, option_value, (socklen_t)option_len);
    default: /* Other IPv6 options TODO on macOS. */
      break;
  }
  // If we got here, something was invalid. Set errno accordingly
  errno = EINVAL;
  return -1;
}

void ms_to_timeval(int ms, struct timeval* tv)
{
  if (!ETCPAL_ASSERT_VERIFY(tv))
    return;

  tv->tv_sec  = ms / 1000;
  tv->tv_usec = (ms % 1000) * 1000;
}

etcpal_error_t etcpal_shutdown(etcpal_socket_t id, int how)
{
  if (how >= 0 && how < ETCPAL_NUM_SHUT)
  {
    int res = shutdown(id, shutmap[how]);
    return (res == 0 ? kEtcPalErrOk : errno_os_to_etcpal(errno));
  }
  return kEtcPalErrInvalid;
}

etcpal_error_t etcpal_socket(unsigned int family, unsigned int type, etcpal_socket_t* id)
{
  if (id)
  {
    if (family < ETCPAL_NUM_AF && type < ETCPAL_NUM_TYPE)
    {
      int sock = socket(sfmap[family], stmap[type], 0);
      if (sock != -1)
      {
        *id = sock;
        return kEtcPalErrOk;
      }
      else
      {
        *id = ETCPAL_SOCKET_INVALID;
        return errno_os_to_etcpal(errno);
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
  int val = fcntl(id, F_GETFL, 0);
  if (val >= 0)
  {
    val = fcntl(id, F_SETFL, (blocking ? (val & (int)(~O_NONBLOCK)) : (val | O_NONBLOCK)));
  }
  return (val >= 0 ? kEtcPalErrOk : errno_os_to_etcpal(errno));
}

etcpal_error_t etcpal_getblocking(etcpal_socket_t id, bool* blocking)
{
  if (blocking)
  {
    int val = fcntl(id, F_GETFL, 0);
    if (val >= 0)
    {
      *blocking = ((val & O_NONBLOCK) == 0);
      return kEtcPalErrOk;
    }
    return errno_os_to_etcpal(errno);
  }
  return kEtcPalErrInvalid;
}

etcpal_error_t etcpal_poll_context_init(EtcPalPollContext* context)
{
  if (!context)
    return kEtcPalErrInvalid;

  context->kq_fd = kqueue();
  if (context->kq_fd >= 0)
  {
    etcpal_rbtree_init(&context->sockets, poll_socket_compare, poll_socket_alloc, poll_socket_free);
    context->valid = true;
    return kEtcPalErrOk;
  }
  else
  {
    return errno_os_to_etcpal(errno);
  }
}

void etcpal_poll_context_deinit(EtcPalPollContext* context)
{
  if (context && context->valid)
  {
    etcpal_rbtree_clear(&context->sockets);
    close(context->kq_fd);
    context->valid = false;
  }
}

etcpal_error_t etcpal_poll_add_socket(EtcPalPollContext*   context,
                                      etcpal_socket_t      socket,
                                      etcpal_poll_events_t events,
                                      void*                user_data)
{
  if (context && context->valid && socket != ETCPAL_SOCKET_INVALID && (events & ETCPAL_POLL_VALID_INPUT_EVENT_MASK))
  {
    EtcPalPollSocket* sock_desc = (EtcPalPollSocket*)malloc(sizeof(EtcPalPollSocket));
    if (sock_desc)
    {
      sock_desc->sock           = socket;
      sock_desc->events         = events;
      etcpal_error_t insert_res = etcpal_rbtree_insert(&context->sockets, sock_desc);
      if (insert_res == kEtcPalErrOk)
      {
        struct kevent os_events[ETCPAL_SOCKET_MAX_KEVENTS];
        int           num_events = events_etcpal_to_kqueue(socket, 0, events, user_data, os_events);

        int res = kevent(context->kq_fd, os_events, num_events, NULL, 0, NULL);
        if (res == 0)
        {
          return kEtcPalErrOk;
        }
        else
        {
          // Our node dealloc function also deallocates sock_desc, so no need to free it here.
          etcpal_rbtree_remove(&context->sockets, sock_desc);
          return errno_os_to_etcpal(errno);
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
      return kEtcPalErrNoMem;
    }
  }
  else
  {
    return kEtcPalErrInvalid;
  }
}

etcpal_error_t etcpal_poll_modify_socket(EtcPalPollContext*   context,
                                         etcpal_socket_t      socket,
                                         etcpal_poll_events_t new_events,
                                         void*                new_user_data)
{
  if (context && context->valid && socket != ETCPAL_SOCKET_INVALID && (new_events & ETCPAL_POLL_VALID_INPUT_EVENT_MASK))
  {
    EtcPalPollSocket* sock_desc = (EtcPalPollSocket*)etcpal_rbtree_find(&context->sockets, &socket);
    if (sock_desc)
    {
      struct kevent os_events[ETCPAL_SOCKET_MAX_KEVENTS];
      int num_events = events_etcpal_to_kqueue(socket, sock_desc->events, new_events, new_user_data, os_events);

      int res = kevent(context->kq_fd, os_events, num_events, NULL, 0, NULL);
      if (res == 0)
      {
        sock_desc->events = new_events;
        return kEtcPalErrOk;
      }
      else
      {
        return errno_os_to_etcpal(errno);
      }
    }
    else
    {
      return kEtcPalErrNotFound;
    }
  }
  else
  {
    return kEtcPalErrInvalid;
  }
}

void etcpal_poll_remove_socket(EtcPalPollContext* context, etcpal_socket_t socket)
{
  if (context && context->valid)
  {
    EtcPalPollSocket* sock_desc = (EtcPalPollSocket*)etcpal_rbtree_find(&context->sockets, &socket);
    if (sock_desc)
    {
      struct kevent os_events[ETCPAL_SOCKET_MAX_KEVENTS];
      int           num_events = events_etcpal_to_kqueue(socket, sock_desc->events, 0, NULL, os_events);

      kevent(context->kq_fd, os_events, num_events, NULL, 0, NULL);
      etcpal_rbtree_remove(&context->sockets, sock_desc);
    }
  }
}

etcpal_error_t etcpal_poll_wait(EtcPalPollContext* context, EtcPalPollEvent* event, int timeout_ms)
{
  if (context && context->valid && event)
  {
    if (etcpal_rbtree_size(&context->sockets) > 0)
    {
      struct timespec  os_timeout;
      struct timespec* os_timeout_ptr;
      if (timeout_ms == ETCPAL_WAIT_FOREVER)
      {
        os_timeout_ptr = NULL;
      }
      else
      {
        os_timeout.tv_sec  = timeout_ms / 1000;
        os_timeout.tv_nsec = (timeout_ms % 1000) * 1000000;
        os_timeout_ptr     = &os_timeout;
      }

      struct kevent kevt;
      int           wait_res = kevent(context->kq_fd, NULL, 0, &kevt, 1, os_timeout_ptr);
      if (wait_res > 0)
      {
        etcpal_socket_t   sock      = (etcpal_socket_t)kevt.ident;
        EtcPalPollSocket* sock_desc = (EtcPalPollSocket*)etcpal_rbtree_find(&context->sockets, &sock);
        if (sock_desc)
        {
          event->socket    = sock_desc->sock;
          event->events    = events_kqueue_to_etcpal(&kevt, sock_desc);
          event->err       = kEtcPalErrOk;
          event->user_data = kevt.udata;

          // Check for errors
          int       error;
          socklen_t error_size = sizeof error;
          if (getsockopt(sock_desc->sock, SOL_SOCKET, SO_ERROR, &error, &error_size) == 0)
          {
            if (error != 0)
            {
              event->events |= ETCPAL_POLL_ERR;
              event->err = errno_os_to_etcpal(error);
            }
          }

          return kEtcPalErrOk;
        }
        else
        {
          return kEtcPalErrSys;
        }
      }
      else if (wait_res == 0)
      {
        return kEtcPalErrTimedOut;
      }
      else
      {
        return errno_os_to_etcpal(errno);
      }
    }
    else
    {
      return kEtcPalErrNoSockets;
    }
  }
  else
  {
    return kEtcPalErrInvalid;
  }
}

int events_etcpal_to_kqueue(etcpal_socket_t      socket,
                            etcpal_poll_events_t prev_events,
                            etcpal_poll_events_t new_events,
                            void*                user_data,
                            struct kevent*       kevents)
{
  if (!ETCPAL_ASSERT_VERIFY(kevents))
    return 0;

  int num_events = 0;

  // Process EVFILT_READ changes
  if (new_events & ETCPAL_POLL_IN)
  {
    // Re-add the socket even if it was already added before - user data might be modified.
    EV_SET(&kevents[num_events], socket, EVFILT_READ, EV_ADD, 0, 0, user_data);
    ++num_events;
  }
  else if (prev_events & ETCPAL_POLL_IN)
  {
    EV_SET(&kevents[num_events], socket, EVFILT_READ, EV_DELETE, 0, 0, user_data);
    ++num_events;
  }

  // Process EVFILT_WRITE changes
  if (new_events & (ETCPAL_POLL_OUT | ETCPAL_POLL_CONNECT))
  {
    // Re-add the socket even if it was already added before - user data might be modified.
    EV_SET(&kevents[num_events], socket, EVFILT_WRITE, EV_ADD, 0, 0, user_data);
    ++num_events;
  }
  else if (prev_events & (ETCPAL_POLL_OUT | ETCPAL_POLL_CONNECT))
  {
    EV_SET(&kevents[num_events], socket, EVFILT_WRITE, EV_DELETE, 0, 0, user_data);
    ++num_events;
  }

  // EVFILT_EXCEPT is not available on older versions of macOS. It was added somewhere between
  // 10.11 and 10.14.
#ifdef EVFILT_EXCEPT
  // Process EVFILT_EXCEPT changes
  if (new_events & ETCPAL_POLL_OOB)
  {
    // Re-add the socket even if it was already added before - user data might be modified.
    EV_SET(&kevents[num_events], socket, EVFILT_EXCEPT, EV_ADD, NOTE_OOB, 0, user_data);
    ++num_events;
  }
  else if (prev_events & ETCPAL_POLL_OOB)
  {
    EV_SET(&kevents[num_events], socket, EVFILT_EXCEPT, EV_DELETE, 0, 0, user_data);
    ++num_events;
  }
#endif

  return num_events;
}

etcpal_poll_events_t events_kqueue_to_etcpal(const struct kevent* kevent, const EtcPalPollSocket* sock_desc)
{
  if (!ETCPAL_ASSERT_VERIFY(kevent) || !ETCPAL_ASSERT_VERIFY(sock_desc))
    return (etcpal_poll_events_t)0;

  etcpal_poll_events_t events_out = 0;
  if (kevent->filter == EVFILT_READ)
  {
    events_out |= ETCPAL_POLL_IN;
    if (kevent->flags & EV_EOF)
      events_out |= ETCPAL_POLL_ERR;
    if (kevent->flags & EV_OOBAND && (sock_desc->events & ETCPAL_POLL_OOB))
      events_out |= ETCPAL_POLL_OOB;
  }
  if (kevent->filter == EVFILT_WRITE)
  {
    if (sock_desc->events & ETCPAL_POLL_OUT)
      events_out |= ETCPAL_POLL_OUT;
    if (sock_desc->events & ETCPAL_POLL_CONNECT)
      events_out |= ETCPAL_POLL_CONNECT;
    if (kevent->flags & EV_EOF)
      events_out |= ETCPAL_POLL_ERR;
  }
#ifdef EVFILT_EXCEPT
  if (kevent->filter == EVFILT_EXCEPT)
  {
    events_out |= ETCPAL_POLL_OOB;
    if (kevent->flags & EV_EOF)
      events_out |= ETCPAL_POLL_ERR;
  }
#endif

  return events_out;
}

int poll_socket_compare(const EtcPalRbTree* tree, const void* value_a, const void* value_b)
{
  ETCPAL_UNUSED_ARG(tree);

  if (!ETCPAL_ASSERT_VERIFY(value_a) || !ETCPAL_ASSERT_VERIFY(value_b))
    return 0;

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
  if (!ETCPAL_ASSERT_VERIFY(node))
    return;

  free(node->value);
  free(node);
}

void construct_msghdr(const EtcPalMsgHdr*      in_msg,
                      struct sockaddr_storage* name_store,
                      struct iovec*            buf_store,
                      struct msghdr*           out_msg)
{
  if (!ETCPAL_ASSERT_VERIFY(in_msg) || !ETCPAL_ASSERT_VERIFY(name_store) || !ETCPAL_ASSERT_VERIFY(buf_store) ||
      !ETCPAL_ASSERT_VERIFY(out_msg))
  {
    return;
  }

  out_msg->msg_name       = name_store;
  out_msg->msg_namelen    = sizeof(*name_store);
  out_msg->msg_iov        = buf_store;
  out_msg->msg_iovlen     = 1;
  out_msg->msg_control    = in_msg->control;
  out_msg->msg_controllen = in_msg->controllen;
  out_msg->msg_flags      = rcvmsg_flags_etcpal_to_os(in_msg->flags);
  buf_store->iov_base     = in_msg->buf;
  buf_store->iov_len      = in_msg->buflen;

  sockaddr_etcpal_to_os(&in_msg->name, (etcpal_os_sockaddr_t*)name_store);
}

int rcvmsg_flags_os_to_etcpal(int os_flags)
{
  int result = 0;

  if (os_flags & MSG_TRUNC)
    result |= ETCPAL_MSG_TRUNC;

  if (os_flags & MSG_CTRUNC)
    result |= ETCPAL_MSG_CTRUNC;

  if (os_flags & MSG_PEEK)
    result |= ETCPAL_MSG_PEEK;

  return result;
}

int rcvmsg_flags_etcpal_to_os(int etcpal_flags)
{
  int result = 0;

  if (etcpal_flags & ETCPAL_MSG_TRUNC)
    result |= MSG_TRUNC;

  if (etcpal_flags & ETCPAL_MSG_CTRUNC)
    result |= MSG_CTRUNC;

  if (etcpal_flags & ETCPAL_MSG_PEEK)
    result |= MSG_PEEK;

  return result;
}

bool get_first_compatible_cmsg(struct msghdr* msg, struct cmsghdr* start, EtcPalCMsgHdr* cmsg)
{
  if (!ETCPAL_ASSERT_VERIFY(msg) || !ETCPAL_ASSERT_VERIFY(cmsg))
    return false;

  bool get_next_cmsg = true;
  for (struct cmsghdr* hdr = start; hdr && (hdr->cmsg_len > 0) && get_next_cmsg; hdr = CMSG_NXTHDR(msg, hdr))
  {
    get_next_cmsg = false;

    cmsg->pd  = hdr;
    cmsg->len = hdr->cmsg_len;

    if (hdr->cmsg_level == IPPROTO_IP)
    {
      cmsg->level = ETCPAL_IPPROTO_IP;

      if (hdr->cmsg_type == IP_PKTINFO)
        cmsg->type = ETCPAL_IP_PKTINFO;
      else
        get_next_cmsg = true;
    }
    else if (hdr->cmsg_level == IPPROTO_IPV6)
    {
      cmsg->level = ETCPAL_IPPROTO_IPV6;

      if (hdr->cmsg_type == IPV6_PKTINFO)
        cmsg->type = ETCPAL_IPV6_PKTINFO;
      else
        get_next_cmsg = true;
    }
    else
    {
      get_next_cmsg = true;
    }
  }

  cmsg->valid = !get_next_cmsg;

  return cmsg->valid;
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
    pf_hints.ai_flags    = (hints->ai_flags < ETCPAL_NUM_AIF) ? aiflagmap[hints->ai_flags] : 0;
    pf_hints.ai_family   = (hints->ai_family < ETCPAL_NUM_AF) ? aifammap[hints->ai_family] : AF_UNSPEC;
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
  return (res == 0 ? kEtcPalErrOk : errno_os_to_etcpal(res));
}

bool etcpal_nextaddr(EtcPalAddrinfo* ai)
{
  if (ai && ai->pd[1])
  {
    struct addrinfo* pf_ai = (struct addrinfo*)ai->pd[1];
    ai->ai_flags           = 0;
    if (!sockaddr_os_to_etcpal(pf_ai->ai_addr, &ai->ai_addr))
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
    ai->pd[1]        = pf_ai->ai_next;

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
