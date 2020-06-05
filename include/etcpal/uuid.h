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

/* etcpal/uuid.h: A type and helper functions for a Universally Unique Identifier (UUID) */

#ifndef ETCPAL_UUID_H_
#define ETCPAL_UUID_H_

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "etcpal/error.h"

/**
 * @defgroup etcpal_uuid uuid (UUIDs)
 * @ingroup etcpal
 * @brief Type and helper functions for a Universally Unique Identifier (UUID).
 *
 * UUIDs (per RFC 4122) are used in several libraries supported by EtcPal. This module contains a
 * type for a UUID and functions to generate, inspect and manipulate them.
 *
 * ```c
 * #include "etcpal/uuid.h"
 * ```
 *
 * The basic UUID type is quite simple - an array of 16 data bytes. Functions are provided to
 * generate every current UUID type except Version 2, which is very rarely used. Additionally,
 * etcpal_generate_os_preferred_uuid() generates the UUID type preferred by the underlying OS, and
 * etcpal_generate_device_uuid() generates a special type of V5 UUID which can be used to identify
 * embedded devices.
 *
 * The basic form of UUID generation is:
 * @code
 * EtcPalUuid uuid;
 * if (etcpal_generate_v1_uuid(&uuid) == kEtcPalErrOk)
 * {
 *   // uuid now contains a valid Version 1 UUID.
 * }
 * @endcode
 *
 * **Note:** Not all UUID types are available on all systems. V3, V5 and Device UUIDs are
 * guaranteed to be available in all ports of EtcPal, but V1, V4 and os_preferred UUIDs are
 * generally not available on embedded systems:
 *
 * EtcPal Platform | V1  | V3  | V4  | V5  | os_preferred | device |
 * ----------------|-----|-----|-----|-----|--------------|--------|
 * FreeRTOS        | No  | Yes | No  | Yes | No           | Yes    |
 * Linux           | Yes | Yes | Yes | Yes | Yes          | Yes    |
 * macOS           | Yes | Yes | Yes | Yes | Yes          | Yes    |
 * MQX             | No  | Yes | No  | Yes | No           | Yes    |
 * Windows         | Yes | Yes | Yes | Yes | Yes          | Yes    |
 *
 * You can also convert UUIDs to and from strings:
 * @code
 * EtcPalUuid uuid;
 * etcpal_string_to_uuid("39713ce1-2320-46aa-b602-4977be36e155", &uuid);
 *
 * char str_buf[ETCPAL_UUID_STRING_BYTES];
 * etcpal_uuid_to_string(&uuid, str_buf);
 * @endcode
 *
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/** The number of bytes that make up a UUID. */
#define ETCPAL_UUID_BYTES 16u

/** The UUID type. */
typedef struct EtcPalUuid
{
  uint8_t data[ETCPAL_UUID_BYTES]; /**< The 16-byte UUID data. */
} EtcPalUuid;

/**
 * @brief Compare two UUIDs.
 * @param uuid1ptr Pointer to first EtcPalUuid to compare.
 * @param uuid2ptr Pointer to second EtcPalUuid to compare.
 * @return < 0: uuid1ptr is less than uuid2ptr
 * @return 0: uuid1ptr is equal to uuid2ptr
 * @return > 0: uuid1ptr is greater than uuid2ptr
 */
#define ETCPAL_UUID_CMP(uuid1ptr, uuid2ptr) memcmp((uuid1ptr)->data, (uuid2ptr)->data, ETCPAL_UUID_BYTES)

/** A null (all 0's) UUID, used by ETCPAL_UUID_IS_NULL() for comparison. */
extern const EtcPalUuid kEtcPalNullUuid;

/**
 * @brief Determine if a UUID is null.
 *
 * A UUID is said to be 'null' when it is made up of all 0's.
 *
 * @param uuidptr Pointer to UUID to null-check.
 * @return true (UUID is null) or false (UUID is not null).
 */
#define ETCPAL_UUID_IS_NULL(uuidptr) (memcmp((uuidptr)->data, kEtcPalNullUuid.data, ETCPAL_UUID_BYTES) == 0)

/** The maximum number of bytes required to hold an ASCII string representation of a UUID. */
#define ETCPAL_UUID_STRING_BYTES 37

/** The maximum length of a device string used as an input to etcpal_generate_device_uuid(). */
#define ETCPAL_UUID_DEV_STR_MAX_LEN 32

bool etcpal_uuid_to_string(const EtcPalUuid* uuid, char* buf);
bool etcpal_string_to_uuid(const char* str, EtcPalUuid* uuid);

/************************ UUID Generation Functions **************************/

etcpal_error_t etcpal_generate_v1_uuid(EtcPalUuid* uuid);
etcpal_error_t etcpal_generate_v3_uuid(const EtcPalUuid* ns, const void* name, size_t name_len, EtcPalUuid* uuid);
etcpal_error_t etcpal_generate_v4_uuid(EtcPalUuid* uuid);
etcpal_error_t etcpal_generate_v5_uuid(const EtcPalUuid* ns, const void* name, size_t name_len, EtcPalUuid* uuid);
etcpal_error_t etcpal_generate_os_preferred_uuid(EtcPalUuid* uuid);
etcpal_error_t etcpal_generate_device_uuid(const char*    dev_str,
                                           const uint8_t* mac_addr,
                                           uint32_t       uuid_num,
                                           EtcPalUuid*    uuid);

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

/**
 * @}
 */

#endif /* ETCPAL_UUID_H_ */
