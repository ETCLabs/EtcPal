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

#ifndef ETCPAL_OS_RWLOCK_H_
#define ETCPAL_OS_RWLOCK_H_

#include <stdbool.h>
#include <pthread.h>
#include "etcpal/common.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef pthread_rwlock_t etcpal_rwlock_t;

#define ETCPAL_RWLOCK_HAS_TIMED_LOCK 0

#define etcpal_rwlock_create(idptr)       ((bool)(!pthread_rwlock_init((idptr), NULL)))
#define etcpal_rwlock_readlock(idptr)     ((bool)(!pthread_rwlock_rdlock(idptr)))
#define etcpal_rwlock_try_readlock(idptr) ((bool)(!pthread_rwlock_tryrdlock(idptr)))
bool etcpal_rwlock_timed_readlock(etcpal_rwlock_t* id, int timeout_ms);
#define etcpal_rwlock_readunlock(idptr)    ((void)pthread_rwlock_unlock(idptr))
#define etcpal_rwlock_writelock(idptr)     ((bool)(!pthread_rwlock_wrlock(idptr)))
#define etcpal_rwlock_try_writelock(idptr) ((bool)(!pthread_rwlock_trywrlock(idptr)))
bool etcpal_rwlock_timed_writelock(etcpal_rwlock_t* id, int timeout_ms);
#define etcpal_rwlock_writeunlock(idptr) ((void)pthread_rwlock_unlock(idptr))
#define etcpal_rwlock_destroy(idptr)     ((void)pthread_rwlock_destroy(idptr))

#ifdef __cplusplus
}
#endif

#endif /* ETCPAL_OS_RWLOCK_H_ */
