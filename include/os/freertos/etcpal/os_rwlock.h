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
#include <FreeRTOS.h>
#include <semphr.h>
#include "etcpal/common.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
  bool              valid;
  SemaphoreHandle_t sem;
  unsigned int      reader_count;
} etcpal_rwlock_t;

#define ETCPAL_RWLOCK_HAS_TIMED_LOCK 1

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

#ifdef __cplusplus
}
#endif

#endif /* ETCPAL_OS_RWLOCK_H_ */
