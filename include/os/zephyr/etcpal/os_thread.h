/******************************************************************************
 * Copyright 2024 ETC Inc.
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
#include <zephyr/kernel.h>
#include "etcpal/common.h"

#ifdef __cplusplus
extern "C" {
#endif

// TODO Check these defaults later
#define ETCPAL_THREAD_DEFAULT_PRIORITY 7
#define ETCPAL_THREAD_DEFAULT_STACK    512
#define ETCPAL_THREAD_DEFAULT_NAME     "etcpal_thread"
#define ETCPAL_THREAD_NAME_MAX_LENGTH  CONFIG_THREAD_MAX_NAME_LEN

#define ETCPAL_THREAD_HAS_TIMED_JOIN 1

typedef k_tid_t etcpal_thread_os_handle_t;
#define ETCPAL_THREAD_OS_HANDLE_INVALID NULL

typedef struct
{
  struct k_thread   thread;
  k_thread_stack_t* stack;
} etcpal_thread_t;

#define etcpal_thread_get_current_os_handle k_current_get

#ifdef __cplusplus
}
#endif

#endif /* ETCPAL_OS_THREAD_H_ */
