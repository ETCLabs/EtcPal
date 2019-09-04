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

/**************************** Private variables ******************************/

static const uint8_t v6_wildcard[ETCPAL_IPV6_BYTES] = {0};
static const uint8_t v6_loopback[ETCPAL_IPV6_BYTES] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1};

/*************************** Function definitions ****************************/

/*! \brief Determine whether a EtcPalIpAddr contains a link-local address.
 *
 *  Works for both IPv4 and IPv6 addresses.
 *
 *  \param[in] ip Address to check.
 *  \return true: ip contains a link-local address.
 *  \return false: ip does not contain a link-local address.
 */
bool etcpal_ip_is_link_local(const EtcPalIpAddr* ip)
{
  if (ip)
  {
    if (ETCPAL_IP_IS_V4(ip))
    {
      // An IPv4 address is link-local if the first two octets are 0xa9fe (169.254.0.0/16)
      return ((ETCPAL_IP_V4_ADDRESS(ip) & 0xffff0000u) == 0xa9fe0000u);
    }
    else if (ETCPAL_IP_IS_V6(ip))
    {
      // An IPv6 address is link-local if the first 10 bits are 1111111010 (fe80::/10)
      const uint8_t* addr_buf = ETCPAL_IP_V6_ADDRESS(ip);
      return (addr_buf[0] == 0xfeu && ((addr_buf[1] & 0xc0u) == 0x80u));
    }
  }
  return false;
}

/*! \brief Determine whether a EtcPalIpAddr contains a loopback address.
 *
 *  Works for both IPv4 and IPv6 addresses.
 *
 *  \param[in] ip Address to check.
 *  \return true: ip contains a loopback address.
 *  \return false: ip does not contain a loopback address.
 */
bool etcpal_ip_is_loopback(const EtcPalIpAddr* ip)
{
  if (ip)
  {
    if (ETCPAL_IP_IS_V4(ip))
    {
      // An IPv4 address is loopback if the first octet is 0x7f (127.0.0.0/8)
      return ((ETCPAL_IP_V4_ADDRESS(ip) & 0xff000000u) == 0x7f000000u);
    }
    else if (ETCPAL_IP_IS_V6(ip))
    {
      // An IPv6 address is loopback if it is equal to the address ::1
      return (0 == memcmp(ETCPAL_IP_V6_ADDRESS(ip), v6_loopback, ETCPAL_IPV6_BYTES));
    }
  }
  return false;
}

/*! \brief Determine whether a EtcPalIpAddr contains a multicast address.
 *
 *  Works for both IPv4 and IPv6 addresses.
 *
 *  \param[in] ip Address to check.
 *  \return true: ip contains a multicast address.
 *  \return false: ip does not contain a multicast address.
 */
bool etcpal_ip_is_multicast(const EtcPalIpAddr* ip)
{
  if (ip)
  {
    if (ETCPAL_IP_IS_V4(ip))
    {
      // An IPv4 address is multicast if it is in the range 224.0.0.0 to 239.255.255.255, inclusive
      // (224.0.0.0/4)
      return (ETCPAL_IP_V4_ADDRESS(ip) >= 0xe0000000u && ETCPAL_IP_V4_ADDRESS(ip) <= 0xefffffffu);
    }
    else if (ETCPAL_IP_IS_V6(ip))
    {
      // An IPv6 address is multicast if the first octet is 0xff (ff00::/8)
      return (ETCPAL_IP_V6_ADDRESS(ip)[0] == 0xff);
    }
  }
  return false;
}

/*! \brief Determine whether a EtcPalIpAddr contains a wildcard address.
 *
 *  Works for both IPv4 and IPv6 addresses. The wildcard address is used as an argument to
 *  etcpal_bind() to indicate that a socket should be bound to all available network interfaces. It
 *  should not be used as a placeholder or invalid address - use ETCPAL_IP_SET_INVALID() and
 *  ETCPAL_IP_IS_INVALID() for that.
 *
 *  \param[in] ip Address to check.
 *  \return true: ip contains a wildcard address.
 *  \return false: ip does not contain a wildcard address.
 */
