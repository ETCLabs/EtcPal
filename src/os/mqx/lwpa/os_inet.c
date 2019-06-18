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

#include <mqx.h>
#include <rtcs.h>

bool sockaddr_os_to_lwpa(LwpaSockaddr* sa, const struct sockaddr* pfsa)
{
  if (pfsa->sa_family == AF_INET)
  {
    struct sockaddr_in* sin = (struct sockaddr_in*)pfsa;
    sa->port = sin->sin_port;
    LWPA_IP_SET_V4_ADDRESS(&sa->ip, sin->sin_addr.s_addr);
    return true;
  }
  else if (pfsa->sa_family == AF_INET6)
  {
    struct sockaddr_in6* sin6 = (struct sockaddr_in6*)pfsa;
    sa->port = sin6->sin6_port;
    LWPA_IP_SET_V6_ADDRESS(&sa->ip, sin6->sin6_addr.s6_addr);
    return true;
  }
  return false;
}

size_t sockaddr_lwpa_to_os(struct sockaddr* pfsa, const LwpaSockaddr* sa)
{
  size_t ret = 0;
  if (LWPA_IP_IS_V4(&sa->ip))
  {
    struct sockaddr_in* sin = (struct sockaddr_in*)pfsa;
    sin->sin_family = AF_INET;
    sin->sin_port = sa->port;
    sin->sin_addr.s_addr = LWPA_IP_V4_ADDRESS(&sa->ip);
    ret = sizeof(struct sockaddr_in);
  }
  else if (LWPA_IP_IS_V6(&sa->ip))
  {
    struct sockaddr_in6* sin6 = (struct sockaddr_in6*)pfsa;
    sin6->sin6_family = AF_INET6;
    sin6->sin6_port = sa->port;
    memcpy(sin6->sin6_addr.s6_addr, LWPA_IP_V6_ADDRESS(&sa->ip), IPV6_BYTES);
    ret = sizeof(struct sockaddr_in6);
  }
  return ret;
}

lwpa_error_t lwpa_inet_ntop(const LwpaIpAddr* src, char* dest, size_t size)
{
  if (!src || !dest)
    return kLwpaErrInvalid;

  switch (src->type)
  {
    case LWPA_IPV4:
    {
      struct in_addr addr;
      /* RTCS expects host byte order in their in_addrs. Thus no htonl is needed. */
      addr.s_addr = LWPA_IP_V4_ADDRESS(src);
      if (NULL != inet_ntop(AF_INET, &addr, dest, size))
        return kLwpaErrOk;
      return kLwpaErrSys;
    }
    case LWPA_IPV6:
    {
      struct in6_addr addr;
      memcpy(addr.s6_addr, LWPA_IP_V6_ADDRESS(src), IPV6_BYTES);
      if (NULL != inet_ntop(AF_INET6, &addr, dest, size))
        return kLwpaErrOk;
      return kLwpaErrSys;
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
    case LWPA_IPV4:
    {
      struct in_addr addr;
      if (RTCS_OK != inet_pton(AF_INET, src, &addr, sizeof addr))
        return kLwpaErrSys;
      /* RTCS gives us host byte order in their in_addrs. Thus no htonl is needed. */
      LWPA_IP_SET_V4_ADDRESS(dest, addr.s_addr);
      return kLwpaErrOk;
    }
    case LWPA_IPV6:
    {
      struct in6_addr addr;
      if (RTCS_OK != inet_pton(AF_INET6, src, &addr, sizeof addr))
        return kLwpaErrSys;
      LWPA_IP_SET_V6_ADDRESS(dest, addr.s6_addr);
      return kLwpaErrOk;
    }
    default:
      return kLwpaErrInvalid;
  }
}

