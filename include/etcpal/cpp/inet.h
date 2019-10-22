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

/// \file etcpal/cpp/inet.h
/// \brief C++ wrapper and utilities for etcpal/inet.h

#ifndef ETCPAL_CPP_INET_H_
#define ETCPAL_CPP_INET_H_

#include <algorithm>
#include <array>
#include <cstring>
#include <iterator>
#include <string>
#include "etcpal/inet.h"

namespace etcpal
{
/// \defgroup etcpal_cpp_inet etcpal_cpp_inet
/// \ingroup etcpal_cpp
/// \brief C++ utilities for the \ref etcpal_inet module.

/// \ingroup etcpal_cpp_inet
/// \brief A wrapper class for the EtcPal IP address type.
///
/// Provides C++ syntactic sugar for working with IP addresses.
class IpAddr
{
public:
  IpAddr() noexcept;
  // Note: this constructor is not explicit by design, to allow implicit conversions e.g.
  //   etcpal::IpAddr ip = etcpal_c_function_that_returns_ip();
  IpAddr(const EtcPalIpAddr& c_ip) noexcept;
  IpAddr& operator=(const EtcPalIpAddr& c_ip) noexcept;

  IpAddr(uint32_t v4_data) noexcept;
  IpAddr(const uint8_t* v6_data) noexcept;
  IpAddr(const uint8_t* v6_data, unsigned long scope_id) noexcept;

  const EtcPalIpAddr& get() const noexcept;
  std::string ToString() const;
  uint32_t v4_data() const noexcept;
  const uint8_t* v6_data() const noexcept;
  unsigned long scope_id() const noexcept;

  bool IsValid() const noexcept;
  etcpal_iptype_t type() const noexcept;
  bool IsV4() const noexcept;
  bool IsV6() const noexcept;
  bool IsLinkLocal() const noexcept;
  bool IsLoopback() const noexcept;
  bool IsMulticast() const noexcept;
  bool IsWildcard() const noexcept;

  void SetAddress(uint32_t v4_data) noexcept;
  void SetAddress(const uint8_t* v6_data) noexcept;
  void SetAddress(const uint8_t* v6_data, unsigned long scope_id) noexcept;

