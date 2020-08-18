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

#ifndef ETCPAL_OS_LOCK_H_
#define ETCPAL_OS_LOCK_H_

#include <stdbool.h>
#include <mqx.h>
#include <lwevent.h>
#include "etcpal/common.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef LWSEM_STRUCT etcpal_mutex_t;

#define ETCPAL_MUTEX_HAS_TIMED_LOCK 1

#define etcpal_mutex_create(idptr) (MQX_OK == _lwsem_create((idptr), 1))
#define etcpal_mutex_lock(idptr) (MQX_OK == _lwsem_wait(idptr))
#define etcpal_mutex_try_lock(idptr) (MQX_OK == _lwsem_wait_ticks((idptr), 1u))
bool etcpal_mutex_timed_lock(etcpal_mutex_t* id, int timeout_ms);
#define etcpal_mutex_unlock(idptr) ((void)_lwsem_post(idptr))
#define etcpal_mutex_destroy(idptr) ((void)_lwsem_destroy(idptr))

typedef LWEVENT_STRUCT etcpal_signal_t;

#define ETCPAL_SIGNAL_HAS_TIMED_WAIT 1
#define ETCPAL_SIGNAL_HAS_POST_FROM_ISR 0

#define etcpal_signal_create(idptr) (MQX_OK == _lwevent_create((idptr), LWEVENT_AUTO_CLEAR))
#define etcpal_signal_wait(idptr) (MQX_OK == _lwevent_wait_ticks((idptr), 1u, true, 0u))
#define etcpal_signal_try_wait(idptr) (MQX_OK == _lwevent_wait_ticks((idptr), 1u, true, 1u))
bool etcpal_signal_timed_wait(etcpal_signal_t* id, int timeout_ms);
#define etcpal_signal_post(idptr) ((void)_lwevent_set((idptr), 1u))
#define etcpal_signal_post_from_isr etcpal_signal_post
#define etcpal_signal_destroy(idptr) ((void)_lwevent_destroy(idptr))

#define ETCPAL_RWLOCK_HAS_TIMED_LOCK 1

typedef struct
{
  bool         valid;
  LWSEM_STRUCT sem;
  unsigned int reader_count;
} etcpal_rwlock_t;

bool etcpal_rwlock_create(etcpal_rwlock_t* id);
bool etcpal_rwlock_readlock(etcpal_rwlock_t* id);
bool etcpal_rwlock_try_readlock(etcpal_rwlock_t* id);
bool etcpal_rwlock_timed_readlock(etcpal_rwlock_t* id, int timeout_ms);
void etcpal_rwlock_readunlock(etcpal_rwlock_t* id);
bool etcpal_rwlock_writelock(etcpal_rwlock_t* id);
bool etcpal_rwlock_try_writelock(etcpal_rwlock_t* id);
bool etcpal_rwlock_timed_writelock(etcpal_rwlock_t* id, int timeout_ms);
void etcpal_rwlock_writeunlock(etcpal_rwlock_t* id);
void etcpal_rwlock_destroy(etcpal_rwlock_t* id);

typedef LWSEM_STRUCT etcpal_sem_t;

#define ETCPAL_SEM_HAS_TIMED_WAIT 1
#define ETCPAL_SEM_HAS_POST_FROM_ISR 0
#define ETCPAL_SEM_HAS_MAX_COUNT 0
#define ETCPAL_SEM_MUST_BE_BALANCED 0

#define etcpal_sem_create(idptr, initial_count, max_count) (MQX_OK == _lwsem_create((idptr), (_mqx_int)(initial_count)))
#define etcpal_sem_wait(idptr) (MQX_OK == _lwsem_wait(idptr))
#define etcpal_sem_try_wait(idptr) (MQX_OK == _lwsem_wait_ticks((idptr), 1u))
bool etcpal_sem_timed_wait(etcpal_sem_t* id, int timeout_ms);
#define etcpal_sem_post(idptr) (MQX_OK == _lwsem_post(idptr))
#define etcpal_sem_post_from_isr etcpal_sem_post
#define etcpal_sem_destroy(idptr) ((void)_lwsem_destroy(idptr))

#ifdef __cplusplus
}
#endif

#endif /* ETCPAL_OS_LOCK_H_ */
