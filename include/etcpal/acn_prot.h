/******************************************************************************
 * Copyright 2022 ETC Inc.
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

/* etcpal/acn_prot.h: ACN protocol constants. */

#ifndef ETCPAL_ACN_PROT_H_
#define ETCPAL_ACN_PROT_H_

#include <stdint.h>

typedef uint32_t acn_protocol_t;

#define ACN_PROTOCOL_SDT             0x00000001u
#define ACN_PROTOCOL_DMP             0x00000002u
#define ACN_PROTOCOL_DRAFT_E131_DATA 0x00000003u
#define ACN_PROTOCOL_E131_DATA       0x00000004u
#define ACN_PROTOCOL_E131_EXTENDED   0x00000008u
#define ACN_PROTOCOL_LLRP            0x0000000Au
#define ACN_PROTOCOL_BROKER          0x00000009u
#define ACN_PROTOCOL_RPT             0x00000005u
#define ACN_PROTOCOL_EPT             0x0000000Bu

#endif /* ETCPAL_ACN_PROT_H_ */
