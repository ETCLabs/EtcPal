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

#include <unistd.h>


#include <zephyr/net/socket.h>
#include <zephyr/net/net_if.h>

#include <fcntl.h>

#include "etcpal/common.h"
#include "etcpal/socket.h"
#include "etcpal/timer.h"
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
	ZSOCK_SHUT_RD,
	ZSOCK_SHUT_WR,
	ZSOCK_SHUT_RDWR
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

#ifdef NAM
// Helpers for etcpal_poll API
static void events_etcpal_to_epoll(etcpal_poll_events_t events, struct epoll_event* epoll_evt);
static void events_epoll_to_etcpal(const struct epoll_event* epoll_evt,
	const EtcPalPollSocket*   sock_desc,
	etcpal_poll_events_t*     events);
#endif // NAM

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
	int                     res     = zsock_accept(id, (struct sockaddr*)&ss, &sa_size);

	if (res != -1)
	{
		if (address && !sockaddr_os_to_etcpal((etcpal_os_sockaddr_t*)&ss, address))
		{
			zsock_close(res);
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

	int res = zsock_bind(id, (struct sockaddr*)&ss, sa_size);
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

	int res = zsock_connect(id, (struct sockaddr*)&ss, sa_size);
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

// NOLINTNEXTLINE(readability-non-const-parameter)
etcpal_error_t etcpal_getsockopt(etcpal_socket_t id, int level, int option_name, void* option_value, size_t* option_len)
{
	/* TODO */
	ETCPAL_UNUSED_ARG(id);
	ETCPAL_UNUSED_ARG(level);
	ETCPAL_UNUSED_ARG(option_name);
	ETCPAL_UNUSED_ARG(option_value);
	ETCPAL_UNUSED_ARG(option_len);
	return kEtcPalErrNotImpl;
}

etcpal_error_t etcpal_listen(etcpal_socket_t id, int backlog)
{
	int res = zsock_listen(id, backlog);
	return (res == 0 ? kEtcPalErrOk : errno_os_to_etcpal(errno));
}

int etcpal_recv(etcpal_socket_t id, void* buffer, size_t length, int flags)
{
	if (!buffer)
		return kEtcPalErrInvalid;

	int impl_flags = (flags & ETCPAL_MSG_PEEK) ? ZSOCK_MSG_PEEK : 0;
	int res        = (int)zsock_recv(id, buffer, length, impl_flags);
	return (res >= 0 ? res : (int)errno_os_to_etcpal(errno));
}

int etcpal_recvfrom(etcpal_socket_t id, void* buffer, size_t length, int flags, EtcPalSockAddr* address)
{
	if (!buffer)
		return (int)kEtcPalErrInvalid;

	struct sockaddr_storage fromaddr;
	socklen_t               fromlen    = sizeof fromaddr;
	int                     impl_flags = (flags & ETCPAL_MSG_PEEK) ? ZSOCK_MSG_PEEK : 0;
	int                     res = (int)zsock_recvfrom(id, buffer, length, impl_flags, (struct sockaddr*)&fromaddr, &fromlen);

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

	struct msghdr           impl_msg = { 0 };
	struct sockaddr_storage impl_name = { 0 };
	struct iovec            impl_buf = { 0 };
	construct_msghdr(msg, &impl_name, &impl_buf, &impl_msg);

	int res = (int)zsock_recv(id, msg->buf, msg->buflen, msg->flags);

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
		struct msghdr           impl_msg = { 0 };
		struct sockaddr_storage impl_name = { 0 };
		struct iovec            impl_buf = { 0 };
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
		struct msghdr           impl_msg = { 0 };
		struct sockaddr_storage impl_name = { 0 };
		struct iovec            impl_buf = { 0 };
		construct_msghdr(msgh, &impl_name, &impl_buf, &impl_msg);

		result = get_first_compatible_cmsg(&impl_msg, CMSG_NXTHDR(&impl_msg, (struct cmsghdr*)cmsg->pd), nxthdr);
	}

	return result;
}

// Zephyr does not really have a pktinfo analog that I could find
#ifdef NAM
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
				
#ifdef NAM
				struct in_pktinfo impl_pktinfo;
				memcpy(&impl_pktinfo, impl_data, sizeof(impl_pktinfo));

				struct sockaddr_in impl_addr;
				impl_addr.sin_family = AF_INET;
				impl_addr.sin_addr   = impl_pktinfo.ipi_addr;

				ip_os_to_etcpal((struct sockaddr*)&impl_addr, &pktinfo->addr);
				pktinfo->ifindex = impl_pktinfo.ipi_ifindex;
#endif // NAM
				result = true;
			}
			else if ((cmsg->level == ETCPAL_IPPROTO_IPV6) && (cmsg->type == ETCPAL_IPV6_PKTINFO))
			{
				// No IPV6 for now
#ifdef NAM
				struct in6_pktinfo impl_pktinfo;
				memcpy(&impl_pktinfo, impl_data, sizeof(impl_pktinfo));

				struct sockaddr_in6 impl_addr;
				impl_addr.sin6_family = AF_INET6;
				impl_addr.sin6_addr   = impl_pktinfo.ipi6_addr;

				ip_os_to_etcpal((struct sockaddr*)&impl_addr, &pktinfo->addr);
				pktinfo->ifindex = impl_pktinfo.ipi6_ifindex;
#endif // NAM
				result = true;
				
			}
		}
	}

	return result;
}
#endif // NAM

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

	int res = (int)zsock_sendto(id, message, length, 0, (struct sockaddr*)&ss, ss_size);

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
	switch (option_name)
	{
	case ETCPAL_SO_RCVBUF:
		return zsock_setsockopt(id, SOL_SOCKET, SO_RCVBUF, option_value, (socklen_t)option_len);
	case ETCPAL_SO_SNDBUF:
		return zsock_setsockopt(id, SOL_SOCKET, SO_SNDBUF, option_value, (socklen_t)option_len);
	case ETCPAL_SO_RCVTIMEO:
		if (option_len == sizeof(int))
		{
			int            val = *(int*)option_value;
			struct timeval sys_val;
			ms_to_timeval(val, &sys_val);
			return zsock_setsockopt(id, SOL_SOCKET, SO_RCVTIMEO, &sys_val, sizeof sys_val);
		}
		break;
	case ETCPAL_SO_SNDTIMEO:
		if (option_len == sizeof(int))
		{
			int            val = *(int*)option_value;
			struct timeval sys_val;
			ms_to_timeval(val, &sys_val);
			return zsock_setsockopt(id, SOL_SOCKET, SO_SNDTIMEO, &sys_val, sizeof sys_val);
		}
		break;
	case ETCPAL_SO_REUSEADDR:
		return zsock_setsockopt(id, SOL_SOCKET, SO_REUSEADDR, option_value, (socklen_t)option_len);
	case ETCPAL_SO_REUSEPORT:
		return zsock_setsockopt(id, SOL_SOCKET, SO_REUSEPORT, option_value, (socklen_t)option_len);
	case ETCPAL_SO_BROADCAST:
		return zsock_setsockopt(id, SOL_SOCKET, SO_BROADCAST, option_value, (socklen_t)option_len);
	case ETCPAL_SO_KEEPALIVE:
		return zsock_setsockopt(id, SOL_SOCKET, SO_KEEPALIVE, option_value, (socklen_t)option_len);
	case ETCPAL_SO_LINGER: // Set not supported
	case ETCPAL_SO_ERROR:  // Set not supported
	case ETCPAL_SO_TYPE:   // Set not supported
	default:
		break;
	}
	// If we got here, something was invalid. Set errno accordingly
	errno = EINVAL;
	return -1;
}
#ifdef NAM
static int ip4_ifindex_to_addr(unsigned int ifindex, struct in_addr* addr)
{
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
#endif // NAM

int setsockopt_ip(etcpal_socket_t id, int option_name, const void* option_value, size_t option_len)
{
	EtcPalGroupReq group;
	struct net_if_mcast_addr mcast_addr;
		
	switch (option_name)
	{
	case ETCPAL_IP_TTL:
		return -1;
	case ETCPAL_IP_ADD_MEMBERSHIP:
		return -1;
	case ETCPAL_IP_DROP_MEMBERSHIP:
		return -1;
	case ETCPAL_MCAST_JOIN_GROUP:
		memcpy(&group, option_value, option_len);
		// Unsure if this is the correct type
		mcast_addr.address.family = IPPROTO_IP;
		mcast_addr.address.in_addr.s4_addr32[0] = group.group.addr.v4;
		net_if_ipv4_maddr_join(&mcast_addr);
		return 0;

	case ETCPAL_MCAST_LEAVE_GROUP:
		memcpy(&group, option_value, option_len);
		// Unsure if this is the correct type
		mcast_addr.address.family = IPPROTO_IP;
		mcast_addr.address.in_addr.s4_addr32[0] = group.group.addr.v4;
		net_if_ipv4_maddr_leave(&mcast_addr);
		return 0;
		
	case ETCPAL_IP_MULTICAST_IF:
		return -1;
	case ETCPAL_IP_MULTICAST_TTL:
		return -1;
	case ETCPAL_IP_MULTICAST_LOOP:
		return -1;
	case ETCPAL_IP_PKTINFO:
		return -1;
	default:
		break;
	}
	// If we got here, something was invalid. Set errno accordingly
	errno = EINVAL;
	return -1;
}

int setsockopt_ip6(etcpal_socket_t id, int option_name, const void* option_value, size_t option_len)
{
	switch (option_name)
	{
	case ETCPAL_MCAST_JOIN_GROUP:
		return -1;

	case ETCPAL_MCAST_LEAVE_GROUP:
		return -1;

	case ETCPAL_IP_MULTICAST_IF:
		return -1;
	case ETCPAL_IP_MULTICAST_TTL:
		return -1;
	case ETCPAL_IP_MULTICAST_LOOP:
		return -1;
	case ETCPAL_IPV6_V6ONLY:
		return zsock_setsockopt(id, IPPROTO_IPV6, IPV6_V6ONLY, option_value, (socklen_t)option_len);
	case ETCPAL_IPV6_PKTINFO:
		return -1;
	default: /* Other IPv6 options TODO on linux. */
		break;
	}
	// If we got here, something was invalid. Set errno accordingly
	errno = EINVAL;
	return -1;
}

void ms_to_timeval(int ms, struct timeval* tv)
{
	tv->tv_sec  = ms / 1000;
	tv->tv_usec = (ms % 1000) * 1000;
}

etcpal_error_t etcpal_shutdown(etcpal_socket_t id, int how)
{
	if (how >= 0 && how < ETCPAL_NUM_SHUT)
	{
		int res = zsock_shutdown(id, kShutMap[how]);
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

	int sock = zsock_socket(kSfMap[family], kStMap[type], 0);
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
	int val = fcntl(id, F_GETFL, 0);
	if (val >= 0)
	{
		val = fcntl(id, F_SETFL, (blocking ? (val & (int)(~O_NONBLOCK)) : (val | O_NONBLOCK)));
	}
	return (val >= 0 ? kEtcPalErrOk : errno_os_to_etcpal(errno));
}

etcpal_error_t etcpal_getblocking(etcpal_socket_t id, bool* blocking)
{
	if (!blocking)
		return kEtcPalErrInvalid;

	int val = fcntl(id, F_GETFL, 0);
	if (val < 0)
		return errno_os_to_etcpal(errno);

	*blocking = ((val & O_NONBLOCK) == 0);
	return kEtcPalErrOk;
}

etcpal_error_t etcpal_poll_context_init(EtcPalPollContext* context)
{
	// Taken from LWIP port
	if (!context)
		return kEtcPalErrInvalid;

	//init_context_socket_array(context);
	context->valid = true;
	return kEtcPalErrOk;
#ifdef NAM
	// TODO: Populate this?
	if (!context)
		return kEtcPalErrInvalid;

  //k_poll_event_init(K_POLL_TYPE_DATA_AVAILABLE, K_POLL_MODE_NOTIFY_ONLY, )

	if (context->zpoll_fd >= 0)
	{
		etcpal_rbtree_init(&context->sockets, poll_socket_compare, poll_socket_alloc, poll_socket_free);
		context->valid = true;
		return kEtcPalErrOk;
	}
//	return errno_os_to_etcpal(errno);
#endif // NAM
}

void etcpal_poll_context_deinit(EtcPalPollContext* context)
{
	if (context && context->valid)
	{
		etcpal_rbtree_clear(&context->sockets);
		
		context->valid = false;
	}
}

etcpal_error_t etcpal_poll_add_socket(EtcPalPollContext*   context,
	etcpal_socket_t      socket,
	etcpal_poll_events_t events,
	void*                user_data)
{
	// Not implemented
	return kEtcPalErrNotImpl;
}

etcpal_error_t etcpal_poll_modify_socket(EtcPalPollContext*   context,
	etcpal_socket_t      socket,
	etcpal_poll_events_t new_events,
	void*                new_user_data)
{
	// Not implemented
	return kEtcPalErrNotImpl;
}

void etcpal_poll_remove_socket(EtcPalPollContext* context, etcpal_socket_t socket)
{
	return;
}

etcpal_error_t etcpal_poll_wait(EtcPalPollContext* context, EtcPalPollEvent* event, int timeout_ms)
{
	if (!context || !context->valid || !event)
	{
		return kEtcPalErrInvalid;
	}

	if (context->sockets.size == 0)
	{
		return kEtcPalErrNoSockets;
	}

	int sys_timeout = (timeout_ms == ETCPAL_WAIT_FOREVER ? -1 : timeout_ms);

	//struct epoll_event epoll_evt;
	//int wait_res = epoll_wait(context->epoll_fd, &epoll_fd, 1, sys_timeout);
	//if (wait_res == 0)
	//{
	//	return kEtcPalErrTimedOut;
	//}
	
}


int poll_socket_compare(const EtcPalRbTree* tree, const void* value_a, const void* value_b)
{
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
	if (node)
	{
		free(node->value);
		free(node);
	}
}

void construct_msghdr(const EtcPalMsgHdr*      in_msg,
	struct sockaddr_storage* name_store,
	struct iovec*            buf_store,
	struct msghdr*           out_msg)
{
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

	if (os_flags & ZSOCK_MSG_TRUNC)
		result |= ETCPAL_MSG_TRUNC;

	if (os_flags & ZSOCK_MSG_PEEK)
		result |= ETCPAL_MSG_PEEK;

	return result;
}

int rcvmsg_flags_etcpal_to_os(int etcpal_flags)
{
	int result = 0;

	if (etcpal_flags & ETCPAL_MSG_TRUNC)
		result |= ZSOCK_MSG_TRUNC;

	if (etcpal_flags & ETCPAL_MSG_PEEK)
		result |= ZSOCK_MSG_PEEK;

	return result;
}

bool get_first_compatible_cmsg(struct msghdr* msg, struct cmsghdr* start, EtcPalCMsgHdr* cmsg)
{
	bool get_next_cmsg = true;
	for (struct cmsghdr* hdr = start; hdr && (hdr->cmsg_len > 0) && get_next_cmsg; hdr = CMSG_NXTHDR(msg, hdr))
	{
		get_next_cmsg = false;

		cmsg->pd  = hdr;
		cmsg->len = hdr->cmsg_len;

		if (hdr->cmsg_level == IPPROTO_IP)
		{
			cmsg->level = ETCPAL_IPPROTO_IP;
		}
		else if (hdr->cmsg_level == IPPROTO_IPV6)
		{
			cmsg->level = ETCPAL_IPPROTO_IPV6;
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

	struct zsock_addrinfo pf_hints;
	memset(&pf_hints, 0, sizeof pf_hints);
	if (hints)
	{
		pf_hints.ai_flags    = (hints->ai_flags < ETCPAL_NUM_AIF) ? kAiFlagMap[hints->ai_flags] : 0;
		pf_hints.ai_family   = (hints->ai_family < ETCPAL_NUM_AF) ? kAiFamMap[hints->ai_family] : AF_UNSPEC;
		pf_hints.ai_socktype = (hints->ai_socktype < ETCPAL_NUM_TYPE) ? kStMap[hints->ai_socktype] : 0;
		pf_hints.ai_protocol = (hints->ai_protocol < ETCPAL_NUM_IPPROTO) ? kAiProtMap[hints->ai_protocol] : 0;
	}

	struct zsock_addrinfo* pf_res = NULL;
	int              res    = zsock_getaddrinfo(hostname, service, hints ? &pf_hints : NULL, &pf_res);
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
		struct zsock_addrinfo* pf_ai = (struct zsock_addrinfo*)ai->pd[1];
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
		zsock_freeaddrinfo((struct addrinfo*)ai->pd[0]);
}

#endif  // !defined(ETCPAL_BUILDING_MOCK_LIB)
