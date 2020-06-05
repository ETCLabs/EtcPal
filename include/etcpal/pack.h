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

/* etcpal/pack.h: Platform- and endianness-independent buffer packing and unpacking. */

#ifndef ETCPAL_PACK_H_
#define ETCPAL_PACK_H_

#include <stdint.h>

/**
 * @defgroup etcpal_pack pack (Buffer Packing and Unpacking)
 * @ingroup etcpal
 * @brief Platform- and endianness-independent buffer packing and unpacking.
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
 * For example:
 *
 * @code
 * uint8_t buf[4];
 * etcpal_pack_u32b(buf, 0x11223344); // Pack 32-bit value, big-endian
 *
 * // buf now contains { 0x11, 0x22, 0x33, 0x44 }
 *
 * uint32_t val = etcpal_unpack_u32b(&buf); // Unpack 32-bit value, big-endian
 * etcpal_pack_u32l(&buf, val); // Pack 32-bit value, little-endian
 *
 * // buf now contains { 0x44, 0x33, 0x22, 0x11 }
 * @endcode
 *
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

int16_t etcpal_unpack_i16b(const uint8_t* buf);
void    etcpal_pack_i16b(uint8_t* buf, int16_t val);
int16_t etcpal_unpack_i16l(const uint8_t* buf);
void    etcpal_pack_i16l(uint8_t* buf, int16_t val);

uint16_t etcpal_unpack_u16b(const uint8_t* buf);
void     etcpal_pack_u16b(uint8_t* buf, uint16_t val);
uint16_t etcpal_unpack_u16l(const uint8_t* buf);
void     etcpal_pack_u16l(uint8_t* buf, uint16_t val);

int32_t etcpal_unpack_i32b(const uint8_t* buf);
void    etcpal_pack_i32b(uint8_t* buf, int32_t val);
int32_t etcpal_unpack_i32l(const uint8_t* buf);
void    etcpal_pack_i32l(uint8_t* buf, int32_t val);

uint32_t etcpal_unpack_u32b(const uint8_t* buf);
void     etcpal_pack_u32b(uint8_t* buf, uint32_t val);
uint32_t etcpal_unpack_u32l(const uint8_t* buf);
void     etcpal_pack_u32l(uint8_t* buf, uint32_t val);

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif /* ETCPAL_PACK_H_ */
