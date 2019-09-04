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

#ifndef _ETCPAL_OS_THREAD_H_
#define _ETCPAL_OS_THREAD_H_

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

#define ETCPAL_THREAD_DEFAULT_PRIORITY 11
#define ETCPAL_THREAD_DEFAULT_STACK 8000
#define ETCPAL_THREAD_DEFAULT_NAME "etcpal_thread"
#define ETCPAL_THREAD_MQX_DEFAULT_ATTRIBUTES NULL
#define ETCPAL_THREAD_MQX_DEFAULT_TIME_SLICE 0

typedef struct
{
  void (*fn)(void*);
  void* arg;
  LWSEM_STRUCT sig;
  _task_id tid;
} etcpal_thread_t;

#define etcpal_thread_sleep(sleep_ms) _time_delay(sleep_ms)

#ifdef __cplusplus
}
#endif

#endif /* _ETCPAL_OS_THREAD_H_ */
