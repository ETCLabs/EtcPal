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

/* rootlayerpdu.h: Functions to parse or pack a Root Layer PDU block (as
 * defined in ANSI E1.17) and its associated preambles. */
#ifndef _LWPA_ROOTLAYERPDU_H_
#define _LWPA_ROOTLAYERPDU_H_

#include <stddef.h>
#include "lwpa_int.h"
#include "lwpa_bool.h"
#include "lwpa_cid.h"
#include "acn_prot.h"
#include "lwpa_pdu.h"

/*! \defgroup lwpa_rootlayerpdu lwpa_rootlayerpdu
 *  \ingroup lwpa_pdu
 *  \brief Parse or pack a Root Layer PDU block.
 *
 *  \#include "lwpa_rootlayerpdu.h"
 *
 *  @{
 */

/*! Size of an ACN family TCP preamble. */
#define ACN_TCP_PREAMBLE_SIZE 16
/*! Size of an ACN family UDP preamble. */
#define ACN_UDP_PREAMBLE_SIZE 16
/*! Size of a Root Layer PDU header when the length is less than 4096. */
#define RLP_HEADER_SIZE_NORMAL_LEN 22
/*! Size of the Root Layer PDU header when the length is 4096 or greater. */
#define RLP_HEADER_SIZE_EXT_LEN 23

/*! \name Protocol Vectors
 *  Each ACN family protocol is defined by a protocol vector in a Root Layer
 *  PDU. These values occupy the vector field of a RootLayerPdu to
 *  identify the contents of its data segment.
 *  @{
 */
#define VECTOR_ROOT_SDT ACN_PROTOCOL_SDT
#define VECTOR_ROOT_DRAFT_E131_DATA ACN_PROTOCOL_DRAFT_E131_DATA
#define VECTOR_ROOT_E131_DATA ACN_PROTOCOL_E131_DATA
#define VECTOR_ROOT_E131_EXTENDED ACN_PROTOCOL_E131_EXTENDED
#define VECTOR_ROOT_LLRP ACN_PROTOCOL_LLRP
#define VECTOR_ROOT_BROKER ACN_PROTOCOL_BROKER
#define VECTOR_ROOT_RPT ACN_PROTOCOL_RPT
#define VECTOR_ROOT_EPT ACN_PROTOCOL_EPT
/*!@}*/

/*! Holds the information contained in an ACN TCP Preamble. */
typedef struct TcpPreamble
{
  /*! Pointer to the beginning of the Root Layer PDU block */
  const uint8_t *rlp_block;
  /*! Length of the Root Layer PDU block */
  size_t rlp_block_len;
} TcpPreamble;

/*! Holds the information contained in an ACN UDP Preamble. */
typedef struct UdpPreamble
{
  /*! Pointer to the beginning of the Root Layer PDU block */
  const uint8_t *rlp_block;
  /*! Length of the Root Layer PDU block */
  size_t rlp_block_len;
} UdpPreamble;

/*! Holds the information contained in an ACN Root Layer PDU. */
typedef struct RootLayerPdu
{
  /*! The CID of the component that sent this Root Layer PDU. */
  LwpaCid sender_cid;
  /*! The Vector indicates the type of data contained in the Data segment. */
  uint32_t vector;
  /*! A pointer to the Data segment of this PDU. */
  const uint8_t *pdata;
  /*! The length of the Data segment of this PDU. */
  size_t datalen;
} RootLayerPdu;

#ifdef __cplusplus
extern "C" {
#endif

bool parse_tcp_preamble(const uint8_t *buf, size_t buflen, TcpPreamble *preamble);
bool parse_udp_preamble(const uint8_t *buf, size_t buflen, UdpPreamble *preamble);
bool parse_root_layer_header(const uint8_t *buf, size_t buflen, RootLayerPdu *pdu, RootLayerPdu *last_pdu);
bool parse_root_layer_pdu(const uint8_t *buf, size_t buflen, RootLayerPdu *pdu, LwpaPdu *last_pdu);

size_t pack_tcp_preamble(uint8_t *buf, size_t buflen, size_t rlp_block_len);
size_t pack_udp_preamble(uint8_t *buf, size_t buflen);
size_t root_layer_buf_size(const RootLayerPdu *pdu_block, size_t num_pdus);
size_t pack_root_layer_header(uint8_t *buf, size_t buflen, const RootLayerPdu *pdu);
size_t pack_root_layer_block(uint8_t *buf, size_t buflen, const RootLayerPdu *pdu_block, size_t num_pdus);

#ifdef __cplusplus
}
#endif

/*!@}*/

#endif /* _LWPA_ROOTLAYERPDU_H_ */
