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

#ifndef NOMINMAX
#define NOMINMAX 1    /* Suppress some conflicting definitions in the Windows headers */
#include <winsock2.h> /* To fix winsock include order issues */
#include <windows.h>
#undef NOMINMAX
#else
#include <winsock2.h>
#include <windows.h>
#endif

#include <stdbool.h>
#include "etcpal/common.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Windows has no additional platform data and thus platform_data in the EtcPalThreadParams struct
 * is ignored. */

#define ETCPAL_THREAD_DEFAULT_PRIORITY THREAD_PRIORITY_NORMAL
#define ETCPAL_THREAD_DEFAULT_STACK 0
#define ETCPAL_THREAD_DEFAULT_NAME "etcpal_thread"
#define ETCPAL_THREAD_JOIN_CAN_TIMEOUT 0 /* Timeout unavailable on windows */

#define ETCPAL_THREAD_NAME_MAX_LENGTH 32

typedef struct
{
  void (*fn)(void*);
  void*  arg;
  HANDLE tid;
  char   name[ETCPAL_THREAD_NAME_MAX_LENGTH];
} etcpal_thread_t;

#ifdef __cplusplus
#define etcpal_thread_sleep(sleep_ms) ::Sleep(static_cast<DWORD>(sleep_ms))
#else
#define etcpal_thread_sleep(sleep_ms) Sleep((DWORD)sleep_ms)
#endif

#ifdef __cplusplus
}
#endif

#endif /* ETCPAL_OS_THREAD_H_ */