  static IpAddr FromString(const std::string& ip_str) noexcept;
  static IpAddr WildcardV4() noexcept;
  static IpAddr WildcardV6() noexcept;
  static IpAddr Wildcard(etcpal_iptype_t type) noexcept;

private:
  EtcPalIpAddr addr_{};
};

/// \brief Constructs an invalid IP address by default.
inline IpAddr::IpAddr() noexcept
{
  ETCPAL_IP_SET_INVALID(&addr_);
}

/// \brief Construct an IP address copied from an instance of the C EtcPalIpAddr type.
inline IpAddr::IpAddr(const EtcPalIpAddr& c_ip) noexcept : addr_(c_ip)
{
}

/// \brief Assign an instance of the C EtcPalIpAddr type to an instance of this class.
inline IpAddr& IpAddr::operator=(const EtcPalIpAddr& c_ip) noexcept
{
  addr_ = c_ip;
  return *this;
}

/// \brief Construct an IPv4 address from its raw 32-bit representation.
/// \param[in] v4_data The address data in host byte order.
inline IpAddr::IpAddr(uint32_t v4_data) noexcept
{
  ETCPAL_IP_SET_V4_ADDRESS(&addr_, v4_data);
}

/// \brief Construct an IPv6 address from its raw 16-byte array representation.
/// \param[in] v6_data Pointer to the address data, an array of length #ETCPAL_IPV6_BYTES.
inline IpAddr::IpAddr(const uint8_t* v6_data) noexcept
{
  ETCPAL_IP_SET_V6_ADDRESS(&addr_, v6_data);
}

/// \brief Construct an IPv6 address from its raw 16-bytes array representation and scope ID.
///
/// See scope_id() for more information.
///
/// \param[in] v6_data Pointer to the address data, an array of length #ETCPAL_IPV6_BYTES.
/// \param[in] scope_id The address's scope ID.
inline IpAddr::IpAddr(const uint8_t* v6_data, unsigned long scope_id) noexcept
{
  ETCPAL_IP_SET_V6_ADDRESS_WITH_SCOPE_ID(&addr_, v6_data, scope_id);
}

/// \brief Get a reference to the underlying C type.
inline const EtcPalIpAddr& IpAddr::get() const noexcept
{
  return addr_;
}

/// \brief Convert the IP address to a string representation.
///
/// See etcpal_inet_ntop() for more information.
inline std::string IpAddr::ToString() const
{
  char str_buf[ETCPAL_INET6_ADDRSTRLEN];
  auto result = etcpal_inet_ntop(&addr_, str_buf, ETCPAL_INET6_ADDRSTRLEN);
  if (result == kEtcPalErrOk)
    return str_buf;
  else
    return std::string();
}

/// \brief Get the raw 32-bit representation of an IPv4 address.
///
/// This function will return undefined data if the IpAddr's type is V6 or Invalid.
///
/// \return The address data in host byte order.
inline uint32_t IpAddr::v4_data() const noexcept
{
  return ETCPAL_IP_V4_ADDRESS(&addr_);
}

/// \brief Get the raw 16-byte array representation of an IPv6 address.
///
/// This function will return undefined data if the IpAddr's type is V4 or Invalid.
///
/// \return Pointer to an array of length #ETCPAL_IPV6_BYTES containing the address data.
inline const uint8_t* IpAddr::v6_data() const noexcept
{
  return ETCPAL_IP_V6_ADDRESS(&addr_);
}

/// \brief Get the scope ID of an IPv6 address.
///
/// Scope IDs (sometimes referred to as Zone IDs or Zone Indices) are a lightly-documented portion
/// of IPv6 addressing, but one concrete use is to indicate the network interface index of a
/// link-local IPv6 address. See also \ref interface_indexes.
///
/// This function will return undefined data if the IpAddr's type is V4 or Invalid.
///
/// \return The scope ID.
inline unsigned long IpAddr::scope_id() const noexcept
{
  return addr_.addr.v6.scope_id;
}

/// \brief Whether an IpAddr contains a valid IPv4 or IPv6 address.
inline bool IpAddr::IsValid() const noexcept
{
  return !ETCPAL_IP_IS_INVALID(&addr_);
}

/// \brief Get the type of the IP address.
inline etcpal_iptype_t IpAddr::type() const noexcept
{
  return addr_.type;
}

/// \brief Whether an IpAddr contains a valid IPv4 address.
inline bool IpAddr::IsV4() const noexcept
{
  return ETCPAL_IP_IS_V4(&addr_);
}

/// \brief Whether an IpAddr contains a valid IPv6 address.
inline bool IpAddr::IsV6() const noexcept
{
  return ETCPAL_IP_IS_V6(&addr_);
}

/// \brief Whether an IpAddr contains a link-local address.
inline bool IpAddr::IsLinkLocal() const noexcept
{
  return etcpal_ip_is_link_local(&addr_);
}

/// \brief Whether an IpAddr contains a loopback address.
inline bool IpAddr::IsLoopback() const noexcept
{
  return etcpal_ip_is_loopback(&addr_);
}

/// \brief Whether an IpAddr contains a multicast address.
inline bool IpAddr::IsMulticast() const noexcept
{
  return etcpal_ip_is_multicast(&addr_);
}

/// \brief Whether an IpAddr contains a multicast address.
///
/// See etcpal_ip_is_wildcard() for more information.
inline bool IpAddr::IsWildcard() const noexcept
{
  return etcpal_ip_is_wildcard(&addr_);
}

/// \brief Set the IPv4 address data.
///
/// Automatically converts this address's type to V4.
///
/// \param[in] v4_data The address data in host byte order.
inline void IpAddr::SetAddress(uint32_t v4_data) noexcept
{
  ETCPAL_IP_SET_V4_ADDRESS(&addr_, v4_data);
}

/// \brief Set the IPv6 address data.
///
/// Automatically converts this address's type to V6.
///
/// \param[in] v6_data Pointer to the address data, an array of length #ETCPAL_IPV6_BYTES.
inline void IpAddr::SetAddress(const uint8_t* v6_data) noexcept
{
  ETCPAL_IP_SET_V6_ADDRESS(&addr_, v6_data);
}

/// \brief Set the IPv6 address data and scope ID.
///
/// See scope_id() for more information.
///
/// \param[in] v6_data Pointer to the address data, an array of length #ETCPAL_IPV6_BYTES.
/// \param[in] scope_id The address's scope ID.
inline void IpAddr::SetAddress(const uint8_t* v6_data, unsigned long scope_id) noexcept
{
  ETCPAL_IP_SET_V6_ADDRESS_WITH_SCOPE_ID(&addr_, v6_data, scope_id);
}

/// \brief Construct an IpAddr from a string representation.
///
/// See etcpal_inet_pton() for more information.
inline IpAddr IpAddr::FromString(const std::string& ip_str) noexcept
{
  IpAddr result;
  if (etcpal_inet_pton(kEtcPalIpTypeV4, ip_str.c_str(), &result.addr_) != kEtcPalErrOk)
  {
    etcpal_inet_pton(kEtcPalIpTypeV6, ip_str.c_str(), &result.addr_);
  }
  return result;
}

/// \brief Construct a wildcard IPv4 address.
///
/// See etcpal_ip_set_wildcard() for more information.
inline IpAddr IpAddr::WildcardV4() noexcept
{
  return Wildcard(kEtcPalIpTypeV4);
}

/// \brief Construct a wildcard IPv6 address.
///
/// See etcpal_ip_set_wildcard() for more information.
inline IpAddr IpAddr::WildcardV6() noexcept
{
  return Wildcard(kEtcPalIpTypeV6);
}

/// \brief Construct a wildcard address of the type specified.
///
/// See etcpal_ip_set_wildcard() for more information.
inline IpAddr IpAddr::Wildcard(etcpal_iptype_t type) noexcept
{
  IpAddr result;
  etcpal_ip_set_wildcard(type, &result.addr_);
  return result;
}

/// \ingroup etcpal_cpp_inet
/// \brief A wrapper for the EtcPal socket address type.
///
/// Provides C++ syntactic sugar for working with socket addresses, which contain an IP address and
/// port.
class SockAddr
{
public:
  SockAddr() noexcept;
  // Note: this constructor is not explicit by design, to allow implicit conversions e.g.
  //   etcpal::SockAddr sa = etcpal_c_function_that_returns_sockaddr();
  SockAddr(const EtcPalSockAddr& c_sa) noexcept;
  SockAddr& operator=(const EtcPalSockAddr& c_sa) noexcept;