bool etcpal_ip_is_wildcard(const EtcPalIpAddr* ip)
{
  if (ip)
  {
    // Wildcard addresses are all-zeroes
    if (ETCPAL_IP_IS_V4(ip))
    {
      return (ETCPAL_IP_V4_ADDRESS(ip) == 0);
    }
    else if (ETCPAL_IP_IS_V6(ip))
    {
      return (0 == memcmp(ETCPAL_IP_V6_ADDRESS(ip), v6_wildcard, ETCPAL_IPV6_BYTES));
    }
  }
  return false;
}

/*! \brief Initialize a EtcPalIpAddr with a wildcard address.
 *
 *  Works for both IPv4 and IPv6 addresses. The wildcard address is used as an argument to
 *  etcpal_bind() to indicate that a socket should be bound to all available network interfaces. It
 *  should not be used as a placeholder or invalid address - use ETCPAL_IP_SET_INVALID() and
 *  ETCPAL_IP_IS_INVALID() for that.
 *
 *  \param[in] type Type of wildcard to create, either IPv4 or IPv6.
 *  \param[out] ip Address in which to store the wildcard value.
 */
void etcpal_ip_set_wildcard(etcpal_iptype_t type, EtcPalIpAddr* ip)
{
  if (ip)
  {
    switch (type)
    {
      case kEtcPalIpTypeV4:
        ETCPAL_IP_SET_V4_ADDRESS(ip, 0);
        break;
      case kEtcPalIpTypeV6:
        ETCPAL_IP_SET_V6_ADDRESS(ip, v6_wildcard);
        break;
      default:
        ETCPAL_IP_SET_INVALID(ip);
        break;
    }
  }
}

/*! \brief Determine whether two instances of EtcPalIpAddr contain identical addresses.
 *
 *  The type (IPv4 or IPv6) must be the same, as well as the value of the relevant address.
 *
 *  \param[in] ip1 First EtcPalIpAddr to compare.
 *  \param[in] ip2 Second EtcPalIpAddr to compare.
 *  \return true: IPs are identical.
 *  \return false: IPs are not identical.
 */
bool etcpal_ip_equal(const EtcPalIpAddr* ip1, const EtcPalIpAddr* ip2)
{
  if (ip1 && ip2 && ip1->type == ip2->type)
  {
    if (ip1->type == kEtcPalIpTypeV4)
    {
      return ETCPAL_IP_V4_ADDRESS(ip1) == ETCPAL_IP_V4_ADDRESS(ip2);
    }
    else
    {
      return (0 == memcmp(ETCPAL_IP_V6_ADDRESS(ip1), ETCPAL_IP_V6_ADDRESS(ip2), ETCPAL_IPV6_BYTES));
    }
  }
  return false;
}

/*! Compare two EtcPalIpAddrs.
 *
 *  Rules for comparison:
 *  * All Invalid addresses are considered to be equal to each other and < all IPv4 and IPv6
 *    addresses
 *  * All IPv4 addresses are considered to be < all IPv6 addresses
 *  * For two IPv4 or IPv6 addresses, the numerical address value is compared
 *
 *  \param[in] ip1 First EtcPalIpAddr to compare.
 *  \param[in] ip2 Second EtcPalIpAddr to compare.
 *  \return < 0: ip1 < ip2
 *  \return 0: ip1 == ip2
 *  \return > 0: ip1 > ip2
 */
