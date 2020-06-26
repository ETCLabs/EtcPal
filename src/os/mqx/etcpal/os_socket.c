/******************************************************************************
 * Copyright 2020 ETC Inc.
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
#include <mqx.h>
#include <bsp.h>
#include <rtcs.h>

#include "etcpal/private/socket.h"

/***************************** Private macros ********************************/

#define ETCPAL_FD_ZERO(setptr)  \
  RTCS_FD_ZERO(&(setptr)->set); \
  (setptr)->count = 0

#define ETCPAL_FD_SET(sock, setptr)  \
  RTCS_FD_SET(sock, &(setptr)->set); \
  (setptr)->count++

#define ETCPAL_FD_CLEAR(sock, setptr) \
  RTCS_FD_CLR(sock, &(setptr)->set);  \
  (setptr)->count--

#define ETCPAL_FD_ISSET(sock, setptr) RTCS_FD_ISSET(sock, &(setptr)->set)

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
static const uint32_t sfmap[ETCPAL_NUM_AF] =
{
  AF_UNSPEC,
  PF_INET,
  PF_INET6
};

#define ETCPAL_NUM_TYPE   2
static const uint32_t stmap[ETCPAL_NUM_TYPE] =
{
  SOCK_STREAM,
  SOCK_DGRAM
};

#define ETCPAL_NUM_AIF    8
static const uint32_t aiflagmap[ETCPAL_NUM_AIF] =
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

static const uint16_t aifammap[ETCPAL_NUM_AF] =
{
  AF_UNSPEC,
  AF_INET,
  AF_INET6
};

#define ETCPAL_NUM_IPPROTO    6
static const uint16_t aiprotmap[ETCPAL_NUM_IPPROTO] =
{
  0,
  0,
  0,
  0,
  IPPROTO_TCP,
  IPPROTO_UDP
};

/* clang-format on */

/*********************** Private function prototypes *************************/

// Helper functions for etcpal_poll API
static void                 init_context_socket_array(EtcPalPollContext* context);
static EtcPalPollCtxSocket* find_socket(EtcPalPollContext* context, etcpal_socket_t socket);
static EtcPalPollCtxSocket* find_hole(EtcPalPollContext* context);
static void                 set_in_fd_sets(EtcPalPollContext* context, const EtcPalPollCtxSocket* sock);
static void                 clear_in_fd_sets(EtcPalPollContext* context, const EtcPalPollCtxSocket* sock);
static etcpal_error_t       handle_select_result(EtcPalPollContext* context,
                                                 EtcPalPollEvent*   event,
                                                 etcpal_error_t     socket_error,
                                                 const rtcs_fd_set* readfds,
                                                 const rtcs_fd_set* writefds);

// Helper functions for etcpal_setsockopt()
static int32_t join_leave_mcast_group_ipv4(etcpal_socket_t id, const struct EtcPalMreq* mreq, bool join);

/*************************** Function definitions ****************************/

static etcpal_error_t err_os_to_etcpal(uint32_t rtcserr)
{
  switch (rtcserr)
  {
    case RTCS_OK:
      return kEtcPalErrOk;
    case RTCSERR_OUT_OF_MEMORY:
    case RTCSERR_OUT_OF_BUFFERS:
    case RTCSERR_OUT_OF_SOCKETS:
      return kEtcPalErrNoMem;
    case RTCSERR_TIMEOUT:
      return kEtcPalErrTimedOut;
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
      return kEtcPalErrInvalid;
    case RTCSERR_FEATURE_NOT_ENABLED:
    case RTCSERR_SOCK_NOT_SUPPORTED:
      return kEtcPalErrNotImpl;
    case RTCSERR_SOCK_NOT_CONNECTED:
      return kEtcPalErrNotConn;
    case RTCSERR_SOCK_IS_BOUND:
    case RTCSERR_SOCK_IS_LISTENING:
      return kEtcPalErrAlready;
    case RTCSERR_SOCK_IS_CONNECTED:
      return kEtcPalErrIsConn;
    case RTCSERR_SOCK_ESHUTDOWN:
      return kEtcPalErrShutdown;
    default:
      return kEtcPalErrSys;
  }
}

etcpal_error_t etcpal_socket_init(void)
{
  // No initialization is necessary on this platform.
  return kEtcPalErrOk;
}

void etcpal_socket_deinit(void)
{
  // No deinitialization is necessary on this platform.
}

