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
#include <stdlib.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <mswsock.h>
#include <Windows.h>

#include "etcpal/common.h"
#include "etcpal/private/common.h"
#include "etcpal/private/socket.h"
#include "os_error.h"

/*************************** Private constants *******************************/

#define POLL_CONTEXT_ARR_CHUNK_SIZE 10

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

/***************************** Private macros ********************************/

#define ETCPAL_FD_ZERO(setptr)      \
  if (ETCPAL_ASSERT_VERIFY(setptr)) \
  {                                 \
    FD_ZERO(&(setptr)->set);        \
    (setptr)->count = 0;            \
  }

#define ETCPAL_FD_SET(sock, setptr) \
  if (ETCPAL_ASSERT_VERIFY(setptr)) \
  {                                 \
    FD_SET(sock, &(setptr)->set);   \
    ++(setptr)->count;              \
  }

#define ETCPAL_FD_CLEAR(sock, setptr) \
  if (ETCPAL_ASSERT_VERIFY(setptr))   \
  {                                   \
    FD_CLR(sock, &(setptr)->set);     \
    --(setptr)->count;                \
  }

#define ETCPAL_FD_ISSET(sock, setptr) (ETCPAL_ASSERT_VERIFY(setptr) ? FD_ISSET(sock, &(setptr)->set) : false)

/**************************** Private variables ******************************/

#if !defined(ETCPAL_BUILDING_MOCK_LIB)

/* clang-format off */

#define ETCPAL_NUM_SHUT 3
static const int kShutMap[ETCPAL_NUM_SHUT] =
{
  SD_RECEIVE,
  SD_SEND,
  SD_BOTH
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

static LPFN_WSARECVMSG wsa_recvmsg = NULL;

/*********************** Private function prototypes *************************/

// Helpers for etcpal_setsockopt()
static int setsockopt_socket(etcpal_socket_t id, int option_name, const void* option_value, size_t option_len);
static int setsockopt_ip(etcpal_socket_t id, int option_name, const void* option_value, size_t option_len);
static int setsockopt_ip6(etcpal_socket_t id, int option_name, const void* option_value, size_t option_len);

// Helpers for etcpal_getsockopt()
static int getsockopt_socket(etcpal_socket_t id, int option_name, void* option_value, size_t* option_len);

// Helper functions for the etcpal_poll API
static void           set_in_fd_sets(EtcPalPollContext* context, const EtcPalPollSocket* sock);
static void           clear_in_fd_sets(EtcPalPollContext* context, const EtcPalPollSocket* sock);
static etcpal_error_t handle_select_result(EtcPalPollContext*     context,
                                           EtcPalPollEvent*       event,
                                           const EtcPalPollFdSet* readfds,
                                           const EtcPalPollFdSet* writefds,
                                           const EtcPalPollFdSet* exceptfds);

static int           poll_socket_compare(const EtcPalRbTree* tree, const void* value_a, const void* value_b);
static EtcPalRbNode* poll_socket_alloc(void);
static void          poll_socket_free(EtcPalRbNode* node);

static etcpal_error_t init_extension_functions();
static etcpal_error_t get_wsarecvmsg(LPFN_WSARECVMSG* result);

// Helpers for etcpal_recvmsg()
static void  construct_wsamsg(const EtcPalMsgHdr* in_msg,
                              LPSOCKADDR_STORAGE  name_store,
                              LPWSABUF            buf_store,
                              LPWSAMSG            out_msg);
static int   rcvmsg_flags_os_to_etcpal(ULONG os_flags);
static ULONG rcvmsg_flags_etcpal_to_os(int etcpal_flags);
static bool  get_first_compatible_cmsg(LPWSAMSG msg, LPWSACMSGHDR start, EtcPalCMsgHdr* cmsg);

/*************************** Function definitions ****************************/

etcpal_error_t etcpal_socket_init(void)
{
  WSADATA wsdata      = {0};
  WORD    wsver       = MAKEWORD(2, 2);
  int     startup_res = WSAStartup(wsver, &wsdata);
  if (startup_res != 0)
  {
    return err_winsock_to_etcpal(startup_res);
  }

  etcpal_error_t res = init_extension_functions();

  if (res != kEtcPalErrOk)
    WSACleanup();

  return res;
}

void etcpal_socket_deinit(void)
{
  WSACleanup();
}

etcpal_error_t etcpal_accept(etcpal_socket_t id, EtcPalSockAddr* address, etcpal_socket_t* conn_sock)
{
  if ((id == ETCPAL_SOCKET_INVALID) || !conn_sock)
    return kEtcPalErrInvalid;

  struct sockaddr_storage ss      = {0};
  int                     sa_size = sizeof ss;

  SOCKET res = accept(id, (struct sockaddr*)&ss, &sa_size);

  if (res != INVALID_SOCKET)
  {
    if (address && !sockaddr_os_to_etcpal((etcpal_os_sockaddr_t*)&ss, address))
    {
      closesocket(res);
      return kEtcPalErrSys;
    }

    *conn_sock = res;
    return kEtcPalErrOk;
  }

  return err_winsock_to_etcpal(WSAGetLastError());
}

etcpal_error_t etcpal_bind(etcpal_socket_t id, const EtcPalSockAddr* address)
{
  if ((id == ETCPAL_SOCKET_INVALID) || !address)
    return kEtcPalErrInvalid;

  struct sockaddr_storage ss      = {0};
  size_t                  sa_size = 0;

  sa_size = sockaddr_etcpal_to_os(address, (etcpal_os_sockaddr_t*)&ss);
  if (sa_size == 0)
    return kEtcPalErrInvalid;

  int res = bind(id, (struct sockaddr*)&ss, (int)sa_size);
  return (res == 0 ? kEtcPalErrOk : err_winsock_to_etcpal(WSAGetLastError()));
}

etcpal_error_t etcpal_close(etcpal_socket_t id)
{
  if (id == ETCPAL_SOCKET_INVALID)
    return kEtcPalErrInvalid;

  int res = closesocket(id);
  return (res == 0 ? kEtcPalErrOk : err_winsock_to_etcpal(WSAGetLastError()));
}

etcpal_error_t etcpal_connect(etcpal_socket_t id, const EtcPalSockAddr* address)
{
  if ((id == ETCPAL_SOCKET_INVALID) || !address)
    return kEtcPalErrInvalid;

  struct sockaddr_storage ss = {0};

  size_t sa_size = sockaddr_etcpal_to_os(address, (etcpal_os_sockaddr_t*)&ss);
  if (sa_size == 0)
    return kEtcPalErrInvalid;

  int res = connect(id, (struct sockaddr*)&ss, (int)sa_size);
  return (res == 0 ? kEtcPalErrOk : err_winsock_to_etcpal(WSAGetLastError()));
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

  int res = getsockname(id, (struct sockaddr*)&ss, &size);
  if (res == 0)
  {
    if (!sockaddr_os_to_etcpal((etcpal_os_sockaddr_t*)&ss, address))
      return kEtcPalErrSys;

    return kEtcPalErrOk;
  }

  return err_winsock_to_etcpal(WSAGetLastError());
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
      return kEtcPalErrNotImpl; /* TODO */
    case ETCPAL_IPPROTO_IPV6:
      return kEtcPalErrNotImpl; /* TODO */
    default:
      return kEtcPalErrInvalid;
  }

  return (res == 0 ? kEtcPalErrOk : err_winsock_to_etcpal(WSAGetLastError()));
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
    case ETCPAL_SO_SNDBUF:
      return getsockopt(id, SOL_SOCKET, SO_SNDBUF, option_value, (int*)option_len);
    case ETCPAL_SO_RCVBUF:    /* TODO */
    case ETCPAL_SO_RCVTIMEO:  /* TODO */
    case ETCPAL_SO_SNDTIMEO:  /* TODO */
    case ETCPAL_SO_REUSEADDR: /* TODO */
    case ETCPAL_SO_BROADCAST: /* TODO */
    case ETCPAL_SO_KEEPALIVE: /* TODO */
    case ETCPAL_SO_LINGER:    /* TODO */
    case ETCPAL_SO_ERROR:     /* Set not supported */
    case ETCPAL_SO_REUSEPORT: /* Not supported on this OS. */
    case ETCPAL_SO_TYPE:      /* Not supported */
    default:
      break;
  }

  // If we got here, something was invalid. Set errno accordingly
  WSASetLastError(WSAEINVAL);
  return -1;
}

