/******************************************************************************
 * Copyright 2018 ETC Inc.
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

/* lwpa/pack.h: Platform- and endianness-independent buffer packing and unpacking. */
#ifndef _LWPA_PACK_H_
#define _LWPA_PACK_H_

#include "lwpa/int.h"
#include "lwpa/opts.h"

/*! \defgroup lwpa_pack lwpa_pack
 *  \ingroup lwpa
 *  \brief Platform- and endianness-independent buffer packing and unpacking.
 *
 *  \#include "lwpa/pack.h"
 *
 *  This module defines macros for packing and unpacking integer types to/from a byte buffer. They
 *  are architected in such a way that the endianness of the integer in the buffer is always known,
 *  regardless of the endianness of your platform.
 *
 *  @{
 */

/* clang-format off */

/*! \brief Unpack a uint16_t from a known big-endian buffer.
 *  \param ptr Pointer to the buffer from which to unpack a uint16_t.
 *  \return Unpacked uint16_t.
 */
#define upack_16b(ptr)                        \
  (((uint16_t)0u) |                           \
   (((uint16_t)((uint8_t *)(ptr))[0]) << 8) | \
    ((uint16_t)((uint8_t *)(ptr))[1]))

/*! \brief Pack a uint16_t to a known big-endian buffer.
 *  \param ptr Pointer to the buffer into which to pack a uint16_t.
 *  \param val uint16_t value to pack into the buffer.
 */
#define pack_16b(ptr, val)                                    \
  do                                                          \
  {                                                           \
    ((uint8_t *)(ptr))[1] = (uint8_t) ((val) & 0xff);         \
    ((uint8_t *)(ptr))[0] = (uint8_t)(((val) & 0xff00) >> 8); \
  } while (0)

/*! \brief Unpack a uint16_t from a known little-endian buffer.
 *  \param ptr Pointer to the buffer from which to unpack a uint16_t.
 *  \return Unpacked uint16_t.
 */
#define upack_16l(ptr)                        \
  (((uint16_t)0u) |                           \
   (((uint16_t)((uint8_t *)(ptr))[1]) << 8) | \
    ((uint16_t)((uint8_t *)(ptr))[0]))

/*! \brief Pack a uint16_t to a known little-endian buffer.
 *  \param ptr Pointer to the buffer into which to pack a uint16_t.
 *  \param val uint16_t value to pack into the buffer.
 */
#define pack_16l(ptr, val)                                    \
  do                                                          \
  {                                                           \
    ((uint8_t *)(ptr))[0] = (uint8_t) ((val) & 0xff);         \
    ((uint8_t *)(ptr))[1] = (uint8_t)(((val) & 0xff00) >> 8); \
  } while (0)

/*! \brief Unpack a uint32_t from a known big-endian buffer.
 *  \param ptr Pointer to the buffer from which to unpack a uint32_t.
 *  \return Unpacked uint32_t.
 */
#define upack_32b(ptr)                         \
  (((uint32_t)0u) |                            \
   (((uint32_t)((uint8_t *)(ptr))[0]) << 24) | \
   (((uint32_t)((uint8_t *)(ptr))[1]) << 16) | \
   (((uint32_t)((uint8_t *)(ptr))[2]) << 8)  | \
    ((uint32_t)((uint8_t *)(ptr))[3]))

/*! \brief Pack a uint32_t to a known big-endian buffer.
 *  \param ptr Pointer to the buffer into which to pack a uint32_t.
 *  \param val uint32_t value to pack into the buffer.
 */
#define pack_32b(ptr, val)                                         \
  do                                                               \
  {                                                                \
    ((uint8_t *)(ptr))[3] = (uint8_t) ((val) & 0xff);              \
    ((uint8_t *)(ptr))[2] = (uint8_t)(((val) & 0xff00) >> 8);      \
    ((uint8_t *)(ptr))[1] = (uint8_t)(((val) & 0xff0000) >> 16);   \
    ((uint8_t *)(ptr))[0] = (uint8_t)(((val) & 0xff000000) >> 24); \
  } while (0)

/*! \brief Unpack a uint32_t from a known little-endian buffer.
 *  \param ptr Pointer to the buffer from which to unpack a uint32_t.
 *  \return Unpacked uint32_t.
 */
#define upack_32l(ptr)                         \
  (((uint32_t)0u) |                            \
   (((uint32_t)((uint8_t *)(ptr))[3]) << 24) | \
   (((uint32_t)((uint8_t *)(ptr))[2]) << 16) | \
   (((uint32_t)((uint8_t *)(ptr))[1]) << 8)  | \
    ((uint32_t)((uint8_t *)(ptr))[0]))

/*! \brief Pack a uint32_t to a known little-endian buffer.
 *  \param ptr Pointer to the buffer into which to pack a uint32_t.
 *  \param val uint32_t value to pack into the buffer.
 */
