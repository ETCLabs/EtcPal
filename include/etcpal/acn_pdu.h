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

/* etcpal/acn_pdu.h: Functions to parse and pack a generic ACN PDU as defined in ANSI E1.17. */

#ifndef ETCPAL_ACN_PDU_H_
#define ETCPAL_ACN_PDU_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/**
 * @defgroup etcpal_acn_pdu acn_pdu (ACN Protocol Family PDUs)
 * @ingroup etcpal
 * @brief Parse or pack a PDU or PDU block as defined in ANSI E1.17 (ACN).
 *
 * ```c
 * #include "etcpal/acn_pdu.h"
 * ```
 *
 * This module handles building and parsing the ACN PDU format as defined in ANSI E1.17, ACN
 * Architecture document, &sect; 2. EtcPal supports a few different ACN protocol family libraries,
 * including sACN and RDMnet, so it's convenient to have this logic here.
 *
 * @{
 */

/**
 * @name Get the inheritance and length bits in an ACN PDU Flags & Length segment.
 * The first byte of each PDU contains a flags field which indicates the inheritance properties of
 * that PDU. For more information, see ANSI E1.17, ACN Architecture, &sect; 2.4.
 * @{
 */

/**
 * @brief Determine whether the L flag is set in an ACN PDU flags field.
 * @param flags_byte The first byte of the PDU.
 * @return true (the L flag is set) or false (the L flags is not set).
 */
#define ACN_PDU_L_FLAG_SET(flags_byte) ((bool)((flags_byte)&0x80u))

/**
 * @brief Determine whether the V flag is set in an ACN PDU flags field.
 * @param flags_byte The first byte of the PDU.
 * @return true (the V flag is set) or false (the V flag is not set).
 */
#define ACN_PDU_V_FLAG_SET(flags_byte) ((bool)((flags_byte)&0x40u))

/**
 * @brief Determine whether the H flag is set in an ACN PDU flags field.
 * @param flags_byte The first byte of the PDU.
 * @return true (the H flag is set) or false (the H flag is not set).
 */
#define ACN_PDU_H_FLAG_SET(flags_byte) ((bool)((flags_byte)&0x20u))

/**
 * @brief Determine whether the D flag is set in an ACN PDU flags field.
 * @param flags_byte The first byte of the PDU.
 * @return true (the D flag is set) or false (the D flag is not set).
 */
#define ACN_PDU_D_FLAG_SET(flags_byte) ((bool)((flags_byte)&0x10u))

/**
 * @}
 */

/**
 * @brief Get the length from the Length field of an ACN PDU.
 * @param pdu_buf Pointer to the start of the PDU buffer.
 * @return The length of the PDU.
 */
#define ACN_PDU_LENGTH(pdu_buf)                                                                                   \
  ((uint32_t)(ACN_PDU_L_FLAG_SET(pdu_buf[0])                                                                      \
                  ? ((uint32_t)((pdu_buf[0] & 0x0fu) << 16) | (uint32_t)(pdu_buf[1] << 8) | (uint32_t)pdu_buf[2]) \
                  : ((uint32_t)((pdu_buf[0] & 0x0fu) << 8) | (uint32_t)pdu_buf[1])))

/**
 * @name Set the inheritance and length bits in a PDU Flags & Length segment.
 * The first byte of each PDU contains a flags field which indicates the inheritance properties of
 * that PDU. For more information, see ANSI E1.17, ACN Architecture, &sect; 2.4.
 * @{
 */

/**
 * @brief Set the L flag in an ACN PDU flags field.
 * @param flags_byte The first byte of the PDU.
 */
#define ACN_PDU_SET_L_FLAG(flags_byte) (flags_byte |= 0x80u)

/**
 * @brief Set the V flag in an ACN PDU flags field.
 * @param flags_byte The first byte of the PDU.
 */
#define ACN_PDU_SET_V_FLAG(flags_byte) (flags_byte |= 0x40u)

/**
 * @brief Set the H flag in an ACN PDU flags field.
 * @param flags_byte The first byte of the PDU.
 */
#define ACN_PDU_SET_H_FLAG(flags_byte) (flags_byte |= 0x20u)

/**
 * @brief Set the D flag in an ACN PDU flags field.
 * @param flags_byte The first byte of the PDU.
 */
#define ACN_PDU_SET_D_FLAG(flags_byte) (flags_byte |= 0x10u)

/**
 * @}
 */

/**
 * @brief Fill in the Length field of an ACN PDU which has a length less than 4096.
 *
 * The L flag of this PDU must be set to 0.
 *
 * @param pdu_buf Pointer to the start of the PDU buffer.
 * @param length Length of this PDU.
 */
#define ACN_PDU_PACK_NORMAL_LEN(pdu_buf, length)                                 \
  do                                                                             \
  {                                                                              \
    (pdu_buf)[0] = (uint8_t)(((pdu_buf)[0] & 0xf0) | (((length) >> 8) & 0x0fu)); \
    (pdu_buf)[1] = (uint8_t)(length)&0xffu;                                      \
  } while (0)

/**
 * @brief Fill in the Length field of an ACN PDU which has a length 4096 or greater.
 *
 * The L flag of this PDU must be set to 1.
 *
 * @param pdu_buf Pointer to the start of the PDU buffer.
 * @param length Length of this PDU.
 */
#define ACN_PDU_PACK_EXT_LEN(pdu_buf, length)                                     \
  do                                                                              \
  {                                                                               \
    (pdu_buf)[0] = (uint8_t)(((pdu_buf)[0] & 0xf0) | (((length) >> 16) & 0x0fu)); \
    (pdu_buf)[1] = (uint8_t)(((length) >> 8) & 0xffu);                            \
    (pdu_buf)[2] = (uint8_t)(length)&0xffu;                                       \
  } while (0)

/** Holds state data used when parsing multiple PDUs in a PDU block. */
typedef struct AcnPdu
{
  const uint8_t* pvector;  /**< Pointer to the most recent PDU's vector. */
  const uint8_t* pheader;  /**< Pointer to the most recent PDU's header. */
  const uint8_t* pdata;    /**< Pointer to the most recent PDU's data. */
  size_t         data_len; /**< Length of the PDU's data section. */
  const uint8_t* pnextpdu; /**< Pointer to the beginning of the next PDU. */
} AcnPdu;

/** Default AcnPdu initializer values; must be used to intialize an AcnPdu when parsing the first
 *  PDU in a block. */
#define ACN_PDU_INIT          \
  {                           \
    NULL, NULL, NULL, 0, NULL \
  }

/**
 * An alternative to #ACN_PDU_INIT which can be used on an existing AcnPdu to re-initialize its
 * values.
 *
 * @param pduptr Pointer to AcnPdu to initialize.
 */
#define ACN_INIT_PDU(pduptr)   \
  do                           \
  {                            \
    (pduptr)->pvector = NULL;  \
    (pduptr)->pheader = NULL;  \
    (pduptr)->pdata = NULL;    \
    (pduptr)->data_len = 0;    \
    (pduptr)->pnextpdu = NULL; \
  } while (0)

/** Contains specific PDU info used by the generic helper acn_parse_pdu(). */
typedef struct AcnPduConstraints
{
  size_t vector_size; /**< The size of the Vector segment of this PDU. */
  size_t header_size; /**< The size of the Header segment of this PDU. */
} AcnPduConstraints;

#ifdef __cplusplus
extern "C" {
#endif

bool acn_parse_pdu(const uint8_t* buf, size_t buflen, const AcnPduConstraints* constraints, AcnPdu* pdu);

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif /* ETCPAL_ACN_PDU_H_ */
