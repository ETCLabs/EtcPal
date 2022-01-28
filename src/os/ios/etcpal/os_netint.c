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

/*
 * TODO
 * Empty os_netint stubs for iOS. Haven't figured out how or if iOS allows network interface
 * enumeration yet.
 */

#include "etcpal/common.h"
#include "etcpal/private/netint.h"

etcpal_error_t os_enumerate_interfaces(CachedNetintInfo* cache)
{
  ETCPAL_UNUSED_ARG(cache);
  return kEtcPalErrNotImpl;
}

void os_free_interfaces(CachedNetintInfo* cache)
{
  ETCPAL_UNUSED_ARG(cache);
}

etcpal_error_t os_resolve_route(const EtcPalIpAddr* dest, const CachedNetintInfo* cache, unsigned int* index)
{
  ETCPAL_UNUSED_ARG(dest);
  ETCPAL_UNUSED_ARG(cache);
  ETCPAL_UNUSED_ARG(index);
  return kEtcPalErrNotImpl;
}

bool os_netint_is_up(unsigned int index, const CachedNetintInfo* cache)
{
  ETCPAL_UNUSED_ARG(index);
  ETCPAL_UNUSED_ARG(cache);
  return false;
}