#define pack_32l(ptr, val)                                         \
  do                                                               \
  {                                                                \
    ((uint8_t *)(ptr))[0] = (uint8_t) ((val) & 0xff);              \
    ((uint8_t *)(ptr))[1] = (uint8_t)(((val) & 0xff00) >> 8);      \
    ((uint8_t *)(ptr))[2] = (uint8_t)(((val) & 0xff0000) >> 16);   \
    ((uint8_t *)(ptr))[3] = (uint8_t)(((val) & 0xff000000) >> 24); \
  } while (0)

#if LWPA_64BIT_SUPPORT

/*! \brief Unpack a uint64_t from a known big-endian buffer.
 *  \param ptr Pointer to the buffer from which to unpack a uint64_t.
 *  \return Unpacked uint64_t.
 */
#define upack_64b(ptr)                         \
  (((uint64_t)0u) |                            \
   (((uint64_t)((uint8_t *)(ptr))[0]) << 56) | \
   (((uint64_t)((uint8_t *)(ptr))[1]) << 48) | \
   (((uint64_t)((uint8_t *)(ptr))[2]) << 40) | \
   (((uint64_t)((uint8_t *)(ptr))[3]) << 32) | \
   (((uint64_t)((uint8_t *)(ptr))[4]) << 24) | \
   (((uint64_t)((uint8_t *)(ptr))[5]) << 16) | \
   (((uint64_t)((uint8_t *)(ptr))[6]) << 8)  | \
    ((uint64_t)((uint8_t *)(ptr))[7]))

/*! \brief Pack a uint64_t to a known big-endian buffer.
 *  \param ptr Pointer to the buffer into which to pack a uint64_t.
 *  \param val uint64_t value to pack into the buffer.
 */
#define pack_64b(ptr, val)                                                 \
  do                                                                       \
  {                                                                        \
    ((uint8_t *)(ptr))[7] = (uint8_t) ((val) & 0xff);                      \
    ((uint8_t *)(ptr))[6] = (uint8_t)(((val) & 0xff00) >> 8);              \
    ((uint8_t *)(ptr))[5] = (uint8_t)(((val) & 0xff0000) >> 16);           \
    ((uint8_t *)(ptr))[4] = (uint8_t)(((val) & 0xff000000) >> 24);         \
    ((uint8_t *)(ptr))[3] = (uint8_t)(((val) & 0xff00000000) >> 32);       \
    ((uint8_t *)(ptr))[2] = (uint8_t)(((val) & 0xff0000000000) >> 40);     \
    ((uint8_t *)(ptr))[1] = (uint8_t)(((val) & 0xff000000000000) >> 48);   \
    ((uint8_t *)(ptr))[0] = (uint8_t)(((val) & 0xff00000000000000) >> 56); \
  } while (0)

/*! \brief Unpack a uint64_t from a known little-endian buffer.
 *  \param ptr Pointer to the buffer from which to unpack a uint64_t.
 *  \return Unpacked uint64_t.
 */
#define upack_64l(ptr)                         \
  (((uint64_t)0u) |                            \
   (((uint64_t)((uint8_t *)(ptr))[7]) << 56) | \
   (((uint64_t)((uint8_t *)(ptr))[6]) << 48) | \
   (((uint64_t)((uint8_t *)(ptr))[5]) << 40) | \
   (((uint64_t)((uint8_t *)(ptr))[4]) << 32) | \
   (((uint64_t)((uint8_t *)(ptr))[3]) << 24) | \
   (((uint64_t)((uint8_t *)(ptr))[2]) << 16) | \
   (((uint64_t)((uint8_t *)(ptr))[1]) << 8)  | \
    ((uint64_t)((uint8_t *)(ptr))[0]))

/*! \brief Pack a uint64_t to a known little-endian buffer.
 *  \param ptr Pointer to the buffer into which to pack a uint64_t.
 *  \param val uint64_t value to pack into the buffer.
 */
#define pack_64l(ptr, val)                                                 \
  do                                                                       \
  {                                                                        \
    ((uint8_t *)(ptr))[0] = (uint8_t) ((val) & 0xff);                      \
    ((uint8_t *)(ptr))[1] = (uint8_t)(((val) & 0xff00) >> 8);              \
    ((uint8_t *)(ptr))[2] = (uint8_t)(((val) & 0xff0000) >> 16);           \
    ((uint8_t *)(ptr))[3] = (uint8_t)(((val) & 0xff000000) >> 24);         \
    ((uint8_t *)(ptr))[4] = (uint8_t)(((val) & 0xff00000000) >> 32);       \
    ((uint8_t *)(ptr))[5] = (uint8_t)(((val) & 0xff0000000000) >> 40);     \
    ((uint8_t *)(ptr))[6] = (uint8_t)(((val) & 0xff000000000000) >> 48);   \
    ((uint8_t *)(ptr))[7] = (uint8_t)(((val) & 0xff00000000000000) >> 56); \
  } while (0)

#endif /* LWPA_64BIT_SUPPORT */

/*!@}*/

#endif /* _LWPA_PACK_H_ */
