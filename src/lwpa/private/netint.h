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

#ifndef _LWPA_PRIVATE_NETINT_H_
#define _LWPA_PRIVATE_NETINT_H_

#include "lwpa/netint.h"
#include "lwpa/error.h"

typedef struct DefaultNetint
{
  bool v4_valid;
  unsigned int v4_index;

  bool v6_valid;
  unsigned int v6_index;
} DefaultNetint;

typedef struct CachedNetintInfo
{
  size_t num_netints;
  LwpaNetintInfo* netints;
  DefaultNetint def;
} CachedNetintInfo;

lwpa_error_t lwpa_netint_init();
void lwpa_netint_deinit();

lwpa_error_t os_enumerate_interfaces(CachedNetintInfo* cache);
void os_free_interfaces(CachedNetintInfo* cache);
lwpa_error_t os_resolve_route(const LwpaIpAddr* dest, const CachedNetintInfo* cache, unsigned int* index);

#endif /* _LWPA_PRIVATE_NETINT_H_ */
