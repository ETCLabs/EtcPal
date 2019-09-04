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

#ifndef _LWPA_OS_THREAD_H_
#define _LWPA_OS_THREAD_H_

#ifndef NOMINMAX
#define NOMINMAX 1    /* Suppress some conflicting definitions in the Windows headers */
#include <winsock2.h> /* To fix winsock include order issues */
#include <windows.h>
#undef NOMINMAX
#else
#include <winsock2.h>
#include <windows.h>
#endif

#include "etcpal/common.h"
#include "etcpal/bool.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Windows has no additional platform data and thus platform_data in the LwpaThreadParams struct
 * is ignored. */

#define LWPA_THREAD_DEFAULT_PRIORITY THREAD_PRIORITY_NORMAL
#define LWPA_THREAD_DEFAULT_STACK 0
#define LWPA_THREAD_DEFAULT_NAME "lwpa_thread"

#define LWPA_THREAD_NAME_MAX_LENGTH 32

typedef struct
{
  void (*fn)(void*);
  void* arg;
  HANDLE tid;
  char name[LWPA_THREAD_NAME_MAX_LENGTH];
} lwpa_thread_t;

#define lwpa_thread_sleep(sleep_ms) Sleep(sleep_ms)

#ifdef __cplusplus
}
#endif

#endif /* _LWPA_OS_THREAD_H_ */
