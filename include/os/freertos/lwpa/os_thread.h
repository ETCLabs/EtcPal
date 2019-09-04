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

#ifndef _LWPA_OS_THREAD_H_
#define _LWPA_OS_THREAD_H_

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include "lwpa/common.h"
#include "lwpa/bool.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LWPA_THREAD_DEFAULT_PRIORITY (configMAX_PRIORITIES / 2)
#define LWPA_THREAD_DEFAULT_STACK 500
#define LWPA_THREAD_DEFAULT_NAME "lwpa_thread"

typedef struct
{
  void (*fn)(void*);
  void* arg;
  SemaphoreHandle_t sig;
  TaskHandle_t tid;
} lwpa_thread_t;

#define lwpa_thread_sleep(sleep_ms) vTaskDelay(pdMS_TO_TICKS(sleep_ms))

#ifdef __cplusplus
}
#endif

#endif /* _LWPA_OS_THREAD_H_ */
