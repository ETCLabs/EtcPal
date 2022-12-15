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

#ifndef ETCPAL_OS_INET_H_
#define ETCPAL_OS_INET_H_

#include <zephyr/net/socket.h>

#if defined(CONFIG_NET_NATIVE_IPV4)
  #if CONFIG_NET_IF_UNICAST_IPV4_ADDR_COUNT > 1
    #error ("ETCPal Allows only 1 IP configuration per interface. Adjust Kconfig value of NET_IF_UNICAST_IPV4_ADDR_COUNT")
  #elseif CONFIG_NET_IF_MCAST_IPV4_ADDR_COUNT > 1
    #error ("ETCPal Allows only 1 IP configuration per interface. Adjust Kconfig value of NET_IF_UNICAST_IPV6_ADDR_COUNT")
  #endif
#endif

typedef struct sockaddr etcpal_os_sockaddr_t;
typedef struct sockaddr etcpal_os_ipaddr_t;

#endif /* ETCPAL_OS_INET_H_ */
