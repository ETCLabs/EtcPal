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

/* etcpal/pack64.h: Optional extensions to etcpal_pack for 64-bit types. */
#ifndef _ETCPAL_PACK64_H_
#define _ETCPAL_PACK64_H_

#include "etcpal/int.h"

/*! \addtogroup etcpal_pack
 *  @{
 */

#ifdef __cplusplus
extern "C" {
#endif

uint64_t etcpal_upack_64b(const uint8_t* buf);
void etcpal_pack_64b(uint8_t* buf, uint64_t val);
uint64_t etcpal_upack_64l(const uint8_t* buf);
void etcpal_pack_64l(uint8_t* buf, uint64_t val);

#ifdef __cplusplus
}
#endif

/*! @} */

#endif /* _ETCPAL_PACK64_H_ */
