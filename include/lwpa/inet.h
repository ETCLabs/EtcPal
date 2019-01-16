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

/* lwpa/inet.h: POSIX-like identifiers for IP addresses, network interfaces and related items. */
#ifndef _LWPA_INET_H_
#define _LWPA_INET_H_

#include <string.h>
#include "lwpa/int.h"
#include "lwpa/bool.h"

/*! \defgroup lwpa_inet lwpa_inet
 *  \ingroup lwpa
 *  \brief POSIX-like identifiers for IP addresses and network interfaces.
 *
 *  \#include "lwpa/inet.h"
 *
 *  @{
 */

/*! Used to identify the type of IP address contained in a LwpaIpAddr. */
typedef enum
{
  /*! This LwpaIpAddr is not valid. */
  kLwpaIpTypeInvalid,
  /*! This LwpaIpAddr contains an IPv4 address. */
  kLwpaIpTypeV4,
  /*! This LwpaIpAddr contains an IPv6 address. */
  kLwpaIpTypeV6
} lwpa_iptype_t;

/*! The number of bytes in an IPv6 address. */
#define LWPA_IPV6_BYTES 16

/*! An IP address. Can hold either an IPv4 or IPv6 address. IPv4 addresses are in host byte
 *  order. */
typedef struct LwpaIpAddr
{
  /*! The IP address type (IPv4 or IPv6) */
  lwpa_iptype_t type;
  /*! The address, use either v4 or v6 depending on the value of type */
  union AddrUnion
  {
    uint32_t v4;
    uint8_t v6[LWPA_IPV6_BYTES];
  } addr;
} LwpaIpAddr;

/*! \name LwpaIpAddr macros
 *  A set of macros for interacting with a LwpaIpAddr. It is recommended to use these macros where
 *  possible rather than accessing the structure members directly; otherwise, it's easy to forget
 *  to fill in a field.
 *  @{
 */

/*! Determine whether a LwpaIpAddr contains an IPv4 address.
 *  \param lwpa_ip_ptr Pointer to a LwpaIpAddr.
 *  \return true (contains an IPv4 address) or false (does not contain an IPv4 address). */
#define lwpaip_is_v4(lwpa_ip_ptr) ((lwpa_ip_ptr)->type == kLwpaIpTypeV4)
/*! Determine whether a LwpaIpAddr contains an IPv6 address.
 *  \param lwpa_ip_ptr Pointer to a LwpaIpAddr.
 *  \return true (contains an IPv6 address) or false (does not contain an IPv6 address). */
#define lwpaip_is_v6(lwpa_ip_ptr) ((lwpa_ip_ptr)->type == kLwpaIpTypeV6)
/*! Determine whether a LwpaIpAddr contains an invalid address.
 *  \param lwpa_ip_ptr Pointer to a LwpaIpAddr.
 *  \return true (is invalid) or false (is not invalid). */
#define lwpaip_is_invalid(lwpa_ip_ptr) ((lwpa_ip_ptr)->type == kLwpaIpTypeInvalid)
/*! Get the IPv4 address from a LwpaIpAddr. It is recommended to first use lwpaip_is_v4() to make
 *  sure this LwpaIpAddr contains a valid IPv4 address.
 *  \param lwpa_ip_ptr Pointer to a LwpaIpAddr.
 *  \return The IPv4 address (uint32_t). */
#define lwpaip_v4_address(lwpa_ip_ptr) ((lwpa_ip_ptr)->addr.v4)
/*! Get the IPv6 address from a LwpaIpAddr. It is recommended to first use lwpaip_is_v6() to make
 *  sure this LwpaIpAddr contains a valid IPv6 address.
 *  \param lwpa_ip_ptr Pointer to a LwpaIpAddr.
 *  \return The IPv6 address (uint8_t[]). */
#define lwpaip_v6_address(lwpa_ip_ptr) ((lwpa_ip_ptr)->addr.v6)
/*! Set the IPv4 address in a LwpaIpAddr. Also sets the type field to indicate that this LwpaIpAddr
 *  contains an IPv4 address.
 *  \param lwpa_ip_ptr Pointer to a LwpaIpAddr.
 *  \param val IPv4 address to set (uint32_t). */
#define lwpaip_set_v4_address(lwpa_ip_ptr, val) \
  do                                            \
  {                                             \
    (lwpa_ip_ptr)->type = kLwpaIpTypeV4;        \
    (lwpa_ip_ptr)->addr.v4 = val;               \
  } while (0)
/*! Set the IPv6 address in a LwpaIpAddr. Also sets the type field to indicate that this LwpaIpAddr
 *  contains an IPv6 address.
 *  \param lwpa_ip_ptr Pointer to a LwpaIpAddr.
 *  \param val IPv6 address to set (uint8_t[]). Must be at least of length LWPA_IPV6_BYTES. Gets
 *             copied into the struct. */
#define lwpaip_set_v6_address(lwpa_ip_ptr, val)           \
  do                                                      \
  {                                                       \
    (lwpa_ip_ptr)->type = kLwpaIpTypeV6;                  \
    memcpy((lwpa_ip_ptr)->addr.v6, val, LWPA_IPV6_BYTES); \
  } while (0)
/*! Set the type field in a LwpaIpAddr to indicate that it does not contain a valid address.
 *  \param lwpa_ip_ptr Pointer to a LwpaIpAddr. */
