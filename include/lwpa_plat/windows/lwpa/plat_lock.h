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

#ifndef _LWPA_PLAT_LOCK_H_
#define _LWPA_PLAT_LOCK_H_

#ifndef NOMINMAX
#define NOMINMAX 1    /* Suppress some conflicting definitions in the Windows headers */
#include <winsock2.h> /* To fix winsock include order issues */
#include <windows.h>
#undef NOMINMAX
#else
#include <winsock2.h>
#include <windows.h>
#endif

#include "lwpa/common.h"
#include "lwpa/bool.h"

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
#define lwpa_mutex_take(idptr, wait_ms)                                                                          \
  (((idptr) && (idptr)->valid)                                                                                   \
       ? (((wait_ms) == 0) ? TryEnterCriticalSection(&(idptr)->cs) : (EnterCriticalSection(&(idptr)->cs), true)) \
       : false)
#define lwpa_mutex_give(idptr)            \
  do                                      \
  {                                       \
    if ((idptr) && (idptr)->valid)        \
      LeaveCriticalSection(&(idptr)->cs); \
  } while (0)
#define lwpa_mutex_destroy(idptr)          \
  do                                       \
  {                                        \
    if ((idptr) && (idptr)->valid)         \
    {                                      \
      DeleteCriticalSection(&(idptr)->cs); \
      (idptr)->valid = false;              \
    }                                      \
  } while (0)

typedef HANDLE lwpa_signal_t;

bool lwpa_signal_create(lwpa_signal_t* id);
#define lwpa_signal_wait(idptr, wait_ms)                                                                   \
  ((idptr) ? (WAIT_OBJECT_0 ==                                                                             \
              WaitForSingleObject(*(idptr), ((wait_ms) == LWPA_WAIT_FOREVER ? INFINITE : (DWORD)wait_ms))) \
           : false)
#define lwpa_signal_post(idptr)     \
  do                                \
  {                                 \
    if (idptr)                      \
      SetEvent((HANDLE) * (idptr)); \
  } while (0)
#define lwpa_signal_destroy(idptr)     \
  do                                   \
  {                                    \
    if (idptr)                         \
      CloseHandle((HANDLE) * (idptr)); \
  } while (0)

typedef struct
{
  bool valid;
  CRITICAL_SECTION cs;
  LONG reader_count;
} lwpa_rwlock_t;

bool lwpa_rwlock_create(lwpa_rwlock_t* id);
bool lwpa_rwlock_readlock(lwpa_rwlock_t* id, int wait_ms);
#define lwpa_rwlock_readunlock(idptr)               \
  do                                                \
  {                                                 \
    if (idptr)                                      \
      InterlockedDecrement(&(idptr)->reader_count); \
  } while (0)
bool lwpa_rwlock_writelock(lwpa_rwlock_t* id, int wait_ms);
#define lwpa_rwlock_writeunlock(idptr)    \
  do                                      \
  {                                       \
    if (idptr)                            \
      LeaveCriticalSection(&(idptr)->cs); \
  } while (0)
#define lwpa_rwlock_destroy(idptr)         \
  do                                       \
  {                                        \
    if ((idptr) && (idptr)->valid)         \
      DeleteCriticalSection(&(idptr)->cs); \
    (idptr)->valid = false;                \
  } while (0)

#ifdef __cplusplus
}
#endif

#endif /* _LWPA_PLAT_LOCK_H_ */
