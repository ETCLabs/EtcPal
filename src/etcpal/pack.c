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

#include "etcpal/pack.h"
#include "etcpal/private/opts.h"

/*************************** Function definitions ****************************/

/*!
 * \brief Unpack a uint16_t from a known big-endian buffer.
 * \param buf Pointer to the buffer from which to unpack a uint16_t.
 * \return Unpacked uint16_t.
 */
uint16_t etcpal_upack_16b(const uint8_t* buf)
{
  uint16_t val = 0;
  if (buf)
  {
    val |= (uint16_t)((uint16_t)buf[0] << 8);
    val |= (uint16_t)buf[1];
  }
  return val;
}

/*!
 * \brief Pack a uint16_t to a known big-endian buffer.
 * \param buf Pointer to the buffer into which to pack a uint16_t.
 * \param val uint16_t value to pack into the buffer.
 */
void etcpal_pack_16b(uint8_t* buf, uint16_t val)
{
  if (buf)
  {
    buf[1] = (uint8_t)(val & 0xff);
    buf[0] = (uint8_t)((val & 0xff00) >> 8);
  }
}

/*!
 * \brief Unpack a uint16_t from a known little-endian buffer.
 * \param buf Pointer to the buffer from which to unpack a uint16_t.
 * \return Unpacked uint16_t.
 */
uint16_t etcpal_upack_16l(const uint8_t* buf)
{
  uint16_t val = 0;
  if (buf)
  {
    val |= (uint16_t)((uint16_t)buf[1] << 8);
    val |= (uint16_t)buf[0];
  }
  return val;
}

/*!
 * \brief Pack a uint16_t to a known little-endian buffer.
 * \param buf Pointer to the buffer into which to pack a uint16_t.
 * \param val uint16_t value to pack into the buffer.
 */
void etcpal_pack_16l(uint8_t* buf, uint16_t val)
{
  if (buf)
  {
    buf[0] = (uint8_t)(val & 0xff);
    buf[1] = (uint8_t)((val & 0xff00) >> 8);
  }
}

/*!
 * \brief Unpack a uint32_t from a known big-endian buffer.
 * \param buf Pointer to the buffer from which to unpack a uint32_t.
 * \return Unpacked uint32_t.
 */
uint32_t etcpal_upack_32b(const uint8_t* buf)
{
  uint32_t val = 0;
  if (buf)
  {
    val |= (uint32_t)((uint32_t)buf[0] << 24);
    val |= (uint32_t)((uint32_t)buf[1] << 16);
    val |= (uint32_t)((uint32_t)buf[2] << 8);
    val |= (uint32_t)buf[3];
  }
  return val;
}

/*!
 * \brief Pack a uint32_t to a known big-endian buffer.
 * \param buf Pointer to the buffer into which to pack a uint32_t.
 * \param val uint32_t value to pack into the buffer.
 */
void etcpal_pack_32b(uint8_t* buf, uint32_t val)
{
  if (buf)
  {
    buf[3] = (uint8_t)(val & 0xff);
    buf[2] = (uint8_t)((val & 0xff00) >> 8);
    buf[1] = (uint8_t)((val & 0xff0000) >> 16);
    buf[0] = (uint8_t)((val & 0xff000000) >> 24);
  }
}

/*!
 * \brief Unpack a uint32_t from a known little-endian buffer.
 * \param buf Pointer to the buffer from which to unpack a uint32_t.
 * \return Unpacked uint32_t.
 */
uint32_t etcpal_upack_32l(const uint8_t* buf)
{
  uint32_t val = 0;
  if (buf)
  {
    val |= (uint32_t)((uint32_t)buf[3] << 24);
    val |= (uint32_t)((uint32_t)buf[2] << 16);
    val |= (uint32_t)((uint32_t)buf[1] << 8);
    val |= (uint32_t)buf[0];
  }
  return val;
}

/*!
 * \brief Pack a uint32_t to a known little-endian buffer.
 * \param buf Pointer to the buffer into which to pack a uint32_t.
 * \param val uint32_t value to pack into the buffer.
 */
