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

#include "etcpal/socket.h"

#include <string.h>
#include <lwip/sockets.h>
#include <lwip/netdb.h>
#include <lwip/netif.h>
#include <lwip/tcpip.h>

#include "etcpal/common.h"
#include "etcpal/private/common.h"
#include "os_error.h"

/***************************** Private macros ********************************/

#define ETCPAL_FD_ZERO(setptr) \
  FD_ZERO(&(setptr)->set);     \
  (setptr)->count = 0

#define ETCPAL_FD_SET(sock, setptr) \
  FD_SET(sock, &(setptr)->set);     \
  (setptr)->count++

#define ETCPAL_FD_CLEAR(sock, setptr) \
  FD_CLR(sock, &(setptr)->set);       \
  (setptr)->count--

#define ETCPAL_FD_ISSET(sock, setptr) FD_ISSET(sock, &(setptr)->set)

/**************************** Private variables ******************************/

#if !defined(ETCPAL_BUILDING_MOCK_LIB)

/* clang-format off */

#define ETCPAL_NUM_SHUT   3
static const int shutmap[ETCPAL_NUM_SHUT] =
{
  SHUT_RD,
  SHUT_WR,
  SHUT_RDWR
};

#define ETCPAL_NUM_AF     3
static const int sfmap[ETCPAL_NUM_AF] =
{
  AF_UNSPEC,
  AF_INET,
  AF_INET6
};

#define ETCPAL_NUM_TYPE   2
static const int stmap[ETCPAL_NUM_TYPE] =
{
  SOCK_STREAM,
  SOCK_DGRAM
};

#if LWIP_DNS

#define ETCPAL_NUM_AIF    8
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

#define ETCPAL_NUM_IPPROTO    6
static const int aiprotmap[ETCPAL_NUM_IPPROTO] =
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

// Helpers for etcpal_setsockopt()
static void ms_to_timeval(int ms, struct timeval* tv);
static int  setsockopt_socket(etcpal_socket_t id, int option_name, const void* option_value, size_t option_len);
#if LWIP_IPV4
static int setsockopt_ip(etcpal_socket_t id, int option_name, const void* option_value, size_t option_len);
#endif
#if LWIP_IPV6
static int setsockopt_ip6(etcpal_socket_t id, int option_name, const void* option_value, size_t option_len);
#endif

// Helper functions for the etcpal_poll API
static void              init_context_socket_array(EtcPalPollContext* context);
static EtcPalPollSocket* find_socket(EtcPalPollContext* context, etcpal_socket_t sock);
static EtcPalPollSocket* find_hole(EtcPalPollContext* context);
static void              set_in_fd_sets(EtcPalPollContext* context, const EtcPalPollSocket* sock);
static void              clear_in_fd_sets(EtcPalPollContext* context, const EtcPalPollSocket* sock);
static etcpal_error_t    handle_select_result(EtcPalPollContext* context,
                                              EtcPalPollEvent*   event,
                                              const fd_set*      readfds,
                                              const fd_set*      writefds,
                                              const fd_set*      exceptfds);

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
  /* No initialization is necessary on this os. */
  return kEtcPalErrOk;
}

void etcpal_socket_deinit(void)
{
  /* No deinitialization is necessary on this os. */
}

etcpal_error_t etcpal_accept(etcpal_socket_t id, EtcPalSockAddr* address, etcpal_socket_t* conn_sock)
{
  if ((id == ETCPAL_SOCKET_INVALID) || !conn_sock)
    return kEtcPalErrInvalid;

  struct sockaddr_storage ss      = {0};
  socklen_t               sa_size = sizeof ss;
  int                     res     = lwip_accept(id, (struct sockaddr*)&ss, &sa_size);

  if (res != -1)
  {
    if (address && !sockaddr_os_to_etcpal((etcpal_os_sockaddr_t*)&ss, address))
    {
      lwip_close(res);
      return kEtcPalErrSys;
    }

    *conn_sock = res;
    return kEtcPalErrOk;
  }

  return errno_lwip_to_etcpal(errno);
}

