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

#include "etcpal/uuid.h"

#include <stdio.h>
#include <stddef.h>
#include "md5.h"

/****************************** Private macros *******************************/

#ifdef _MSC_VER
#define ETCPAL_SPRINTF __pragma(warning(suppress : 4996)) sprintf
#else
#define ETCPAL_SPRINTF sprintf
#endif

/**************************** Private variables ******************************/

const EtcPalUuid kEtcPalNullUuid = {{0}};

/*************************** Function definitions ****************************/

/*!
 * \brief Create a string representation of a UUID.
 *
 * The resulting string will be of the form: xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx (lowercase is
 * used for hexadecimal letters per RFC 4122 and common convention).
 *
 * \param[in] uuid UUID to convert to a string.
 * \param[out] buf Character buffer to which to write the resulting string. To avoid undefined
 *                 behavior, this buffer should be at least of size #ETCPAL_UUID_STRING_BYTES.
 * \return true (conversion successful) or false (invalid argument).
 */
bool etcpal_uuid_to_string(const EtcPalUuid* uuid, char* buf)
{
  if (!uuid || !buf)
    return false;

  const uint8_t* c = uuid->data;

  /* Automatically suppresses MSVC warning - minimum buffer size is well documented and cannot be
   * exceeded by this format string. */
  ETCPAL_SPRINTF(buf, "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x", c[0], c[1], c[2], c[3],
                 c[4], c[5], c[6], c[7], c[8], c[9], c[10], c[11], c[12], c[13], c[14], c[15]);
  return true;
}

/*!
 * \brief Create a UUID from a string representation.
 *
 * Parses a string-represented UUID and fills in a EtcPalUuid structure with the result. The input
 * should be of the form: xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx (hexadecimal letters can be upper-
 * or lowercase).
 *
 * \param[in] str The null-terminated string to convert.
 * \param[out] uuid UUID to fill in with the parse result.
 * \return true (parse successful) or false (parse failure).
 */
bool etcpal_string_to_uuid(const char* str, EtcPalUuid* uuid)
{
  if (!str || !uuid)
    return false;

  const char* from_ptr = str;
  uint8_t to_buf[ETCPAL_UUID_BYTES];
  uint8_t* to_ptr = to_buf;
  bool first = true; /* Whether we are doing the first or second nibble of the byte */

  while ((to_ptr - to_buf < ETCPAL_UUID_BYTES) && (*from_ptr != '\0'))
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

  if (to_ptr == to_buf + ETCPAL_UUID_BYTES)
  {
    memcpy(uuid->data, to_buf, ETCPAL_UUID_BYTES);
    return true;
  }
  return false;
}

/* This documentation appears here; the actual functions are in os/[os name]/etcpal/os_uuid.c */
/*!
 * \fn etcpal_error_t etcpal_generate_v1_uuid(EtcPalUuid *uuid)
 * \brief Generate a Version 1 UUID.
 *
 * This function uses the underlying OS API to create a UUID that is based on a combination of a
 * local MAC address and the current system time. This method guarantees UUID uniqueness across
 * space and time. Some OSes have deprecated this method because it creates a UUID that is
 * traceable to the MAC address of the machine on which it was generated. If this type of security
 * is a concern, etcpal_generate_v4_uuid() should be preferred. If you want to generate UUIDs that
 * are deterministic for a combination of inputs you provide, see etcpal_generate_v3_uuid().
 *
 * This function may return #kEtcPalErrNotImpl on platforms that do not have this functionality
 * available (this is mostly a concern for RTOS-level embedded platforms).
 *
 * \param[out] uuid UUID to fill in with the generation result.
 * \return #kEtcPalErrOk: UUID generated successfully.
 * \return #kEtcPalErrInvalid: Invalid argument provided.
 * \return #kEtcPalErrNotImpl: This UUID generation method is not available on this platform.
 * \return #kEtcPalErrSys: An internal library of system call error occurred.
 */

/* Quick utility for generating a uuid out of a md5 hash buffer */
static void generate_from_hash(EtcPalUuid* uuid_out, MD5_CTX* pmd5)
{
  uint8_t buffer[ETCPAL_UUID_BYTES];
  MD5Final(buffer, pmd5);

  /* Add the UUID flags into the buffer */
  /* The Version bits to say this is a name-based UUID using MD5 */
  buffer[6] = (uint8_t)(0x30 | (buffer[6] & 0x0f));
  /* The variant bits to say this is encoded via RFC 4122 */
  buffer[8] = (uint8_t)(0x80 | (buffer[8] & 0x3f));

  memcpy(uuid_out->data, buffer, ETCPAL_UUID_BYTES);
}

