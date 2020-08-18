/******************************************************************************
 * Copyright 2020 ETC Inc.
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

/* etcpal/pack64.h: Optional extensions to etcpal_pack for 64-bit types. */

#ifndef ETCPAL_PACK64_H_
#define ETCPAL_PACK64_H_

#include <stdint.h>

/**
 * @addtogroup etcpal_pack
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

int64_t etcpal_unpack_i64b(const uint8_t* buf);
void    etcpal_pack_i64b(uint8_t* buf, int64_t val);
int64_t etcpal_unpack_i64l(const uint8_t* buf);
void    etcpal_pack_i64l(uint8_t* buf, int64_t val);

uint64_t etcpal_unpack_u64b(const uint8_t* buf);
void     etcpal_pack_u64b(uint8_t* buf, uint64_t val);
uint64_t etcpal_unpack_u64l(const uint8_t* buf);
void     etcpal_pack_u64l(uint8_t* buf, uint64_t val);

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif /* ETCPAL_PACK64_H_ */
