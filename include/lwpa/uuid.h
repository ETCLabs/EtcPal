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

/* lwpa/uuid.h: A type and helper functions for a Universally Unique Identifier (UUID) */
#ifndef _LWPA_UUID_H_
#define _LWPA_UUID_H_

#include <string.h>
#include "lwpa/int.h"
#include "lwpa/bool.h"
#include "lwpa/error.h"

/*! \defgroup lwpa_uuid lwpa_uuid
 *  \ingroup lwpa
 *  \brief Type and helper functions for a Universally Unique Identifier (UUID).
 *
 *  UUIDs (per RFC 4122) are used in several protocols supported by lwpa. This module contains a
 *  type for a UUID and functions to generate, inspect and manipulate them.
 *
 *  \#include "lwpa/uuid.h"
 *
 *  @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/*! The number of bytes that make up a UUID. */
#define LWPA_UUID_BYTES 16u

/*! The UUID type. */
typedef struct LwpaUuid
{
  uint8_t data[LWPA_UUID_BYTES];
} LwpaUuid;

/*! \brief Compare two UUIDs.
 *  \param uuid1ptr Pointer to first LwpaUuid to compare.
 *  \param uuid2ptr Pointer to second LwpaUuid to compare.
 *  \return < 0 (uuid1ptr is less than uuid2ptr)\n
 *            0 (uuid1ptr is equal to uuid2ptr)\n
 *          > 0 (uuid1ptr is greater than uuid2ptr)
 */
#define lwpa_uuid_cmp(uuid1ptr, uuid2ptr) memcmp((uuid1ptr)->data, (uuid2ptr)->data, LWPA_UUID_BYTES)

/*! A null (all 0's) UUID, used by uuid_isnull() for comparison. */
extern const LwpaUuid kLwpaNullUuid;

/*! \brief Determine if a UUID is null.
 *
 *  A UUID is said to be 'null' when it is made up of all 0's.
 *
 *  \param uuidptr Pointer to UUID to null-check.
 *  \return true (UUID is null) or false (UUID is not null).
 */
#define lwpa_uuid_is_null(uuidptr) (memcmp((uuidptr)->data, kLwpaNullUuid.data, LWPA_UUID_BYTES) == 0)

/*! The maximum number of bytes required to hold an ASCII string representation of a UUID. */
#define LWPA_UUID_STRING_BYTES 37

void lwpa_uuid_to_string(char *buf, const LwpaUuid *uuid);
bool lwpa_string_to_uuid(LwpaUuid *uuid, const char *buf, size_t buflen);

/************************ UUID Generation Functions **************************/

lwpa_error_t lwpa_generate_v1_uuid(LwpaUuid *uuid);
lwpa_error_t lwpa_generate_v3_uuid(LwpaUuid *uuid, const char *devstr, const uint8_t *macaddr, uint32_t uuidnum);
lwpa_error_t lwpa_generate_v4_uuid(LwpaUuid *uuid);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
/* C++ utilities */

/* Comparison operators for UUIDs */

inline bool operator<(const LwpaUuid &a, const LwpaUuid &b)
{
  return (lwpa_uuid_cmp(&a, &b) < 0);
}

inline bool operator==(const LwpaUuid &a, const LwpaUuid &b)
{
  return (lwpa_uuid_cmp(&a, &b) == 0);
}

#endif

/*!@}*/

#endif /* _LWPA_UUID_H_ */
