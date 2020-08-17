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

#ifndef ETCPAL_OS_QUEUE_H_
#define ETCPAL_OS_QUEUE_H_

#include <stdbool.h>
#include <FreeRTOS.h>
#include "etcpal/common.h"
#include <queue.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef QueueHandle_t etcpal_queue_t;
typedef TickType_t etcpal_ticks_t;

bool etcpal_queue_create(etcpal_queue_t* id, size_t size, size_t item_size);

bool etcpal_queue_send(etcpal_queue_t* id, const void* data);
bool etcpal_queue_timed_send(etcpal_queue_t* id, const void* data, int timeout_ms);
bool etcpal_queue_send_from_isr(etcpal_queue_t* id, const void* data);

bool etcpal_queue_receive(etcpal_queue_t* id, void* data);
bool etcpal_queue_timed_receive(etcpal_queue_t* id, void* data, int timeout_ms);

bool etcpal_queue_is_empty(const etcpal_queue_t* id);
bool etcpal_queue_is_empty_from_isr(const etcpal_queue_t* id);

#ifdef __cplusplus
}
#endif

#endif  // ETCPAL_OS_QUEUE_H_
