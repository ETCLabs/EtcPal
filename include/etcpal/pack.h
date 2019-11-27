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

/* etcpal/pack.h: Platform- and endianness-independent buffer packing and unpacking. */

#ifndef ETCPAL_PACK_H_
#define ETCPAL_PACK_H_

#include "etcpal/int.h"

/*!
 * \defgroup etcpal_pack Buffer Packing and Unpacking (pack)
 * \ingroup etcpal
 * \brief Platform- and endianness-independent buffer packing and unpacking.
 *
 * For 16- and 32-bit integer packing and unpacking:
 * ```c
 * #include "etcpal/pack.h"
 * ```
 * For 64-bit integer packing and unpacking:
 * ```c
 * #include "etcpal/pack64.h"
 * ```
 *
 * This module defines macros for packing and unpacking integer types to/from a byte buffer. They
 * are architected in such a way that the endianness of the integer in the buffer is always known,
 * regardless of the endianness of your platform.
 *
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

uint16_t etcpal_upack_16b(const uint8_t* buf);
void etcpal_pack_16b(uint8_t* buf, uint16_t val);
uint16_t etcpal_upack_16l(const uint8_t* buf);
void etcpal_pack_16l(uint8_t* buf, uint16_t val);
uint32_t etcpal_upack_32b(const uint8_t* buf);
void etcpal_pack_32b(uint8_t* buf, uint32_t val);
uint32_t etcpal_upack_32l(const uint8_t* buf);
void etcpal_pack_32l(uint8_t* buf, uint32_t val);

#ifdef __cplusplus
}
#endif

/*!
 * @}
 */

#endif /* ETCPAL_PACK_H_ */
