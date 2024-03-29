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

#include "etcpal_mock/netint.h"

DEFINE_FAKE_VALUE_FUNC(etcpal_error_t, etcpal_netint_get_interfaces, EtcPalNetintInfo*, size_t*);
DEFINE_FAKE_VALUE_FUNC(etcpal_error_t,
                       etcpal_netint_get_interfaces_for_index,
                       unsigned int,
                       EtcPalNetintInfo*,
                       size_t*);
DEFINE_FAKE_VALUE_FUNC(etcpal_error_t, etcpal_netint_get_interface_with_ip, const EtcPalIpAddr*, EtcPalNetintInfo*);
DEFINE_FAKE_VALUE_FUNC(etcpal_error_t, etcpal_netint_get_default_interface, etcpal_iptype_t, unsigned int*);
DEFINE_FAKE_VALUE_FUNC(etcpal_error_t, etcpal_netint_get_interface_for_dest, const EtcPalIpAddr*, unsigned int*);
DEFINE_FAKE_VALUE_FUNC(etcpal_error_t, etcpal_netint_refresh_interfaces);
DEFINE_FAKE_VALUE_FUNC(bool, etcpal_netint_is_up, unsigned int);

void etcpal_netint_reset_all_fakes(void)
{
  RESET_FAKE(etcpal_netint_get_interfaces);
  RESET_FAKE(etcpal_netint_get_interfaces_for_index);
  RESET_FAKE(etcpal_netint_get_interface_with_ip);
  RESET_FAKE(etcpal_netint_get_default_interface);
  RESET_FAKE(etcpal_netint_get_interface_for_dest);
  RESET_FAKE(etcpal_netint_refresh_interfaces);
  RESET_FAKE(etcpal_netint_is_up);
}
