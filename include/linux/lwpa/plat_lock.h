/******************************************************************************
 * Copyright 2018 ETC Inc.
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

#include <pthread.h>
#include "lwpa/bool.h"

typedef pthread_mutex_t lwpa_mutex_t;

/* Temporary placeholders */
#define lwpa_mutex_create(idptr) false
#define lwpa_mutex_take(idptr, wait_ms) false
#define lwpa_mutex_give(idptr)
#define lwpa_mutex_destroy(idptr)

typedef pthread_cond_t lwpa_signal_t;

/* Temporary placeholders */
#define lwpa_signal_create(idptr) false
#define lwpa_signal_wait(idptr, wait_ms) false
#define lwpa_signal_post(idptr)
#define lwpa_signal_destroy(idptr)

typedef pthread_rwlock_t lwpa_rwlock_t;

/* Temporary placeholders */
#define lwpa_rwlock_create(idptr) false
#define lwpa_rwlock_readlock(idptr, wait_ms) false
#define lwpa_rwlock_readunlock(idptr)
#define lwpa_rwlock_writelock(idptr, wait_ms) false
#define lwpa_rwlock_writeunlock(idptr)
#define lwpa_rwlock_destroy(idptr)

#endif /* _LWPA_PLAT_LOCK_H_ */