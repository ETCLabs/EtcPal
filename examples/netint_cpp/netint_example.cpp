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

/*
 * An example application showing usage of the etcpal_cpp_netint functions.
 * This example will print information about each network interface on your machine. It is
 * platform-neutral.
 */

#include <iomanip>
#include <iostream>
#include "etcpal/cpp/netint.h"
#include "etcpal/socket.h"

std::string id_col_header      = "ID ";
std::string addr_col_header    = "Address ";
std::string netmask_col_header = "Netmask ";
std::string mac_col_header     = "MAC ";
std::string index_col_header   = "OS Index ";
std::string state_col_header   = "State";

void ResizeHeaderStrings(const std::vector<etcpal::NetintInfo>& netint_arr)
{
  for (const auto& netint : netint_arr)
  {
    size_t id_len      = netint.id().length() + 1u;
    size_t addr_len    = netint.addr().ToString().length() + 1u;
    size_t netmask_len = netint.mask().ToString().length() + 1u;
    size_t mac_len     = netint.mac().ToString().length() + 1u;

    if (id_len > id_col_header.length())
      id_col_header.resize(id_len, ' ');
    if (addr_len > addr_col_header.length())
      addr_col_header.resize(addr_len, ' ');
    if (netmask_len > netmask_col_header.length())
      netmask_col_header.resize(netmask_len, ' ');
    if (mac_len > mac_col_header.length())
      mac_col_header.resize(mac_len, ' ');
  }
}

int main()
{
  etcpal::Error init_res = etcpal_init(ETCPAL_FEATURE_NETINTS);
  if (init_res != kEtcPalErrOk)
  {
    std::cout << "etcpal_init() failed with error: '" << init_res.ToString() << "'\n";
    return 1;
  }

  auto netint_arr = etcpal::NetintGetInterfaces();
  if (!netint_arr)
  {
    if (netint_arr == kEtcPalErrNotFound)
      std::cout << "Error: No network interfaces found on system.\n";
    else
      std::cout << "Error: " << netint_arr.error().ToString() << "\n";

    etcpal_deinit(ETCPAL_FEATURE_NETINTS);
    return 1;
  }

  ResizeHeaderStrings(*netint_arr);

  std::cout << "Network interfaces found:\n"
            << id_col_header << addr_col_header << netmask_col_header << mac_col_header << index_col_header
            << state_col_header << "\n";

  for (const auto& netint : *netint_arr)
  {
    std::cout << std::left << std::setw(id_col_header.length()) << netint.id();
    std::cout << std::left << std::setw(addr_col_header.length()) << netint.addr().ToString();
    std::cout << std::left << std::setw(netmask_col_header.length()) << netint.mask().ToString();
    std::cout << std::left << std::setw(mac_col_header.length()) << netint.mac().ToString();
    std::cout << std::right << std::setw(index_col_header.length() - 1u) << netint.index().value() << " ";
    std::cout << std::left << std::setw(state_col_header.length())
              << (etcpal::NetintIsUp(netint.index()) ? "Up" : "Down") << "\n";
  }

  auto default_v4 = etcpal::NetintGetDefaultInterface(etcpal::IpAddrType::kV4);
  if (default_v4)
  {
    auto arr = etcpal::NetintGetInterfacesForIndex(*default_v4);
    if (arr)
      std::cout << "Default IPv4 interface: " << (*arr)[0].friendly_name() << " (" << default_v4->value() << ")\n";
  }

  auto default_v6 = etcpal::NetintGetDefaultInterface(etcpal::IpAddrType::kV6);
  if (default_v6)
  {
    auto arr = etcpal::NetintGetInterfacesForIndex(*default_v6);
    if (arr)
      std::cout << "Default IPv6 interface: " << (*arr)[0].friendly_name() << " (" << default_v6->value() << ")\n";
  }

  etcpal_deinit(ETCPAL_FEATURE_NETINTS);

  return 0;
}
