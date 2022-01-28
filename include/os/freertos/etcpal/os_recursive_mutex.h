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
#include <FreeRTOS.h>
#include <semphr.h>
#include "etcpal/common.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef SemaphoreHandle_t etcpal_recursive_mutex_t;

#define ETCPAL_RECURSIVE_MUTEX_HAS_TIMED_LOCK 1

bool etcpal_recursive_mutex_create(etcpal_recursive_mutex_t* id);
bool etcpal_recursive_mutex_lock(etcpal_recursive_mutex_t* id);
bool etcpal_recursive_mutex_try_lock(etcpal_recursive_mutex_t* id);
bool etcpal_recursive_mutex_timed_lock(etcpal_recursive_mutex_t* id, int timeout_ms);
void etcpal_recursive_mutex_unlock(etcpal_recursive_mutex_t* id);
void etcpal_recursive_mutex_destroy(etcpal_recursive_mutex_t* id);

#ifdef __cplusplus
}
#endif

#endif /* ETCPAL_OS_RECURSIVE_MUTEX_H_ */
