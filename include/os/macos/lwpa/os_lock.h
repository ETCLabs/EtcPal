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

#include <pthread.h>
#include "lwpa/bool.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef pthread_mutex_t lwpa_mutex_t;

#define lwpa_mutex_create(idptr) ((pthread_mutex_init((idptr), NULL) == 0) ? true : false)
#define lwpa_mutex_take(idptr) ((pthread_mutex_lock(idptr) == 0) ? true : false)
#define lwpa_mutex_try_take(idptr) ((pthread_mutex_trylock(idptr) == 0) ? true : false)
#define lwpa_mutex_give(idptr) ((void)pthread_mutex_unlock(idptr))
#define lwpa_mutex_destroy(idptr) ((void)pthread_mutex_destroy(idptr))

typedef struct
{
  bool valid;
  bool signaled;
  pthread_cond_t cond;
  pthread_mutex_t mutex;
} lwpa_signal_t;

bool lwpa_signal_create(lwpa_signal_t* id);
bool lwpa_signal_wait(lwpa_signal_t* id);
bool lwpa_signal_poll(lwpa_signal_t* id);
void lwpa_signal_post(lwpa_signal_t* id);
void lwpa_signal_destroy(lwpa_signal_t* id);

typedef struct
{
  bool valid;
  unsigned int reader_count;
  pthread_mutex_t readcount_mutex;
  pthread_mutex_t mutex;
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
