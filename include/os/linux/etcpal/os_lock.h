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

#ifndef ETCPAL_OS_LOCK_H_
#define ETCPAL_OS_LOCK_H_

#include <stdbool.h>
#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef pthread_mutex_t etcpal_mutex_t;

#define etcpal_mutex_create(idptr) ((pthread_mutex_init((idptr), NULL) == 0) ? true : false)
#define etcpal_mutex_take(idptr) ((pthread_mutex_lock(idptr) == 0) ? true : false)
#define etcpal_mutex_try_take(idptr) ((pthread_mutex_trylock(idptr) == 0) ? true : false)
#define etcpal_mutex_give(idptr) ((void)pthread_mutex_unlock(idptr))
#define etcpal_mutex_destroy(idptr) ((void)pthread_mutex_destroy(idptr))

typedef struct
{
  bool valid;
  bool signaled;
  pthread_cond_t cond;
  pthread_mutex_t mutex;
} etcpal_signal_t;

bool etcpal_signal_create(etcpal_signal_t* id);
bool etcpal_signal_wait(etcpal_signal_t* id);
bool etcpal_signal_poll(etcpal_signal_t* id);
void etcpal_signal_post(etcpal_signal_t* id);
void etcpal_signal_destroy(etcpal_signal_t* id);

typedef struct
{
  bool valid;
  unsigned int reader_count;
  pthread_mutex_t readcount_mutex;
  pthread_mutex_t mutex;
} etcpal_rwlock_t;

bool etcpal_rwlock_create(etcpal_rwlock_t* id);
bool etcpal_rwlock_readlock(etcpal_rwlock_t* id);
bool etcpal_rwlock_try_readlock(etcpal_rwlock_t* id);
void etcpal_rwlock_readunlock(etcpal_rwlock_t* id);
bool etcpal_rwlock_writelock(etcpal_rwlock_t* id);
bool etcpal_rwlock_try_writelock(etcpal_rwlock_t* id);
void etcpal_rwlock_writeunlock(etcpal_rwlock_t* id);
void etcpal_rwlock_destroy(etcpal_rwlock_t* id);

#ifdef __cplusplus
}
#endif

#endif /* ETCPAL_OS_LOCK_H_ */