#define lwpaip_set_invalid(lwpa_ip_ptr) ((lwpa_ip_ptr)->type = kLwpaIpTypeInvalid)
/*! Determine whether a LwpaIpAddr contains a multicast address. Works for either an IPv4 or an
 *  IPv6 address.
 *  \param lwpa_ip_ptr Pointer to a LwpaIpAddr.
 *  \return true (contains a multicast address) or false (does not contain a multicast address). */
#define lwpaip_is_multicast(lwpa_ip_ptr)                                                  \
  (((lwpa_ip_ptr)->type == kLwpaIpTypeV4)                                                 \
       ? (((lwpa_ip_ptr)->addr.v4 > 0xe0000000) && ((lwpa_ip_ptr)->addr.v4 < 0xefffffff)) \
       : ((lwpa_ip_ptr)->addr.v6[0] == 0xff))
/*! Determine whether two instances of LwpaIpAddr contain identical addresses. The type (IPv4 or
 *  IPv6) must be the same, as well as the value of the relevant address.
 *  \param ipptr1 Pointer to the first LwpaIpAddr to compare.
 *  \param ipptr2 Pointer to the second LwpaIpAddr to compare.
 *  \return true (IPs are identical) or false (IPs are not identical). */
#define lwpaip_equal(ipptr1, ipptr2)                                                                                \
  (((ipptr1)->type == (ipptr2)->type)                                                                               \
       ? (((ipptr1)->type == kLwpaIpTypeV4) ? ((ipptr1)->addr.v4 == (ipptr2)->addr.v4)                              \
                                            : (0 == memcmp((ipptr1)->addr.v6, (ipptr2)->addr.v6, LWPA_IPV6_BYTES))) \
       : false)
/*! A comparison algorithm for lwpa_ip addresses. All IPv4 addresses are considered to be < all
 *  IPv6 addresses. For matching types, the numerical address value is compared.
 *  \param ipptr1 Pointer to the first LwpaIpAddr to compare.
 *  \param ipptr2 Pointer to the second LwpaIpAddr to compare.
 *  \return < 0 (ipptr1 < ipptr2), 0 (ipptr1 == ipptr2), > 0 (ipptr1 > ipptr2)
 */
#define lwpaip_cmp(ipptr1, ipptr2)                                                              \
  (((ipptr1)->type != (ipptr2)->type)                                                           \
       ? ((ipptr1)->type - (ipptr2)->type)                                                      \
       : (((ipptr1)->type == kLwpaIpTypeV4) ? ((int)(ipptr1)->addr.v4 - (int)(ipptr2)->addr.v4) \
                                            : memcmp((ipptr1)->addr.v6, (ipptr2)->addr.v6, LWPA_IPV6_BYTES)))

#define LWPA_INADDR_ANY 0

/*! Set a LwpaIpAddr to the IPv4 wildcard address. Use this macro; do not use LWPA_INADDR_ANY
 *  directly.
 *  \param lwpa_ip_ptr Pointer to a LwpaIpAddr. */
#define lwpaip_make_any_v4(lwpa_ip_ptr) lwpaip_set_v4_address(lwpa_ip_ptr, LWPA_INADDR_ANY)
/*! Set a LwpaIpAddr to the IPv6 wildcard address.
 *  \param lwpa_ip_ptr Pointer to a LwpaIpAddr. */
#define lwpaip_make_any_v6(lwpa_ip_ptr)                          \
  do                                                             \
  {                                                              \
    (lwpa_ip_ptr)->type = kLwpaIpTypeV6;                         \
    memset(lwpa_ip_v6_address(lwpa_ip_ptr), 0, LWPA_IPV6_BYTES); \
  } while (0)
/*!@}*/

/*! An IP address with associated interface and port. Ports are in host byte order. */
typedef struct LwpaSockaddr
{
  uint16_t port;     /*!< TCP or UDP port. */
  LwpaIpAddr ip;     /*!< IP address. */
  uint32_t scope_id; /*!< IPv6 scope ID. */
} LwpaSockaddr;

/*! Determine whether two instances of LwpaSockaddr contain identical IP addresses and ports.
 *  \param sockptr1 Pointer to first LwpaSockaddr to compare.
 *  \param sockptr2 Pointer to second LwpaSockaddr to compare.
 *  \return true (the IP address and port are identical) or false (the IP address and port are not
 *          identical). */
#define lwpasock_ip_port_equal(sockptr1, sockptr2) \
  (lwpaip_equal(&(sockptr1)->ip, &(sockptr2)->ip) && ((sockptr1)->port == (sockptr2)->port))

#define LWPA_NETINTINFO_MAC_LEN 6
#define LWPA_NETINTINFO_NAME_LEN 64

/*! A description of a network interface. */
typedef struct LwpaNetintInfo
{
  /*! The OS-specific network interface number. Not used on all OSes. May need to be casted away
   *  from int (e.g. unsigned int) on your OS. */
  int ifindex;
  /*! The interface ip address. */
  LwpaIpAddr addr;
  /*! The subnet mask for this interface. */
  LwpaIpAddr mask;
  /*! The address of the default gateway for this interface. */
  LwpaIpAddr gate;
  /*! The adapter MAC address. */
  uint8_t mac[LWPA_NETINTINFO_MAC_LEN];
  /*! The adapter name as a string. */
  char name[LWPA_NETINTINFO_NAME_LEN];
  /*! Whether this is the default network interface. */
  bool is_default;
} LwpaNetintInfo;

/*!@}*/

#endif /* _LWPA_INET_H_ */
