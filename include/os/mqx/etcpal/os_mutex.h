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

#ifndef ETCPAL_OS_MUTEX_H_
#define ETCPAL_OS_MUTEX_H_

#include <stdbool.h>
#include <mqx.h>
#include "etcpal/common.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef LWSEM_STRUCT etcpal_mutex_t;
#define ETCPAL_MUTEX_INIT \
  {                       \
    0                     \
  }

#define ETCPAL_MUTEX_HAS_TIMED_LOCK 1

#define etcpal_mutex_create(idptr)   (MQX_OK == _lwsem_create((idptr), 1))
#define etcpal_mutex_lock(idptr)     (MQX_OK == _lwsem_wait(idptr))
#define etcpal_mutex_try_lock(idptr) (MQX_OK == _lwsem_wait_ticks((idptr), 1u))
bool etcpal_mutex_timed_lock(etcpal_mutex_t* id, int timeout_ms);
#define etcpal_mutex_unlock(idptr)  ((void)_lwsem_post(idptr))
#define etcpal_mutex_destroy(idptr) ((void)_lwsem_destroy(idptr))

#ifdef __cplusplus
}
#endif

#endif /* ETCPAL_OS_MUTEX_H_ */
