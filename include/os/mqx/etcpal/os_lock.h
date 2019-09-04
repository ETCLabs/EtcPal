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

#ifndef _LWPA_OS_LOCK_H_
#define _LWPA_OS_LOCK_H_

#include <mqx.h>
#include <lwevent.h>
#include "etcpal/common.h"
#include "etcpal/bool.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef LWSEM_STRUCT etcpal_mutex_t;

#define etcpal_mutex_create(idptr) (MQX_OK == _lwsem_create((idptr), 1))
#define etcpal_mutex_take(idptr) (MQX_OK == _lwsem_wait(idptr))
#define etcpal_mutex_try_take(idptr) (MQX_OK == _lwsem_wait_ticks((idptr), 1u))
#define etcpal_mutex_give(idptr) ((void)_lwsem_post(idptr))
#define etcpal_mutex_destroy(idptr) ((void)_lwsem_destroy(idptr))

typedef LWEVENT_STRUCT etcpal_signal_t;

#define etcpal_signal_create(idptr) (MQX_OK == _lwevent_create((idptr), LWEVENT_AUTO_CLEAR))
#define etcpal_signal_wait(idptr) (MQX_OK == _lwevent_wait_ticks((idptr), 1u, true, 0u))
#define etcpal_signal_poll(idptr) (MQX_OK == _lwevent_wait_ticks((idptr), 1u, true, 1u))
#define etcpal_signal_post(idptr) ((void)_lwevent_set((idptr), 1u))
#define etcpal_signal_destroy(idptr) ((void)_lwevent_destroy(idptr))

typedef struct
{
  bool valid;
  LWSEM_STRUCT sem;
  unsigned int reader_count;
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

#endif /* _LWPA_OS_LOCK_H_ */