etcpal_error_t etcpal_accept(etcpal_socket_t id, EtcPalSockAddr* address, etcpal_socket_t* conn_sock)
{
  /* TODO */
  return kEtcPalErrNotImpl;
}

etcpal_error_t etcpal_bind(etcpal_socket_t id, const EtcPalSockAddr* address)
{
  struct sockaddr ss;
  size_t          sa_size;

  if (!address)
    return kEtcPalErrInvalid;

  sa_size = sockaddr_etcpal_to_os(address, &ss);
  if (sa_size == 0)
    return kEtcPalErrInvalid;

  return err_os_to_etcpal(bind(id, &ss, (uint16_t)sa_size));
}

etcpal_error_t etcpal_close(etcpal_socket_t id)
{
  return err_os_to_etcpal((uint32_t)closesocket(id));
}

etcpal_error_t etcpal_connect(etcpal_socket_t id, const EtcPalSockAddr* address)
{
  /* TODO */
  return kEtcPalErrNotImpl;
}

etcpal_error_t etcpal_getpeername(etcpal_socket_t id, EtcPalSockAddr* address)
{
  /* TODO */
  return kEtcPalErrNotImpl;
}

etcpal_error_t etcpal_getsockname(etcpal_socket_t id, EtcPalSockAddr* address)
{
  struct sockaddr ss;
  uint16_t        size = sizeof ss;
  uint32_t        res;

  if (!address)
    return kEtcPalErrInvalid;

  res = getsockname(id, &ss, &size);
  if (res == RTCS_OK)
  {
    if (!sockaddr_os_to_etcpal(&ss, address))
      return kEtcPalErrSys;
  }
  return err_os_to_etcpal(res);
}

etcpal_error_t etcpal_getsockopt(etcpal_socket_t id, int level, int option_name, void* option_value, size_t* option_len)
{
  /* TODO */
  return kEtcPalErrNotImpl;
}

etcpal_error_t etcpal_listen(etcpal_socket_t id, int backlog)
{
  /* TODO */
  return kEtcPalErrNotImpl;
}

int etcpal_recv(etcpal_socket_t id, void* buffer, size_t length, int flags)
{
  /* TODO */
  return kEtcPalErrNotImpl;
}

int etcpal_recvfrom(etcpal_socket_t id, void* buffer, size_t length, int flags, EtcPalSockAddr* address)
{
  int32_t         res;
  uint32_t        impl_flags = (flags & ETCPAL_MSG_PEEK) ? RTCS_MSG_PEEK : 0;
  struct sockaddr fromaddr;
  uint16_t        fromlen = sizeof fromaddr;

  if (!buffer)
    return kEtcPalErrInvalid;

  res = recvfrom(id, buffer, length, impl_flags, &fromaddr, &fromlen);

  if (res != RTCS_ERROR)
  {
    if (address && fromlen > 0)
    {
      if (!sockaddr_os_to_etcpal(&fromaddr, address))
        return kEtcPalErrInvalid;
    }
  }
  return (res == RTCS_ERROR ? err_os_to_etcpal(RTCS_geterror(id)) : res);
}

int etcpal_send(etcpal_socket_t id, const void* message, size_t length, int flags)
{
  /* TODO */
  return kEtcPalErrNotImpl;
}

int etcpal_sendto(etcpal_socket_t id, const void* message, size_t length, int flags, const EtcPalSockAddr* dest_addr)
{
  int32_t         res;
  size_t          ss_size;
  struct sockaddr ss;

  if (!dest_addr || !message)
    return kEtcPalErrInvalid;

  if ((ss_size = sockaddr_etcpal_to_os(dest_addr, &ss)) == 0)
    return kEtcPalErrInvalid;

  res = sendto(id, (char*)message, (uint32_t)length, 0, &ss, (uint16_t)ss_size);
  return (res == RTCS_ERROR ? err_os_to_etcpal(RTCS_geterror(id)) : res);
}

