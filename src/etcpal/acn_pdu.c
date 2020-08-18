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

#include "etcpal/acn_pdu.h"

/**
 * @brief Parse a generic ACN PDU.
 *
 * A function to parse a generic ACN PDU. The specific PDU info needed for parsing is contained in
 * a struct AcnPduConstraints. Uses AcnPdu to maintain state across multiple PDUs in a block.
 *
 * @param[in] buf Byte buffer containing a PDU.
 * @param[in] buflen Size in bytes of buf.
 * @param[in] constraints Specific information about the PDU being parsed
 * @param[in,out] pdu PDU data from the last PDU in the block. The data is used and then replaced
 *                    with data from this PDU.
 * @return true (PDU was parsed successfully) or false (parse error or no more PDUs in the block).
 */
bool acn_parse_pdu(const uint8_t* buf, size_t buflen, const AcnPduConstraints* constraints, AcnPdu* pdu)
{
  if (!buf || !buflen || !constraints || !pdu)
    return false;

  const uint8_t* buf_end = buf + buflen;

  const uint8_t* this_pdu = buf;
  const uint8_t* prev_vect = NULL;
  const uint8_t* prev_head = NULL;
  const uint8_t* prev_data = NULL;
  if (pdu->pnextpdu)
  {
    // We have already parsed one or more PDUs in this block. Try to parse the next one.
    if (pdu->pnextpdu < buf || pdu->pnextpdu >= buf_end)
      return false;

    this_pdu = pdu->pnextpdu;
    prev_vect = pdu->pvector;
    prev_head = pdu->pheader;
    prev_data = pdu->pdata;
  }
  // Else this is the first PDU in the block - leave pointers at their default values

  // Check the inheritance and the size of the length field
  uint8_t flags_byte = *this_pdu;
  bool    extlength = ACN_PDU_L_FLAG_SET(flags_byte);
  bool    inheritvect = !ACN_PDU_V_FLAG_SET(flags_byte);
  bool    inherithead = !ACN_PDU_H_FLAG_SET(flags_byte);
  bool    inheritdata = !ACN_PDU_D_FLAG_SET(flags_byte);

  const uint8_t* cur_ptr = this_pdu;
  if (cur_ptr + (extlength ? 3 : 2) >= buf_end)
  {
    // Not even enough room for the length?? Get outta here.
    return false;
  }

  uint32_t pdu_len = ACN_PDU_LENGTH(this_pdu);
  uint32_t min_pdu_len = (uint32_t)((extlength ? 3 : 2) + (inheritvect ? 0 : constraints->vector_size) +
                                    (inherithead ? 0 : constraints->header_size));

  if ((inheritvect && !prev_vect) || (inherithead && !prev_head) || (inheritdata && !prev_data) ||
      (pdu_len < min_pdu_len) || (this_pdu + pdu_len > buf_end))
  {
    return false;
  }

  // Now fill in the pdu structure with the current PDU info
  cur_ptr += extlength ? 3 : 2;

  if (!inheritvect)
  {
    pdu->pvector = cur_ptr;
    cur_ptr += constraints->vector_size;
  }

  if (!inherithead)
  {
    pdu->pheader = cur_ptr;
    cur_ptr += constraints->header_size;
  }

  if (!inheritdata)
  {
    pdu->pdata = cur_ptr;
    pdu->data_len = pdu_len - (uint32_t)(cur_ptr - this_pdu);
    cur_ptr += pdu->data_len;
  }

  pdu->pnextpdu = cur_ptr;
  return true;
}
