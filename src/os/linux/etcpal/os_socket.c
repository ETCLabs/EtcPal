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

// This is defined before the includes to enable IPV6_PKTINFO support.
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include "etcpal/socket.h"

#include <errno.h>
#include <stdlib.h>

#include <arpa/inet.h>
#include <fcntl.h>
#include <net/if.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "etcpal/common.h"
#include "etcpal/private/common.h"
#include "os_error.h"

/**************************** Private constants ******************************/

/* Per the docs, this constant is ignored in Linux 2.6.8 and later, but must be > 0.
 * Here is a random number. */
#define EPOLL_CREATE_SIZE 1024

/****************************** Private types ********************************/

/* A struct to track sockets being polled by the etcpal_poll() API */
typedef struct EtcPalPollSocket
{
  // 'sock' must always remain as the first member in the struct to facilitate an EtcPalRbTree lookup
  // shortcut
  etcpal_socket_t      sock;
  etcpal_poll_events_t events;
  void*                user_data;
} EtcPalPollSocket;

/**************************** Private variables ******************************/

#if !defined(ETCPAL_BUILDING_MOCK_LIB)

/* clang-format off */

#define ETCPAL_NUM_SHUT 3
static const int kShutMap[ETCPAL_NUM_SHUT] =
{
  SHUT_RD,
  SHUT_WR,
  SHUT_RDWR
};

#define ETCPAL_NUM_AF 3
static const int kSfMap[ETCPAL_NUM_AF] =
{
  AF_UNSPEC,
  AF_INET,
  AF_INET6
};

#define ETCPAL_NUM_TYPE 2
static const int kStMap[ETCPAL_NUM_TYPE] =
{
  SOCK_STREAM,
  SOCK_DGRAM
};

#define ETCPAL_NUM_AIF 8
static const int kAiFlagMap[ETCPAL_NUM_AIF] =
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

static const int kAiFamMap[ETCPAL_NUM_AF] =
{
  AF_UNSPEC,
  AF_INET,
  AF_INET6
};

#define ETCPAL_NUM_IPPROTO 6
static const int kAiProtMap[ETCPAL_NUM_IPPROTO] =
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
static void events_etcpal_to_epoll(etcpal_poll_events_t events, struct epoll_event* epoll_evt);
static void events_epoll_to_etcpal(const struct epoll_event* epoll_evt,
                                   const EtcPalPollSocket*   sock_desc,
                                   etcpal_poll_events_t*     events);

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
  if ((id == ETCPAL_SOCKET_INVALID) || !conn_sock)
    return kEtcPalErrInvalid;

  struct sockaddr_storage ss      = {0};
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
  if ((id == ETCPAL_SOCKET_INVALID) || !address)
    return kEtcPalErrInvalid;

  struct sockaddr_storage ss      = {0};
  socklen_t               sa_size = (socklen_t)sockaddr_etcpal_to_os(address, (etcpal_os_sockaddr_t*)&ss);
  if (sa_size == 0)
    return kEtcPalErrInvalid;

  int res = bind(id, (struct sockaddr*)&ss, sa_size);
  return (res == 0 ? kEtcPalErrOk : errno_os_to_etcpal(errno));
}

etcpal_error_t etcpal_close(etcpal_socket_t id)
{
  if (id == ETCPAL_SOCKET_INVALID)
    return kEtcPalErrInvalid;

  int res = close(id);
  return (res == 0 ? kEtcPalErrOk : errno_os_to_etcpal(errno));
}

