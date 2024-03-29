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

#ifndef ETCPAL_MQX_COMMON_H_
#define ETCPAL_MQX_COMMON_H_

#include <stdbool.h>
#include <mqx.h>

#ifdef __cplusplus
extern "C" {
#endif

bool milliseconds_to_ticks(int ms, MQX_TICK_STRUCT* tick_struct);
bool lwsem_timed_wait(LWSEM_STRUCT* sem, int timeout_ms);

#ifdef __cplusplus
}
#endif

#endif /* ETCPAL_MQX_COMMON_H_ */