void etcpal_pack_32l(uint8_t* buf, uint32_t val)
{
  if (buf)
  {
    buf[0] = (uint8_t)(val & 0xff);
    buf[1] = (uint8_t)((val & 0xff00) >> 8);
    buf[2] = (uint8_t)((val & 0xff0000) >> 16);
    buf[3] = (uint8_t)((val & 0xff000000) >> 24);
  }
}

#if ETCPAL_INCLUDE_PACK_64 || DOXYGEN

/*!
 * \brief Unpack a uint64_t from a known big-endian buffer.
 * \param buf Pointer to the buffer from which to unpack a uint64_t.
 * \return Unpacked uint64_t.
 */
uint64_t etcpal_upack_64b(const uint8_t* buf)
{
  uint64_t val = 0;
  if (buf)
  {
    val |= (uint64_t)((uint64_t)buf[0] << 56);
    val |= (uint64_t)((uint64_t)buf[1] << 48);
    val |= (uint64_t)((uint64_t)buf[2] << 40);
    val |= (uint64_t)((uint64_t)buf[3] << 32);
    val |= (uint64_t)((uint64_t)buf[4] << 24);
    val |= (uint64_t)((uint64_t)buf[5] << 16);
    val |= (uint64_t)((uint64_t)buf[6] << 8);
    val |= (uint64_t)buf[7];
  }
  return val;
}

/*!
 * \brief Pack a uint64_t to a known big-endian buffer.
 * \param buf Pointer to the buffer into which to pack a uint64_t.
 * \param val uint64_t value to pack into the buffer.
 */
void etcpal_pack_64b(uint8_t* buf, uint64_t val)
{
  if (buf)
  {
    buf[7] = (uint8_t)(val & 0xff);
    buf[6] = (uint8_t)((val & 0xff00) >> 8);
    buf[5] = (uint8_t)((val & 0xff0000) >> 16);
    buf[4] = (uint8_t)((val & 0xff000000) >> 24);
    buf[3] = (uint8_t)((val & 0xff00000000) >> 32);
    buf[2] = (uint8_t)((val & 0xff0000000000) >> 40);
    buf[1] = (uint8_t)((val & 0xff000000000000) >> 48);
    buf[0] = (uint8_t)((val & 0xff00000000000000) >> 56);
  }
}

/*!
 * \brief Unpack a uint64_t from a known little-endian buffer.
 * \param buf Pointer to the buffer from which to unpack a uint64_t.
 * \return Unpacked uint64_t.
 */
uint64_t etcpal_upack_64l(const uint8_t* buf)
{
  uint64_t val = 0;
  if (buf)
  {
    val |= ((uint64_t)buf[7] << 56);
    val |= ((uint64_t)buf[6] << 48);
    val |= ((uint64_t)buf[5] << 40);
    val |= ((uint64_t)buf[4] << 32);
    val |= ((uint64_t)buf[3] << 24);
    val |= ((uint64_t)buf[2] << 16);
    val |= ((uint64_t)buf[1] << 8);
    val |= ((uint64_t)buf[0]);
  }
  return val;
}

/*!
 * \brief Pack a uint64_t to a known little-endian buffer.
 * \param buf Pointer to the buffer into which to pack a uint64_t.
 * \param val uint64_t value to pack into the buffer.
 */
void etcpal_pack_64l(uint8_t* buf, uint64_t val)
{
  if (buf)
  {
    buf[0] = (uint8_t)(val & 0xff);
    buf[1] = (uint8_t)((val & 0xff00) >> 8);
    buf[2] = (uint8_t)((val & 0xff0000) >> 16);
    buf[3] = (uint8_t)((val & 0xff000000) >> 24);
    buf[4] = (uint8_t)((val & 0xff00000000) >> 32);
    buf[5] = (uint8_t)((val & 0xff0000000000) >> 40);
    buf[6] = (uint8_t)((val & 0xff000000000000) >> 48);
    buf[7] = (uint8_t)((val & 0xff00000000000000) >> 56);
  }
}

#endif /* ETCPAL_INCLUDE_PACK_64 */