etcpal_error_t etcpal_bind(etcpal_socket_t id, const EtcPalSockAddr* address)
{
  if ((id == ETCPAL_SOCKET_INVALID) || !address)
    return kEtcPalErrInvalid;

  struct sockaddr_storage ss      = {0};
  socklen_t               sa_size = (socklen_t)sockaddr_etcpal_to_os(address, (etcpal_os_sockaddr_t*)&ss);
  if (sa_size == 0)
    return kEtcPalErrInvalid;

  int res = lwip_bind(id, (struct sockaddr*)&ss, sa_size);
  return (res == 0 ? kEtcPalErrOk : errno_lwip_to_etcpal(errno));
}

etcpal_error_t etcpal_close(etcpal_socket_t id)
{
  if (id == ETCPAL_SOCKET_INVALID)
    return kEtcPalErrInvalid;

  int res = lwip_close(id);
  return (res == 0 ? kEtcPalErrOk : errno_lwip_to_etcpal(errno));
}

etcpal_error_t etcpal_connect(etcpal_socket_t id, const EtcPalSockAddr* address)
{
  if ((id == ETCPAL_SOCKET_INVALID) || !address)
    return kEtcPalErrInvalid;

  struct sockaddr_storage ss      = {0};
  socklen_t               sa_size = (socklen_t)sockaddr_etcpal_to_os(address, (etcpal_os_sockaddr_t*)&ss);
  if (sa_size == 0)
    return kEtcPalErrInvalid;

  int res = lwip_connect(id, (struct sockaddr*)&ss, sa_size);
  return (res == 0 ? kEtcPalErrOk : errno_lwip_to_etcpal(errno));
}

etcpal_error_t etcpal_getpeername(etcpal_socket_t id, EtcPalSockAddr* address)
{
  // TODO
  ETCPAL_UNUSED_ARG(id);
  ETCPAL_UNUSED_ARG(address);
  return kEtcPalErrNotImpl;
}

etcpal_error_t etcpal_getsockname(etcpal_socket_t id, EtcPalSockAddr* address)
{
  if ((id == ETCPAL_SOCKET_INVALID) || !address)
    return kEtcPalErrInvalid;

  struct sockaddr_storage ss   = {0};
  socklen_t               size = (socklen_t)sizeof ss;
  int                     res  = lwip_getsockname(id, (struct sockaddr*)&ss, &size);
  if (res == 0)
  {
    if (!sockaddr_os_to_etcpal((etcpal_os_sockaddr_t*)&ss, address))
      return kEtcPalErrSys;

    return kEtcPalErrOk;
  }

  return errno_lwip_to_etcpal(errno);
}

etcpal_error_t etcpal_getsockopt(etcpal_socket_t id, int level, int option_name, void* option_value, size_t* option_len)
{
  // TODO
  ETCPAL_UNUSED_ARG(id);
  ETCPAL_UNUSED_ARG(level);
  ETCPAL_UNUSED_ARG(option_name);
  ETCPAL_UNUSED_ARG(option_value);
  ETCPAL_UNUSED_ARG(option_len);
  return kEtcPalErrNotImpl;
}

etcpal_error_t etcpal_listen(etcpal_socket_t id, int backlog)
{
  if (id == ETCPAL_SOCKET_INVALID)
    return kEtcPalErrInvalid;

  int res = lwip_listen(id, backlog);
  return (res == 0 ? kEtcPalErrOk : errno_lwip_to_etcpal(errno));
}

int etcpal_recv(etcpal_socket_t id, void* buffer, size_t length, int flags)
{
  if ((id == ETCPAL_SOCKET_INVALID) || !buffer)
    return (int)kEtcPalErrInvalid;

  int impl_flags = (flags & ETCPAL_MSG_PEEK) ? MSG_PEEK : 0;
  int res        = (int)lwip_recv(id, buffer, length, impl_flags);
  return (res >= 0 ? res : (int)errno_lwip_to_etcpal(errno));
}

