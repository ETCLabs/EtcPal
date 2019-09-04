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

#include <mqx.h>
#include "etcpal/common.h"
#include "etcpal/bool.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct LwpaThreadParamsMqx
{
  _mqx_uint task_attributes;
  _mqx_uint time_slice;
} LwpaThreadParamsMqx;

#define LWPA_THREAD_DEFAULT_PRIORITY 11
#define LWPA_THREAD_DEFAULT_STACK 8000
#define LWPA_THREAD_DEFAULT_NAME "lwpa_thread"
#define LWPA_THREAD_MQX_DEFAULT_ATTRIBUTES NULL
#define LWPA_THREAD_MQX_DEFAULT_TIME_SLICE 0

typedef struct
{
  void (*fn)(void*);
  void* arg;
  LWSEM_STRUCT sig;
  _task_id tid;
} lwpa_thread_t;

#define lwpa_thread_sleep(sleep_ms) _time_delay(sleep_ms)

#ifdef __cplusplus
}
#endif

#endif /* _LWPA_OS_THREAD_H_ */