  SockAddr(uint32_t v4_data, uint16_t port) noexcept;
  SockAddr(const uint8_t* v6_data, uint16_t port) noexcept;
  SockAddr(IpAddr ip, uint16_t port) noexcept;

  const EtcPalSockAddr& get() const noexcept;
  std::string ToString() const;
  IpAddr ip() const noexcept;
  uint16_t port() const noexcept;

private:
  EtcPalSockAddr addr_{};
};

/// \brief Constructs an invalid SockAddr by default.
inline SockAddr::SockAddr() noexcept
{
  ETCPAL_IP_SET_INVALID(&addr_.ip);
}

/// \brief Construct a SockAddr copied from an instance of the C EtcPalSockAddr type.
inline SockAddr::SockAddr(const EtcPalSockAddr& c_sa) noexcept : addr_(c_sa)
{
}

/// \brief Assign an instance of the C EtcPalSockAddr type to an instance of this class.
inline SockAddr& SockAddr::operator=(const EtcPalSockAddr& c_sa) noexcept
{
  addr_ = c_sa;
  return *this;
}

/// \brief Construct a SockAddr from a raw 32-bit IPv4 representation and port number.
/// \param[in] v4_data The address data in host byte order.
/// \param[in] port The port number in host byte order.
inline SockAddr::SockAddr(uint32_t v4_data, uint16_t port) noexcept
{
  ETCPAL_IP_SET_V4_ADDRESS(&addr_.ip, v4_data);
  addr_.port = port;
}

/// \brief Construct a SockAddr from a raw 16-byte IPv6 representation and port number.
/// \param[in] v6_data Pointer to the address data, an array of length #ETCPAL_IPV6_BYTES.
/// \param[in] port The port number in host byte order.
inline SockAddr::SockAddr(const uint8_t* v6_data, uint16_t port) noexcept
{
  ETCPAL_IP_SET_V6_ADDRESS(&addr_.ip, v6_data);
  addr_.port = port;
}

/// \brief Construct a SockAddr from an IpAddr and port number.
/// \param[in] ip The IP address.
/// \param[in] port The port number in host byte order.
inline SockAddr::SockAddr(IpAddr ip, uint16_t port) noexcept
{
  addr_.ip = ip.get();
  addr_.port = port;
}

/// \brief Get a reference to the underlying C type.
inline const EtcPalSockAddr& SockAddr::get() const noexcept
{
  return addr_;
}

/// \brief Convert the IP address and port to a string representation.
///
/// The string will be of the form ddd.ddd.ddd.ddd:ppppp for IPv4 addresses, and
/// [xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:xxxx]:ppppp for IPv6 addresses (common conventions and
/// rules for compressing these representations will also apply).
inline std::string SockAddr::ToString() const
{
  if (ETCPAL_IP_IS_V4(&addr_.ip))
    return ip().ToString() + ':' + std::to_string(addr_.port);
  else if (ETCPAL_IP_IS_V6(&addr_.ip))
    return '[' + ip().ToString() + "]:" + std::to_string(addr_.port);
  else
    return std::string();
}

/// \brief Get the IP address from the SockAddr.
inline IpAddr SockAddr::ip() const noexcept
{
  return addr_.ip;
}

/// \brief Get the port number from the SockAddr.
inline uint16_t SockAddr::port() const noexcept
{
  return addr_.port;
}

/// \ingroup etcpal_cpp_inet
/// \brief A wrapper for the EtcPal MAC address type.
///
/// Provides C++ syntactic sugar for working with MAC addresses.
class MacAddr
{
public:
  /// \brief Constructs a null MAC address by default.
  MacAddr() = default;
  // Note: this constructor is not explicit by design, to allow implicit conversions e.g.
  //   etcpal::MacAddr sa = etcpal_c_function_that_returns_macaddr();
  MacAddr(const EtcPalMacAddr& c_mac) noexcept;
  MacAddr& operator=(const EtcPalMacAddr& c_mac) noexcept;
  explicit MacAddr(const uint8_t* mac_data) noexcept;

