/******************************************************************************
 * Copyright 2021 ETC Inc.
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
#include <mqx.h>
#include "etcpal/common.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef LWSEM_STRUCT etcpal_sem_t;

#define ETCPAL_SEM_HAS_TIMED_WAIT    1
#define ETCPAL_SEM_HAS_POST_FROM_ISR 0
#define ETCPAL_SEM_HAS_MAX_COUNT     0
#define ETCPAL_SEM_MUST_BE_BALANCED  0

#define etcpal_sem_create(idptr, initial_count, max_count) (MQX_OK == _lwsem_create((idptr), (_mqx_int)(initial_count)))
#define etcpal_sem_wait(idptr)                             (MQX_OK == _lwsem_wait(idptr))
#define etcpal_sem_try_wait(idptr)                         (MQX_OK == _lwsem_wait_ticks((idptr), 1u))
bool etcpal_sem_timed_wait(etcpal_sem_t* id, int timeout_ms);
#define etcpal_sem_post(idptr)    (MQX_OK == _lwsem_post(idptr))
#define etcpal_sem_post_from_isr  etcpal_sem_post
#define etcpal_sem_destroy(idptr) ((void)_lwsem_destroy(idptr))

#ifdef __cplusplus
}
#endif

#endif /* ETCPAL_OS_SEM_H_ */
