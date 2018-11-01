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

#include "lwpa/uuid.h"

#include <stdio.h>
#include <stddef.h>
#include "md5.h"

/* Suppress sprintf() warning on Windows/MSVC. */
#ifdef _MSC_VER
#pragma warning(disable : 4996)
#endif

const LwpaUuid NULL_UUID = {{0}};

/*! \brief Create a string representation of a UUID.
 *
 *  The resulting string will be of the form: xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx
 *  (lowercase is used for hexadecimal letters per RFC 4122 and common convention).
 *
 *  \param[out] buf Character buffer to which to write the resulting string. To avoid undefined
 *                  behavior, this buffer should be at least of size #UUID_STRING_BYTES.
 *  \param[in] uuid UUID to convert to a string.
 */
void uuid_to_string(char *buf, const LwpaUuid *uuid)
{
  const uint8_t *c = uuid->data;
  sprintf(buf, "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x", c[0], c[1], c[2], c[3], c[4],
          c[5], c[6], c[7], c[8], c[9], c[10], c[11], c[12], c[13], c[14], c[15]);
}

/*! \brief Create a UUID from a string representation.
 *
 *  Parses a string-represented UUID and fills in a LwpaUuid structure with the result. The input
 *  should be of the form: xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx
 *  (hexadecimal letters can be upper- or lowercase).
 *
 *  \param[out] uuid UUID to fill in with the parse result.
 *  \param[in] buf Character buffer containing the string to convert.
 *  \param[in] buflen Size in bytes of buf.
 *  \return true (parse successful) or false (parse failure).
 */
bool string_to_uuid(LwpaUuid *uuid, const char *buf, size_t buflen)
{
  const char *from_ptr = buf;
  uint8_t to_buf[UUID_BYTES];
  uint8_t *to_ptr = to_buf;
  bool first = true; /* Whether we are doing the first or second nibble of the byte */

  if (!uuid || !buf || buflen == 0)
    return false;

  while ((to_ptr - to_buf < UUID_BYTES) && (from_ptr - buf < (ptrdiff_t)buflen))
  {
    uint8_t offset = 0;
    if ((*from_ptr >= '0') && (*from_ptr <= '9'))
      offset = '0';
    else if ((*from_ptr >= 'A') && (*from_ptr <= 'F'))
      offset = 0x37; /* 0x41('A') - 0x37('7') = 0xa */
    else if ((*from_ptr >= 'a') && (*from_ptr <= 'f'))
      offset = 0x57; /* 0x61('a') - 0x57('W') = 0xa */

    if (offset != 0)
    {
      if (first)
      {
        *to_ptr = (uint8_t)(*from_ptr - offset);
        *to_ptr = (uint8_t)(*to_ptr << 4);
        first = false;
      }
      else
      {
        *to_ptr |= (uint8_t)(*from_ptr - offset);
        ++to_ptr;
        first = true;
      }
    }
    ++from_ptr;
  }

  if (to_ptr == to_buf + UUID_BYTES)
  {
    memcpy(uuid->data, to_buf, UUID_BYTES);
    return true;
  }
  return false;
}

/* Quick utility for generating a uuid out of a md5 hash buffer */
static void generate_from_hash(LwpaUuid *uuid_out, MD5_CTX *pmd5)
{
  uint8_t buffer[UUID_BYTES];
  MD5Final(buffer, pmd5);

  /* Add the UUID flags into the buffer */
  /* The Version bits to say this is a name-based UUID using MD5 */
  buffer[6] = (uint8_t)(0x30 | (buffer[6] & 0x0f));
  /* The variant bits to say this is encoded via RFC 4122 */
  buffer[8] = (uint8_t)(0x80 | (buffer[8] & 0x3f));

  memcpy(uuid_out->data, buffer, UUID_BYTES);
}

/*! \brief Generate a UUID from a combination of a custom string and MAC address.
 *
 *  This function is for use by devices that want to create their own UUIDs, and create the same
 *  UUIDs each time. The UUID output is deterministic for each combination of devstr, macaddr and
 *  uuidnum inputs.
 *
 *  \param[out] uuid UUID to fill in with the generation result.
 *  \param[in] devstr The device-specific string, such as the model name. This should never change
 *                    on the device. It also allows different programs running on the device to
 *                    generate different UUID sets.
 *  \param[in] macaddr The device's MAC address as an array of 6 bytes.
 *  \param[in] uuidnum Component number. By changing this number, multiple unique UUIDs can be
 *                     generated for the same device string-MAC address combination.
 */
void generate_uuid(LwpaUuid *uuid, const char *devstr, const uint8_t *macaddr, uint32_t uuidnum)
{
  MD5_CTX md5;
  uint8_t num[4];
  /* RFC4122 requires that we use a name space UUID before the string */
  uint8_t ns[16] = {0x57, 0x32, 0x31, 0x03, 0xDB, 0x01, 0x44, 0xb3, 0xba, 0xfa, 0xab, 0xde, 0xe3, 0xf3, 0x7c, 0x1a};

  if (!uuid || !devstr || !macaddr)
    return;

  MD5Init(&md5);
  MD5Update(&md5, ns, 16);

  /* The string we'll be encoding is "lwpa device,[devstr][macaddr][uuidnum in network order]" */
  MD5Update(&md5, (uint8_t *)"lwpa device, ", 13);
  if (devstr)
    MD5Update(&md5, (uint8_t *)(devstr), (unsigned int)strlen(devstr));

  MD5Update(&md5, macaddr, 6);

  num[0] = (uint8_t)(uuidnum & 0xFF);
  num[1] = (uint8_t)((uuidnum >> 8) & 0xFF);
  num[2] = (uint8_t)((uuidnum >> 16) & 0xFF);
  num[3] = (uint8_t)((uuidnum >> 24) & 0xFF);
  MD5Update(&md5, num, 4);

  generate_from_hash(uuid, &md5);
}

/*! \brief Generate a UUID from an RDM UID.
 *
 *  This function is used to consistently create the same UUID for an RDM device, regardless of the
 *  ACN component/location in which the device is being instanced.
 *
 *  \param[out] uuid UUID to fill in with the generation result.
 *  \param[in] rdmuid The 6-byte RDM unique identifier. The first 2 bytes are the manufacturer ID,
 *                    and the last 4 bytes are the device ID.
 */
void generate_rdm_uuid(LwpaUuid *uuid, const uint8_t *rdmuid)
{
  MD5_CTX md5;
  uint8_t ns[16] = {0xba, 0x8a, 0xfd, 0x46, 0x35, 0x78, 0x46, 0x3c, 0xa5, 0xa7, 0xa2, 0x8c, 0xe3, 0xd3, 0x2f, 0xe4};

  MD5Init(&md5);

  /* RFC4122 requires that we use a name space UUID before the string. */
  MD5Update(&md5, ns, 16);

  /* The string we'll be encoding is "RDM Device, [rdmuid]" */
  MD5Update(&md5, (uint8_t *)"RDM Device, ", 12);
  MD5Update(&md5, rdmuid, 6);

  generate_from_hash(uuid, &md5);
}