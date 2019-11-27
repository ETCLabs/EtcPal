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

/* etcpal/root_layer_pdu.h: Functions to parse or pack a Root Layer PDU block (as defined in ANSI
 * E1.17) and its associated preambles. */

#ifndef ETCPAL_ROOT_LAYER_PDU_H_
#define ETCPAL_ROOT_LAYER_PDU_H_

#include <stddef.h>
#include "etcpal/int.h"
#include "etcpal/bool.h"
#include "etcpal/uuid.h"
#include "etcpal/acn_prot.h"
#include "etcpal/pdu.h"

/*!
 * \defgroup etcpal_rootlayerpdu ACN Root Layer PDUs (rootlayerpdu)
 * \ingroup etcpal_pdu
 * \brief Parse or pack a Root Layer PDU block.
 *
 * ```c
 * #include "etcpal/root_layer_pdu.h"
 * ```
 *
 * @{
 */

/*! Size of an ACN family TCP preamble. */
#define ACN_TCP_PREAMBLE_SIZE 16
/*! Size of an ACN family UDP preamble. */
#define ACN_UDP_PREAMBLE_SIZE 16
/*! Size of a Root Layer PDU header when the length is less than 4096. */
#define ACN_RLP_HEADER_SIZE_NORMAL_LEN 22
/*! Size of the Root Layer PDU header when the length is 4096 or greater. */
#define ACN_RLP_HEADER_SIZE_EXT_LEN 23

/*!
 * \name Protocol Vectors
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
/*!
 * @}
 */

/*! Holds the information contained in an ACN TCP Preamble. */
typedef struct EtcPalTcpPreamble
{
  /*! Pointer to the beginning of the Root Layer PDU block */
  const uint8_t* rlp_block;
  /*! Length of the Root Layer PDU block */
  size_t rlp_block_len;
} EtcPalTcpPreamble;

/*! Holds the information contained in an ACN UDP Preamble. */
typedef struct EtcPalUdpPreamble
{
  /*! Pointer to the beginning of the Root Layer PDU block */
  const uint8_t* rlp_block;
  /*! Length of the Root Layer PDU block */
  size_t rlp_block_len;
} EtcPalUdpPreamble;

/*! Holds the information contained in an ACN Root Layer PDU. */
typedef struct EtcPalRootLayerPdu
{
  /*! The CID of the component that sent this Root Layer PDU. */
  EtcPalUuid sender_cid;
  /*! The Vector indicates the type of data contained in the Data segment. */
  uint32_t vector;
  /*! A pointer to the Data segment of this PDU. */
  const uint8_t* pdata;
  /*! The length of the Data segment of this PDU. */
  size_t datalen;
} EtcPalRootLayerPdu;

#ifdef __cplusplus
extern "C" {
#endif

bool etcpal_parse_tcp_preamble(const uint8_t* buf, size_t buflen, EtcPalTcpPreamble* preamble);
bool etcpal_parse_udp_preamble(const uint8_t* buf, size_t buflen, EtcPalUdpPreamble* preamble);
bool etcpal_parse_root_layer_header(const uint8_t* buf, size_t buflen, EtcPalRootLayerPdu* pdu,
                                    EtcPalRootLayerPdu* last_pdu);
bool etcpal_parse_root_layer_pdu(const uint8_t* buf, size_t buflen, EtcPalRootLayerPdu* pdu, EtcPalPdu* last_pdu);

size_t etcpal_pack_tcp_preamble(uint8_t* buf, size_t buflen, size_t rlp_block_len);
size_t etcpal_pack_udp_preamble(uint8_t* buf, size_t buflen);
size_t etcpal_root_layer_buf_size(const EtcPalRootLayerPdu* pdu_block, size_t num_pdus);
size_t etcpal_pack_root_layer_header(uint8_t* buf, size_t buflen, const EtcPalRootLayerPdu* pdu);
size_t etcpal_pack_root_layer_block(uint8_t* buf, size_t buflen, const EtcPalRootLayerPdu* pdu_block, size_t num_pdus);

#ifdef __cplusplus
}
#endif

/*!
 * @}
 */

#endif /* ETCPAL_ROOT_LAYER_PDU_H_ */
