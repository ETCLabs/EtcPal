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

#include "etcpal/root_layer_pdu.h"
#include "etcpal/pack.h"

#include <string.h>

#define ACN_RLP_HEADER_SIZE 16u
#define RLP_VECTOR_SIZE 4u

#define RLP_EXTENDED_LENGTH(inheritvec, inherithead, datalen) \
  ((datalen + (inheritvec ? 0 : RLP_VECTOR_SIZE) + (inherithead ? 0 : ACN_RLP_HEADER_SIZE)) > 4095)

#define PROT_MANDATES_L_FLAG(vector)                                                                            \
  ((vector == ACN_VECTOR_ROOT_LLRP) || (vector == ACN_VECTOR_ROOT_BROKER) || (vector == ACN_VECTOR_ROOT_RPT) || \
   (vector == ACN_VECTOR_ROOT_EPT))

/* Make sure to use memcmp, not strcmp, because of the extra nulls */
#define ACN_PACKET_IDENT "ASC-E1.17\0\0\0"
#define ACN_PACKET_IDENT_SIZE 12

/*!
 * \brief Parse an ACN TCP Preamble.
 *
 * Determine whether this byte buffer, received over TCP, starts with an ACN TCP Preamble and thus
 * contains an ACN protocol family packet. For best results, wait until you receive at least
 * #ACN_TCP_PREAMBLE_SIZE bytes before calling this function.
 *
 * \param[in]   buf       Received TCP buffer.
 * \param[in]   buflen    Size in bytes of buf.
 * \param[out]  preamble  Struct to contain parsed preamble information.
 * \return true (preamble was parsed successfully) or false (buffer too short or buffer does not
 *         contain a valid TCP preamble).
 */
bool etcpal_parse_tcp_preamble(const uint8_t* buf, size_t buflen, EtcPalTcpPreamble* preamble)
{
  if (!preamble || !buf || (buflen < ACN_TCP_PREAMBLE_SIZE))
    return false;

  if (memcmp(buf, ACN_PACKET_IDENT, ACN_PACKET_IDENT_SIZE) == 0)
  {
    preamble->rlp_block = buf + ACN_TCP_PREAMBLE_SIZE;
    preamble->rlp_block_len = etcpal_upack_32b(buf + ACN_PACKET_IDENT_SIZE);
    return true;
  }
  return false;
}

/*!
 * \brief Parse an ACN UDP Preamble.
 *
 * Determine whether this received UDP datagram contains an ACN protocol family packet by
 * determining whether it starts with the ACN UDP preamble.
 *
 * \param[in] buf Received UDP buffer.
 * \param[in] buflen Size in bytes of buf.
 * \param[out] preamble Struct to contain parsed preamble information.
 * \return true (preamble was parsed successfully) or false (buffer too short or buffer does not
 *         contain a valid UDP preamble).
 */
bool etcpal_parse_udp_preamble(const uint8_t* buf, size_t buflen, EtcPalUdpPreamble* preamble)
{
  const uint8_t* pcur;
  uint16_t preamble_len, postamble_len;

  if (!preamble || !buf || (buflen < ACN_UDP_PREAMBLE_SIZE))
    return false;

  pcur = buf;
  preamble_len = etcpal_upack_16b(pcur);
  pcur += 2;
  postamble_len = etcpal_upack_16b(pcur);
  pcur += 2;
  if ((preamble_len == ACN_UDP_PREAMBLE_SIZE) && (memcmp(pcur, ACN_PACKET_IDENT, ACN_PACKET_IDENT_SIZE) == 0) &&
      (buflen > (unsigned)(preamble_len + postamble_len)))
  {
    preamble->rlp_block = buf + ACN_UDP_PREAMBLE_SIZE;
    preamble->rlp_block_len = buflen - preamble_len - postamble_len;
    return true;
  }
  return false;
}

/*!
 * \brief Parse only the header of an ACN Root Layer PDU.
 *
 * This can be useful when working with stream sockets.
 *
 * \param[in] buf Root Layer Protocol buffer.
 * \param[in] buflen Size in bytes of buf.
 * \param[in] pdu Struct to contain parsed PDU header.
 * \param[in] last_pdu The last PDU structure that was parsed by this function, or NULL for the
 *                     first PDU in the block.
 * \return true (PDU was parsed successfully) or false (parse error or no more PDUs in the block).
 */
