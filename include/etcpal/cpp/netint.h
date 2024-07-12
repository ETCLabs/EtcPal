/******************************************************************************
 * Copyright 2024 ETC Inc.
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

/// @file etcpal/cpp/netint.h
/// @brief C++ wrapper and utilities for etcpal/netint.h

#ifndef ETCPAL_CPP_NETINT_H_
#define ETCPAL_CPP_NETINT_H_

#include <algorithm>
#include <vector>
#include "etcpal/netint.h"
#include "etcpal/cpp/common.h"
#include "etcpal/cpp/error.h"
#include "etcpal/cpp/inet.h"

namespace etcpal
{
/// @defgroup etcpal_cpp_netint netint (Network Interfaces)
/// @ingroup etcpal_net
/// @brief C++ utilities for the @ref etcpal_netint module.
///
//////*WARNING:** This module must be explicitly initialized before use. Initialize the module by
/// calling etcpal_init() with the relevant feature mask:
/// @code
/// etcpal_init(ETCPAL_FEATURE_NETINTS);
/// @endcode
///
/// After initialization, an array of the set of network interfaces which were present on the system
/// at initialization time is kept internally. This array can be retrieved using
/// etcpal::netint::GetInterfaces() and refreshed using etcpal::netint::RefreshInterfaces(). Here's how
/// to retrieve the interfaces:
///
/// @code
/// auto netints = etcpal::netint::GetInterfaces();
/// if (netints)
/// {
///   for (const auto& netint :///netints)
///   {
///     // Record information or do something with each network interface in turn
///   }
/// }
/// @endcode
///
/// The network interface array is sorted by interface index (see @ref interface_indexes); multiple
/// IP addresses assigned to the same physical interface are separated into different entries. This
/// means multiple entries in the array can have the same index.
///
/// The module also attempts to determine the interface used for the system's default route (the
/// route used to get to an arbitrary internet destination) and calls that the "default interface",
/// which can be retrieved using etcpal::netint::GetDefaultInterface().
///
/// The routing information can also be used to determine which network interface will be used for a
/// given IP address destination, which can be handy for multicasting.
///
/// @code
/// // Index will either hold an error or the index of the interface that will be used
/// auto index = etcpal::netint::GetInterfaceForDest(etcpal::IpAddr::FromString("192.168.200.35"));
/// @endcode
///
/// The list of network interfaces is cached and will only change if the
/// etcpal::netint::RefreshInterfaces() function is called. These functions are all thread-safe, so
/// the interfaces can be refreshed on one thread while other queries are made on another thread.

/// @addtogroup etcpal_cpp_netint
/// @{

namespace netint
{
/// @cond Implementation detail functions

namespace detail
{
inline etcpal::Expected<std::vector<etcpal::NetintInfo>> GetInterfaces(NetintIndex index = NetintIndex()) noexcept
{
  static constexpr size_t kInitialEstimatedCount = 4u;

  int resize_count = 0;

  size_t                        num_netints = kInitialEstimatedCount;  // Start with estimate
  std::vector<EtcPalNetintInfo> c_netints(num_netints);
  auto                          err = kEtcPalErrOk;
  while ((err = (index.IsValid() ? etcpal_netint_get_interfaces_for_index(index.value(), c_netints.data(), &num_netints)
                                 : etcpal_netint_get_interfaces(c_netints.data(), &num_netints))) == kEtcPalErrBufSize)
  {
    if (resize_count > 10)
      return kEtcPalErrBufSize;  // Something screwy is going on, so avoid a potentially infinite loop

    c_netints.resize(num_netints);
    ++resize_count;
  }

  if (err == kEtcPalErrOk)
  {
    std::vector<etcpal::NetintInfo> netints(num_netints);
    for (size_t i = 0u; i < num_netints; ++i)
      netints[i] = etcpal::NetintInfo(c_netints[i]);

    return netints;
  }

  return err;
}
}  // namespace detail

/// @endcond

/// @addtogroup etcpal_cpp_netint
/// @{

/// @brief Get a list of network interfaces on the system.
///
/// For NICs with multiple IP addresses assigned, this module separates each address into its own
/// entry in the netint array. Because of this, multiple array entries could have the same value
/// for the index, mac and id parameters.
///
/// @return The array of network interfaces on success.
/// @return #kEtcPalErrInvalid: Invalid argument provided.
/// @return #kEtcPalErrNotInit: Module not initialized.
/// @return #kEtcPalErrNotFound: No system interfaces were found.
inline etcpal::Expected<std::vector<etcpal::NetintInfo>> GetInterfaces() noexcept
{
  return detail::GetInterfaces();
}

/// @brief Get a list of network interfaces that have the index specified.
///
/// See @ref interface_indexes for more information.
///
/// @param[in] index Index for which to get interfaces.
/// @return The array of matching network interfaces on success.
/// @return #kEtcPalErrInvalid: Invalid argument provided.
/// @return #kEtcPalErrNotInit: Module not initialized.
/// @return #kEtcPalErrNotFound: No interfaces found for this index.
inline etcpal::Expected<std::vector<etcpal::NetintInfo>> GetInterfacesForIndex(NetintIndex index) noexcept
{
  if (!index)
    return kEtcPalErrInvalid;

  return detail::GetInterfaces(index);
}

/// @brief Get the network interface that has the specified IP address.
///
/// @param[in] ip The IP address assigned to the desired interface.
/// @return Information for the matching interface if found.
/// @return #kEtcPalErrInvalid: Invalid argument provided.
/// @return #kEtcPalErrNotInit: Module not initialized.
/// @return #kEtcPalErrNotFound: No interfaces found for this IP address.
/// @return #kEtcPalErrSys: Multiple interfaces were configured at the same IP address, which means the system is
/// misconfigured.
inline etcpal::Expected<etcpal::NetintInfo> GetInterfaceWithIp(const IpAddr& ip) noexcept
{
  if (!ip.IsValid())
    return kEtcPalErrInvalid;

  EtcPalNetintInfo c_netint{};
  auto             err = etcpal_netint_get_interface_with_ip(&ip.get(), &c_netint);

  if (err == kEtcPalErrOk)
    return etcpal::NetintInfo(c_netint);

  return err;
}

/// @brief Get information about the default network interface.
///
/// For our purposes, the 'default' network interface is defined as the interface that is chosen
/// for the default IP route. The default interface is given as an OS network interface index - see
/// @ref interface_indexes for more information. Note that since network interfaces can have
/// multiple IP addresses assigned, this index may be shared by many entries returned by
/// etcpal_netint_get_interfaces().
///
/// @param[in] type The IP protocol for which to get the default network interface, either
///                 #kEtcPalIpTypeV4 or #kEtcPalIpTypeV6. A separate default interface is maintained for
///                 each.
/// @return The index of the default interface on success.
/// @return #kEtcPalErrInvalid: Invalid argument provided.
/// @return #kEtcPalErrNotInit: Module not initialized.
/// @return #kEtcPalErrNotFound: No default interface found for this type.
inline etcpal::Expected<NetintIndex> GetDefaultInterface(etcpal::IpAddrType type) noexcept
{
  unsigned int index = 0u;
  auto         err   = etcpal_netint_get_default_interface(static_cast<etcpal_iptype_t>(type), &index);

  if (err == kEtcPalErrOk)
    return NetintIndex(index);

  return err;
}

/// @brief Get the network interface that the system will choose when routing an IP packet to the
///        specified destination.
///
/// @param[in] dest IP address of the destination.
/// @param[out] netint_index Pointer to value to fill in with
/// @return The index of the chosen interface on success.
/// @return #kEtcPalErrInvalid: Invalid argument provided.
/// @return #kEtcPalErrNotInit: Module not initialized.
/// @return #kEtcPalErrNoNetints: No network interfaces found on system.
/// @return #kEtcPalErrNotFound: No route was able to be resolved to the destination.
inline etcpal::Expected<NetintIndex> GetInterfaceForDest(const etcpal::IpAddr& dest) noexcept
{
  unsigned int index = 0u;
  auto         err   = etcpal_netint_get_interface_for_dest(&dest.get(), &index);

  if (err == kEtcPalErrOk)
    return NetintIndex(index);

  return err;
}

/// @brief Refresh the list of network interfaces.
///
/// Rebuilds the cached array of network interfaces that is returned via the
/// etcpal::netint::GetInterfaces() function.
///
/// @return #kEtcPalErrOk: Interfaces refreshed.
/// @return Other error codes from the underlying platform are possible here.
inline etcpal::Error RefreshInterfaces() noexcept
{
  return etcpal_netint_refresh_interfaces();
}

/// @brief Determine whether a network interface is currently up and running.
///
/// @note On Windows, cached network interface information is used to determine this, so the result
///       for a given index will not change until etcpal::netint::RefreshInterfaces() is called.
///
/// @param index Index of the interface to check.
/// @return true: The interface indicated by index is up.
/// @return false: The interface indicated by index is down, or index is invalid.
inline bool IsUp(NetintIndex index) noexcept
{
  return etcpal_netint_is_up(index.value());
}

}  // namespace netint

/// @}

}  // namespace etcpal

#endif  // ETCPAL_CPP_NETINT_H_
