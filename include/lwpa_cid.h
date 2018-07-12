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

/* lwpa_cid.h: A type and helper functions for the Component IDentifier (CID)
 * used in the ACN family of protocols. */
#ifndef _LWPA_CID_H_
#define _LWPA_CID_H_

#include <string.h>
#include "lwpa_int.h"
#include "lwpa_bool.h"

/*! \defgroup lwpa_cid lwpa_cid
 *  \ingroup lwpa
 *  \brief Type and helper functions for the Component IDentifier (CID) used in
 *         the ACN family of protocols.
 *
 *  ACN-based protocols use an identifier called a CID to identify a Component,
 *  which E1.17 defines as "a distinct endpoint transmitting and receiving ACN
 *  data". A CID is simply a UUID which should not change over the lifetime of
 *  a device. To this end, functions are provided which help with CID
 *  generation.
 *
 *  \#include "lwpa_cid.h"
 *
 *  @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/*! The number of bytes that make up a CID. */
#define CID_BYTES 16u

/*! The CID type. Note that this can also be used to identify a DCID, as
 *  defined in DDL (or, for that matter, a UUID used for any other purpose). */
typedef struct LwpaCid
{
  uint8_t data[CID_BYTES];
} LwpaCid;

/*! \brief Compare two CIDs.
 *  \param cid1ptr Pointer to first LwpaCid to compare.
 *  \param cid2ptr Pointer to second LwpaCid to compare.
 *  \return < 0 (cid1ptr is less than cid2ptr)\n
 *            0 (cid1ptr is equal to cid2ptr)\n
 *          > 0 (cid1ptr is greater than cid2ptr)
 */
#define cidcmp(cid1ptr, cid2ptr) memcmp((cid1ptr)->data, (cid2ptr)->data, CID_BYTES)

/*! A null (all 0's) CID, used by cid_isnull() for comparison. */
extern const LwpaCid NULL_CID;

/*! \brief Determine if a CID is null.
 *
 *  A CID is said to be 'null' when it is made up of all 0's.
 *
 *  \param cidptr Pointer to CID to null-check.
 *  \return true (CID is null) or false (CID is not null).
 */
#define cid_isnull(cidptr) (memcmp((cidptr)->data, NULL_CID.data, CID_BYTES) == 0)

/*! The maximum number of bytes required to hold an ASCII string representation
 *  of a CID. */
#define CID_STRING_BYTES 37

void cid_to_string(char *buf, const LwpaCid *cid);
bool string_to_cid(LwpaCid *cid, const char *buf, size_t buflen);

/************************ CID Generation Functions ***************************/

void generate_cid(LwpaCid *cid, const char *devstr, const uint8_t *macaddr, uint32_t cidnum);
void generate_rdm_cid(LwpaCid *cid, const uint8_t *rdmuid);

#ifdef __cplusplus
}
#endif

/*!@}*/

#endif /* _LWPA_CID_H_ */