etcpal_error_t etcpal_connect(etcpal_socket_t id, const EtcPalSockAddr* address)
{
  if ((id == ETCPAL_SOCKET_INVALID) || !address)
    return kEtcPalErrInvalid;

  struct sockaddr_storage ss      = {0};
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
  if ((id == ETCPAL_SOCKET_INVALID) || !address)
    return kEtcPalErrInvalid;

  struct sockaddr_storage ss   = {0};
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

// NOLINTNEXTLINE(readability-non-const-parameter)
etcpal_error_t etcpal_getsockopt(etcpal_socket_t id, int level, int option_name, void* option_value, size_t* option_len)
{
  int res = -1;

  if ((id == ETCPAL_SOCKET_INVALID) || !option_value || !option_len)
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
  if (!ETCPAL_ASSERT_VERIFY(id != ETCPAL_SOCKET_INVALID) || !ETCPAL_ASSERT_VERIFY(option_value) ||
      !ETCPAL_ASSERT_VERIFY(option_len))
  {
    return -1;
  }

  switch (option_name)
  {
    case ETCPAL_SO_SNDBUF: {
      int res = getsockopt(id, SOL_SOCKET, SO_SNDBUF, option_value, (socklen_t*)option_len);
      if (res == 0)
        (*((int*)option_value)) /= 2;  // Linux returns a value double the configured size

      return res;
    }
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
  if (id == ETCPAL_SOCKET_INVALID)
    return kEtcPalErrInvalid;

  int res = listen(id, backlog);
  return (res == 0 ? kEtcPalErrOk : errno_os_to_etcpal(errno));
}

int etcpal_recv(etcpal_socket_t id, void* buffer, size_t length, int flags)
{
  if ((id == ETCPAL_SOCKET_INVALID) || !buffer)
    return kEtcPalErrInvalid;

  int impl_flags = (flags & ETCPAL_MSG_PEEK) ? MSG_PEEK : 0;
  int res        = (int)recv(id, buffer, length, impl_flags);
  return (res >= 0 ? res : (int)errno_os_to_etcpal(errno));
}

int etcpal_recvfrom(etcpal_socket_t id, void* buffer, size_t length, int flags, EtcPalSockAddr* address)
{
  if ((id == ETCPAL_SOCKET_INVALID) || !buffer)
    return (int)kEtcPalErrInvalid;

  struct sockaddr_storage fromaddr   = {0};
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
  if ((id == ETCPAL_SOCKET_INVALID) || !msg)
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
      if ((cmsg->level == ETCPAL_IPPROTO_IP) && (cmsg->type == ETCPAL_IP_PKTINFO) &&
          ETCPAL_ASSERT_VERIFY(cmsg->len >= sizeof(struct in_pktinfo)))
      {
        struct in_pktinfo impl_pktinfo = {0};
        memcpy(&impl_pktinfo, impl_data, sizeof(impl_pktinfo));

        struct sockaddr_in impl_addr = {0};
        impl_addr.sin_family         = AF_INET;
        impl_addr.sin_addr           = impl_pktinfo.ipi_addr;

        ip_os_to_etcpal((struct sockaddr*)&impl_addr, &pktinfo->addr);
        pktinfo->ifindex = impl_pktinfo.ipi_ifindex;

        result = true;
      }
      else if ((cmsg->level == ETCPAL_IPPROTO_IPV6) && (cmsg->type == ETCPAL_IPV6_PKTINFO) &&
               ETCPAL_ASSERT_VERIFY(cmsg->len >= sizeof(struct in6_pktinfo)))
      {
        struct in6_pktinfo impl_pktinfo = {0};
        memcpy(&impl_pktinfo, impl_data, sizeof(impl_pktinfo));

        struct sockaddr_in6 impl_addr = {0};
        impl_addr.sin6_family         = AF_INET6;
        impl_addr.sin6_addr           = impl_pktinfo.ipi6_addr;

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

  if ((id == ETCPAL_SOCKET_INVALID) || !message)
    return (int)kEtcPalErrInvalid;

  int res = (int)send(id, message, length, 0);
  return (res >= 0 ? res : (int)errno_os_to_etcpal(errno));
}

int etcpal_sendto(etcpal_socket_t id, const void* message, size_t length, int flags, const EtcPalSockAddr* dest_addr)
{
  ETCPAL_UNUSED_ARG(flags);

  if ((id == ETCPAL_SOCKET_INVALID) || !dest_addr || !message)
    return (int)kEtcPalErrInvalid;

  struct sockaddr_storage ss      = {0};
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

  if ((id == ETCPAL_SOCKET_INVALID) || !option_value)
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
  if (!ETCPAL_ASSERT_VERIFY(id != ETCPAL_SOCKET_INVALID) || !ETCPAL_ASSERT_VERIFY(option_value))
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
        int            val     = *(int*)option_value;
        struct timeval sys_val = {0};
        ms_to_timeval(val, &sys_val);
        return setsockopt(id, SOL_SOCKET, SO_RCVTIMEO, &sys_val, sizeof sys_val);
      }
      break;
    case ETCPAL_SO_SNDTIMEO:
      if (option_len == sizeof(int))
      {
        int            val     = *(int*)option_value;
        struct timeval sys_val = {0};
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
        EtcPalLinger* ll  = (EtcPalLinger*)option_value;
        struct linger val = {0};
        val.l_onoff       = (u_short)ll->onoff;
        val.l_linger      = (u_short)ll->linger;
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

static int ip4_ifindex_to_addr(unsigned int ifindex, struct in_addr* addr)
{
  if (!ETCPAL_ASSERT_VERIFY(addr))
    return -1;

  struct ifreq req = {0};
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
  if (!ETCPAL_ASSERT_VERIFY(id != ETCPAL_SOCKET_INVALID) || !ETCPAL_ASSERT_VERIFY(option_value))
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
          struct ip_mreq val       = {0};
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
          struct ip_mreq val       = {0};
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
        if (ETCPAL_IP_IS_V4(&greq->group))
        {
          struct group_req val = {0};
          val.gr_interface     = (uint32_t)greq->ifindex;

          memset(&val.gr_group, 0, sizeof val.gr_group);
          struct sockaddr_in* sin = (struct sockaddr_in*)&val.gr_group;
          sin->sin_family         = AF_INET;
          sin->sin_addr.s_addr    = htonl(ETCPAL_IP_V4_ADDRESS(&greq->group));
          return setsockopt(id, IPPROTO_IP, MCAST_JOIN_GROUP, &val, sizeof val);
        }
      }
      break;
    case ETCPAL_MCAST_LEAVE_GROUP:
      if (option_len == sizeof(EtcPalGroupReq))
      {
        EtcPalGroupReq* greq = (EtcPalGroupReq*)option_value;
        if (ETCPAL_IP_IS_V4(&greq->group))
        {
          struct group_req val = {0};
          val.gr_interface     = (uint32_t)greq->ifindex;

          memset(&val.gr_group, 0, sizeof val.gr_group);
          struct sockaddr_in* sin = (struct sockaddr_in*)&val.gr_group;
          sin->sin_family         = AF_INET;
          sin->sin_addr.s_addr    = htonl(ETCPAL_IP_V4_ADDRESS(&greq->group));
          return setsockopt(id, IPPROTO_IP, MCAST_LEAVE_GROUP, &val, sizeof val);
        }
      }
      break;
    case ETCPAL_IP_MULTICAST_IF:
      if (option_len == sizeof(unsigned int))
      {
        struct in_addr val = {0};
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
  if (!ETCPAL_ASSERT_VERIFY(id != ETCPAL_SOCKET_INVALID) || !ETCPAL_ASSERT_VERIFY(option_value))
    return -1;

  switch (option_name)
  {
    case ETCPAL_MCAST_JOIN_GROUP:
      if (option_len == sizeof(EtcPalGroupReq))
      {
        EtcPalGroupReq* greq = (EtcPalGroupReq*)option_value;
        if (ETCPAL_IP_IS_V6(&greq->group))
        {
          struct group_req val = {0};
          val.gr_interface     = (uint32_t)greq->ifindex;

          memset(&val.gr_group, 0, sizeof val.gr_group);
          struct sockaddr_in6* sin6 = (struct sockaddr_in6*)&val.gr_group;
          sin6->sin6_family         = AF_INET6;
          memcpy(sin6->sin6_addr.s6_addr, ETCPAL_IP_V6_ADDRESS(&greq->group), ETCPAL_IPV6_BYTES);
          return setsockopt(id, IPPROTO_IPV6, MCAST_JOIN_GROUP, &val, sizeof val);
        }
      }
      break;
    case ETCPAL_MCAST_LEAVE_GROUP:
      if (option_len == sizeof(EtcPalGroupReq))
      {
        EtcPalGroupReq* greq = (EtcPalGroupReq*)option_value;
        if (ETCPAL_IP_IS_V6(&greq->group))
        {
          struct group_req val = {0};
          val.gr_interface     = (uint32_t)greq->ifindex;

          memset(&val.gr_group, 0, sizeof val.gr_group);
          struct sockaddr_in6* sin6 = (struct sockaddr_in6*)&val.gr_group;
          sin6->sin6_family         = AF_INET6;
          memcpy(sin6->sin6_addr.s6_addr, ETCPAL_IP_V6_ADDRESS(&greq->group), ETCPAL_IPV6_BYTES);
          return setsockopt(id, IPPROTO_IPV6, MCAST_LEAVE_GROUP, &val, sizeof val);
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
    default: /* Other IPv6 options TODO on linux. */
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
  if ((id != ETCPAL_SOCKET_INVALID) && (how >= 0) && (how < ETCPAL_NUM_SHUT))
  {
    int res = shutdown(id, kShutMap[how]);
    return (res == 0 ? kEtcPalErrOk : errno_os_to_etcpal(errno));
  }

  return kEtcPalErrInvalid;
}

etcpal_error_t etcpal_socket(unsigned int family, unsigned int type, etcpal_socket_t* id)
{
  if (!id)
    return kEtcPalErrInvalid;

  if (family >= ETCPAL_NUM_AF || type >= ETCPAL_NUM_TYPE)
  {
    *id = ETCPAL_SOCKET_INVALID;
    return kEtcPalErrInvalid;
  }

  int sock = socket(kSfMap[family], kStMap[type], 0);
  if (sock < 0)
  {
    *id = ETCPAL_SOCKET_INVALID;
    return errno_os_to_etcpal(errno);
  }

  *id = sock;
  return kEtcPalErrOk;
}

etcpal_error_t etcpal_setblocking(etcpal_socket_t id, bool blocking)
{
  if (id == ETCPAL_SOCKET_INVALID)
    return kEtcPalErrInvalid;

  int val = fcntl(id, F_GETFL, 0);
  if (val >= 0)
    val = fcntl(id, F_SETFL, (blocking ? (val & (int)(~O_NONBLOCK)) : (val | O_NONBLOCK)));

  return (val >= 0 ? kEtcPalErrOk : errno_os_to_etcpal(errno));
}

etcpal_error_t etcpal_getblocking(etcpal_socket_t id, bool* blocking)
{
  if ((id == ETCPAL_SOCKET_INVALID) || !blocking)
    return kEtcPalErrInvalid;

  int val = fcntl(id, F_GETFL, 0);
  if (val < 0)
    return errno_os_to_etcpal(errno);

  *blocking = ((val & O_NONBLOCK) == 0);
  return kEtcPalErrOk;
}

etcpal_error_t etcpal_poll_context_init(EtcPalPollContext* context)
{
  if (!context)
    return kEtcPalErrInvalid;

  context->epoll_fd = epoll_create(EPOLL_CREATE_SIZE);
  if (context->epoll_fd >= 0)
  {
    etcpal_rbtree_init(&context->sockets, poll_socket_compare, poll_socket_alloc, poll_socket_free);
    context->valid = true;
    return kEtcPalErrOk;
  }

  return errno_os_to_etcpal(errno);
}

void etcpal_poll_context_deinit(EtcPalPollContext* context)
{
  if (context && context->valid)
  {
    etcpal_rbtree_clear(&context->sockets);
    close(context->epoll_fd);
    context->valid = false;
  }
}

etcpal_error_t etcpal_poll_add_socket(EtcPalPollContext*   context,
                                      etcpal_socket_t      socket,
                                      etcpal_poll_events_t events,
                                      void*                user_data)
{
  if (!context || !context->valid || socket == ETCPAL_SOCKET_INVALID || !(events & ETCPAL_POLL_VALID_INPUT_EVENT_MASK))
    return kEtcPalErrInvalid;

  EtcPalPollSocket* sock_desc = (EtcPalPollSocket*)malloc(sizeof(EtcPalPollSocket));
  if (!sock_desc)
    return kEtcPalErrNoMem;

  sock_desc->sock           = socket;
  sock_desc->events         = events;
  sock_desc->user_data      = user_data;
  etcpal_error_t insert_res = etcpal_rbtree_insert(&context->sockets, sock_desc);
  if (insert_res != kEtcPalErrOk)
  {
    free(sock_desc);
    return insert_res;
  }

  struct epoll_event ep_evt = {0};
  events_etcpal_to_epoll(events, &ep_evt);
  ep_evt.data.fd = socket;

  int res = epoll_ctl(context->epoll_fd, EPOLL_CTL_ADD, socket, &ep_evt);
  if (res != 0)
  {
    // Our node dealloc function also deallocates sock_desc, so no need to free it here.
    etcpal_rbtree_remove(&context->sockets, sock_desc);
    return errno_os_to_etcpal(errno);
  }

  return kEtcPalErrOk;
}

etcpal_error_t etcpal_poll_modify_socket(EtcPalPollContext*   context,
                                         etcpal_socket_t      socket,
                                         etcpal_poll_events_t new_events,
                                         void*                new_user_data)
{
  if (!context || !context->valid || socket == ETCPAL_SOCKET_INVALID ||
      !(new_events & ETCPAL_POLL_VALID_INPUT_EVENT_MASK))
  {
    return kEtcPalErrInvalid;
  }

  EtcPalPollSocket* sock_desc = (EtcPalPollSocket*)etcpal_rbtree_find(&context->sockets, &socket);
  if (!sock_desc)
    return kEtcPalErrNotFound;

  struct epoll_event ep_evt = {0};
  events_etcpal_to_epoll(new_events, &ep_evt);
  ep_evt.data.fd = socket;

  int res = epoll_ctl(context->epoll_fd, EPOLL_CTL_MOD, socket, &ep_evt);
  if (res != 0)
    return errno_os_to_etcpal(errno);

  sock_desc->events    = new_events;
  sock_desc->user_data = new_user_data;
  return kEtcPalErrOk;
}

void etcpal_poll_remove_socket(EtcPalPollContext* context, etcpal_socket_t socket)
{
  if (context && context->valid)
  {
    // Need a dummy struct for portability - some versions require event to always be non-NULL
    // even though it is ignored
    struct epoll_event ep_evt = {0};
    epoll_ctl(context->epoll_fd, EPOLL_CTL_DEL, socket, &ep_evt);
    etcpal_rbtree_remove(&context->sockets, &socket);
  }
}

etcpal_error_t etcpal_poll_wait(EtcPalPollContext* context, EtcPalPollEvent* event, int timeout_ms)
{
  if (!context || !context->valid || !event)
    return kEtcPalErrInvalid;

  if (etcpal_rbtree_size(&context->sockets) == 0)
    return kEtcPalErrNoSockets;

  int sys_timeout = (timeout_ms == ETCPAL_WAIT_FOREVER ? -1 : timeout_ms);

  struct epoll_event epoll_evt = {0};
  int                wait_res  = epoll_wait(context->epoll_fd, &epoll_evt, 1, sys_timeout);
  if (wait_res == 0)
    return kEtcPalErrTimedOut;
  if (wait_res < 0)
    return errno_os_to_etcpal(errno);

  EtcPalPollSocket* sock_desc = (EtcPalPollSocket*)etcpal_rbtree_find(&context->sockets, &epoll_evt.data.fd);
  if (!sock_desc)
    return kEtcPalErrSys;

  event->socket = sock_desc->sock;
  events_epoll_to_etcpal(&epoll_evt, sock_desc, &event->events);
  event->err       = kEtcPalErrOk;
  event->user_data = sock_desc->user_data;

  // Check for errors
  int       error      = 0;
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

void events_etcpal_to_epoll(etcpal_poll_events_t events, struct epoll_event* epoll_evt)
{
  if (!ETCPAL_ASSERT_VERIFY(epoll_evt))
    return;

  epoll_evt->events = 0;
  if (events & ETCPAL_POLL_IN)
    epoll_evt->events |= EPOLLIN;
  if ((events & ETCPAL_POLL_OUT) || (events & ETCPAL_POLL_CONNECT))
    epoll_evt->events |= EPOLLOUT;
  if (events & ETCPAL_POLL_OOB)
    epoll_evt->events |= EPOLLPRI;
}

void events_epoll_to_etcpal(const struct epoll_event* epoll_evt,
                            const EtcPalPollSocket*   sock_desc,
                            etcpal_poll_events_t*     events_out)
{
  if (!ETCPAL_ASSERT_VERIFY(epoll_evt) || !ETCPAL_ASSERT_VERIFY(sock_desc) || !ETCPAL_ASSERT_VERIFY(events_out))
    return;

  *events_out = 0;
  if (epoll_evt->events & EPOLLIN)
    *events_out |= ETCPAL_POLL_IN;
  if (epoll_evt->events & EPOLLOUT)
  {
    if (sock_desc->events & ETCPAL_POLL_OUT)
      *events_out |= ETCPAL_POLL_OUT;
    if (sock_desc->events & ETCPAL_POLL_CONNECT)
      *events_out |= ETCPAL_POLL_CONNECT;
  }
  if (epoll_evt->events & EPOLLPRI)
    *events_out |= (ETCPAL_POLL_OOB);
  if (epoll_evt->events & EPOLLERR)
    *events_out |= (ETCPAL_POLL_ERR);
}

int poll_socket_compare(const EtcPalRbTree* tree, const void* value_a, const void* value_b)
{
  ETCPAL_UNUSED_ARG(tree);

  if (!ETCPAL_ASSERT_VERIFY(value_a) || !ETCPAL_ASSERT_VERIFY(value_b))
    return 0;

  const etcpal_socket_t* a = (const etcpal_socket_t*)value_a;
  const etcpal_socket_t* b = (const etcpal_socket_t*)value_b;

  return (*a > *b) - (*a < *b);
}

EtcPalRbNode* poll_socket_alloc(void)
{
  return (EtcPalRbNode*)malloc(sizeof(EtcPalRbNode));
}

void poll_socket_free(EtcPalRbNode* node)
{
  if (!ETCPAL_ASSERT_VERIFY(node))
    return;

  if (ETCPAL_ASSERT_VERIFY(node->value))
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
  if ((!hostname && !service) || !result)
    return kEtcPalErrInvalid;

  struct addrinfo pf_hints = {0};
  memset(&pf_hints, 0, sizeof pf_hints);
  if (hints)
  {
    pf_hints.ai_flags    = (hints->ai_flags < ETCPAL_NUM_AIF) ? kAiFlagMap[hints->ai_flags] : 0;
    pf_hints.ai_family   = (hints->ai_family < ETCPAL_NUM_AF) ? kAiFamMap[hints->ai_family] : AF_UNSPEC;
    pf_hints.ai_socktype = (hints->ai_socktype < ETCPAL_NUM_TYPE) ? kStMap[hints->ai_socktype] : 0;
    pf_hints.ai_protocol = (hints->ai_protocol < ETCPAL_NUM_IPPROTO) ? kAiProtMap[hints->ai_protocol] : 0;
  }
  else
  {
    pf_hints.ai_family = AF_UNSPEC;
  }

  struct addrinfo* pf_res = NULL;
  int              res    = getaddrinfo(hostname, service, hints ? &pf_hints : NULL, &pf_res);
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
  {
    freeaddrinfo((struct addrinfo*)ai->pd[0]);
    ai->pd[0] = NULL;
    ai->pd[1] = NULL;
  }
}

#endif  // !defined(ETCPAL_BUILDING_MOCK_LIB)