bool etcpal_parse_root_layer_header(const uint8_t* buf, size_t buflen, EtcPalRootLayerPdu* pdu,
                                    EtcPalRootLayerPdu* last_pdu)
{
  uint32_t min_pdu_len, pdu_len;
  bool extlength, inheritvect, inherithead, inheritdata;
  uint8_t flags_byte;
  const uint8_t *cur_ptr, *buf_end;

  if (!buf || !pdu)
    return false;

  buf_end = buf + buflen;

  flags_byte = *buf;
  extlength = ETCPAL_PDU_L_FLAG_SET(flags_byte);
  inheritvect = !ETCPAL_PDU_V_FLAG_SET(flags_byte);
  inherithead = !ETCPAL_PDU_H_FLAG_SET(flags_byte);
  inheritdata = !ETCPAL_PDU_D_FLAG_SET(flags_byte);

  cur_ptr = buf;
  if (cur_ptr + (extlength ? 3 : 2) >= buf_end)
  {
    /* Not even enough room for the length?? Get outta here. */
    return false;
  }

  pdu_len = ETCPAL_PDU_LENGTH(buf);
  min_pdu_len = (uint32_t)((extlength ? 3 : 2) + (inheritvect ? 0 : 4) + (inherithead ? 0 : 16));
  if (((inheritvect || inherithead || inheritdata) && !last_pdu) || (pdu_len < min_pdu_len))
  {
    return false;
  }

  cur_ptr += extlength ? 3 : 2;
  if (inheritvect)
    pdu->vector = last_pdu->vector;
  else
  {
    pdu->vector = etcpal_upack_32b(cur_ptr);
    cur_ptr += 4;
    if (PROT_MANDATES_L_FLAG(pdu->vector) && !extlength)
      return false;
  }
  if (inherithead)
  {
    pdu->sender_cid = last_pdu->sender_cid;
  }
  else
  {
    memcpy(pdu->sender_cid.data, cur_ptr, ETCPAL_UUID_BYTES);
    cur_ptr += ETCPAL_UUID_BYTES;
  }
  if (inheritdata)
  {
    pdu->pdata = last_pdu->pdata;
    pdu->datalen = last_pdu->datalen;
  }
  else
  {
    pdu->pdata = cur_ptr;
    pdu->datalen = (size_t)(pdu_len - (size_t)(cur_ptr - buf));
  }
  return true;
}

/*!
 * \brief Parse an ACN Root Layer PDU.
 *
 * Parse a Root Layer PDU from a Root Layer PDU block. Fills in the EtcPalRootLayerPdu structure with
 * the parsed PDU information, and stores state data in a EtcPalPdu structure for parsing the next
 * PDU in the block, if there is one.
 *
 * Example usage:
 *
 * After parsing an ACN protocol family preamble (perhaps by using etcpal_parse_udp_preamble() or
 * etcpal_parse_tcp_preamble()), buf points to the data starting after the preamble and buflen is the
 * length parsed from the preamble...
 *
 * \code
 * EtcPalPdu pdu = ETCPAL_PDU_INIT; // Must initialize this!!
 * bool res = false;
 * do
 * {
 *   EtcPalRootLayerPdu cur;
 *   res = etcpal_parse_root_layer_pdu(buf, buflen, &cur, &pdu);
 *   if (res)
 *   {
 *     // Example application handler function that forwards the pdu appropriately based on
 *     // vector
 *     handle_root_layer_pdu(&cur);
 *   }
 * } while (res);
 * \endcode
 *
 * \param[in] buf Root Layer Protocol buffer.
 * \param[in] buflen Size in bytes of buf.
 * \param[out] pdu Struct to contain parsed PDU.
 * \param[in,out] last_pdu State data for future calls.
 * \return true (PDU was parsed successfully) or false (parse error or no more PDUs in the block).
 */
bool etcpal_parse_root_layer_pdu(const uint8_t* buf, size_t buflen, EtcPalRootLayerPdu* pdu, EtcPalPdu* last_pdu)
{
  EtcPalPduConstraints rlp_constraints = {/* vector_size */ 4,
                                          /* header_size */ 16};

  if (!buf || !pdu || !last_pdu)
    return false;

  if (etcpal_parse_pdu(buf, buflen, &rlp_constraints, last_pdu))
  {
    pdu->vector = etcpal_upack_32b(last_pdu->pvector);
    memcpy(pdu->sender_cid.data, last_pdu->pheader, ETCPAL_UUID_BYTES);
    pdu->pdata = last_pdu->pdata;
    pdu->datalen = last_pdu->datalen;
    return true;
  }
  return false;
}

/*!
 * \brief Pack an ACN UDP Preamble into a buffer.
 *
 * All ACN protocol family packets sent over UDP must start with a UDP Preamble.
 *
 * \param[out] buf Buffer into which to pack the preamble.
 * \param[in] buflen Size in bytes of buf. Buffer should be at least of length
 *                   #ACN_UDP_PREAMBLE_SIZE.
 * \return Number of bytes packed (success) or 0 (failure).
 */