etcpal_error_t etcpal_listen(etcpal_socket_t id, int backlog)
{
  if (id == ETCPAL_SOCKET_INVALID)
    return kEtcPalErrInvalid;

  int res = listen(id, backlog);
  return (res == 0 ? kEtcPalErrOk : err_winsock_to_etcpal(WSAGetLastError()));
}

int etcpal_recv(etcpal_socket_t id, void* buffer, size_t length, int flags)
{
  if ((id == ETCPAL_SOCKET_INVALID) || !buffer)
    return kEtcPalErrInvalid;

  int impl_flags = (flags & ETCPAL_MSG_PEEK) ? MSG_PEEK : 0;

  int res = recv(id, buffer, (int)length, impl_flags);
  return (res >= 0 ? res : (int)err_winsock_to_etcpal(WSAGetLastError()));
}

int etcpal_recvfrom(etcpal_socket_t id, void* buffer, size_t length, int flags, EtcPalSockAddr* address)
{
  if ((id == ETCPAL_SOCKET_INVALID) || !buffer)
    return (int)kEtcPalErrInvalid;

  int                     impl_flags = (flags & ETCPAL_MSG_PEEK) ? MSG_PEEK : 0;
  struct sockaddr_storage fromaddr   = {0};
  socklen_t               fromlen    = sizeof fromaddr;

  int res = recvfrom(id, buffer, (int)length, impl_flags, (struct sockaddr*)&fromaddr, &fromlen);

  if (res >= 0)
  {
    if (address && fromlen > 0)
    {
      if (!sockaddr_os_to_etcpal((etcpal_os_sockaddr_t*)&fromaddr, address))
        return kEtcPalErrSys;
    }

    return res;
  }

  return (int)err_winsock_to_etcpal(WSAGetLastError());
}