int etcpal_recvfrom(etcpal_socket_t id, void* buffer, size_t length, int flags, EtcPalSockAddr* address)
{
  if ((id == ETCPAL_SOCKET_INVALID) || !buffer)
    return (int)kEtcPalErrInvalid;

  struct sockaddr_storage fromaddr   = {0};
  socklen_t               fromlen    = (socklen_t)sizeof fromaddr;
  int                     impl_flags = (flags & ETCPAL_MSG_PEEK) ? MSG_PEEK : 0;
  int res = (int)lwip_recvfrom(id, buffer, length, impl_flags, (struct sockaddr*)&fromaddr, &fromlen);

  if (res >= 0)
  {
    if (address && fromlen > 0)
    {
      if (!sockaddr_os_to_etcpal((const etcpal_os_sockaddr_t*)&fromaddr, address))
        return (int)kEtcPalErrSys;
    }

    return res;
  }

  return (int)errno_lwip_to_etcpal(errno);
}

int etcpal_recvmsg(etcpal_socket_t id, EtcPalMsgHdr* msg, int flags)
{
  if ((id == ETCPAL_SOCKET_INVALID) || !msg)
    return (int)kEtcPalErrInvalid;

  struct msghdr           impl_msg  = {0};
  struct sockaddr_storage impl_name = {0};
  struct iovec            impl_buf  = {0};
  construct_msghdr(msg, &impl_name, &impl_buf, &impl_msg);

  int res = (int)lwip_recvmsg(id, &impl_msg, rcvmsg_flags_etcpal_to_os(flags));

  msg->flags = rcvmsg_flags_os_to_etcpal(impl_msg.msg_flags);

  if (res < 0)
    return (int)errno_lwip_to_etcpal(errno);

  if (!sockaddr_os_to_etcpal((const etcpal_os_sockaddr_t*)&impl_name, &msg->name))
    return (int)kEtcPalErrSys;

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

      // TODO: IPv6 once lwIP supports it
    }
  }

  return result;
}

int etcpal_send(etcpal_socket_t id, const void* message, size_t length, int flags)
{
  ETCPAL_UNUSED_ARG(flags);

  if ((id == ETCPAL_SOCKET_INVALID) || !message)
    return (int)kEtcPalErrInvalid;

  int res = (int)lwip_send(id, message, length, 0);
  return (res >= 0 ? res : (int)errno_lwip_to_etcpal(errno));
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

  int res = (int)lwip_sendto(id, message, length, 0, (struct sockaddr*)&ss, ss_size);
  return (res >= 0 ? res : (int)errno_lwip_to_etcpal(errno));
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

  switch (level)
  {
    case ETCPAL_SOL_SOCKET:
      res = setsockopt_socket(id, option_name, option_value, option_len);
      break;
    case ETCPAL_IPPROTO_IP:
#if LWIP_IPV4
      res = setsockopt_ip(id, option_name, option_value, option_len);
#else
      errno = EAFNOSUPPORT;
#endif
      break;
    case ETCPAL_IPPROTO_IPV6:
#if LWIP_IPV6
      res = setsockopt_ip6(id, option_name, option_value, option_len);
#else
      errno = EAFNOSUPPORT;
#endif
      break;
    default:
      break;
  }

  return (res == 0 ? kEtcPalErrOk : errno_lwip_to_etcpal(errno));
}