etcpal_error_t etcpal_setsockopt(etcpal_socket_t id,
                                 int             level,
                                 int             option_name,
                                 const void*     option_value,
                                 size_t          option_len)
{
  int32_t res = RTCSERR_SOCK_INVALID_OPTION;

  if (!option_value)
    return kEtcPalErrInvalid;

  switch (option_name)
  {
    case ETCPAL_SO_RCVBUF:
      if (level == ETCPAL_SOL_SOCKET && option_len >= sizeof(uint32_t))
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
    case ETCPAL_SO_SNDBUF:
      if (level == ETCPAL_SOL_SOCKET && option_len >= sizeof(uint32_t))
      {
        uint32_t val = (uint32_t)(*(int*)option_value);
        res = setsockopt(id, SOL_TCP, OPT_TBSIZE, &val, sizeof val);
      }
      break;
    case ETCPAL_SO_RCVTIMEO:
      if (level == ETCPAL_SOL_SOCKET && option_len >= sizeof(uint32_t))
      {
        uint32_t val = (uint32_t)(*(int*)option_value);
        res = setsockopt(id, SOL_SOCKET, SO_RCVTIMEO, &val, sizeof val);
      }
      break;
    case ETCPAL_SO_SNDTIMEO:
      if (level == ETCPAL_SOL_SOCKET && option_len >= sizeof(uint32_t))
      {
        uint32_t val = (uint32_t)(*(int*)option_value);
        res = setsockopt(id, SOL_SOCKET, SO_SNDTIMEO, &val, sizeof val);
      }
      break;
    case ETCPAL_IP_TTL:
      if (level == ETCPAL_IPPROTO_IP && option_len >= sizeof(unsigned char))
      {
        unsigned char val = (unsigned char)(*(int*)option_value);
        res = setsockopt(id, SOL_IP, RTCS_SO_IP_TX_TTL, &val, sizeof val);
      }
      break;
    case ETCPAL_MCAST_JOIN_GROUP:
    case ETCPAL_MCAST_LEAVE_GROUP:
      if (option_len == sizeof(EtcPalGroupReq))
      {
        EtcPalGroupReq* greq = (EtcPalGroupReq*)option_value;
        if (ETCPAL_IP_IS_V4(&greq->group) && level == ETCPAL_IPPROTO_IP)
        {
          struct EtcPalMreq mreq;
          bool              keep_going = true;

          if (greq->ifindex != 0)
          {
            IPCFG_IP_ADDRESS_DATA ip_data;
            if (ipcfg_get_ip(greq->ifindex - 1, &ip_data))
              ETCPAL_IP_SET_V4_ADDRESS(&mreq.netint, ip_data.ip);
            else
              keep_going = false;
          }
          else
          {
            etcpal_ip_set_wildcard(kEtcPalIpTypeV4, &mreq.netint);
          }

          if (keep_going)
          {
            mreq.group = greq->group;
            res = join_leave_mcast_group_ipv4(id, &mreq, option_name == ETCPAL_MCAST_JOIN_GROUP ? true : false);
          }
        }
        else if (ETCPAL_IP_IS_V6(&greq->group) && level == ETCPAL_IPPROTO_IPV6)
        {
          struct ipv6_mreq val;
          bool             keep_going = true;

          if (greq->ifindex != 0)
          {
            uint32_t scope_id = ipcfg6_get_scope_id(greq->ifindex - 1);
            if (scope_id != 0)
              val.ipv6imr_interface = scope_id;
            else
              keep_going = false;
          }
          else
          {
            val.ipv6imr_interface = 0;
          }

          if (keep_going)
          {
            memcpy(&val.ipv6imr_multiaddr.s6_addr, ETCPAL_IP_V6_ADDRESS(&greq->group), ETCPAL_IPV6_BYTES);
            res = setsockopt(id, SOL_IP6,
                             option_name == ETCPAL_MCAST_JOIN_GROUP ? RTCS_SO_IP6_JOIN_GROUP : RTCS_SO_IP6_LEAVE_GROUP,
                             &val, sizeof val);
          }
        }
      }
      break;
    case ETCPAL_IP_ADD_MEMBERSHIP:
      if (option_len == sizeof(EtcPalMreq) && level == ETCPAL_IPPROTO_IP)
      {
        res = join_leave_mcast_group_ipv4(id, (EtcPalMreq*)option_value, true);
      }
      break;
    case ETCPAL_IP_DROP_MEMBERSHIP:
      if (option_len == sizeof(EtcPalMreq) && level == ETCPAL_IPPROTO_IP)
      {
        res = join_leave_mcast_group_ipv4(id, (EtcPalMreq*)option_value, false);
      }
      break;
    case ETCPAL_SO_REUSEADDR:
    case ETCPAL_SO_REUSEPORT:
      /* Option not supported, but seems to be allowed by default */
      res = RTCS_OK;
      break;
    case ETCPAL_IP_MULTICAST_IF:
      /* This one is a bit tricky. The stack doesn't actually support this functionality, but for
       * systems with only one network interface we want to pretend that it does if called
       * correctly. This makes the behavior for os-neutral calling code more consistent. */
#if BSP_ENET_DEVICE_COUNT == 1
      if (option_len == sizeof(EtcPalIpAddr))
      {
        EtcPalIpAddr*         netint_requested = (EtcPalIpAddr*)option_value;
        IPCFG_IP_ADDRESS_DATA ip_data;

        if (ipcfg_get_ip(BSP_DEFAULT_ENET_DEVICE, &ip_data))
        {
          EtcPalIpAddr default_netint_ip;

          ETCPAL_IP_SET_V4_ADDRESS(&default_netint_ip, ip_data.ip);
          if (etcpal_ip_cmp(netint_requested, &default_netint_ip) == 0)
            res = kEtcPalErrOk;
        }
      }
#endif
      break;
    case ETCPAL_SO_BROADCAST:      /* Not supported */
    case ETCPAL_SO_ERROR:          /* Set not supported */
    case ETCPAL_SO_KEEPALIVE:      /* TODO */
    case ETCPAL_SO_LINGER:         /* TODO */
    case ETCPAL_SO_TYPE:           /* Not supported */
    case ETCPAL_IP_MULTICAST_TTL:  /* Not supported */
    case ETCPAL_IP_MULTICAST_LOOP: /* TODO */
    default:
      break;
  }

  return err_os_to_etcpal((uint32_t)res);
}