/*!
 * \brief Generate a Version 3 UUID from a combination of a custom string and MAC address.
 *
 * This function is for use by devices that want to create their own UUIDs, and create the same
 * UUIDs each time. It creates a Version 3 UUID (defined in RFC 4122) within a constant, hardcoded
 * namespace. The UUID output is deterministic for each combination of devstr, macaddr and
 * uuidnum inputs.
 *
 * The namespace UUID used is: 57323103-db01-44b3-bafa-abdee3f37c1a
 *
 * \param[in] devstr The device-specific string, such as the model name. This should never change
 *                   on the device. It also allows different programs running on the device to
 *                   generate different UUID sets.
 * \param[in] macaddr The device's MAC address as an array of 6 bytes.
 * \param[in] uuidnum Component number. By changing this number, multiple unique UUIDs can be
 *                    generated for the same device string-MAC address combination.
 * \param[out] uuid UUID to fill in with the generation result.
 * \return #kEtcPalErrOk: UUID generated successfully.
 * \return #kEtcPalErrInvalid: Invalid argument provided.
 */
etcpal_error_t etcpal_generate_v3_uuid(const char* devstr, const uint8_t* macaddr, uint32_t uuidnum, EtcPalUuid* uuid)
{
  if (!devstr || !macaddr || !uuid)
    return kEtcPalErrInvalid;

  MD5_CTX md5;
  /* RFC4122 requires that we use a name space UUID before the string */
  uint8_t ns[16] = {0x57, 0x32, 0x31, 0x03, 0xdb, 0x01, 0x44, 0xb3, 0xba, 0xfa, 0xab, 0xde, 0xe3, 0xf3, 0x7c, 0x1a};

  MD5Init(&md5);
  MD5Update(&md5, ns, 16);

  /* The string we'll be encoding is "EtcPal device,[devstr][macaddr][uuidnum in network order]" */
  MD5Update(&md5, (uint8_t*)"EtcPal device, ", 15);
  if (devstr)
    MD5Update(&md5, (uint8_t*)(devstr), (unsigned int)strlen(devstr));

  MD5Update(&md5, macaddr, 6);

  uint8_t num[4];
  num[0] = (uint8_t)(uuidnum & 0xFF);
  num[1] = (uint8_t)((uuidnum >> 8) & 0xFF);
  num[2] = (uint8_t)((uuidnum >> 16) & 0xFF);
  num[3] = (uint8_t)((uuidnum >> 24) & 0xFF);
  MD5Update(&md5, num, 4);

  generate_from_hash(uuid, &md5);
  return kEtcPalErrOk;
}

/* This documentation appears here; the actual functions are in os/[os name]/etcpal/os_uuid.c */
/*!
 * \fn etcpal_error_t etcpal_generate_v4_uuid(EtcPalUuid *uuid)
 * \brief Generate a Version 4 UUID.
 *
 * This function uses the underlying OS API to create a UUID that is based on random data. The
 * quality of the random data used may differ across platforms. If you want to generate UUIDs that
 * are deterministic for a combination of inputs you provide, see etcpal_generate_v3_uuid().
 *
 * This function may return #kEtcPalErrNotImpl on platforms that do not have this functionality
 * available (this is mostly a concern for RTOS-level embedded platforms).
 *
 * \param[out] uuid UUID to fill in with the generation result.
 * \return #kEtcPalErrOk: UUID generated successfully.
 * \return #kEtcPalErrInvalid: Invalid argument provided.
 * \return #kEtcPalErrNotImpl: This UUID generation method is not available on this platform.
 * \return #kEtcPalErrSys: An internal library of system call error occurred.
 */

/* This documentation appears here; the actual functions are in os/[os name]/etcpal/os_uuid.c */
/*!
 * \fn etcpal_error_t etcpal_generate_os_preferred_uuid(EtcPalUuid *uuid)
 * \brief Generate the preferred UUID version of the underlying OS.
 *
 * This function uses the underlying OS API to create a UUID of the recommended type per the
 * underlying OS API. In practice, this is often a V4 UUID, although this is not guaranteed.
 *
 * This function may return #kEtcPalErrNotImpl on platforms that do not have this functionality
 * available (this is mostly a concern for RTOS-level embedded platforms).
 *
 * \param[out] uuid UUID to fill in with the generation result.
 * \return #kEtcPalErrOk: UUID generated successfully.
 * \return #kEtcPalErrInvalid: Invalid argument provided.
 * \return #kEtcPalErrNotImpl: This UUID generation method is not available on this platform.
 * \return #kEtcPalErrSys: An internal library of system call error occurred.
 */