int setsockopt_socket(etcpal_socket_t id, int option_name, const void* option_value, size_t option_len)
{
  if (!ETCPAL_ASSERT_VERIFY(id != ETCPAL_SOCKET_INVALID) || !ETCPAL_ASSERT_VERIFY(option_value))
    return -1;

  switch (option_name)
  {
    case ETCPAL_SO_RCVBUF:
      return lwip_setsockopt(id, SOL_SOCKET, SO_RCVBUF, option_value, option_len);
    case ETCPAL_SO_SNDBUF:
      return lwip_setsockopt(id, SOL_SOCKET, SO_SNDBUF, option_value, option_len);
    case ETCPAL_SO_RCVTIMEO:
      if (option_len == sizeof(int))
      {
        int            ms  = *(int*)option_value;
        struct timeval val = {0};
        ms_to_timeval(ms, &val);
        return lwip_setsockopt(id, SOL_SOCKET, SO_RCVTIMEO, &val, sizeof val);
      }
      break;
    case ETCPAL_SO_SNDTIMEO:
      if (option_len == sizeof(int))
      {
        int            ms  = *(int*)option_value;
        struct timeval val = {0};
        ms_to_timeval(ms, &val);
        return lwip_setsockopt(id, SOL_SOCKET, SO_SNDTIMEO, &val, sizeof val);
      }
      break;
    case ETCPAL_SO_REUSEADDR:
      return lwip_setsockopt(id, SOL_SOCKET, SO_REUSEADDR, option_value, option_len);
    case ETCPAL_SO_REUSEPORT:
      return lwip_setsockopt(id, SOL_SOCKET, SO_REUSEPORT, option_value, option_len);
    case ETCPAL_SO_BROADCAST:
      return lwip_setsockopt(id, SOL_SOCKET, SO_BROADCAST, option_value, option_len);
    case ETCPAL_SO_KEEPALIVE:
      return lwip_setsockopt(id, SOL_SOCKET, SO_KEEPALIVE, option_value, option_len);
    case ETCPAL_SO_LINGER:
      if (option_len == sizeof(EtcPalLinger))
      {
        EtcPalLinger* ll  = (EtcPalLinger*)option_value;
        struct linger val = {0};
        val.l_onoff       = ll->onoff;
        val.l_linger      = ll->linger;
        return lwip_setsockopt(id, SOL_SOCKET, SO_LINGER, &val, sizeof val);
      }
      break;
    case ETCPAL_SO_ERROR:  // Set not supported
    case ETCPAL_SO_TYPE:   // Set not supported
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
  if (!ETCPAL_ASSERT_VERIFY(addr))
    return false;

  bool result = false;
  LOCK_TCPIP_CORE();
  struct netif* lwip_netif = netif_get_by_index((u8_t)netif_index);
  if (lwip_netif)
  {
    const ip4_addr_t* lwip_addr = netif_ip4_addr(lwip_netif);
    addr->s_addr                = lwip_addr->addr;
    result                      = true;
  }

  UNLOCK_TCPIP_CORE();
  return result;
}

int setsockopt_ip(etcpal_socket_t id, int option_name, const void* option_value, size_t option_len)
{
  if (!ETCPAL_ASSERT_VERIFY(id != ETCPAL_SOCKET_INVALID) || !ETCPAL_ASSERT_VERIFY(option_value))
    return -1;

  switch (option_name)
  {
    case ETCPAL_IP_TTL:
      return lwip_setsockopt(id, IPPROTO_IP, IP_TTL, option_value, option_len);
#if LWIP_IGMP
    case ETCPAL_IP_ADD_MEMBERSHIP:
      if (option_len == sizeof(EtcPalMreq))
      {
        EtcPalMreq* amreq = (EtcPalMreq*)option_value;
        if (ETCPAL_IP_IS_V4(&amreq->group))
        {
          struct ip_mreq val       = {0};
          val.imr_multiaddr.s_addr = lwip_htonl(ETCPAL_IP_V4_ADDRESS(&amreq->group));
          val.imr_interface.s_addr = lwip_htonl(ETCPAL_IP_V4_ADDRESS(&amreq->netint));
          return lwip_setsockopt(id, IPPROTO_IP, IP_ADD_MEMBERSHIP, &val, sizeof val);
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
          val.imr_multiaddr.s_addr = lwip_htonl(ETCPAL_IP_V4_ADDRESS(&amreq->group));
          val.imr_interface.s_addr = lwip_htonl(ETCPAL_IP_V4_ADDRESS(&amreq->netint));
          return lwip_setsockopt(id, IPPROTO_IP, IP_DROP_MEMBERSHIP, &val, sizeof val);
        }
      }
      break;
    case ETCPAL_MCAST_JOIN_GROUP:
      if (option_len == sizeof(EtcPalGroupReq))
      {
        EtcPalGroupReq* greq     = (EtcPalGroupReq*)option_value;
        struct ip_mreq  val      = {0};
        val.imr_multiaddr.s_addr = lwip_htonl(ETCPAL_IP_V4_ADDRESS(&greq->group));
        if (!netif_index_to_ipv4_addr(greq->ifindex, &val.imr_interface))
          return -1;
        return lwip_setsockopt(id, IPPROTO_IP, IP_ADD_MEMBERSHIP, &val, sizeof val);
      }
      break;
    case ETCPAL_MCAST_LEAVE_GROUP:
      if (option_len == sizeof(EtcPalGroupReq))
      {
        EtcPalGroupReq* greq     = (EtcPalGroupReq*)option_value;
        struct ip_mreq  val      = {0};
        val.imr_multiaddr.s_addr = lwip_htonl(ETCPAL_IP_V4_ADDRESS(&greq->group));
        if (!netif_index_to_ipv4_addr(greq->ifindex, &val.imr_interface))
          return -1;
        return lwip_setsockopt(id, IPPROTO_IP, IP_DROP_MEMBERSHIP, &val, sizeof val);
      }
      break;
#endif  // LWIP_IGMP
#if LWIP_MULTICAST_TX_OPTIONS
    case ETCPAL_IP_MULTICAST_IF:
      if (option_len == sizeof(unsigned int))
      {
        unsigned int   netint_index = *((unsigned int*)option_value);
        struct in_addr val          = {0};
        if (!netif_index_to_ipv4_addr(netint_index, &val))
          return -1;
        return lwip_setsockopt(id, IPPROTO_IP, IP_MULTICAST_IF, &val, sizeof val);
      }
      break;
    case ETCPAL_IP_MULTICAST_TTL: {
      if (option_len == sizeof(int))
      {
        unsigned char val = (unsigned char)*(int*)option_value;
        return lwip_setsockopt(id, IPPROTO_IP, IP_MULTICAST_TTL, &val, sizeof val);
      }
    }
    case ETCPAL_IP_MULTICAST_LOOP: {
      if (option_len == sizeof(int))
      {
        unsigned char val = (unsigned char)*(int*)option_value;
        return lwip_setsockopt(id, IPPROTO_IP, IP_MULTICAST_LOOP, &val, sizeof val);
      }
    }
    case ETCPAL_IP_PKTINFO:
      return lwip_setsockopt(id, IPPROTO_IP, IP_PKTINFO, option_value, option_len);
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
          struct ipv6_mreq val = {0};
          val.ipv6mr_interface = greq->ifindex;
          memcpy(&val.ipv6mr_multiaddr.s6_addr, ETCPAL_IP_V6_ADDRESS(&greq->group), ETCPAL_IPV6_BYTES);
          return lwip_setsockopt(id, IPPROTO_IPV6, IPV6_JOIN_GROUP, &val, sizeof val);
        }
      }
      break;
    case ETCPAL_MCAST_LEAVE_GROUP:
      if (option_len == sizeof(EtcPalGroupReq))
      {
        EtcPalGroupReq* greq = (EtcPalGroupReq*)option_value;
        if (ETCPAL_IP_IS_V6(&greq->group))
        {
          struct ipv6_mreq val = {0};
          val.ipv6mr_interface = greq->ifindex;
          memcpy(&val.ipv6mr_multiaddr.s6_addr, ETCPAL_IP_V6_ADDRESS(&greq->group), ETCPAL_IPV6_BYTES);
          return lwip_setsockopt(id, IPPROTO_IPV6, IPV6_JOIN_GROUP, &val, sizeof val);
        }
      }
      break;
    case ETCPAL_IPV6_V6ONLY:
      return lwip_setsockopt(id, IPPROTO_IPV6, IPV6_V6ONLY, option_value, option_len);
    default:  // lwIP does not have support for the IPv6 multicast TX options or pktinfo option yet.
      break;
  }

  // If we got here, something was invalid. Set errno accordingly.
  errno = EINVAL;
  return -1;
}
#endif  // LWIP_IPV6

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
    int res = lwip_shutdown(id, shutmap[how]);
    return (res == 0 ? kEtcPalErrOk : errno_lwip_to_etcpal(errno));
  }

  return kEtcPalErrInvalid;
}

