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

/* etcpal/inet.h: POSIX-like identifiers for IP addresses, network interfaces and related items. */
#ifndef _LWPA_INET_H_
#define _LWPA_INET_H_

#include <string.h>
#include "etcpal/bool.h"
#include "etcpal/error.h"
#include "etcpal/int.h"
#include "etcpal/os_inet.h"

/*! \defgroup lwpa_inet lwpa_inet
 *  \ingroup lwpa
 *  \brief POSIX-like identifiers for IP addresses and network interfaces.
 *
 *  \#include "etcpal/inet.h"
 *
 *  @{
 */

#ifdef __cplusplus
extern "C" {
#endif

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

/*! \brief An IP address.
 *
 *  Can hold either an IPv4 or IPv6 address. IPv4 addresses are in host byte order. IPv6 addresses
 *  also contain a scope ID, which is also sometimes referred to as a zone index (RFC 4007), to
 *  help disambiguate link-local addresses, among other uses. In most cases, this field can be left
 *  at its default value, which is set by the LWPA_IP_SET_V6_ADDRESS() macro.
 */
typedef struct LwpaIpAddr
{
  /*! The IP address type (IPv4 or IPv6) */
  lwpa_iptype_t type;
  /*! The address (use the macros to access), either v4 or v6 depending on the value of type. */
  union AddrUnion
  {
    uint32_t v4;
    struct LwpaIpv6Addr
    {
      uint8_t addr_buf[LWPA_IPV6_BYTES];
      unsigned long scope_id;
    } v6;
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
#define LWPA_IP_IS_V4(lwpa_ip_ptr) ((lwpa_ip_ptr)->type == kLwpaIpTypeV4)

/*! Determine whether a LwpaIpAddr contains an IPv6 address.
 *  \param lwpa_ip_ptr Pointer to a LwpaIpAddr.
 *  \return true (contains an IPv6 address) or false (does not contain an IPv6 address). */
#define LWPA_IP_IS_V6(lwpa_ip_ptr) ((lwpa_ip_ptr)->type == kLwpaIpTypeV6)

/*! Determine whether a LwpaIpAddr contains an invalid address.
 *  \param lwpa_ip_ptr Pointer to a LwpaIpAddr.
 *  \return true (is invalid) or false (is not invalid). */
#define LWPA_IP_IS_INVALID(lwpa_ip_ptr) ((lwpa_ip_ptr)->type == kLwpaIpTypeInvalid)

/*! Get the IPv4 address from a LwpaIpAddr. It is recommended to first use LWPA_IP_IS_V4() to make
 *  sure this LwpaIpAddr contains a valid IPv4 address.
 *  \param lwpa_ip_ptr Pointer to a LwpaIpAddr.
 *  \return The IPv4 address (uint32_t). */
#define LWPA_IP_V4_ADDRESS(lwpa_ip_ptr) ((lwpa_ip_ptr)->addr.v4)

/*! Get the IPv6 address from a LwpaIpAddr. It is recommended to first use LWPA_IP_IS_V6() to make
 *  sure this LwpaIpAddr contains a valid IPv6 address.
 *  \param lwpa_ip_ptr Pointer to a LwpaIpAddr.
 *  \return The IPv6 address (uint8_t[]). */
#define LWPA_IP_V6_ADDRESS(lwpa_ip_ptr) ((lwpa_ip_ptr)->addr.v6.addr_buf)

/*! Set the IPv4 address in a LwpaIpAddr. Also sets the type field to indicate that this LwpaIpAddr
 *  contains an IPv4 address.
 *  \param lwpa_ip_ptr Pointer to a LwpaIpAddr.
 *  \param val IPv4 address to set (uint32_t). */
#define LWPA_IP_SET_V4_ADDRESS(lwpa_ip_ptr, val) \
  do                                             \
  {                                              \
    (lwpa_ip_ptr)->type = kLwpaIpTypeV4;         \
    (lwpa_ip_ptr)->addr.v4 = val;                \
  } while (0)

/*! Set the IPv6 address in a LwpaIpAddr. Also sets the type field to indicate that this LwpaIpAddr
 *  contains an IPv6 address.
 *  \param lwpa_ip_ptr Pointer to a LwpaIpAddr.
 *  \param addr_val IPv6 address to set (uint8_t[]). Must be at least of length #LWPA_IPV6_BYTES.
 *                  Gets copied into the struct. */
#define LWPA_IP_SET_V6_ADDRESS(lwpa_ip_ptr, addr_val) LWPA_IP_SET_V6_ADDRESS_WITH_SCOPE_ID(lwpa_ip_ptr, addr_val, 0u)

/*! Set an IPv6 address with an explicit scope ID in a LwpaIpAddr. Also sets the type field to
 *  indicate that this LwpaIpAddr contains an IPv6 address.
 *  \param lwpa_ip_ptr Pointer to a LwpaIpAddr.
 *  \param addr_val IPv6 address to set (uint8_t[]). Must be at least of length #LWPA_IPV6_BYTES.
 *                  Gets copied into the struct.
 *  \param scope_id_val IPv6 scope ID to set. */
#define LWPA_IP_SET_V6_ADDRESS_WITH_SCOPE_ID(lwpa_ip_ptr, addr_val, scope_id_val) \
  do                                                                              \
  {                                                                               \
    (lwpa_ip_ptr)->type = kLwpaIpTypeV6;                                          \
    memcpy((lwpa_ip_ptr)->addr.v6.addr_buf, (addr_val), LWPA_IPV6_BYTES);         \
    (lwpa_ip_ptr)->addr.v6.scope_id = (scope_id_val);                             \
  } while (0)

/*! Set the type field in a LwpaIpAddr to indicate that it does not contain a valid address.
 *  \param lwpa_ip_ptr Pointer to a LwpaIpAddr. */
#define LWPA_IP_SET_INVALID(lwpa_ip_ptr) ((lwpa_ip_ptr)->type = kLwpaIpTypeInvalid)

/*! @} */

/*! An IP address with associated interface and port. Ports are in host byte order. */
typedef struct LwpaSockaddr
{
  uint16_t port; /*!< TCP or UDP port. */
  LwpaIpAddr ip; /*!< IP address. */
} LwpaSockaddr;

#define LWPA_NETINTINFO_MAC_LEN 6
#define LWPA_NETINTINFO_NAME_LEN 64
#define LWPA_NETINTINFO_FRIENDLY_NAME_LEN 64

/*! A description of a single address assigned to a network interface. */
typedef struct LwpaNetintInfo
{
  /*! The OS-specific network interface number. Since interfaces can have multiple IPv4 and IPv6
   *  addresses assigned simultaneously, there can be a one-to-many relationship between physical
   *  network interfaces and LwpaNetintInfo descriptions on the same system, all of which will have
   *  the same value for this field. It is also used for IPv6, multicast and IP-version-neutral
   *  APIs. See \ref interface_indexes for more information. */
  unsigned int index;
  /*! The interface ip address. */
  LwpaIpAddr addr;
  /*! The subnet mask for this interface. */
  LwpaIpAddr mask;
  /*! The adapter MAC address. */
  uint8_t mac[LWPA_NETINTINFO_MAC_LEN];
  /*! The system name for the interface. This name will not change unless the adapter is removed or
   *  reconfigured. */
  char name[LWPA_NETINTINFO_NAME_LEN];
  /*! A user-friendly name for the interface. On some systems, this is the same as the name field.
   *  Others allow users to create and change a friendly name for network interfaces that's
   *  different than the system name. This field should be used when printing the adapter list in a
   *  UI. */
  char friendly_name[LWPA_NETINTINFO_FRIENDLY_NAME_LEN];
  /*! Whether this is the default network interface. The default network interface is defined as
   *  the network interface chosen for the default IP route on a system. */
  bool is_default;
} LwpaNetintInfo;

/*! Maximum length of the string representation of an IPv4 address. */
#define LWPA_INET_ADDRSTRLEN 16
/*! Maximum length of the string representation of an IPv6 address. */
#define LWPA_INET6_ADDRSTRLEN 46

bool lwpa_ip_is_link_local(const LwpaIpAddr* ip);
bool lwpa_ip_is_loopback(const LwpaIpAddr* ip);
bool lwpa_ip_is_multicast(const LwpaIpAddr* ip);
bool lwpa_ip_is_wildcard(const LwpaIpAddr* ip);
void lwpa_ip_set_wildcard(lwpa_iptype_t type, LwpaIpAddr* ip);

bool lwpa_ip_equal(const LwpaIpAddr* ip1, const LwpaIpAddr* ip2);
int lwpa_ip_cmp(const LwpaIpAddr* ip1, const LwpaIpAddr* ip2);
bool lwpa_ip_and_port_equal(const LwpaSockaddr* sock1, const LwpaSockaddr* sock2);

unsigned int lwpa_ip_mask_length(const LwpaIpAddr* netmask);
LwpaIpAddr lwpa_ip_mask_from_length(lwpa_iptype_t type, unsigned int mask_length);
bool lwpa_ip_network_portions_equal(const LwpaIpAddr* ip1, const LwpaIpAddr* ip2, const LwpaIpAddr* netmask);

bool ip_os_to_lwpa(const lwpa_os_ipaddr_t* os_ip, LwpaIpAddr* ip);
size_t ip_lwpa_to_os(const LwpaIpAddr* ip, lwpa_os_ipaddr_t* os_ip);

bool sockaddr_os_to_lwpa(const lwpa_os_sockaddr_t* os_sa, LwpaSockaddr* sa);
size_t sockaddr_lwpa_to_os(const LwpaSockaddr* sa, lwpa_os_sockaddr_t* os_sa);

lwpa_error_t lwpa_inet_ntop(const LwpaIpAddr* src, char* dest, size_t size);
lwpa_error_t lwpa_inet_pton(lwpa_iptype_t type, const char* src, LwpaIpAddr* dest);

#ifdef __cplusplus
}
#endif

/*! @} */

#endif /* _LWPA_INET_H_ */
