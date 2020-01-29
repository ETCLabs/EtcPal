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

#include "etcpal/inet.h"

#include <lwip/sockets.h>
#include "os_error.h"

#if !LWIP_SOCKET
#error "LWIP_SOCKET is necessary in lwipopts.h to use the EtcPal lwIP port."
#endif

bool ip_os_to_etcpal(const etcpal_os_ipaddr_t* os_ip, EtcPalIpAddr* ip)
{
#if LWIP_IPV4
  if (os_ip->sa_family == AF_INET)
  {
    struct sockaddr_in* sin = (struct sockaddr_in*)os_ip;
    ETCPAL_IP_SET_V4_ADDRESS(ip, lwip_ntohl(sin->sin_addr.s_addr));
    return true;
  }
#endif
#if LWIP_IPV6
  if (os_ip->sa_family == AF_INET6)
  {
    struct sockaddr_in6* sin6 = (struct sockaddr_in6*)os_ip;
    ETCPAL_IP_SET_V6_ADDRESS_WITH_SCOPE_ID(ip, sin6->sin6_addr.s6_addr, sin6->sin6_scope_id);
    return true;
  }
#endif
  return false;
}

size_t ip_etcpal_to_os(const EtcPalIpAddr* ip, etcpal_os_ipaddr_t* os_ip)
{
  size_t ret = 0;
#if LWIP_IPV4
  if (ETCPAL_IP_IS_V4(ip))
  {
    struct sockaddr_in* sin = (struct sockaddr_in*)os_ip;
    memset(sin, 0, sizeof(struct sockaddr_in));
    sin->sin_family = AF_INET;
    sin->sin_addr.s_addr = lwip_htonl(ETCPAL_IP_V4_ADDRESS(ip));
    ret = sizeof(struct sockaddr_in);
  }
#endif
#if LWIP_IPV6
  if (ETCPAL_IP_IS_V6(ip))
  {
    struct sockaddr_in6* sin6 = (struct sockaddr_in6*)os_ip;
    memset(sin6, 0, sizeof(struct sockaddr_in6));
    sin6->sin6_family = AF_INET6;
    sin6->sin6_scope_id = ip->addr.v6.scope_id;
    memcpy(sin6->sin6_addr.s6_addr, ETCPAL_IP_V6_ADDRESS(ip), ETCPAL_IPV6_BYTES);
    ret = sizeof(struct sockaddr_in6);
  }
#endif
  return ret;
}

bool sockaddr_os_to_etcpal(const etcpal_os_sockaddr_t* os_sa, EtcPalSockAddr* sa)
{
  if (ip_os_to_etcpal(os_sa, &sa->ip))
  {
#if LWIP_IPV4
    if (os_sa->sa_family == AF_INET)
    {
      sa->port = lwip_ntohs(((const struct sockaddr_in*)os_sa)->sin_port);
      return true;
    }
#endif
#if LWIP_IPV6
    if (os_sa->sa_family == AF_INET6)
    {
      sa->port = lwip_ntohs(((const struct sockaddr_in6*)os_sa)->sin6_port);
      return true;
    }
#endif
  }
  return false;
}

size_t sockaddr_etcpal_to_os(const EtcPalSockAddr* sa, etcpal_os_sockaddr_t* os_sa)
{
  size_t ret = ip_etcpal_to_os(&sa->ip, os_sa);
  if (ret != 0)
  {
#if LWIP_IPV4
    if (ETCPAL_IP_IS_V4(&sa->ip))
      ((struct sockaddr_in*)os_sa)->sin_port = lwip_htons(sa->port);
#endif
#if LWIP_IPV6
    if (ETCPAL_IP_IS_V6(&sa->ip))
      ((struct sockaddr_in6*)os_sa)->sin6_port = lwip_htons(sa->port);
#endif
  }
  return ret;
}

etcpal_error_t etcpal_inet_ntop(const EtcPalIpAddr* src, char* dest, size_t size)
{
  if (!src || !dest)
    return kEtcPalErrInvalid;

  switch (src->type)
  {
    case kEtcPalIpTypeV4: {
      struct in_addr addr;
      addr.s_addr = lwip_htonl(ETCPAL_IP_V4_ADDRESS(src));
      if (NULL != lwip_inet_ntop(AF_INET, &addr, dest, size))
        return kEtcPalErrOk;
      return errno_lwip_to_etcpal(errno);
    }
    case kEtcPalIpTypeV6: {
      struct in6_addr addr;
      memcpy(addr.s6_addr, ETCPAL_IP_V6_ADDRESS(src), ETCPAL_IPV6_BYTES);
      if (NULL != lwip_inet_ntop(AF_INET6, &addr, dest, size))
        return kEtcPalErrOk;
      return errno_lwip_to_etcpal(errno);
    }
    default:
      return kEtcPalErrInvalid;
  }
}

etcpal_error_t etcpal_inet_pton(etcpal_iptype_t type, const char* src, EtcPalIpAddr* dest)
{
  if (!src || !dest)
    return kEtcPalErrInvalid;

  switch (type)
  {
    case kEtcPalIpTypeV4: {
      struct in_addr addr;
      if (lwip_inet_pton(AF_INET, src, &addr) <= 0)
        return kEtcPalErrInvalid;
      ETCPAL_IP_SET_V4_ADDRESS(dest, lwip_ntohl(addr.s_addr));
      return kEtcPalErrOk;
    }
    case kEtcPalIpTypeV6: {
      struct in6_addr addr;
      if (lwip_inet_pton(AF_INET6, src, &addr) <= 0)
        return kEtcPalErrInvalid;
      ETCPAL_IP_SET_V6_ADDRESS(dest, addr.s6_addr);
      return kEtcPalErrOk;
    }
    default:
      return kEtcPalErrInvalid;
  }
}