size_t etcpal_pack_udp_preamble(uint8_t* buf, size_t buflen)
{
  uint8_t* cur_ptr;

  if (!buf || buflen < ACN_UDP_PREAMBLE_SIZE)
    return 0;

  cur_ptr = buf;
  etcpal_pack_16b(cur_ptr, ACN_UDP_PREAMBLE_SIZE);
  cur_ptr += 2;
  etcpal_pack_16b(cur_ptr, 0);
  cur_ptr += 2;
  memcpy(cur_ptr, ACN_PACKET_IDENT, ACN_PACKET_IDENT_SIZE);
  cur_ptr += ACN_PACKET_IDENT_SIZE;
  return (size_t)(cur_ptr - buf);
}

/*!
 * \brief Pack an ACN TCP Preamble into a buffer.
 *
 * All ACN protocol family packets sent over TCP must start with a TCP Preamble.
 *
 * \param[out] buf Buffer into which to pack the preamble.
 * \param[in] buflen Size in bytes of buf. Buffer should be at least of length
 *                   #ACN_TCP_PREAMBLE_SIZE.
 * \param[in] rlp_block_len Full length in bytes of the Root Layer PDU block that will come after
 *                          this TCP preamble.
 * \return Number of bytes packed (success) or 0 (failure).
 */
size_t etcpal_pack_tcp_preamble(uint8_t* buf, size_t buflen, size_t rlp_block_len)
{
  uint8_t* cur_ptr;

  if (!buf || buflen < ACN_TCP_PREAMBLE_SIZE)
    return 0;

  cur_ptr = buf;
  memcpy(cur_ptr, ACN_PACKET_IDENT, ACN_PACKET_IDENT_SIZE);
  cur_ptr += ACN_PACKET_IDENT_SIZE;
  etcpal_pack_32b(cur_ptr, (uint32_t)rlp_block_len);
  cur_ptr += 4;
  return (size_t)(cur_ptr - buf);
}

/*!
 * \brief Get the buffer size to allocate for a Root Layer PDU block.
 *
 * Calculate the buffer size to allocate for a future call to etcpal_pack_root_layer_block(), given
 * an array of EtcPalRootLayerPdu.
 *
 * \param[in] pdu_block Array of structs representing a Root Layer PDU block.
 * \param[in] num_pdus Size of the pdu_block array.
 * \return Buffer size to allocate.
 */
size_t etcpal_root_layer_buf_size(const EtcPalRootLayerPdu* pdu_block, size_t num_pdus)
{
  const EtcPalRootLayerPdu* pdu;
  size_t block_size = 0;

  for (pdu = pdu_block; pdu < pdu_block + num_pdus; ++pdu)
  {
    block_size += pdu->datalen;
    /* We assume no inheritance here, so the largest possible buffer is allocated. */
    block_size += (PROT_MANDATES_L_FLAG(pdu->vector) || RLP_EXTENDED_LENGTH(false, false, pdu->datalen))
                      ? ACN_RLP_HEADER_SIZE_EXT_LEN
                      : ACN_RLP_HEADER_SIZE_NORMAL_LEN;
  }
  return block_size;
}

/*!
 * \brief Pack only the header of a Root Layer PDU block into a buffer.
 *
 * This can be useful when working with stream sockets.
 *
 * \param[out] buf Buffer into which to pack the Root Layer PDU header.
 * \param[in] buflen Size in bytes of buf. Buffer should be at least of size
 *                   #ACN_RLP_HEADER_SIZE_EXT_LEN.
 * \param[in] pdu PDU for which to pack the header into a buffer.
 * \return Number of bytes packed (success) or 0 (failure).
 */
size_t etcpal_pack_root_layer_header(uint8_t* buf, size_t buflen, const EtcPalRootLayerPdu* pdu)
{
  uint8_t* cur_ptr = buf;

  if (!buf || !pdu || buflen < ACN_RLP_HEADER_SIZE_EXT_LEN)
    return 0;

  *cur_ptr = 0x70;
  if (PROT_MANDATES_L_FLAG(pdu->vector) || RLP_EXTENDED_LENGTH(false, false, pdu->datalen))
  {
    ETCPAL_PDU_SET_L_FLAG(*cur_ptr);
    ETCPAL_PDU_PACK_EXT_LEN(cur_ptr, ACN_RLP_HEADER_SIZE_EXT_LEN + pdu->datalen);
    cur_ptr += 3;
  }
  else
  {
    ETCPAL_PDU_PACK_NORMAL_LEN(cur_ptr, ACN_RLP_HEADER_SIZE_NORMAL_LEN + pdu->datalen);
    cur_ptr += 2;
  }
  etcpal_pack_32b(cur_ptr, pdu->vector);
  cur_ptr += 4;
  memcpy(cur_ptr, pdu->sender_cid.data, ETCPAL_UUID_BYTES);
  cur_ptr += ETCPAL_UUID_BYTES;
  return (size_t)(cur_ptr - buf);
}