int32_t join_leave_mcast_group_ipv4(etcpal_socket_t id, const struct EtcPalMreq* mreq, bool join)
{
  struct ip_mreq os_mreq;
  os_mreq.imr_interface.s_addr = ETCPAL_IP_V4_ADDRESS(&mreq->netint);
  os_mreq.imr_multiaddr.s_addr = ETCPAL_IP_V4_ADDRESS(&mreq->group);
  int32_t res = setsockopt(id, SOL_IGMP, join ? RTCS_SO_IGMP_ADD_MEMBERSHIP : RTCS_SO_IGMP_DROP_MEMBERSHIP, &os_mreq,
                           sizeof os_mreq);
  if (res == RTCS_OK)
  {
    /* Voodoo to allow locally-sent multicast traffic to be received by local sockets. We
     * don't check the result. */
    os_mreq.imr_interface.s_addr = 0x7f000001;
    setsockopt(id, SOL_IGMP, join ? RTCS_SO_IGMP_ADD_MEMBERSHIP : RTCS_SO_IGMP_DROP_MEMBERSHIP, &os_mreq,
               sizeof os_mreq);
  }
  return res;
}

etcpal_error_t etcpal_shutdown(etcpal_socket_t id, int how)
{
  if (how >= 0 && how < ETCPAL_NUM_SHUT)
  {
    return err_os_to_etcpal((uint32_t)shutdownsocket(id, (int32_t)shutmap[how]));
  }
  return kEtcPalErrInvalid;
}