etcpal_error_t etcpal_socket(unsigned int family, unsigned int type, etcpal_socket_t* id)
{
  if (id)
  {
    if (family < ETCPAL_NUM_AF && type < ETCPAL_NUM_TYPE)
    {
      int sock = lwip_socket(sfmap[family], stmap[type], 0);
      if (sock >= 0)
      {
        *id = sock;
        return kEtcPalErrOk;
      }

      *id = ETCPAL_SOCKET_INVALID;
      return errno_lwip_to_etcpal(errno);
    }

    *id = ETCPAL_SOCKET_INVALID;
  }

  return kEtcPalErrInvalid;
}

etcpal_error_t etcpal_setblocking(etcpal_socket_t id, bool blocking)
{
  if (id == ETCPAL_SOCKET_INVALID)
    return kEtcPalErrInvalid;

  int val = lwip_fcntl(id, F_GETFL, 0);
  if (val >= 0)
  {
    val = lwip_fcntl(id, F_SETFL, (blocking ? (val & (int)(~O_NONBLOCK)) : (val | O_NONBLOCK)));
  }

  return (val >= 0 ? kEtcPalErrOk : errno_lwip_to_etcpal(errno));
}

etcpal_error_t etcpal_getblocking(etcpal_socket_t id, bool* blocking)
{
  if ((id != ETCPAL_SOCKET_INVALID) && blocking)
  {
    int val = lwip_fcntl(id, F_GETFL, 0);
    if (val >= 0)
    {
      *blocking = ((val & O_NONBLOCK) == 0);
      return kEtcPalErrOk;
    }

    return errno_lwip_to_etcpal(errno);
  }

  return kEtcPalErrInvalid;
}

