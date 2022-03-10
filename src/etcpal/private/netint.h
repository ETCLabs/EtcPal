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

#ifndef ETCPAL_PRIVATE_NETINT_H_
#define ETCPAL_PRIVATE_NETINT_H_

#include <stddef.h>
#include "etcpal/netint.h"

// Network interface arrays are currently arranged in a general-purpose buffer like so:
//
// [EtcPalNetintInfo 0..n] [padding for alignment] [EtcPalNetintAddr 0..m] [name0] [name1] ... [nameN]
typedef struct NetintArraySizes
{
  size_t ip_addrs_offset;
  size_t names_offset;
  size_t total_size;
} NetintArraySizes;

#define NETINT_ARRAY_SIZES_INIT \
  {                             \
    0u, 0u, 0u                  \
  }

typedef struct NetintArrayContext
{
  EtcPalNetintInfo* cur_info;
  EtcPalNetintAddr* cur_addr;
  char*             cur_name;
  const uint8_t*    buf_end;
} NetintArrayContext;

// Pass buf (uint8_t*) and sizes (NetintArraySizes*)
#define NETINT_ARRAY_CONTEXT_INIT(buf, sizes)                                        \
  {                                                                                  \
    (EtcPalNetintInfo*)(buf), (EtcPalNetintAddr*)(&(buf)[(sizes)->ip_addrs_offset]), \
        (char*)(&(buf)[(sizes)->names_offset]), &(buf)[(sizes)->total_size]          \
  }

typedef struct DefaultInterfaces
{
  unsigned int default_index_v4;
  unsigned int default_index_v6;
} DefaultInterfaces;

#define DEFAULT_INTERFACES_INIT \
  {                             \
    0, 0                        \
  }

EtcPalNetintInfo* find_existing_netint(const char* name, EtcPalNetintInfo* start, EtcPalNetintInfo* end);

#endif /* ETCPAL_PRIVATE_NETINT_H_ */