etcpal_error_t etcpal_socket(unsigned int family, unsigned int type, etcpal_socket_t* id)
{
  if (id)
  {
    if (family < ETCPAL_NUM_AF && type < ETCPAL_NUM_TYPE)
    {
      uint32_t sock = socket(sfmap[family], stmap[type], 0);
      if (sock != RTCS_SOCKET_ERROR)
      {
        *id = sock;
        return kEtcPalErrOk;
      }
      else
      {
        *id = ETCPAL_SOCKET_INVALID;
        /* RTCS does not provide error codes on socket failure */
        return kEtcPalErrSys;
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
  uint32_t  sock_type;
  socklen_t uint32_size = sizeof(uint32_t);
  if (RTCS_OK == getsockopt(id, SOL_SOCKET, OPT_SOCKET_TYPE, &sock_type, &uint32_size))
  {
    int32_t  res;
    uint32_t opt_value = (blocking ? FALSE : TRUE);
    if (sock_type == SOCK_STREAM)
    {
      res = setsockopt(id, SOL_TCP, OPT_RECEIVE_NOWAIT, &opt_value, uint32_size);
      if (res == RTCS_OK)
        res = setsockopt(id, SOL_TCP, OPT_SEND_NOWAIT, &opt_value, uint32_size);
      return err_os_to_etcpal((uint32_t)res);
    }
    else if (sock_type == SOCK_DGRAM)
    {
      res = setsockopt(id, SOL_UDP, OPT_RECEIVE_NOWAIT, &opt_value, uint32_size);
      if (res == RTCS_OK)
        res = setsockopt(id, SOL_UDP, OPT_SEND_NOWAIT, &opt_value, uint32_size);
      return err_os_to_etcpal((uint32_t)res);
    }
    else
    {
      return kEtcPalErrInvalid;
    }
  }
  return kEtcPalErrSys;
}

etcpal_error_t etcpal_getblocking(etcpal_socket_t id, bool* blocking)
{
  if (blocking)
  {
    uint32_t  sock_type;
    socklen_t uint32_size = sizeof(uint32_t);
    if (RTCS_OK == getsockopt(id, SOL_SOCKET, OPT_SOCKET_TYPE, &sock_type, &uint32_size))
    {
      if (sock_type == SOCK_STREAM)
      {
        uint32_t rcv_nowait;
        uint32_t send_nowait;
        int32_t  res = getsockopt(id, SOL_TCP, OPT_RECEIVE_NOWAIT, &rcv_nowait, &uint32_size);
        if (res == RTCS_OK)
          res = getsockopt(id, SOL_TCP, OPT_SEND_NOWAIT, &send_nowait, &uint32_size);
        if (res == RTCS_OK)
          *blocking = !(rcv_nowait && send_nowait);
        return err_os_to_etcpal((uint32_t)res);
      }
      else if (sock_type == SOCK_DGRAM)
      {
        uint32_t rcv_nowait;
        uint32_t send_nowait;
        int32_t  res = getsockopt(id, SOL_UDP, OPT_RECEIVE_NOWAIT, &rcv_nowait, &uint32_size);
        if (res == RTCS_OK)
          res = getsockopt(id, SOL_UDP, OPT_SEND_NOWAIT, &send_nowait, &uint32_size);
        if (res == RTCS_OK)
          *blocking = !(rcv_nowait && send_nowait);
        return err_os_to_etcpal((uint32_t)res);
      }
      else
      {
        return kEtcPalErrInvalid;
      }
    }
    else
    {
      return kEtcPalErrSys;
    }
  }
  return kEtcPalErrInvalid;
}

etcpal_error_t etcpal_poll_context_init(EtcPalPollContext* context)
{
  if (!context)
    return kEtcPalErrInvalid;

  init_context_socket_array(context);
  ETCPAL_FD_ZERO(&context->readfds);
  ETCPAL_FD_ZERO(&context->writefds);
  context->valid = true;
  return kEtcPalErrOk;
}

void etcpal_poll_context_deinit(EtcPalPollContext* context)
{
  if (!context || !context->valid)
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
  {
    return kEtcPalErrNoMem;
  }
  else
  {
    EtcPalPollCtxSocket* new_sock = find_hole(context);
    if (new_sock)
    {
      new_sock->socket = socket;
      new_sock->events = events;
      new_sock->user_data = user_data;
      set_in_fd_sets(context, new_sock);
      context->num_valid_sockets++;
      return kEtcPalErrOk;
    }
    return kEtcPalErrNoMem;
  }
}

etcpal_error_t etcpal_poll_modify_socket(EtcPalPollContext*   context,
                                         etcpal_socket_t      socket,
                                         etcpal_poll_events_t new_events,
                                         void*                new_user_data)
{
  if (!context || !context->valid || socket == ETCPAL_SOCKET_INVALID ||
      !(new_events & ETCPAL_POLL_VALID_INPUT_EVENT_MASK))
    return kEtcPalErrInvalid;

  EtcPalPollCtxSocket* sock_desc = find_socket(context, socket);
  if (sock_desc)
  {
    clear_in_fd_sets(context, sock_desc);
    sock_desc->events = new_events;
    sock_desc->user_data = new_user_data;
    set_in_fd_sets(context, sock_desc);
    return kEtcPalErrOk;
  }
  else
  {
    return kEtcPalErrNotFound;
  }
}

void etcpal_poll_remove_socket(EtcPalPollContext* context, etcpal_socket_t socket)
{
  if (!context || !context->valid || socket == ETCPAL_SOCKET_INVALID)
    return;

  EtcPalPollCtxSocket* sock_desc = find_socket(context, socket);
  if (sock_desc)
  {
    clear_in_fd_sets(context, sock_desc);
    sock_desc->socket = ETCPAL_SOCKET_INVALID;
    context->num_valid_sockets--;
  }
}

etcpal_error_t etcpal_poll_wait(EtcPalPollContext* context, EtcPalPollEvent* event, int timeout_ms)
{
  if (!context || !context->valid || !event)
    return kEtcPalErrInvalid;

  if (context->readfds.count == 0 && context->writefds.count == 0)
  {
    // No valid sockets are currently added to the context.
    return kEtcPalErrNoSockets;
  }

  rtcs_fd_set readfds = context->readfds.set;
  rtcs_fd_set writefds = context->writefds.set;

  uint32_t os_timeout;
  if (timeout_ms == ETCPAL_WAIT_FOREVER)
    os_timeout = 0;
  else if (timeout_ms == 0)
    os_timeout = 0xffffffff;
  else
    os_timeout = (uint32_t)timeout_ms;

  int32_t nfds =
      (int32_t)((context->readfds.count > context->writefds.count) ? context->readfds.count : context->writefds.count);
  int32_t sel_res = select(nfds, context->readfds.count ? &readfds : NULL, context->writefds.count ? &writefds : NULL,
                           NULL, os_timeout);

  if (sel_res == RTCS_ERROR)
  {
    // RTCS handles some socket errors by returning them from select().
    uint32_t rtcs_err = RTCS_get_errno();
    if (rtcs_err == RTCSERR_SOCK_ESHUTDOWN)
      return handle_select_result(context, event, kEtcPalErrConnClosed, &readfds, &writefds);
    else if (rtcs_err == RTCSERR_SOCK_CLOSED)
      return handle_select_result(context, event, kEtcPalErrNotFound, &readfds, &writefds);
    else
      return err_os_to_etcpal(rtcs_err);
  }
  else if (sel_res == 0)
  {
    return kEtcPalErrTimedOut;
  }
  else
  {
    return handle_select_result(context, event, kEtcPalErrOk, &readfds, &writefds);
  }
}

etcpal_error_t handle_select_result(EtcPalPollContext* context,
                                    EtcPalPollEvent*   event,
                                    etcpal_error_t     socket_error,
                                    const rtcs_fd_set* readfds,
                                    const rtcs_fd_set* writefds)
{
  // Init the event data.
  event->socket = ETCPAL_SOCKET_INVALID;
  event->events = 0;
  event->err = kEtcPalErrOk;

  // The default return; if we don't find any sockets set that we passed to select(), something has
  // gone wrong.
  etcpal_error_t res = kEtcPalErrSys;

  for (EtcPalPollCtxSocket* sock_desc = context->sockets; sock_desc < context->sockets + ETCPAL_SOCKET_MAX_POLL_SIZE;
       ++sock_desc)
  {
    if (sock_desc->socket == ETCPAL_SOCKET_INVALID)
      continue;

    if (RTCS_FD_ISSET(sock_desc->socket, readfds) || RTCS_FD_ISSET(sock_desc->socket, writefds))
    {
      res = kEtcPalErrOk;
      event->socket = sock_desc->socket;
      event->user_data = sock_desc->user_data;

      /* Check for errors */
      if (socket_error != kEtcPalErrOk)
      {
        event->events |= ETCPAL_POLL_ERR;
        event->err = socket_error;
      }

      if (RTCS_FD_ISSET(sock_desc->socket, readfds))
      {
        if (sock_desc->events & ETCPAL_POLL_IN)
          event->events |= ETCPAL_POLL_IN;
      }
      if (RTCS_FD_ISSET(sock_desc->socket, writefds))
      {
        if (sock_desc->events & ETCPAL_POLL_CONNECT)
          event->events |= ETCPAL_POLL_CONNECT;
        else if (sock_desc->events & ETCPAL_POLL_OUT)
          event->events |= ETCPAL_POLL_OUT;
      }
      // ETCPAL_POLL_OOB/exceptfds is not handled properly on this OS

      break;  // We handle one event at a time.
    }
  }
  return res;
}

void init_context_socket_array(EtcPalPollContext* context)
{
  context->num_valid_sockets = 0;
  for (EtcPalPollCtxSocket* ctx_socket = context->sockets; ctx_socket < context->sockets + ETCPAL_SOCKET_MAX_POLL_SIZE;
       ++ctx_socket)
  {
    ctx_socket->socket = ETCPAL_SOCKET_INVALID;
  }
}

EtcPalPollCtxSocket* find_socket(EtcPalPollContext* context, etcpal_socket_t socket)
{
  for (EtcPalPollCtxSocket* cur = context->sockets; cur < context->sockets + ETCPAL_SOCKET_MAX_POLL_SIZE; ++cur)
  {
    if (cur->socket == socket)
      return cur;
  }
  return NULL;
}

EtcPalPollCtxSocket* find_hole(EtcPalPollContext* context)
{
  return find_socket(context, ETCPAL_SOCKET_INVALID);
}

void set_in_fd_sets(EtcPalPollContext* context, const EtcPalPollCtxSocket* sock)
{
  if (sock->events & ETCPAL_POLL_IN)
  {
    ETCPAL_FD_SET(sock->socket, &context->readfds);
  }
  if (sock->events & (ETCPAL_POLL_OUT | ETCPAL_POLL_CONNECT))
  {
    ETCPAL_FD_SET(sock->socket, &context->writefds);
  }
}

void clear_in_fd_sets(EtcPalPollContext* context, const EtcPalPollCtxSocket* sock)
{
  if (sock->events & ETCPAL_POLL_IN)
  {
    ETCPAL_FD_CLEAR(sock->socket, &context->readfds);
  }
  if (sock->events & (ETCPAL_POLL_OUT | ETCPAL_POLL_CONNECT))
  {
    ETCPAL_FD_CLEAR(sock->socket, &context->writefds);
  }
}

etcpal_error_t etcpal_getaddrinfo(const char*           hostname,
                                  const char*           service,
                                  const EtcPalAddrinfo* hints,
                                  EtcPalAddrinfo*       result)
{
  int32_t          res;
  struct addrinfo* pf_res;
  struct addrinfo  pf_hints;

  if ((!hostname && !service) || !result)
    return kEtcPalErrInvalid;

  memset(&pf_hints, 0, sizeof pf_hints);
  if (hints)
  {
    pf_hints.ai_flags = (uint16_t)((hints->ai_flags < ETCPAL_NUM_AIF) ? aiflagmap[hints->ai_flags] : 0);
    pf_hints.ai_family = (uint16_t)((hints->ai_family < ETCPAL_NUM_AF) ? aifammap[hints->ai_family] : AF_UNSPEC);
    pf_hints.ai_socktype = (uint32_t)((hints->ai_socktype < ETCPAL_NUM_TYPE) ? stmap[hints->ai_socktype] : 0);
    pf_hints.ai_protocol = (uint16_t)((hints->ai_protocol < ETCPAL_NUM_IPPROTO) ? aiprotmap[hints->ai_protocol] : 0);
  }

  res = getaddrinfo(hostname, service, hints ? &pf_hints : NULL, &pf_res);
  if (res == 0)
  {
    result->pd[0] = pf_res;
    result->pd[1] = pf_res;
    if (!etcpal_nextaddr(result))
      return kEtcPalErrSys;
  }
  return err_os_to_etcpal((uint32_t)res);
}

bool etcpal_nextaddr(EtcPalAddrinfo* ai)
{
  if (ai && ai->pd[1])
  {
    struct addrinfo* os_ai = (struct addrinfo*)ai->pd[1];
    ai->ai_flags = 0;
    if (!sockaddr_os_to_etcpal(os_ai->ai_addr, &ai->ai_addr))
      return false;
    /* Can't use reverse maps, because we have no guarantee of the numeric values of the OS
     * constants. Ugh. */
    if (os_ai->ai_family == AF_INET)
      ai->ai_family = ETCPAL_AF_INET;
    else if (os_ai->ai_family == AF_INET6)
      ai->ai_family = ETCPAL_AF_INET6;
    else
      ai->ai_family = ETCPAL_AF_UNSPEC;
    if (os_ai->ai_socktype == SOCK_DGRAM)
      ai->ai_socktype = ETCPAL_SOCK_DGRAM;
    else if (os_ai->ai_socktype == SOCK_STREAM)
      ai->ai_socktype = ETCPAL_SOCK_STREAM;
    else
      ai->ai_socktype = 0;
    if (os_ai->ai_protocol == IPPROTO_UDP)
      ai->ai_protocol = ETCPAL_IPPROTO_UDP;
    else if (os_ai->ai_protocol == IPPROTO_TCP)
      ai->ai_protocol = ETCPAL_IPPROTO_TCP;
    else
      ai->ai_protocol = 0;
    ai->ai_canonname = os_ai->ai_canonname;
    ai->pd[1] = os_ai->ai_next;

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