etcpal_error_t etcpal_poll_context_init(EtcPalPollContext* context)
{
  if (!context)
    return kEtcPalErrInvalid;

  init_context_socket_array(context);
  context->max_fd = -1;
  ETCPAL_FD_ZERO(&context->readfds);
  ETCPAL_FD_ZERO(&context->writefds);
  ETCPAL_FD_ZERO(&context->exceptfds);
  context->valid = true;

  return kEtcPalErrOk;
}

void etcpal_poll_context_deinit(EtcPalPollContext* context)
{
  if (!context)
    return;

  context->valid = false;
}

etcpal_error_t etcpal_poll_add_socket(EtcPalPollContext*   context,
                                      etcpal_socket_t      socket,
                                      etcpal_poll_events_t events,
                                      void*                user_data)
{
  if (!context || !context->valid || socket == ETCPAL_SOCKET_INVALID || !(events & ETCPAL_POLL_VALID_INPUT_EVENT_MASK))
    return kEtcPalErrInvalid;

  if (context->num_valid_sockets >= ETCPAL_SOCKET_MAX_POLL_SIZE)
    return kEtcPalErrNoMem;

  EtcPalPollSocket* new_sock = find_hole(context);
  if (new_sock)
  {
    new_sock->sock      = socket;
    new_sock->events    = events;
    new_sock->user_data = user_data;
    set_in_fd_sets(context, new_sock);
    ++context->num_valid_sockets;
    if (socket > context->max_fd)
      context->max_fd = socket;

    return kEtcPalErrOk;
  }

  return kEtcPalErrNoMem;
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

  EtcPalPollSocket* sock_desc = find_socket(context, socket);
  if (sock_desc)
  {
    clear_in_fd_sets(context, sock_desc);
    sock_desc->events    = new_events;
    sock_desc->user_data = new_user_data;
    set_in_fd_sets(context, sock_desc);
    return kEtcPalErrOk;
  }

  return kEtcPalErrNotFound;
}

void etcpal_poll_remove_socket(EtcPalPollContext* context, etcpal_socket_t socket)
{
  if (!context || !context->valid || socket == ETCPAL_SOCKET_INVALID)
    return;

  EtcPalPollSocket* sock_desc = find_socket(context, socket);
  if (sock_desc)
  {
    clear_in_fd_sets(context, sock_desc);
    sock_desc->sock = ETCPAL_SOCKET_INVALID;
    --context->num_valid_sockets;
    if (socket == context->max_fd)
    {
      while (context->max_fd > 0 && (!ETCPAL_FD_ISSET(context->max_fd, &context->readfds) &&
                                     !ETCPAL_FD_ISSET(context->max_fd, &context->writefds) &&
                                     !ETCPAL_FD_ISSET(context->max_fd, &context->exceptfds)))
      {
        --context->max_fd;
      }
    }
  }
}

