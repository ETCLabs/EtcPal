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

#ifndef _LWPA_OS_LOCK_H_
#define _LWPA_OS_LOCK_H_

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

/* Critical sections have been shown by empirical testing to provide a large efficiency boost over
 * Windows mutexes. */
typedef struct
{
  bool valid;
  CRITICAL_SECTION cs;
} lwpa_mutex_t;

bool lwpa_mutex_create(lwpa_mutex_t* id);
bool lwpa_mutex_take(lwpa_mutex_t* id);
bool lwpa_mutex_try_take(lwpa_mutex_t* id);
void lwpa_mutex_give(lwpa_mutex_t* id);
void lwpa_mutex_destroy(lwpa_mutex_t* id);

typedef HANDLE lwpa_signal_t;

bool lwpa_signal_create(lwpa_signal_t* id);
bool lwpa_signal_wait(lwpa_signal_t* id);
bool lwpa_signal_poll(lwpa_signal_t* id);
void lwpa_signal_post(lwpa_signal_t* id);
void lwpa_signal_destroy(lwpa_signal_t* id);

typedef struct
{
  bool valid;
  CRITICAL_SECTION cs;
  LONG reader_count;
} lwpa_rwlock_t;

bool lwpa_rwlock_create(lwpa_rwlock_t* id);
bool lwpa_rwlock_readlock(lwpa_rwlock_t* id);
bool lwpa_rwlock_try_readlock(lwpa_rwlock_t* id);
void lwpa_rwlock_readunlock(lwpa_rwlock_t* id);
bool lwpa_rwlock_writelock(lwpa_rwlock_t* id);
bool lwpa_rwlock_try_writelock(lwpa_rwlock_t* id);
void lwpa_rwlock_writeunlock(lwpa_rwlock_t* id);
void lwpa_rwlock_destroy(lwpa_rwlock_t* id);

#ifdef __cplusplus
}
#endif

#endif /* _LWPA_OS_LOCK_H_ */