int etcpal_recvmsg(etcpal_socket_t id, EtcPalMsgHdr* msg, int flags)
{
  if ((id == ETCPAL_SOCKET_INVALID) || !msg)
    return (int)kEtcPalErrInvalid;

  if (!wsa_recvmsg)
    return (int)kEtcPalErrNotInit;

  msg->flags = flags;

  WSAMSG           impl_msg  = {0};
  SOCKADDR_STORAGE impl_name = {0};
  WSABUF           impl_buf  = {0};
  construct_wsamsg(msg, &impl_name, &impl_buf, &impl_msg);

  DWORD num_bytes_received = 0;
  int   res                = wsa_recvmsg(id, &impl_msg, &num_bytes_received, NULL, NULL);

  msg->flags = rcvmsg_flags_os_to_etcpal(impl_msg.dwFlags);

  if (res == SOCKET_ERROR)
  {
    etcpal_error_t error = err_winsock_to_etcpal(WSAGetLastError());

    // In the case of kEtcPalErrMsgSize, rely on error reporting via the TRUNC/CTRUNC flags and return bytes received.
    if (error != kEtcPalErrMsgSize)
      return (int)error;
  }

  if (!sockaddr_os_to_etcpal((etcpal_os_sockaddr_t*)&impl_name, &msg->name))
    return kEtcPalErrSys;

  return (int)num_bytes_received;
}

bool etcpal_cmsg_firsthdr(EtcPalMsgHdr* msgh, EtcPalCMsgHdr* firsthdr)
{
  bool result = false;

  if (msgh && firsthdr)
  {
    WSAMSG           impl_msg  = {0};
    SOCKADDR_STORAGE impl_name = {0};
    WSABUF           impl_buf  = {0};
    construct_wsamsg(msgh, &impl_name, &impl_buf, &impl_msg);

    result = get_first_compatible_cmsg(&impl_msg, WSA_CMSG_FIRSTHDR(&impl_msg), firsthdr);
  }

  return result;
}

bool etcpal_cmsg_nxthdr(EtcPalMsgHdr* msgh, const EtcPalCMsgHdr* cmsg, EtcPalCMsgHdr* nxthdr)
{
  bool result = false;

  if (msgh && cmsg && nxthdr)
  {
    WSAMSG           impl_msg  = {0};
    SOCKADDR_STORAGE impl_name = {0};
    WSABUF           impl_buf  = {0};
    construct_wsamsg(msgh, &impl_name, &impl_buf, &impl_msg);

    result = get_first_compatible_cmsg(&impl_msg, WSA_CMSG_NXTHDR(&impl_msg, (PWSACMSGHDR)cmsg->pd), nxthdr);
  }

  return result;
}