etcpal_error_t etcpal_poll_wait(EtcPalPollContext* context, EtcPalPollEvent* event, int timeout_ms)
{
  if (!context || !context->valid || !event)
    return kEtcPalErrInvalid;

  if (context->readfds.count == 0 && context->writefds.count == 0 && context->exceptfds.count == 0)
  {
    // No valid sockets are currently added to the context.
    return kEtcPalErrNoSockets;
  }

  fd_set readfds   = context->readfds.set;
  fd_set writefds  = context->writefds.set;
  fd_set exceptfds = context->exceptfds.set;

  struct timeval os_timeout = {0};
  if (timeout_ms != ETCPAL_WAIT_FOREVER)
    ms_to_timeval(timeout_ms, &os_timeout);

  int sel_res = lwip_select(context->max_fd + 1, context->readfds.count ? &readfds : NULL,
                            context->writefds.count ? &writefds : NULL, context->exceptfds.count ? &exceptfds : NULL,
                            timeout_ms == ETCPAL_WAIT_FOREVER ? NULL : &os_timeout);

  if (sel_res < 0)
    return errno_lwip_to_etcpal(errno);
  else if (sel_res == 0)
    return kEtcPalErrTimedOut;

  return handle_select_result(context, event, &readfds, &writefds, &exceptfds);
}

etcpal_error_t handle_select_result(EtcPalPollContext* context,
                                    EtcPalPollEvent*   event,
                                    const fd_set*      readfds,
                                    const fd_set*      writefds,
                                    const fd_set*      exceptfds)
{
  if (!ETCPAL_ASSERT_VERIFY(context) || !ETCPAL_ASSERT_VERIFY(event) || !ETCPAL_ASSERT_VERIFY(readfds) ||
      !ETCPAL_ASSERT_VERIFY(writefds) || !ETCPAL_ASSERT_VERIFY(exceptfds))
  {
    return kEtcPalErrSys;
  }

  // Init the event data.
  event->socket = ETCPAL_SOCKET_INVALID;
  event->events = 0;
  event->err    = kEtcPalErrOk;

  // The default return; if we don't find any sockets set that we passed to select(), something has
  // gone wrong.
  etcpal_error_t res = kEtcPalErrSys;

  for (EtcPalPollSocket* sock_desc = context->sockets; sock_desc < context->sockets + ETCPAL_SOCKET_MAX_POLL_SIZE;
       ++sock_desc)
  {
    if (sock_desc->sock == ETCPAL_SOCKET_INVALID)
      continue;

    if (FD_ISSET(sock_desc->sock, readfds) || FD_ISSET(sock_desc->sock, writefds) ||
        FD_ISSET(sock_desc->sock, exceptfds))
    {
      res              = kEtcPalErrOk;
      event->socket    = sock_desc->sock;
      event->user_data = sock_desc->user_data;

      if (FD_ISSET(sock_desc->sock, readfds))
      {
        if (sock_desc->events & ETCPAL_POLL_IN)
          event->events |= ETCPAL_POLL_IN;
      }
      if (FD_ISSET(sock_desc->sock, writefds))
      {
        if (sock_desc->events & ETCPAL_POLL_CONNECT)
          event->events |= ETCPAL_POLL_CONNECT;
        else if (sock_desc->events & ETCPAL_POLL_OUT)
          event->events |= ETCPAL_POLL_OUT;
      }
      if (FD_ISSET(sock_desc->sock, exceptfds))
      {
        event->events |= ETCPAL_POLL_ERR;
        int       err;
        socklen_t err_size = (socklen_t)sizeof err;
        if (lwip_getsockopt(sock_desc->sock, SOL_SOCKET, SO_ERROR, &err, &err_size) == 0)
          event->err = errno_lwip_to_etcpal(err);
        else
          event->err = kEtcPalErrSys;
      }

      break;  // We handle one event at a time.
    }
  }

  return res;
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
    else  // TODO: IPv6 once lwIP supports it
    {
      get_next_cmsg = true;
    }
  }

  cmsg->valid = !get_next_cmsg;

  return cmsg->valid;
}

void init_context_socket_array(EtcPalPollContext* context)
{
  if (!ETCPAL_ASSERT_VERIFY(context))
    return;

  context->num_valid_sockets = 0;
  for (EtcPalPollSocket* poll_sock = context->sockets; poll_sock < context->sockets + ETCPAL_SOCKET_MAX_POLL_SIZE;
       ++poll_sock)
  {
    poll_sock->sock = ETCPAL_SOCKET_INVALID;
  }
}

