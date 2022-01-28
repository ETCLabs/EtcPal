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

#ifndef ETCPAL_OS_SEM_H_
#define ETCPAL_OS_SEM_H_

#include <stdbool.h>
#include <FreeRTOS.h>
#include <semphr.h>
#include "etcpal/common.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef SemaphoreHandle_t etcpal_sem_t;

#define ETCPAL_SEM_HAS_TIMED_WAIT    1
#define ETCPAL_SEM_HAS_POST_FROM_ISR 1
#define ETCPAL_SEM_HAS_MAX_COUNT     1
#define ETCPAL_SEM_MUST_BE_BALANCED  0

bool etcpal_sem_create(etcpal_sem_t* id, unsigned int initial_count, unsigned int max_count);
bool etcpal_sem_wait(etcpal_sem_t* id);
bool etcpal_sem_try_wait(etcpal_sem_t* id);
bool etcpal_sem_timed_wait(etcpal_sem_t* id, int timeout_ms);
bool etcpal_sem_post(etcpal_sem_t* id);
bool etcpal_sem_post_from_isr(etcpal_sem_t* id);
void etcpal_sem_destroy(etcpal_sem_t* id);

#ifdef __cplusplus
}
#endif

#endif /* ETCPAL_OS_SEM_H_ */