int etcpal_ip_cmp(const EtcPalIpAddr* ip1, const EtcPalIpAddr* ip2)
{
  if (ip1 && ip2)
  {
    if (ip1->type != ip2->type)
    {
      return (int)(ip1->type - ip2->type);
    }
    else if (ip1->type == kEtcPalIpTypeV4)
    {
      return (ETCPAL_IP_V4_ADDRESS(ip1) > ETCPAL_IP_V4_ADDRESS(ip2)) - (ETCPAL_IP_V4_ADDRESS(ip1) < ETCPAL_IP_V4_ADDRESS(ip2));
    }
    else if (ip1->type == kEtcPalIpTypeV6)
    {
      return memcmp(ETCPAL_IP_V6_ADDRESS(ip1), ETCPAL_IP_V6_ADDRESS(ip2), ETCPAL_IPV6_BYTES);
    }
  }
  return 0;
}

/*! \brief Determine whether two instances of EtcPalSockaddr contain identical IP addresses and ports.
 *  \param[in] sock1 First EtcPalSockaddr to compare.
 *  \param[in] sock2 Second EtcPalSockaddr to compare.
 *  \return true: the IP address and port are identical.
 *  \return false: the IP address and port are not identical.
 */
bool etcpal_ip_and_port_equal(const EtcPalSockaddr* sock1, const EtcPalSockaddr* sock2)
{
  if (sock1 && sock2)
    return (etcpal_ip_equal(&sock1->ip, &sock2->ip) && sock1->port == sock2->port);
  else
    return false;
}

/*! \brief Get the length in bits of a netmask.
 *
 *  Counts the number of set ('1') bits in the netmask, starting from the MSB. Works for both IPv4
 *  and IPv6 netmasks.
 *
 *  For example:
 *  netmask = 255.255.0.0 (0xffff0000); result = 16
 *  netmask = ffff:ffff:ffff:ffff::; result = 64
 *
 *  \param[in] netmask Netmask to count.
 *  \return Number of set bits in the netmask.
 */
unsigned int etcpal_ip_mask_length(const EtcPalIpAddr* netmask)
{
  unsigned int length = 0;

  if (netmask)
  {
    if (ETCPAL_IP_IS_V4(netmask))
    {
      uint32_t addr_val = ETCPAL_IP_V4_ADDRESS(netmask);
      uint32_t bit_mask = 0x80000000u;
      while (addr_val & bit_mask)
      {
        ++length;
        bit_mask >>= 1;
      }
    }
    else if (ETCPAL_IP_IS_V6(netmask))
    {
      const uint8_t* addr_buf = ETCPAL_IP_V6_ADDRESS(netmask);
      size_t addr_index = 0;
      while (addr_index < ETCPAL_IPV6_BYTES && addr_buf[addr_index] == 0xffu)
      {
        length += 8;
        addr_index++;
      }

      if (addr_index < ETCPAL_IPV6_BYTES)
      {
        uint8_t addr_val = addr_buf[addr_index];
        uint8_t bit_mask = 0x80u;
        for (; bit_mask != 0; bit_mask >>= 1, ++length)
        {
          if (!(addr_val & bit_mask))
            break;
        }
      }
    }
  }

  return length;
}

/*! \brief Create a netmask given a length in bits.
 *
 *  Creates either an IPv4 or IPv6 netmask, setting the most-significant mask_length bits.
 *
 *  For example:
 *  type = kEtcPalIpTypeV4; mask_length = 16; result = 255.255.0.0
 *  type = kEtcPalIpTypeV6; mask_length = 64; result = ffff:ffff:ffff:ffff::
 *
 *  \param[in] type Type of netmask to create, either IPv4 or IPv6.
 *  \param[in] mask_length Length in bits of the mask, counting from the MSB.
 *  \return EtcPalIpAddr containing the netmask.
 */