EtcPalPollSocket* find_socket(EtcPalPollContext* context, etcpal_socket_t sock)
{
  if (!ETCPAL_ASSERT_VERIFY(context))
    return NULL;

  for (EtcPalPollSocket* poll_sock = context->sockets; poll_sock < context->sockets + ETCPAL_SOCKET_MAX_POLL_SIZE;
       ++poll_sock)
  {
    if (poll_sock->sock == sock)
      return poll_sock;
  }

  return NULL;
}

EtcPalPollSocket* find_hole(EtcPalPollContext* context)
{
  if (!ETCPAL_ASSERT_VERIFY(context))
    return NULL;

  return find_socket(context, ETCPAL_SOCKET_INVALID);
}

void set_in_fd_sets(EtcPalPollContext* context, const EtcPalPollSocket* poll_sock)
{
  if (!ETCPAL_ASSERT_VERIFY(context) || !ETCPAL_ASSERT_VERIFY(poll_sock))
    return;

  if (poll_sock->events & ETCPAL_POLL_IN)
  {
    ETCPAL_FD_SET(poll_sock->sock, &context->readfds);
  }
  if (poll_sock->events & (ETCPAL_POLL_OUT | ETCPAL_POLL_CONNECT))
  {
    ETCPAL_FD_SET(poll_sock->sock, &context->writefds);
  }

  // exceptfds is used for errors on this platform, so set it regardless
  ETCPAL_FD_SET(poll_sock->sock, &context->exceptfds);
}

void clear_in_fd_sets(EtcPalPollContext* context, const EtcPalPollSocket* poll_sock)
{
  if (!ETCPAL_ASSERT_VERIFY(context) || !ETCPAL_ASSERT_VERIFY(poll_sock))
    return;

  if (poll_sock->events & ETCPAL_POLL_IN)
  {
    ETCPAL_FD_CLEAR(poll_sock->sock, &context->readfds);
  }
  if (poll_sock->events & (ETCPAL_POLL_OUT | ETCPAL_POLL_CONNECT))
  {
    ETCPAL_FD_CLEAR(poll_sock->sock, &context->writefds);
  }

  ETCPAL_FD_CLEAR(poll_sock->sock, &context->exceptfds);
}

etcpal_error_t etcpal_getaddrinfo(const char*           hostname,
                                  const char*           service,
                                  const EtcPalAddrinfo* hints,
                                  EtcPalAddrinfo*       result)
{
#if LWIP_DNS
  int              res      = 0;
  struct addrinfo* pf_res   = NULL;
  struct addrinfo  pf_hints = {0};

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
  else
  {
    pf_hints.ai_family = AF_UNSPEC;
  }

  res = lwip_getaddrinfo(hostname, service, hints ? &pf_hints : NULL, &pf_res);
  if (res == 0)
  {
    result->pd[0] = pf_res;
    result->pd[1] = pf_res;
    if (!etcpal_nextaddr(result))
      res = -1;
  }
  return (res == 0 ? kEtcPalErrOk : err_gai_to_etcpal(res));
#else   // LWIP_DNS
  return kEtcPalErrNotImpl;
#endif  // LWIP_DNS
}

bool etcpal_nextaddr(EtcPalAddrinfo* ai)
{
#if LWIP_DNS
  if (ai && ai->pd[1])
  {
    struct addrinfo* pf_ai = (struct addrinfo*)ai->pd[1];
    ai->ai_flags           = 0;
    if (!sockaddr_os_to_etcpal(pf_ai->ai_addr, &ai->ai_addr))
      return false;

    /* Can't use reverse maps, because we have no guarantee of the numeric
     * values of the os constants. Ugh. */
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
#endif  // LWIP_DNS

  return false;
}

void etcpal_freeaddrinfo(EtcPalAddrinfo* ai)
{
#if LWIP_DNS
  if (ai)
  {
    lwip_freeaddrinfo((struct addrinfo*)ai->pd[0]);
    ai->pd[0] = NULL;
    ai->pd[1] = NULL;
  }
#endif  // LWIP_DNS
}

#endif  // !defined(ETCPAL_BUILDING_MOCK_LIB)
