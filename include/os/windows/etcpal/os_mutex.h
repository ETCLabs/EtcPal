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

#ifndef ETCPAL_OS_MUTEX_H_
#define ETCPAL_OS_MUTEX_H_

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

/*
 * SRWLocks have been shown by empirical testing to provide a large efficiency boost over Windows
 * mutexes and critical sections.
 */
typedef struct
{
  bool    valid;
  SRWLOCK lock;
} etcpal_mutex_t;

#define ETCPAL_MUTEX_HAS_TIMED_LOCK 0

bool etcpal_mutex_create(etcpal_mutex_t* id);
bool etcpal_mutex_lock(etcpal_mutex_t* id);
bool etcpal_mutex_try_lock(etcpal_mutex_t* id);
bool etcpal_mutex_timed_lock(etcpal_mutex_t* id, int timeout_ms);
void etcpal_mutex_unlock(etcpal_mutex_t* id);
void etcpal_mutex_destroy(etcpal_mutex_t* id);

#ifdef __cplusplus
}
#endif

#endif /* ETCPAL_OS_MUTEX_H_ */
