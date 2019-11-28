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

/* etcpal/uuid.h: A type and.helper functions for a Universally Unique Identifier (UUID) */

#ifndef ETCPAL_UUID_H_
#define ETCPAL_UUID_H_

#include <string.h>
#include "etcpal/int.h"
#include "etcpal/bool.h"
#include "etcpal/error.h"

/*!
 * \defgroup etcpal_uuid UUIDs (uuid)
 * \ingroup etcpal
 * \brief Type and helper functions for a Universally Unique Identifier (UUID).
 *
 * UUIDs (per RFC 4122) are used in several libraries supported by EtcPal. This module contains a
 * type for a UUID and functions to generate, inspect and manipulate them.
 *
 * ```c
 * #include "etcpal/uuid.h"
 * ```
 *
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/*! The number of bytes that make up a UUID. */
#define ETCPAL_UUID_BYTES 16u

/*! The UUID type. */
typedef struct EtcPalUuid
{
  uint8_t data[ETCPAL_UUID_BYTES]; /*!< The 16-byte UUID data. */
} EtcPalUuid;

/*!
 * \brief Compare two UUIDs.
 * \param uuid1ptr Pointer to first EtcPalUuid to compare.
 * \param uuid2ptr Pointer to second EtcPalUuid to compare.
 * \return < 0: uuid1ptr is less than uuid2ptr
 * \return 0: uuid1ptr is equal to uuid2ptr
 * \return > 0: uuid1ptr is greater than uuid2ptr
 */
#define ETCPAL_UUID_CMP(uuid1ptr, uuid2ptr) memcmp((uuid1ptr)->data, (uuid2ptr)->data, ETCPAL_UUID_BYTES)

/*! A null (all 0's) UUID, used by ETCPAL_UUID_IS_NULL() for comparison. */
extern const EtcPalUuid kEtcPalNullUuid;

/*!
 * \brief Determine if a UUID is null.
 *
 * A UUID is said to be 'null' when it is made up of all 0's.
 *
 * \param uuidptr Pointer to UUID to null-check.
 * \return true (UUID is null) or false (UUID is not null).
 */
#define ETCPAL_UUID_IS_NULL(uuidptr) (memcmp((uuidptr)->data, kEtcPalNullUuid.data, ETCPAL_UUID_BYTES) == 0)

/*! The maximum number of bytes required to hold an ASCII string representation of a UUID. */
#define ETCPAL_UUID_STRING_BYTES 37

bool etcpal_uuid_to_string(const EtcPalUuid* uuid, char* buf);
bool etcpal_string_to_uuid(const char* buf, EtcPalUuid* uuid);

/************************ UUID Generation Functions **************************/

etcpal_error_t etcpal_generate_v1_uuid(EtcPalUuid* uuid);
etcpal_error_t etcpal_generate_v3_uuid(const char* devstr, const uint8_t* macaddr, uint32_t uuidnum, EtcPalUuid* uuid);
etcpal_error_t etcpal_generate_v4_uuid(EtcPalUuid* uuid);
etcpal_error_t etcpal_generate_os_preferred_uuid(EtcPalUuid* uuid);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

/* C++ utilities */

/* Comparison operators for UUIDs */

inline bool operator<(const EtcPalUuid& a, const EtcPalUuid& b)
{
  return (ETCPAL_UUID_CMP(&a, &b) < 0);
}

inline bool operator==(const EtcPalUuid& a, const EtcPalUuid& b)
{
  return (ETCPAL_UUID_CMP(&a, &b) == 0);
}

#endif

/*!
 * @}
 */

#endif /* ETCPAL_UUID_H_ */
