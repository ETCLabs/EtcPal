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

/* etcpal/acn_rlp.h: Functions to parse or pack a Root Layer PDU block (as defined in ANSI
 * E1.17) and its associated preambles. */

#ifndef ETCPAL_ACN_RLP_H_
#define ETCPAL_ACN_RLP_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "etcpal/acn_pdu.h"
#include "etcpal/acn_prot.h"
#include "etcpal/uuid.h"

/**
 * @defgroup etcpal_acn_rlp acn_rlp (ACN Root Layer Protocol)
 * @ingroup etcpal
 * @brief Parse or pack an ACN Root Layer PDU block.
 *
 * ```c
 * #include "etcpal/acn_rlp.h"
 * ```
 *
 * This module handles building and parsing the ACN Root Layer Protocol, as defined in ANSI E1.17,
 * ACN Architecture document, &sect; 2.6. This protocol is also used by sACN (ANSI E1.31) and
 * RDMnet (ANSI E1.33).
 *
 * @{
 */

/** Size of an ACN family TCP preamble. */
#define ACN_TCP_PREAMBLE_SIZE 16
/** Size of an ACN family UDP preamble. */
#define ACN_UDP_PREAMBLE_SIZE 16
/** Size of a Root Layer PDU header when the length is less than 4096. */
#define ACN_RLP_HEADER_SIZE_NORMAL_LEN 22
/** Size of the Root Layer PDU header when the length is 4096 or greater. */
#define ACN_RLP_HEADER_SIZE_EXT_LEN 23

/**
 * @name Protocol Vectors
 * Each ACN family protocol is defined by a protocol vector in a Root Layer PDU. These values
 * occupy the vector field of a RootLayerPdu to identify the contents of its data segment.
 * @{
 */
#define ACN_VECTOR_ROOT_SDT ACN_PROTOCOL_SDT
#define ACN_VECTOR_ROOT_DRAFT_E131_DATA ACN_PROTOCOL_DRAFT_E131_DATA
#define ACN_VECTOR_ROOT_E131_DATA ACN_PROTOCOL_E131_DATA
#define ACN_VECTOR_ROOT_E131_EXTENDED ACN_PROTOCOL_E131_EXTENDED
#define ACN_VECTOR_ROOT_LLRP ACN_PROTOCOL_LLRP
#define ACN_VECTOR_ROOT_BROKER ACN_PROTOCOL_BROKER
#define ACN_VECTOR_ROOT_RPT ACN_PROTOCOL_RPT
#define ACN_VECTOR_ROOT_EPT ACN_PROTOCOL_EPT
/**
 * @}
 */

/** Holds the information contained in an ACN TCP Preamble. */
typedef struct AcnTcpPreamble
{
  /** Pointer to the beginning of the Root Layer PDU block */
  const uint8_t* rlp_block;
  /** Length of the Root Layer PDU block */
  size_t rlp_block_len;
} AcnTcpPreamble;

/** Holds the information contained in an ACN UDP Preamble. */
typedef struct AcnUdpPreamble
{
  /** Pointer to the beginning of the Root Layer PDU block */
  const uint8_t* rlp_block;
  /** Length of the Root Layer PDU block */
  size_t rlp_block_len;
} AcnUdpPreamble;

/** Holds the information contained in an ACN Root Layer PDU. */
typedef struct AcnRootLayerPdu
{
  /** The CID of the component that sent this Root Layer PDU. */
  EtcPalUuid sender_cid;
  /** The Vector indicates the type of data contained in the Data segment. */
  uint32_t vector;
  /** A pointer to the Data segment of this PDU. */
  const uint8_t* pdata;
  /** The length of the Data segment of this PDU. */
  size_t data_len;
} AcnRootLayerPdu;

#ifdef __cplusplus
extern "C" {
#endif

bool acn_parse_tcp_preamble(const uint8_t* buf, size_t buflen, AcnTcpPreamble* preamble);
bool acn_parse_udp_preamble(const uint8_t* buf, size_t buflen, AcnUdpPreamble* preamble);
bool acn_parse_root_layer_header(const uint8_t* buf, size_t buflen, AcnRootLayerPdu* pdu, AcnRootLayerPdu* last_pdu);
bool acn_parse_root_layer_pdu(const uint8_t* buf, size_t buflen, AcnRootLayerPdu* pdu, AcnPdu* last_pdu);

size_t acn_pack_tcp_preamble(uint8_t* buf, size_t buflen, size_t rlp_block_len);
size_t acn_pack_udp_preamble(uint8_t* buf, size_t buflen);
size_t acn_root_layer_buf_size(const AcnRootLayerPdu* pdu_block, size_t num_pdus);
size_t acn_pack_root_layer_header(uint8_t* buf, size_t buflen, const AcnRootLayerPdu* pdu);
size_t acn_pack_root_layer_block(uint8_t* buf, size_t buflen, const AcnRootLayerPdu* pdu_block, size_t num_pdus);

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif /* ETCPAL_ACN_RLP_H_ */