bool etcpal_cmsg_to_pktinfo(const EtcPalCMsgHdr* cmsg, EtcPalPktInfo* pktinfo)
{
  bool result = false;

  if (cmsg && pktinfo)
  {
    PWSACMSGHDR impl_cmsg = (PWSACMSGHDR)cmsg->pd;

    // Per the cmsg(3) manpage, impl_data "cannot be assumed to be suitably aligned for accessing arbitrary payload data
    // types. Applications should not cast it to a pointer type matching the payload, but should instead use memcpy(3)
    // to copy data to or from a suitably declared object."
    void* impl_data = WSA_CMSG_DATA(impl_cmsg);

    if (impl_data)
    {
      if ((cmsg->level == ETCPAL_IPPROTO_IP) && (cmsg->type == ETCPAL_IP_PKTINFO) &&
          ETCPAL_ASSERT_VERIFY(cmsg->len >= sizeof(IN_PKTINFO)))
      {
        IN_PKTINFO impl_pktinfo = {0};
        memcpy(&impl_pktinfo, impl_data, sizeof(impl_pktinfo));

        SOCKADDR_IN impl_addr = {0};
        impl_addr.sin_family  = AF_INET;
        impl_addr.sin_addr    = impl_pktinfo.ipi_addr;

        ip_os_to_etcpal((SOCKADDR*)&impl_addr, &pktinfo->addr);
        pktinfo->ifindex = impl_pktinfo.ipi_ifindex;

        result = true;
      }
      else if ((cmsg->level == ETCPAL_IPPROTO_IPV6) && (cmsg->type == ETCPAL_IPV6_PKTINFO) &&
               ETCPAL_ASSERT_VERIFY(cmsg->len >= sizeof(IN6_PKTINFO)))
      {
        IN6_PKTINFO impl_pktinfo = {0};
        memcpy(&impl_pktinfo, impl_data, sizeof(impl_pktinfo));

        SOCKADDR_IN6 impl_addr = {0};
        impl_addr.sin6_family  = AF_INET6;
        impl_addr.sin6_addr    = impl_pktinfo.ipi6_addr;

        ip_os_to_etcpal((SOCKADDR*)&impl_addr, &pktinfo->addr);
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
    return kEtcPalErrInvalid;

  int res = send(id, message, (int)length, 0);
  return (res >= 0 ? res : (int)err_winsock_to_etcpal(WSAGetLastError()));
}

int etcpal_sendto(etcpal_socket_t id, const void* message, size_t length, int flags, const EtcPalSockAddr* dest_addr)
{
  ETCPAL_UNUSED_ARG(flags);

  if ((id == ETCPAL_SOCKET_INVALID) || !dest_addr || !message)
    return (int)kEtcPalErrInvalid;

  struct sockaddr_storage ss      = {0};
  size_t                  ss_size = sockaddr_etcpal_to_os(dest_addr, (etcpal_os_sockaddr_t*)&ss);
  if (ss_size == 0)
    return (int)kEtcPalErrSys;

  int res = sendto(id, message, (int)length, 0, (struct sockaddr*)&ss, (int)ss_size);

  return (res >= 0 ? res : (int)err_winsock_to_etcpal(WSAGetLastError()));
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
  return (res == 0 ? kEtcPalErrOk : err_winsock_to_etcpal(WSAGetLastError()));
}

int setsockopt_socket(etcpal_socket_t id, int option_name, const void* option_value, size_t option_len)
{
  if (!ETCPAL_ASSERT_VERIFY(id != ETCPAL_SOCKET_INVALID) || !ETCPAL_ASSERT_VERIFY(option_value))
    return -1;

  switch (option_name)
  {
    case ETCPAL_SO_RCVBUF:
      return setsockopt(id, SOL_SOCKET, SO_RCVBUF, option_value, (int)option_len);
    case ETCPAL_SO_SNDBUF:
      return setsockopt(id, SOL_SOCKET, SO_SNDBUF, option_value, (int)option_len);
    case ETCPAL_SO_RCVTIMEO:
      if (option_len == sizeof(int))
      {
        DWORD val = (DWORD) * (int*)option_value;
        return setsockopt(id, SOL_SOCKET, SO_RCVTIMEO, (char*)&val, sizeof val);
      }
      break;
    case ETCPAL_SO_SNDTIMEO:
      if (option_len == sizeof(int))
      {
        DWORD val = (DWORD) * (int*)option_value;
        return setsockopt(id, SOL_SOCKET, SO_SNDTIMEO, (char*)&val, sizeof val);
      }
      break;
    case ETCPAL_SO_REUSEADDR:
      if (option_len == sizeof(int))
      {
        BOOL val = (BOOL) * (int*)option_value;
        return setsockopt(id, SOL_SOCKET, SO_REUSEADDR, (char*)&val, sizeof val);
      }
      break;
    case ETCPAL_SO_BROADCAST:
      if (option_len == sizeof(int))
      {
        BOOL val = (BOOL) * (int*)option_value;
        return setsockopt(id, SOL_SOCKET, SO_BROADCAST, (char*)&val, sizeof val);
      }
      break;
    case ETCPAL_SO_KEEPALIVE:
      if (option_len == sizeof(int))
      {
        BOOL val = (BOOL) * (int*)option_value;
        return setsockopt(id, SOL_SOCKET, SO_KEEPALIVE, (char*)&val, sizeof val);
      }
      break;
    case ETCPAL_SO_LINGER:
      if (option_len == sizeof(EtcPalLinger))
      {
        EtcPalLinger* ll  = (EtcPalLinger*)option_value;
        struct linger val = {0};
        val.l_onoff       = (u_short)ll->onoff;
        val.l_linger      = (u_short)ll->linger;
        return setsockopt(id, SOL_SOCKET, SO_LINGER, (char*)&val, sizeof val);
      }
      break;
    case ETCPAL_SO_ERROR:     /* Set not supported */
    case ETCPAL_SO_REUSEPORT: /* Not supported on this OS. */
    case ETCPAL_SO_TYPE:      /* Not supported */
    default:
      break;
  }
  // If we got here, something was invalid. Set errno accordingly
  WSASetLastError(WSAEINVAL);
  return -1;
}

int setsockopt_ip(etcpal_socket_t id, int option_name, const void* option_value, size_t option_len)
{
  if (!ETCPAL_ASSERT_VERIFY(id != ETCPAL_SOCKET_INVALID) || !ETCPAL_ASSERT_VERIFY(option_value))
    return -1;

  switch (option_name)
  {
    case ETCPAL_IP_TTL:
      if (option_len == sizeof(int))
      {
        DWORD val = (DWORD) * (int*)option_value;
        return setsockopt(id, IPPROTO_IP, IP_TTL, (char*)&val, sizeof val);
      }
      break;
    case ETCPAL_IP_ADD_MEMBERSHIP:
      if (option_len == sizeof(EtcPalMreq))
      {
        EtcPalMreq* amreq = (EtcPalMreq*)option_value;
        if (ETCPAL_IP_IS_V4(&amreq->group))
        {
          struct ip_mreq val       = {0};
          val.imr_multiaddr.s_addr = htonl(ETCPAL_IP_V4_ADDRESS(&amreq->group));
          val.imr_interface.s_addr = htonl(ETCPAL_IP_V4_ADDRESS(&amreq->netint));
          return setsockopt(id, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&val, sizeof val);
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
          return setsockopt(id, IPPROTO_IP, IP_DROP_MEMBERSHIP, (char*)&val, sizeof val);
        }
      }
      break;
    case ETCPAL_MCAST_JOIN_GROUP:
      if (option_len == sizeof(EtcPalGroupReq))
      {
        EtcPalGroupReq* greq = (EtcPalGroupReq*)option_value;
        if (ETCPAL_IP_IS_V4(&greq->group) && greq->ifindex >= 0)
        {
          struct group_req val = {0};
          val.gr_interface     = (ULONG)greq->ifindex;

          memset(&val.gr_group, 0, sizeof val.gr_group);
          struct sockaddr_in* sin = (struct sockaddr_in*)&val.gr_group;
          sin->sin_family         = AF_INET;
          sin->sin_addr.s_addr    = htonl(ETCPAL_IP_V4_ADDRESS(&greq->group));
          return setsockopt(id, IPPROTO_IP, MCAST_JOIN_GROUP, (char*)&val, sizeof val);
        }
      }
      break;
    case ETCPAL_MCAST_LEAVE_GROUP:
      if (option_len == sizeof(EtcPalGroupReq))
      {
        EtcPalGroupReq* greq = (EtcPalGroupReq*)option_value;
        if (ETCPAL_IP_IS_V4(&greq->group) && greq->ifindex >= 0)
        {
          struct group_req val = {0};
          val.gr_interface     = (ULONG)greq->ifindex;

          memset(&val.gr_group, 0, sizeof val.gr_group);
          struct sockaddr_in* sin = (struct sockaddr_in*)&val.gr_group;
          sin->sin_family         = AF_INET;
          sin->sin_addr.s_addr    = htonl(ETCPAL_IP_V4_ADDRESS(&greq->group));
          return setsockopt(id, IPPROTO_IP, MCAST_LEAVE_GROUP, (char*)&val, sizeof val);
        }
      }
      break;
    case ETCPAL_IP_MULTICAST_IF:
      if (option_len == sizeof(unsigned int))
      {
        unsigned int ifindex = *(unsigned int*)option_value;
        // On Windows, this socket option takes either an IPv4 address or an interface index in
        // network byte order. We use the latter option.
        DWORD val = htonl(ifindex);
        return setsockopt(id, IPPROTO_IP, IP_MULTICAST_IF, (char*)&val, sizeof val);
      }
      break;
    case ETCPAL_IP_MULTICAST_TTL:
      if (option_len == sizeof(int))
      {
        DWORD val = (DWORD) * (int*)option_value;
        return setsockopt(id, IPPROTO_IP, IP_MULTICAST_TTL, (char*)&val, sizeof val);
      }
      break;
    case ETCPAL_IP_MULTICAST_LOOP:
      if (option_len == sizeof(int))
      {
        DWORD val = (DWORD) * (int*)option_value;
        return setsockopt(id, IPPROTO_IP, IP_MULTICAST_LOOP, (char*)&val, sizeof val);
      }
      break;
    case ETCPAL_IP_PKTINFO:
      if (option_len == sizeof(int))
      {
        DWORD val = (DWORD) * (int*)option_value;
        return setsockopt(id, IPPROTO_IP, IP_PKTINFO, (char*)&val, sizeof val);
      }
      break;
    default:
      break;
  }
  // If we got here, something was invalid. Set errno accordingly
  WSASetLastError(WSAEINVAL);
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
        if (ETCPAL_IP_IS_V6(&greq->group) && greq->ifindex >= 0)
        {
          struct group_req val = {0};
          val.gr_interface     = (ULONG)greq->ifindex;

          memset(&val.gr_group, 0, sizeof val.gr_group);
          struct sockaddr_in6* sin6 = (struct sockaddr_in6*)&val.gr_group;
          sin6->sin6_family         = AF_INET6;
          memcpy(sin6->sin6_addr.s6_addr, ETCPAL_IP_V6_ADDRESS(&greq->group), ETCPAL_IPV6_BYTES);
          return setsockopt(id, IPPROTO_IPV6, MCAST_JOIN_GROUP, (char*)&val, sizeof val);
        }
      }
      break;
    case ETCPAL_MCAST_LEAVE_GROUP:
      if (option_len == sizeof(EtcPalGroupReq))
      {
        EtcPalGroupReq* greq = (EtcPalGroupReq*)option_value;
        if (ETCPAL_IP_IS_V6(&greq->group) && greq->ifindex >= 0)
        {
          struct group_req val = {0};
          val.gr_interface     = (ULONG)greq->ifindex;

          memset(&val.gr_group, 0, sizeof val.gr_group);
          struct sockaddr_in6* sin6 = (struct sockaddr_in6*)&val.gr_group;
          sin6->sin6_family         = AF_INET6;
          memcpy(sin6->sin6_addr.s6_addr, ETCPAL_IP_V6_ADDRESS(&greq->group), ETCPAL_IPV6_BYTES);
          return setsockopt(id, IPPROTO_IPV6, MCAST_LEAVE_GROUP, (char*)&val, sizeof val);
        }
      }
      break;
    case ETCPAL_IP_MULTICAST_IF:
      if (option_len == sizeof(unsigned int))
      {
        DWORD val = (DWORD) * (unsigned int*)option_value;
        return setsockopt(id, IPPROTO_IPV6, IPV6_MULTICAST_IF, (char*)&val, sizeof val);
      }
      break;
    case ETCPAL_IP_MULTICAST_TTL:
      if (option_len == sizeof(int))
      {
        DWORD val = (DWORD) * (int*)option_value;
        return setsockopt(id, IPPROTO_IPV6, IPV6_MULTICAST_HOPS, (char*)&val, sizeof val);
      }
      break;
    case ETCPAL_IP_MULTICAST_LOOP:
      if (option_len == sizeof(int))
      {
        DWORD val = (DWORD) * (int*)option_value;
        return setsockopt(id, IPPROTO_IPV6, IPV6_MULTICAST_LOOP, (char*)&val, sizeof val);
      }
      break;
    case ETCPAL_IPV6_V6ONLY:
      if (option_len == sizeof(int))
      {
        DWORD val = (DWORD) * (int*)option_value;
        return setsockopt(id, IPPROTO_IPV6, IPV6_V6ONLY, (char*)&val, sizeof val);
      }
      break;
    case ETCPAL_IPV6_PKTINFO:
      if (option_len == sizeof(int))
      {
        DWORD val = (DWORD) * (int*)option_value;
        return setsockopt(id, IPPROTO_IPV6, IPV6_PKTINFO, (char*)&val, sizeof val);
      }
      break;
    default: /* Other IPv6 options TODO on windows. */
      break;
  }

  // If we got here, something was invalid. Set errno accordingly
  WSASetLastError(WSAEINVAL);
  return -1;
}

etcpal_error_t etcpal_shutdown(etcpal_socket_t id, int how)
{
  if ((id != ETCPAL_SOCKET_INVALID) && (how >= 0) && (how < ETCPAL_NUM_SHUT))
  {
    int res = shutdown(id, kShutMap[how]);
    return (res == 0 ? kEtcPalErrOk : err_winsock_to_etcpal(WSAGetLastError()));
  }

  return kEtcPalErrInvalid;
}

etcpal_error_t etcpal_socket(unsigned int family, unsigned int type, etcpal_socket_t* id)
{
  if (id)
  {
    if (family < ETCPAL_NUM_AF && type < ETCPAL_NUM_TYPE)
    {
      SOCKET sock = socket(kSfMap[family], kStMap[type], 0);
      if (sock != INVALID_SOCKET)
      {
        *id = sock;
        return kEtcPalErrOk;
      }

      *id = ETCPAL_SOCKET_INVALID;
      return err_winsock_to_etcpal(WSAGetLastError());
    }

    *id = ETCPAL_SOCKET_INVALID;
  }

  return kEtcPalErrInvalid;
}

etcpal_error_t etcpal_setblocking(etcpal_socket_t id, bool blocking)
{
  if (id == ETCPAL_SOCKET_INVALID)
    return kEtcPalErrInvalid;

  unsigned long val = (blocking ? 0 : 1);
  int           res = ioctlsocket(id, FIONBIO, &val);
  return (res == 0 ? kEtcPalErrOk : err_winsock_to_etcpal(WSAGetLastError()));
}

// NOLINTNEXTLINE(readability-non-const-parameter)
etcpal_error_t etcpal_getblocking(etcpal_socket_t id, bool* blocking)
{
  ETCPAL_UNUSED_ARG(id);
  ETCPAL_UNUSED_ARG(blocking);
  return kEtcPalErrNotImpl;
}

etcpal_error_t etcpal_poll_context_init(EtcPalPollContext* context)
{
  if (!context)
    return kEtcPalErrInvalid;

  if (!etcpal_mutex_create(&context->lock))
    return kEtcPalErrSys;

  etcpal_rbtree_init(&context->sockets, poll_socket_compare, poll_socket_alloc, poll_socket_free);
  ETCPAL_FD_ZERO(&context->readfds);
  ETCPAL_FD_ZERO(&context->writefds);
  ETCPAL_FD_ZERO(&context->exceptfds);
  context->valid = true;
  return kEtcPalErrOk;
}

void etcpal_poll_context_deinit(EtcPalPollContext* context)
{
  if (!context || !context->valid)
    return;

  etcpal_rbtree_clear(&context->sockets);
  etcpal_mutex_destroy(&context->lock);
  context->valid = false;
}

// NOLINTNEXTLINE(readability-function-cognitive-complexity)
void set_in_fd_sets(EtcPalPollContext* context, const EtcPalPollSocket* sock_desc)
{
  if (!ETCPAL_ASSERT_VERIFY(context) || !ETCPAL_ASSERT_VERIFY(sock_desc))
    return;

  if (sock_desc->events & ETCPAL_POLL_IN)
  {
    ETCPAL_FD_SET(sock_desc->sock, &context->readfds);
  }
  if (sock_desc->events & (ETCPAL_POLL_OUT | ETCPAL_POLL_CONNECT))
  {
    ETCPAL_FD_SET(sock_desc->sock, &context->writefds);
  }
  if (sock_desc->events & (ETCPAL_POLL_OOB | ETCPAL_POLL_CONNECT))
  {
    ETCPAL_FD_SET(sock_desc->sock, &context->exceptfds);
  }
}

// NOLINTNEXTLINE(readability-function-cognitive-complexity)
void clear_in_fd_sets(EtcPalPollContext* context, const EtcPalPollSocket* sock_desc)
{
  if (!ETCPAL_ASSERT_VERIFY(context) || !ETCPAL_ASSERT_VERIFY(sock_desc))
    return;

  if (sock_desc->events & ETCPAL_POLL_IN)
  {
    ETCPAL_FD_CLEAR(sock_desc->sock, &context->readfds);
  }
  if (sock_desc->events & (ETCPAL_POLL_OUT | ETCPAL_POLL_CONNECT))
  {
    ETCPAL_FD_CLEAR(sock_desc->sock, &context->writefds);
  }
  if (sock_desc->events & (ETCPAL_POLL_OOB | ETCPAL_POLL_CONNECT))
  {
    ETCPAL_FD_CLEAR(sock_desc->sock, &context->exceptfds);
  }
}

etcpal_error_t etcpal_poll_add_socket(EtcPalPollContext*   context,
                                      etcpal_socket_t      socket,
                                      etcpal_poll_events_t events,
                                      void*                user_data)
{
  if (!context || !context->valid || socket == ETCPAL_SOCKET_INVALID || !(events & ETCPAL_POLL_VALID_INPUT_EVENT_MASK))
    return kEtcPalErrInvalid;

  etcpal_error_t res = kEtcPalErrSys;
  if (etcpal_mutex_lock(&context->lock))
  {
    if (etcpal_rbtree_size(&context->sockets) >= ETCPAL_SOCKET_MAX_POLL_SIZE)
    {
      res = kEtcPalErrNoMem;
    }
    else
    {
      EtcPalPollSocket* new_sock = (EtcPalPollSocket*)malloc(sizeof(EtcPalPollSocket));
      if (new_sock)
      {
        new_sock->sock      = socket;
        new_sock->events    = events;
        new_sock->user_data = user_data;
        res                 = etcpal_rbtree_insert(&context->sockets, new_sock);
        if (res == kEtcPalErrOk)
        {
          set_in_fd_sets(context, new_sock);
        }
        else
        {
          free(new_sock);
        }
      }
      else
      {
        res = kEtcPalErrNoMem;
      }
    }

    etcpal_mutex_unlock(&context->lock);
  }

  return res;
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

  etcpal_error_t res = kEtcPalErrSys;
  if (etcpal_mutex_lock(&context->lock))
  {
    EtcPalPollSocket* sock_desc = (EtcPalPollSocket*)etcpal_rbtree_find(&context->sockets, &socket);
    if (sock_desc)
    {
      clear_in_fd_sets(context, sock_desc);
      sock_desc->events    = new_events;
      sock_desc->user_data = new_user_data;
      set_in_fd_sets(context, sock_desc);
      res = kEtcPalErrOk;
    }
    else
    {
      res = kEtcPalErrNotFound;
    }

    etcpal_mutex_unlock(&context->lock);
  }

  return res;
}

void etcpal_poll_remove_socket(EtcPalPollContext* context, etcpal_socket_t socket)
{
  if (!context || !context->valid || socket == ETCPAL_SOCKET_INVALID)
    return;

  if (etcpal_mutex_lock(&context->lock))
  {
    EtcPalPollSocket* sock_desc = (EtcPalPollSocket*)etcpal_rbtree_find(&context->sockets, &socket);
    if (sock_desc)
    {
      clear_in_fd_sets(context, sock_desc);
      etcpal_rbtree_remove(&context->sockets, sock_desc);
    }

    etcpal_mutex_unlock(&context->lock);
  }
}

etcpal_error_t etcpal_poll_wait(EtcPalPollContext* context, EtcPalPollEvent* event, int timeout_ms)
{
  if (!context || !context->valid || !event)
    return kEtcPalErrInvalid;

  // Get the sets of sockets that we will select on.
  EtcPalPollFdSet readfds = {0};
  ETCPAL_FD_ZERO(&readfds);
  EtcPalPollFdSet writefds = {0};
  ETCPAL_FD_ZERO(&writefds);
  EtcPalPollFdSet exceptfds = {0};
  ETCPAL_FD_ZERO(&exceptfds);
  if (etcpal_mutex_lock(&context->lock))
  {
    if (etcpal_rbtree_size(&context->sockets) > 0)
    {
      readfds   = context->readfds;
      writefds  = context->writefds;
      exceptfds = context->exceptfds;
    }
    etcpal_mutex_unlock(&context->lock);
  }

  // No valid sockets are currently added to the context.
  if (!readfds.count && !writefds.count && !exceptfds.count)
    return kEtcPalErrNoSockets;

  struct timeval os_timeout = {0};
  if (timeout_ms == 0)
  {
    os_timeout.tv_sec  = 0;
    os_timeout.tv_usec = 0;
  }
  else if (timeout_ms != ETCPAL_WAIT_FOREVER)
  {
    os_timeout.tv_sec  = timeout_ms / 1000;
    os_timeout.tv_usec = (timeout_ms % 1000) * 1000;
  }

  int sel_res = select(0, readfds.count ? &readfds.set : NULL, writefds.count ? &writefds.set : NULL,
                       exceptfds.count ? &exceptfds.set : NULL, timeout_ms == ETCPAL_WAIT_FOREVER ? NULL : &os_timeout);

  if (sel_res < 0)
    return err_winsock_to_etcpal(WSAGetLastError());
  if (sel_res == 0)
    return kEtcPalErrTimedOut;

  etcpal_error_t res = kEtcPalErrSys;
  if (context->valid && etcpal_mutex_lock(&context->lock))
  {
    res = handle_select_result(context, event, &readfds, &writefds, &exceptfds);
    etcpal_mutex_unlock(&context->lock);
  }

  return res;
}

etcpal_error_t handle_select_result(EtcPalPollContext*     context,
                                    EtcPalPollEvent*       event,
                                    const EtcPalPollFdSet* readfds,
                                    const EtcPalPollFdSet* writefds,
                                    const EtcPalPollFdSet* exceptfds)
{
  if (!ETCPAL_ASSERT_VERIFY(context) || !ETCPAL_ASSERT_VERIFY(context->valid) || !ETCPAL_ASSERT_VERIFY(event) ||
      !ETCPAL_ASSERT_VERIFY(readfds) || !ETCPAL_ASSERT_VERIFY(writefds) || !ETCPAL_ASSERT_VERIFY(exceptfds))
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

  EtcPalRbIter iter;
  etcpal_rbiter_init(&iter);
  for (EtcPalPollSocket* sock_desc = (EtcPalPollSocket*)etcpal_rbiter_first(&iter, &context->sockets); sock_desc;
       sock_desc                   = (EtcPalPollSocket*)etcpal_rbiter_next(&iter))
  {
    if (sock_desc->sock == ETCPAL_SOCKET_INVALID)
      continue;

    if (ETCPAL_FD_ISSET(sock_desc->sock, readfds) || ETCPAL_FD_ISSET(sock_desc->sock, writefds) ||
        ETCPAL_FD_ISSET(sock_desc->sock, exceptfds))
    {
      res              = kEtcPalErrOk;
      event->socket    = sock_desc->sock;
      event->user_data = sock_desc->user_data;

      /* Check for errors */
      int error      = 0;
      int error_size = sizeof(error);
      if (getsockopt(sock_desc->sock, SOL_SOCKET, SO_ERROR, (char*)&error, &error_size) == 0)
      {
        if (error != 0)
        {
          event->events |= ETCPAL_POLL_ERR;
          event->err = err_winsock_to_etcpal(error);
        }
      }
      else
      {
        res = err_winsock_to_etcpal(WSAGetLastError());
        break;
      }
      if (ETCPAL_FD_ISSET(sock_desc->sock, readfds))
      {
        if (sock_desc->events & ETCPAL_POLL_IN)
          event->events |= ETCPAL_POLL_IN;
      }
      if (ETCPAL_FD_ISSET(sock_desc->sock, writefds))
      {
        if (sock_desc->events & ETCPAL_POLL_CONNECT)
          event->events |= ETCPAL_POLL_CONNECT;
        else if (sock_desc->events & ETCPAL_POLL_OUT)
          event->events |= ETCPAL_POLL_OUT;
      }
      if (ETCPAL_FD_ISSET(sock_desc->sock, exceptfds))
      {
        if (sock_desc->events & ETCPAL_POLL_CONNECT)
          event->events |= ETCPAL_POLL_CONNECT;  // Async connect errors are handled above
        else if (sock_desc->events & ETCPAL_POLL_OOB)
          event->events |= ETCPAL_POLL_OOB;
      }
      break;  // We handle one event at a time.
    }
  }

  return res;
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

etcpal_error_t init_extension_functions()
{
  etcpal_error_t res = get_wsarecvmsg(&wsa_recvmsg);

  return res;
}

etcpal_error_t get_wsarecvmsg(LPFN_WSARECVMSG* result)
{
  if (!ETCPAL_ASSERT_VERIFY(result))
    return kEtcPalErrSys;

  LPFN_WSARECVMSG func  = NULL;
  GUID            guid  = WSAID_WSARECVMSG;
  SOCKET          sock  = INVALID_SOCKET;
  DWORD           bytes = 0;

  sock = socket(AF_INET6, SOCK_DGRAM, 0);

  int err =
      WSAIoctl(sock, SIO_GET_EXTENSION_FUNCTION_POINTER, &guid, sizeof(guid), &func, sizeof(func), &bytes, NULL, NULL);

  if (err == 0)
    *result = func;

  if (sock != INVALID_SOCKET)
    closesocket(sock);

  return (err == 0) ? kEtcPalErrOk : err_winsock_to_etcpal(err);
}

void construct_wsamsg(const EtcPalMsgHdr* in_msg, LPSOCKADDR_STORAGE name_store, LPWSABUF buf_store, LPWSAMSG out_msg)
{
  if (!ETCPAL_ASSERT_VERIFY(in_msg) || !ETCPAL_ASSERT_VERIFY(name_store) || !ETCPAL_ASSERT_VERIFY(buf_store) ||
      !ETCPAL_ASSERT_VERIFY(out_msg))
  {
    return;
  }

  out_msg->name          = (SOCKADDR*)name_store;
  out_msg->namelen       = sizeof(*name_store);
  out_msg->lpBuffers     = buf_store;
  out_msg->dwBufferCount = 1;
  out_msg->Control.buf   = in_msg->control;
  out_msg->Control.len   = (ULONG)in_msg->controllen;
  out_msg->dwFlags       = rcvmsg_flags_etcpal_to_os(in_msg->flags);
  buf_store->buf         = in_msg->buf;
  buf_store->len         = (ULONG)in_msg->buflen;

  sockaddr_etcpal_to_os(&in_msg->name, (etcpal_os_sockaddr_t*)name_store);
}

int rcvmsg_flags_os_to_etcpal(ULONG os_flags)
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

ULONG rcvmsg_flags_etcpal_to_os(int etcpal_flags)
{
  ULONG result = 0;

  if (etcpal_flags & ETCPAL_MSG_TRUNC)
    result |= MSG_TRUNC;

  if (etcpal_flags & ETCPAL_MSG_CTRUNC)
    result |= MSG_CTRUNC;

  if (etcpal_flags & ETCPAL_MSG_PEEK)
    result |= MSG_PEEK;

  return result;
}

bool get_first_compatible_cmsg(LPWSAMSG msg, LPWSACMSGHDR start, EtcPalCMsgHdr* cmsg)
{
  if (!ETCPAL_ASSERT_VERIFY(msg) || !ETCPAL_ASSERT_VERIFY(cmsg))
    return false;

  bool get_next_cmsg = true;
  for (LPWSACMSGHDR hdr = start; hdr && (hdr->cmsg_len > 0) && get_next_cmsg; hdr = WSA_CMSG_NXTHDR(msg, hdr))
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

  struct addrinfo pf_hints;
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

  int res = getaddrinfo(hostname, service, hints ? &pf_hints : NULL, &pf_res);
  if (res == 0)
  {
    result->pd[0] = pf_res;
    result->pd[1] = pf_res;
    if (!etcpal_nextaddr(result))
      res = -1;
  }

  return (res == 0 ? kEtcPalErrOk : err_winsock_to_etcpal(res));
}

bool etcpal_nextaddr(EtcPalAddrinfo* ai)
{
  if (ai && ai->pd[1])
  {
    struct addrinfo* pf_ai = (struct addrinfo*)ai->pd[1];
    ai->ai_flags           = 0;
    if (!sockaddr_os_to_etcpal((etcpal_os_sockaddr_t*)pf_ai->ai_addr, &ai->ai_addr))
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
