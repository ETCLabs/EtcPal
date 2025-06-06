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

#ifndef ETCPAL_OS_THREAD_H_
#define ETCPAL_OS_THREAD_H_

#include <stdbool.h>
#include <mqx.h>
#include "etcpal/common.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct EtcPalThreadParamsMqx
{
  _mqx_uint task_attributes;
  _mqx_uint time_slice;
} EtcPalThreadParamsMqx;

#define ETCPAL_THREAD_DEFAULT_PRIORITY       11
#define ETCPAL_THREAD_DEFAULT_STACK          2000
#define ETCPAL_THREAD_DEFAULT_NAME           "etcpal_thread"
#define ETCPAL_THREAD_MQX_DEFAULT_ATTRIBUTES 0
#define ETCPAL_THREAD_MQX_DEFAULT_TIME_SLICE 0
#define ETCPAL_THREAD_HAS_TIMED_JOIN         0 /* Timeout unavailable on mqx */

typedef _task_id etcpal_thread_os_handle_t;
#define ETCPAL_THREAD_OS_HANDLE_INVALID MQX_NULL_TASK_ID
#define ETCPAL_THREAD_OS_HANDLE_INIT    ETCPAL_THREAD_OS_HANDLE_INVALID

typedef struct
{
  void (*fn)(void*);
  void*        arg;
  LWSEM_STRUCT sig;
  _task_id     tid;
} etcpal_thread_t;
#define ETCPAL_THREAD_INIT \
  {                        \
    0                      \
  }

#define etcpal_thread_get_current_os_handle _task_get_id

#ifdef __cplusplus
}
#endif

#endif /* ETCPAL_OS_THREAD_H_ */
