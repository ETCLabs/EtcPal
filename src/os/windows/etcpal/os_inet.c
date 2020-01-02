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
 * This file is a part of EtcPal. For more information, go to:
 * https://github.com/ETCLabs/EtcPal
 ******************************************************************************/

#include "etcpal/inet.h"
#include <ws2tcpip.h>
#include "os_error.h"

bool ip_os_to_etcpal(const etcpal_os_ipaddr_t* os_ip, EtcPalIpAddr* ip)
{
  if (os_ip->sa_family == AF_INET)
  {
    const struct sockaddr_in* sin = (const struct sockaddr_in*)os_ip;
    ETCPAL_IP_SET_V4_ADDRESS(ip, ntohl(sin->sin_addr.s_addr));
    return true;
  }
  else if (os_ip->sa_family == AF_INET6)
  {
    const struct sockaddr_in6* sin6 = (const struct sockaddr_in6*)os_ip;
    ETCPAL_IP_SET_V6_ADDRESS_WITH_SCOPE_ID(ip, sin6->sin6_addr.s6_addr, sin6->sin6_scope_id);
    return true;
  }
  return false;
}

size_t ip_etcpal_to_os(const EtcPalIpAddr* ip, etcpal_os_ipaddr_t* os_ip)
{
  size_t ret = 0;
  if (ETCPAL_IP_IS_V4(ip))
  {
    struct sockaddr_in* sin = (struct sockaddr_in*)os_ip;
    memset(sin, 0, sizeof(struct sockaddr_in));
    sin->sin_family = AF_INET;
    sin->sin_addr.s_addr = htonl(ETCPAL_IP_V4_ADDRESS(ip));
    ret = sizeof(struct sockaddr_in);
  }
  else if (ETCPAL_IP_IS_V6(ip))
  {
    struct sockaddr_in6* sin6 = (struct sockaddr_in6*)os_ip;
    memset(sin6, 0, sizeof(struct sockaddr_in6));
    sin6->sin6_family = AF_INET6;
    sin6->sin6_scope_id = (ULONG)ETCPAL_IP_V6_SCOPE_ID(ip);
    memcpy(sin6->sin6_addr.s6_addr, ETCPAL_IP_V6_ADDRESS(ip), ETCPAL_IPV6_BYTES);
    ret = sizeof(struct sockaddr_in6);
  }
  return ret;
}

bool sockaddr_os_to_etcpal(const etcpal_os_sockaddr_t* os_sa, EtcPalSockAddr* sa)
{
  if (ip_os_to_etcpal(os_sa, &sa->ip))
  {
    if (os_sa->sa_family == AF_INET)
    {
      sa->port = ntohs(((const struct sockaddr_in*)os_sa)->sin_port);
      return true;
    }
    else if (os_sa->sa_family == AF_INET6)
    {
      sa->port = ntohs(((const struct sockaddr_in6*)os_sa)->sin6_port);
      return true;
    }
  }
  return false;
}

size_t sockaddr_etcpal_to_os(const EtcPalSockAddr* sa, etcpal_os_sockaddr_t* os_sa)
{
  size_t ret = ip_etcpal_to_os(&sa->ip, os_sa);
  if (ret != 0)
  {
    if (ETCPAL_IP_IS_V4(&sa->ip))
      ((struct sockaddr_in*)os_sa)->sin_port = htons(sa->port);
    else if (ETCPAL_IP_IS_V6(&sa->ip))
      ((struct sockaddr_in6*)os_sa)->sin6_port = htons(sa->port);
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
      addr.s_addr = htonl(ETCPAL_IP_V4_ADDRESS(src));
      if (NULL != inet_ntop(AF_INET, &addr, dest, size))
        return kEtcPalErrOk;
      return err_winsock_to_etcpal(WSAGetLastError());
    }
    case kEtcPalIpTypeV6: {
      struct in6_addr addr;
      memcpy(addr.s6_addr, ETCPAL_IP_V6_ADDRESS(src), ETCPAL_IPV6_BYTES);
      if (NULL != inet_ntop(AF_INET6, &addr, dest, size))
        return kEtcPalErrOk;
      return err_winsock_to_etcpal(WSAGetLastError());
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
      INT res = inet_pton(AF_INET, src, &addr);
      if (res == 0)
        return kEtcPalErrInvalid;
      else if (res < 0)
        return err_winsock_to_etcpal(WSAGetLastError());

      ETCPAL_IP_SET_V4_ADDRESS(dest, ntohl(addr.s_addr));
      return kEtcPalErrOk;
    }
    case kEtcPalIpTypeV6: {
      struct in6_addr addr;
      INT res = inet_pton(AF_INET6, src, &addr);
      if (res == 0)
        return kEtcPalErrInvalid;
      else if (res < 0)
        return err_winsock_to_etcpal(WSAGetLastError());

      ETCPAL_IP_SET_V6_ADDRESS(dest, addr.s6_addr);
      return kEtcPalErrOk;
    }
    default:
      return kEtcPalErrInvalid;
  }
}