EtcPalIpAddr etcpal_ip_mask_from_length(etcpal_iptype_t type, unsigned int mask_length)
{
  EtcPalIpAddr result;

  if (type == kEtcPalIpTypeV4)
  {
    uint32_t mask_val = 0;
    uint32_t bit_mask = 0x80000000u;
    unsigned int adjusted_length = (mask_length > 32 ? 32 : mask_length);
    for (unsigned int i = 0; i < adjusted_length; ++i, bit_mask >>= 1)
    {
      mask_val |= bit_mask;
    }

    ETCPAL_IP_SET_V4_ADDRESS(&result, mask_val);
  }
  else if (type == kEtcPalIpTypeV6)
  {
    uint8_t mask_buf[ETCPAL_IPV6_BYTES];
    memset(mask_buf, 0, sizeof(mask_buf));
    unsigned int adjusted_length = (mask_length > 128 ? 128 : mask_length);

    size_t mask_index = 0;
    while (adjusted_length / 8 > 0)
    {
      mask_buf[mask_index++] = 0xff;
      adjusted_length -= 8;
    }

    uint8_t bit_mask = 0x80u;
    do
    {
      if (adjusted_length == 0)
        break;
      mask_buf[mask_index] |= bit_mask;
      --adjusted_length;
      bit_mask >>= 1;
    } while (bit_mask != 0);

    ETCPAL_IP_SET_V6_ADDRESS(&result, mask_buf);
  }
  else
  {
    ETCPAL_IP_SET_INVALID(&result);
  }
  return result;
}

/*! \brief Compare the network portions of two IP addresses using a netmask.
 *
 *  Returns true if the _network_ portions of the IP addresses match. The host portions are
 *  disregarded.
 *
 *  Some examples:
 *  ip1 = 192.168.0.1; ip2 = 192.168.0.2; netmask = 255.255.0.0 (/16); result = true
 *  ip1 = 192.168.0.1; ip2 = 192.168.1.1; netmask = 255.255.255.0 (/24); result = false
 *  ip1 = 2001:db8::1; ip2 = 2001:db8::2; netmask = ffff:ffff:ffff:ffff:: (/64); result = true
 *  ip1 = 2001:db8:1::1; ip2 = 2001:db8:2::1; netmask = ffff:ffff:ffff:ffff:: (/64); result = false
 *
 *  \param[in] ip1 First EtcPalIpAddr to compare.
 *  \param[in] ip2 Second EtcPalIpAddr to compare.
 *  \param[in] netmask The netmask to use for the comparison. This mask is used to determine the
 *                     network portion of each address. The etcpal_ip_mask_from_length() function can
 *                     be used to turn a mask or prefix bit length into a mask.
 *  \return true: The network portions of the IP addresses are equal.
 *  \return false: The network portions are not equal.
 */
bool etcpal_ip_network_portions_equal(const EtcPalIpAddr* ip1, const EtcPalIpAddr* ip2, const EtcPalIpAddr* netmask)
{
  if (ETCPAL_IP_IS_V4(ip1) && ETCPAL_IP_IS_V4(ip2) && ETCPAL_IP_IS_V4(netmask))
  {
    return ((ETCPAL_IP_V4_ADDRESS(ip1) & ETCPAL_IP_V4_ADDRESS(netmask)) ==
            (ETCPAL_IP_V4_ADDRESS(ip2) & ETCPAL_IP_V4_ADDRESS(netmask)));
  }
  else if (ETCPAL_IP_IS_V6(ip1) && ETCPAL_IP_IS_V6(ip2) && ETCPAL_IP_IS_V6(netmask))
  {
    size_t i;
    const uint32_t* p1 = (const uint32_t*)ETCPAL_IP_V6_ADDRESS(ip1);
    const uint32_t* p2 = (const uint32_t*)ETCPAL_IP_V6_ADDRESS(ip2);
    const uint32_t* pm = (const uint32_t*)ETCPAL_IP_V6_ADDRESS(netmask);

    for (i = 0; i < ETCPAL_IPV6_BYTES / 4; ++i, ++p1, ++p2, ++pm)
    {
      if ((*p1 & *pm) != (*p2 & *pm))
        return false;
    }
    return true;
  }
  return false;
}