  const EtcPalMacAddr& get() const noexcept;
  std::string ToString() const;
  const uint8_t* data() const noexcept;

  bool IsNull() const noexcept;

  static MacAddr FromString(const std::string& mac_str) noexcept;

private:
  EtcPalMacAddr addr_{};
};

/// \brief Construct a MAC address copied from an instance of the C EtcPalMacAddr type.
inline MacAddr::MacAddr(const EtcPalMacAddr& c_mac) noexcept : addr_(c_mac)
{
}

/// \brief Assign an instance of the C EtcPalMacAddr type to an instance of this class.
inline MacAddr& MacAddr::operator=(const EtcPalMacAddr& c_mac) noexcept
{
  addr_ = c_mac;
  return *this;
}

/// \brief Construct a MAC address from its raw 6-byte array representation.
/// \param[in] mac_data Pointer to the MAC data, an array of length #ETCPAL_MAC_BYTES.
inline MacAddr::MacAddr(const uint8_t* mac_data) noexcept
{
  std::memcpy(addr_.data, mac_data, ETCPAL_MAC_BYTES);
}

/// \brief Get a reference to the underlying C type.
inline const EtcPalMacAddr& MacAddr::get() const noexcept
{
  return addr_;
}

/// \brief Convert the MAC address to a string representation.
///
/// See etcpal_mac_to_string() for more information.
inline std::string MacAddr::ToString() const
{
  char str_buf[ETCPAL_MAC_STRING_BYTES];
  etcpal_mac_to_string(&addr_, str_buf, ETCPAL_MAC_STRING_BYTES);
  return str_buf;
}

/// \brief Get the raw 6-byte array representation of a MAC address.
/// \return Pointer to an array of length #ETCPAL_MAC_BYTES containing the address data.
inline const uint8_t* MacAddr::data() const noexcept
{
  return addr_.data;
}

/// \brief Whether this MacAddr represents a null (all 0's) MAC address.
inline bool MacAddr::IsNull() const noexcept
{
  return std::all_of(std::begin(addr_.data), std::end(addr_.data), [](uint8_t byte) { return byte == 0; });
  // TODO would this be a better implementation?
  // std::array<uint8_t, ETCPAL_MAC_BYTES> null_mac{};
  // return (std::memcmp(addr_.data, null_mac.data(), ETCPAL_MAC_BYTES) == 0);
}

/// \brief Construct a MacAddr from a string representation.
///
/// See etcpal_string_to_mac() for more information.
inline MacAddr MacAddr::FromString(const std::string& mac_str) noexcept
{
  MacAddr result;
  etcpal_string_to_mac(mac_str.c_str(), &result.addr_);
  return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Operators
////////////////////////////////////////////////////////////////////////////////////////////////////

/// \addtogroup etcpal_cpp_inet
/// @{

// Special operators for comparing with the underlying types

inline bool operator==(const EtcPalIpAddr& c_ip, const IpAddr& ip) noexcept
{
  return c_ip == ip.get();
}

inline bool operator!=(const EtcPalIpAddr& c_ip, const IpAddr& ip) noexcept
{
  return !(c_ip == ip);
}

inline bool operator==(const IpAddr& ip, const EtcPalIpAddr& c_ip) noexcept
{
  return ip.get() == c_ip;
}

inline bool operator!=(const IpAddr& ip, const EtcPalIpAddr& c_ip) noexcept
{
  return !(ip == c_ip);
}

inline bool operator==(const EtcPalSockAddr& c_ip, const SockAddr& ip) noexcept
{
  return c_ip == ip.get();
}

inline bool operator!=(const EtcPalSockAddr& c_ip, const SockAddr& ip) noexcept
{
  return !(c_ip == ip);
}

inline bool operator==(const SockAddr& ip, const EtcPalSockAddr& c_ip) noexcept
{
  return ip.get() == c_ip;
}

inline bool operator!=(const SockAddr& ip, const EtcPalSockAddr& c_ip) noexcept
{
  return !(ip == c_ip);
}

inline bool operator==(const EtcPalMacAddr& c_mac, const MacAddr& mac) noexcept
{
  return c_mac == mac.get();
}

inline bool operator!=(const EtcPalMacAddr& c_mac, const MacAddr& mac) noexcept
{
  return !(c_mac == mac);
}

inline bool operator==(const MacAddr& mac, const EtcPalMacAddr& c_mac) noexcept
{
  return mac.get() == c_mac;
}

inline bool operator!=(const MacAddr& mac, const EtcPalMacAddr& c_mac) noexcept
{
  return !(mac == c_mac);
}

// Standard operators

inline bool operator==(const IpAddr& a, const IpAddr& b) noexcept
{
  return a.get() == b.get();
}

inline bool operator!=(const IpAddr& a, const IpAddr& b) noexcept
{
  return !(a == b);
}

inline bool operator<(const IpAddr& a, const IpAddr& b) noexcept
{
  return a.get() < b.get();
}

inline bool operator>(const IpAddr& a, const IpAddr& b) noexcept
{
  return b < a;
}

inline bool operator<=(const IpAddr& a, const IpAddr& b) noexcept
{
  return !(b < a);
}

inline bool operator>=(const IpAddr& a, const IpAddr& b) noexcept
{
  return !(a < b);
}

inline bool operator==(const SockAddr& a, const SockAddr& b) noexcept
{
  return a.get() == b.get();
}

inline bool operator!=(const SockAddr& a, const SockAddr& b) noexcept
{
  return !(a == b);
}

inline bool operator<(const SockAddr& a, const SockAddr& b) noexcept
{
  return a.get() < b.get();
}

inline bool operator>(const SockAddr& a, const SockAddr& b) noexcept
{
  return b < a;
}

inline bool operator<=(const SockAddr& a, const SockAddr& b) noexcept
{
  return !(b < a);
}

inline bool operator>=(const SockAddr& a, const SockAddr& b) noexcept
{
  return !(a < b);
}

inline bool operator==(const MacAddr& a, const MacAddr& b) noexcept
{
  return a.get() == b.get();
}

inline bool operator!=(const MacAddr& a, const MacAddr& b) noexcept
{
  return !(a == b);
}

inline bool operator<(const MacAddr& a, const MacAddr& b) noexcept
{
  return a.get() < b.get();
}

inline bool operator>(const MacAddr& a, const MacAddr& b) noexcept
{
  return b < a;
}

inline bool operator<=(const MacAddr& a, const MacAddr& b) noexcept
{
  return !(b < a);
}

inline bool operator>=(const MacAddr& a, const MacAddr& b) noexcept
{
  return !(a < b);
}

/// @}

};  // namespace etcpal

#endif  // ETCPAL_CPP_LOCK_H_