/*!
 * \brief Pack a Root Layer PDU block into a buffer.
 *
 * The required buffer size can be calculated in advance using etcpal_root_layer_buf_size().
 *
 * \param[out] buf Buffer into which to pack the Root Layer PDU block.
 * \param[in] buflen Size in bytes of buf.
 * \param[in] pdu_block Array of EtcPalRootLayerPdu representing the PDU block to pack.
 * \param[in] num_pdus Number of EtcPalRootLayerPdu that make up the pdu_block array.
 * \return Number of bytes packed (success) or 0 (failure). Note that this might be less than the
 *         value returned by etcpal_root_layer_buf_size(); this is because this function performs
 *         more optimizations related to PDU inheritance.
 */
size_t etcpal_pack_root_layer_block(uint8_t* buf, size_t buflen, const EtcPalRootLayerPdu* pdu_block, size_t num_pdus)
{
  uint8_t* cur_ptr = buf;
  const EtcPalRootLayerPdu* pdu;
  EtcPalRootLayerPdu last_pdu = {{{0}}, 0, NULL, 0};

  if (!buf || !pdu_block || (etcpal_root_layer_buf_size(pdu_block, num_pdus)) > buflen)
  {
    return 0;
  }

  for (pdu = pdu_block; pdu < pdu_block + num_pdus; ++pdu)
  {
    bool inheritvec = true;
    bool inherithead = true;
    bool inheritdata = true;
    /* Start with no flags set */
    *cur_ptr = 0;
    if (pdu == pdu_block)
    {
      /* First PDU in the block - no inheritance */
      ETCPAL_PDU_SET_V_FLAG(*cur_ptr);
      inheritvec = false;
      last_pdu.vector = pdu->vector;

      ETCPAL_PDU_SET_H_FLAG(*cur_ptr);
      inherithead = false;
      last_pdu.sender_cid = pdu->sender_cid;

      ETCPAL_PDU_SET_D_FLAG(*cur_ptr);
      inheritdata = false;
      last_pdu.pdata = pdu->pdata;
      last_pdu.datalen = pdu->datalen;
    }
    else
    {
      /* Check if we can use some inheritance */
      if (pdu->vector != last_pdu.vector)
      {
        ETCPAL_PDU_SET_V_FLAG(*cur_ptr);
        inheritvec = false;
        last_pdu.vector = pdu->vector;
      }
      if (0 != ETCPAL_UUID_CMP(&pdu->sender_cid, &last_pdu.sender_cid))
      {
        ETCPAL_PDU_SET_H_FLAG(*cur_ptr);
        inherithead = false;
        last_pdu.sender_cid = pdu->sender_cid;
      }
      if ((pdu->datalen != last_pdu.datalen) || (0 != memcmp(pdu->pdata, last_pdu.pdata, last_pdu.datalen)))
      {
        ETCPAL_PDU_SET_D_FLAG(*cur_ptr);
        inheritdata = false;
        last_pdu.pdata = pdu->pdata;
        last_pdu.datalen = pdu->datalen;
      }
    }

    /* Check if we are required to use the 3-byte length field, either by the
     * higher-level protocol or because the length is greater than 4096 */
    if (PROT_MANDATES_L_FLAG(pdu->vector) ||
        RLP_EXTENDED_LENGTH(inheritvec, inherithead, inheritdata ? 0 : pdu->datalen))
    {
      size_t len = 3u + (inheritvec ? 0u : RLP_VECTOR_SIZE) + (inherithead ? 0u : ACN_RLP_HEADER_SIZE) +
                   (inheritdata ? 0u : pdu->datalen);
      ETCPAL_PDU_SET_L_FLAG(*cur_ptr);
      ETCPAL_PDU_PACK_EXT_LEN(cur_ptr, len);
      cur_ptr += 3;
    }
    else
    {
      size_t len = 2 + (inheritvec ? 0u : RLP_VECTOR_SIZE) + (inherithead ? 0u : ACN_RLP_HEADER_SIZE) +
                   (inheritdata ? 0u : pdu->datalen);
      ETCPAL_PDU_PACK_NORMAL_LEN(cur_ptr, len);
      cur_ptr += 2;
    }

    if (!inheritvec)
    {
      etcpal_pack_32b(cur_ptr, pdu->vector);
      cur_ptr += 4;
    }
    if (!inherithead)
    {
      memcpy(cur_ptr, pdu->sender_cid.data, ETCPAL_UUID_BYTES);
      cur_ptr += ETCPAL_UUID_BYTES;
    }
    if (!inheritdata)
    {
      memcpy(cur_ptr, pdu->pdata, pdu->datalen);
      cur_ptr += pdu->datalen;
    }
  }
  return (size_t)(cur_ptr - buf);
}
