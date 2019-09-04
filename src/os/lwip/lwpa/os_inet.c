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

#include "lwpa/inet.h"

#include <lwip/sockets.h>
#include "os_error.h"

#if !LWIP_SOCKET
#error "LWIP_SOCKET is necessary in lwipopts.h to use the lwpa_inet module."
#endif

bool ip_os_to_lwpa(const lwpa_os_ipaddr_t* os_ip, LwpaIpAddr* ip)
{
#if LWIP_IPV4
  if (os_ip->sa_family == AF_INET)
  {
    struct sockaddr_in* sin = (struct sockaddr_in*)os_ip;
    LWPA_IP_SET_V4_ADDRESS(ip, ntohl(sin->sin_addr.s_addr));
    return true;
  }
#endif
#if LWIP_IPV6
  if (os_ip->sa_family == AF_INET6)
  {
    struct sockaddr_in6* sin6 = (struct sockaddr_in6*)os_ip;
    LWPA_IP_SET_V6_ADDRESS_WITH_SCOPE_ID(ip, sin6->sin6_addr.s6_addr, sin6->sin6_scope_id);
    return true;
  }
#endif
  return false;
}

size_t ip_lwpa_to_os(const LwpaIpAddr* ip, lwpa_os_ipaddr_t* os_ip)
{
  size_t ret = 0;
#if LWIP_IPV4
  if (LWPA_IP_IS_V4(ip))
  {
    struct sockaddr_in* sin = (struct sockaddr_in*)os_ip;
    memset(sin, 0, sizeof(struct sockaddr_in));
    sin->sin_len = sizeof(struct sockaddr_in6);
    sin->sin_family = AF_INET;
    sin->sin_addr.s_addr = htonl(LWPA_IP_V4_ADDRESS(ip));
    ret = sizeof(struct sockaddr_in);
  }
#endif
#if LWIP_IPV6
  if (LWPA_IP_IS_V6(ip))
  {
    struct sockaddr_in6* sin6 = (struct sockaddr_in6*)os_ip;
    memset(sin6, 0, sizeof(struct sockaddr_in6));
    sin6->sin6_len = sizeof(struct sockaddr_in6);
    sin6->sin6_family = AF_INET6;
    sin6->sin6_scope_id = ip->addr.v6.scope_id;
    memcpy(sin6->sin6_addr.s6_addr, LWPA_IP_V6_ADDRESS(ip), LWPA_IPV6_BYTES);
    ret = sizeof(struct sockaddr_in6);
  }
#endif
  return ret;
}

bool sockaddr_os_to_lwpa(const lwpa_os_sockaddr_t* os_sa, LwpaSockaddr* sa)
{
  if (ip_os_to_lwpa(os_sa, &sa->ip))
  {
#if LWIP_IPV4
    if (os_sa->sa_family == AF_INET)
    {
      sa->port = ntohs(((const struct sockaddr_in*)os_sa)->sin_port);
      return true;
    }
#endif
#if LWIP_IPV6
    if (os_sa->sa_family == AF_INET6)
    {
      sa->port = ntohs(((const struct sockaddr_in6*)os_sa)->sin6_port);
      return true;
    }
#endif
  }
  return false;
}

size_t sockaddr_lwpa_to_os(const LwpaSockaddr* sa, lwpa_os_sockaddr_t* os_sa)
{
  size_t ret = ip_lwpa_to_os(&sa->ip, os_sa);
  if (ret != 0)
  {
#if LWIP_IPV4
    if (LWPA_IP_IS_V4(&sa->ip))
      ((struct sockaddr_in*)os_sa)->sin_port = htons(sa->port);
#endif
#if LWIP_IPV6
    if (LWPA_IP_IS_V6(&sa->ip))
      ((struct sockaddr_in6*)os_sa)->sin6_port = htons(sa->port);
#endif
  }
  return ret;
}

lwpa_error_t lwpa_inet_ntop(const LwpaIpAddr* src, char* dest, size_t size)
{
  if (!src || !dest)
    return kLwpaErrInvalid;

  switch (src->type)
  {
    case kLwpaIpTypeV4:
    {
      struct in_addr addr;
      addr.s_addr = htonl(LWPA_IP_V4_ADDRESS(src));
      if (NULL != inet_ntop(AF_INET, &addr, dest, size))
        return kLwpaErrOk;
      return errno_lwip_to_lwpa(errno);
    }
    case kLwpaIpTypeV6:
    {
      struct in6_addr addr;
      memcpy(addr.s6_addr, LWPA_IP_V6_ADDRESS(src), LWPA_IPV6_BYTES);
      if (NULL != inet_ntop(AF_INET6, &addr, dest, size))
        return kLwpaErrOk;
      return errno_lwip_to_lwpa(errno);
    }
    default:
      return kLwpaErrInvalid;
  }
}

lwpa_error_t lwpa_inet_pton(lwpa_iptype_t type, const char* src, LwpaIpAddr* dest)
{
  if (!src || !dest)
    return kLwpaErrInvalid;

  switch (type)
  {
    case kLwpaIpTypeV4:
    {
      struct in_addr addr;
      if (inet_pton(AF_INET, src, &addr) <= 0)
        return kLwpaErrInvalid;
      LWPA_IP_SET_V4_ADDRESS(dest, ntohl(addr.s_addr));
      return kLwpaErrOk;
    }
    case kLwpaIpTypeV6:
    {
      struct in6_addr addr;
      if (inet_pton(AF_INET6, src, &addr) <= 0)
        return kLwpaErrInvalid;
      LWPA_IP_SET_V6_ADDRESS(dest, addr.s6_addr);
      return kLwpaErrOk;
    }
    default:
      return kLwpaErrInvalid;
  }
}
