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

/* lwpa/root_layer_pdu.h: Functions to parse or pack a Root Layer PDU block (as defined in ANSI
 * E1.17) and its associated preambles. */
#ifndef _LWPA_ROOT_LAYER_PDU_H_
#define _LWPA_ROOT_LAYER_PDU_H_

#include <stddef.h>
#include "lwpa/int.h"
#include "lwpa/bool.h"
#include "lwpa/uuid.h"
#include "lwpa/acn_prot.h"
#include "lwpa/pdu.h"

/*! \defgroup lwpa_rootlayerpdu lwpa_rootlayerpdu
 *  \ingroup lwpa_pdu
 *  \brief Parse or pack a Root Layer PDU block.
 *
 *  \#include "lwpa/root_layer_pdu.h"
 *
 *  @{
 */

/*! Size of an ACN family TCP preamble. */
#define ACN_TCP_PREAMBLE_SIZE 16
/*! Size of an ACN family UDP preamble. */
#define ACN_UDP_PREAMBLE_SIZE 16
/*! Size of a Root Layer PDU header when the length is less than 4096. */
#define ACN_RLP_HEADER_SIZE_NORMAL_LEN 22
/*! Size of the Root Layer PDU header when the length is 4096 or greater. */
#define ACN_RLP_HEADER_SIZE_EXT_LEN 23

/*! \name Protocol Vectors
 *  Each ACN family protocol is defined by a protocol vector in a Root Layer PDU. These values
 *  occupy the vector field of a RootLayerPdu to identify the contents of its data segment.
 *  @{
 */
#define ACN_VECTOR_ROOT_SDT ACN_PROTOCOL_SDT
#define ACN_VECTOR_ROOT_DRAFT_E131_DATA ACN_PROTOCOL_DRAFT_E131_DATA
#define ACN_VECTOR_ROOT_E131_DATA ACN_PROTOCOL_E131_DATA
#define ACN_VECTOR_ROOT_E131_EXTENDED ACN_PROTOCOL_E131_EXTENDED
#define ACN_VECTOR_ROOT_LLRP ACN_PROTOCOL_LLRP
#define ACN_VECTOR_ROOT_BROKER ACN_PROTOCOL_BROKER
#define ACN_VECTOR_ROOT_RPT ACN_PROTOCOL_RPT
#define ACN_VECTOR_ROOT_EPT ACN_PROTOCOL_EPT
/*!@}*/

/*! Holds the information contained in an ACN TCP Preamble. */
typedef struct LwpaTcpPreamble
{
  /*! Pointer to the beginning of the Root Layer PDU block */
  const uint8_t* rlp_block;
  /*! Length of the Root Layer PDU block */
  size_t rlp_block_len;
} LwpaTcpPreamble;

/*! Holds the information contained in an ACN UDP Preamble. */
typedef struct LwpaUdpPreamble
{
  /*! Pointer to the beginning of the Root Layer PDU block */
  const uint8_t* rlp_block;
  /*! Length of the Root Layer PDU block */
  size_t rlp_block_len;
} LwpaUdpPreamble;

/*! Holds the information contained in an ACN Root Layer PDU. */
typedef struct LwpaRootLayerPdu
{
  /*! The CID of the component that sent this Root Layer PDU. */
  LwpaUuid sender_cid;
  /*! The Vector indicates the type of data contained in the Data segment. */
  uint32_t vector;
  /*! A pointer to the Data segment of this PDU. */
  const uint8_t* pdata;
  /*! The length of the Data segment of this PDU. */
  size_t datalen;
} LwpaRootLayerPdu;

#ifdef __cplusplus
extern "C" {
#endif

bool lwpa_parse_tcp_preamble(const uint8_t* buf, size_t buflen, LwpaTcpPreamble* preamble);
bool lwpa_parse_udp_preamble(const uint8_t* buf, size_t buflen, LwpaUdpPreamble* preamble);
bool lwpa_parse_root_layer_header(const uint8_t* buf, size_t buflen, LwpaRootLayerPdu* pdu, LwpaRootLayerPdu* last_pdu);
bool lwpa_parse_root_layer_pdu(const uint8_t* buf, size_t buflen, LwpaRootLayerPdu* pdu, LwpaPdu* last_pdu);

size_t lwpa_pack_tcp_preamble(uint8_t* buf, size_t buflen, size_t rlp_block_len);
size_t lwpa_pack_udp_preamble(uint8_t* buf, size_t buflen);
size_t lwpa_root_layer_buf_size(const LwpaRootLayerPdu* pdu_block, size_t num_pdus);
size_t lwpa_pack_root_layer_header(uint8_t* buf, size_t buflen, const LwpaRootLayerPdu* pdu);
size_t lwpa_pack_root_layer_block(uint8_t* buf, size_t buflen, const LwpaRootLayerPdu* pdu_block, size_t num_pdus);

#ifdef __cplusplus
}
#endif

/*!@}*/

#endif /* _LWPA_ROOT_LAYER_PDU_H_ */
