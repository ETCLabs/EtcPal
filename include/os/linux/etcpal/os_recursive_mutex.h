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

#ifndef ETCPAL_OS_RECURSIVE_MUTEX_H_
#define ETCPAL_OS_RECURSIVE_MUTEX_H_

#include <stdbool.h>
#include <pthread.h>
#include "etcpal/common.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef pthread_mutex_t etcpal_recursive_mutex_t;
#define ETCPAL_RECURSIVE_MUTEX_INIT PTHREAD_MUTEX_INITIALIZER

#define ETCPAL_RECURSIVE_MUTEX_HAS_TIMED_LOCK 0

bool etcpal_recursive_mutex_create(etcpal_recursive_mutex_t* id);
#define etcpal_recursive_mutex_lock(idptr)     ((bool)(!pthread_mutex_lock(idptr)))
#define etcpal_recursive_mutex_try_lock(idptr) ((bool)(!pthread_mutex_trylock(idptr)))
bool etcpal_recursive_mutex_timed_lock(etcpal_recursive_mutex_t* id, int timeout_ms);
#define etcpal_recursive_mutex_unlock(idptr)  ((void)pthread_mutex_unlock(idptr))
#define etcpal_recursive_mutex_destroy(idptr) ((void)pthread_mutex_destroy(idptr))

#ifdef __cplusplus
}
#endif

#endif /* ETCPAL_OS_RECURSIVE_MUTEX_H_ */
