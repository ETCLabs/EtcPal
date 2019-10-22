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

#include <array>
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
  IpAddr();
  // Note: this constructor is not explicit by design, to allow implicit conversions e.g.
  //   etcpal::IpAddr ip = etcpal_c_function_that_returns_ip();
  IpAddr(const EtcPalIpAddr& c_ip);
  IpAddr& operator=(const EtcPalIpAddr& c_ip);

  IpAddr(uint32_t v4_data);
  IpAddr(const uint8_t* v6_data);
  IpAddr(const uint8_t* v6_data, unsigned long scope_id);

  const EtcPalIpAddr& get() const;
  std::string ToString() const;

  etcpal_iptype_t type() const;
  uint32_t v4_data() const;
  const uint8_t* v6_data() const;
  unsigned long scope_id() const;

  bool IsValid() const;
  bool IsV4() const;
  bool IsV6() const;
  bool IsLinkLocal() const;
  bool IsLoopback() const;
  bool IsMulticast() const;
  bool IsWildcard() const;

  void SetAddress(uint32_t v4_data);
  void SetAddress(const uint8_t* v6_data);

  static IpAddr FromString(const std::string& ip_str);
  static IpAddr WildcardV4();
  static IpAddr WildcardV6();
  static IpAddr Wildcard(etcpal_iptype_t type);

private:
  EtcPalIpAddr addr_{};
};

inline IpAddr::IpAddr()
{
  ETCPAL_IP_SET_INVALID(&addr_);
}

inline IpAddr::IpAddr(const EtcPalIpAddr& c_ip) : addr_(c_ip)
{
}

inline IpAddr& IpAddr::operator=(const EtcPalIpAddr& c_ip)
{
  addr_ = c_ip;
  return *this;
}

inline IpAddr::IpAddr(uint32_t v4_data)
{
  ETCPAL_IP_SET_V4_ADDRESS(&addr_, v4_data);
}

inline IpAddr::IpAddr(const uint8_t* v6_data)
{
  ETCPAL_IP_SET_V6_ADDRESS(&addr_, v6_data);
}

inline const EtcPalIpAddr& IpAddr::get() const
{
  return addr_;
}

inline std::string IpAddr::ToString() const
{
  char str_buf[ETCPAL_INET6_ADDRSTRLEN];
  auto result = etcpal_inet_ntop(&addr_, str_buf, ETCPAL_INET6_ADDRSTRLEN);
  if (result == kEtcPalErrOk)
    return str_buf;
  else
    return std::string();
}

inline etcpal_iptype_t IpAddr::type() const
{
  return addr_.type;
}

inline uint32_t IpAddr::v4_data() const
{
  return ETCPAL_IP_V4_ADDRESS(&addr_);
}

inline const uint8_t* IpAddr::v6_data() const
{
  return ETCPAL_IP_V6_ADDRESS(&addr_);
}

inline bool IpAddr::IsValid() const
{
  return !ETCPAL_IP_IS_INVALID(&addr_);
}

inline bool IpAddr::IsV4() const
{
  return ETCPAL_IP_IS_V4(&addr_);
}

inline bool IpAddr::IsV6() const
{
  return ETCPAL_IP_IS_V6(&addr_);
}

inline bool IpAddr::IsLinkLocal() const
{
  return etcpal_ip_is_link_local(&addr_);
}

inline bool IpAddr::IsLoopback() const
{
  return etcpal_ip_is_loopback(&addr_);
}

inline bool IpAddr::IsMulticast() const
{
  return etcpal_ip_is_multicast(&addr_);
}

inline bool IpAddr::IsWildcard() const
{
  return etcpal_ip_is_wildcard(&addr_);
}

inline void IpAddr::SetAddress(uint32_t v4_data)
{
  ETCPAL_IP_SET_V4_ADDRESS(&addr_, v4_data);
}

inline void IpAddr::SetAddress(const uint8_t* v6_data)
{
  ETCPAL_IP_SET_V6_ADDRESS(&addr_, v6_data);
}

inline IpAddr IpAddr::FromString(const std::string& ip_str)
{
  IpAddr result;
  if (etcpal_inet_pton(kEtcPalIpTypeV4, ip_str.c_str(), &result.addr_) != kEtcPalErrOk)
  {
    etcpal_inet_pton(kEtcPalIpTypeV6, ip_str.c_str(), &result.addr_);
  }
  return result;
}

inline IpAddr IpAddr::WildcardV4()
{
  return Wildcard(kEtcPalIpTypeV4);
}

inline IpAddr IpAddr::WildcardV6()
{
  return Wildcard(kEtcPalIpTypeV6);
}

inline IpAddr IpAddr::Wildcard(etcpal_iptype_t type)
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
  SockAddr();
  // Note: this constructor is not explicit by design, to allow implicit conversions e.g.
  //   etcpal::SockAddr sa = etcpal_c_function_that_returns_sockaddr();
  SockAddr(const EtcPalSockAddr& c_sa);
  SockAddr& operator=(const EtcPalSockAddr& c_sa);

  SockAddr(uint32_t v4_data, uint16_t port);
  SockAddr(const uint8_t* v6_data, uint16_t port);

  const EtcPalSockAddr& get() const;
  IpAddr ip();
  uint16_t port();

private:
  EtcPalSockAddr addr_{};
};

inline SockAddr::SockAddr()
{
  ETCPAL_IP_SET_INVALID(&addr_.ip);
}

/// \ingroup etcpal_cpp_inet
/// \brief A wrapper for the EtcPal MAC address type.
///
/// Provides C++ syntactic sugar for working with MAC addresses.
class MacAddr
{
public:
  MacAddr() = default;
  MacAddr(const EtcPalMacAddr& c_mac);
  MacAddr& operator=(const EtcPalMacAddr& c_mac);

  const EtcPalMacAddr& get() const;
  const std::array<uint8_t, ETCPAL_MAC_BYTES> ToArray() const;
  std::string ToString() const;

private:
  EtcPalMacAddr addr_{};
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// Operators
////////////////////////////////////////////////////////////////////////////////////////////////////

/// \addtogroup etcpal_cpp_inet
/// @{

// Special operators for comparing with EtcPalIpAddrs

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

/// @}

};  // namespace etcpal

#endif  // ETCPAL_CPP_LOCK_H_
