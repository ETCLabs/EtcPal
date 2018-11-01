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

/* lwpa/uuid.h: A type and helper functions for a Universally Unique Identifier (UUID) */
#ifndef _LWPA_UUID_H_
#define _LWPA_UUID_H_

#include <string.h>
#include "lwpa/int.h"
#include "lwpa/bool.h"

/*! \defgroup lwpa_uuid lwpa_uuid
 *  \ingroup lwpa
 *  \brief Type and helper functions for the Component IDentifier (UUID) used in
 *         the ACN family of protocols.
 *
 *  ACN-based protocols use an identifier called a UUID to identify a Component,
 *  which E1.17 defines as "a distinct endpoint transmitting and receiving ACN
 *  data". A UUID is simply a UUID which should not change over the lifetime of
 *  a device. To this end, functions are provided which help with UUID
 *  generation.
 *
 *  \#include "lwpa/uuid.h"
 *
 *  @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/*! The number of bytes that make up a UUID. */
#define UUID_BYTES 16u

/*! The UUID type. */
typedef struct LwpaUuid
{
  uint8_t data[UUID_BYTES];
} LwpaUuid;

/*! \brief Compare two UUIDs.
 *  \param uuid1ptr Pointer to first LwpaUuid to compare.
 *  \param uuid2ptr Pointer to second LwpaUuid to compare.
 *  \return < 0 (uuid1ptr is less than uuid2ptr)\n
 *            0 (uuid1ptr is equal to uuid2ptr)\n
 *          > 0 (uuid1ptr is greater than uuid2ptr)
 */
#define uuidcmp(uuid1ptr, uuid2ptr) memcmp((uuid1ptr)->data, (uuid2ptr)->data, UUID_BYTES)

/*! A null (all 0's) UUID, used by uuid_isnull() for comparison. */
extern const LwpaUuid NULL_UUID;

/*! \brief Determine if a UUID is null.
 *
 *  A UUID is said to be 'null' when it is made up of all 0's.
 *
 *  \param uuidptr Pointer to UUID to null-check.
 *  \return true (UUID is null) or false (UUID is not null).
 */
#define uuid_isnull(uuidptr) (memcmp((uuidptr)->data, NULL_UUID.data, UUID_BYTES) == 0)

/*! The maximum number of bytes required to hold an ASCII string representation of a UUID. */
#define UUID_STRING_BYTES 37

void uuid_to_string(char *buf, const LwpaUuid *uuid);
bool string_to_uuid(LwpaUuid *uuid, const char *buf, size_t buflen);

/************************ UUID Generation Functions **************************/

void generate_uuid(LwpaUuid *uuid, const char *devstr, const uint8_t *macaddr, uint32_t uuidnum);
void generate_rdm_uuid(LwpaUuid *uuid, const uint8_t *rdmuid);

#ifdef __cplusplus
}
#endif

/*!@}*/

#endif /* _LWPA_UUID_H_ */
