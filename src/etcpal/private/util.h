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

#ifndef ETCPAL_PRIVATE_UTIL_H_
#define ETCPAL_PRIVATE_UTIL_H_

#include <stdint.h>
#include "etcpal/common.h"

#define GET_PADDING_FOR_ALIGNMENT(addr, type)        \
  ((((uintptr_t)(addr)) % ETCPAL_ALIGNOF(type) == 0) \
       ? 0                                           \
       : (ETCPAL_ALIGNOF(type) - (((uintptr_t)(addr)) % ETCPAL_ALIGNOF(type))))

#endif  // ETCPAL_PRIVATE_UTIL_H_
