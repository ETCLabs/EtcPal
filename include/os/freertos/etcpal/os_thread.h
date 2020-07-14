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

#ifndef ETCPAL_OS_THREAD_H_
#define ETCPAL_OS_THREAD_H_

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include "etcpal/common.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ETCPAL_THREAD_DEFAULT_PRIORITY (configMAX_PRIORITIES / 2)
#define ETCPAL_THREAD_DEFAULT_STACK 500
#define ETCPAL_THREAD_DEFAULT_NAME "etcpal_thread"
#define ETCPAL_THREAD_JOIN_CAN_TIMEOUT 1

typedef TaskHandle_t etcpal_thread_id_t;

typedef struct
{
  void (*fn)(void*);
  void*             arg;
  SemaphoreHandle_t sig;
  TaskHandle_t      tid;
} etcpal_thread_t;

#define etcpal_thread_sleep(sleep_ms) vTaskDelay(pdMS_TO_TICKS(sleep_ms))

#ifdef __cplusplus
}
#endif

#endif /* ETCPAL_OS_THREAD_H_ */
