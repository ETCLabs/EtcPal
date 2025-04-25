/******************************************************************************
 * Copyright 2024 ETC Inc.
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

#ifndef ETCPAL_OS_SIGNAL_H_
#define ETCPAL_OS_SIGNAL_H_

#include <stdbool.h>
#include <zephyr/kernel.h>
#include "etcpal/common.h"
#include "etcpal/etcpal_zephyr_common.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct k_sem etcpal_signal_t;
#define ETCPAL_SIGNAL_INIT \
  {                        \
  }

#define ETCPAL_SIGNAL_HAS_TIMED_WAIT    1
#define ETCPAL_SIGNAL_HAS_POST_FROM_ISR 1

#define etcpal_signal_create(idptr)                 ((bool)(!k_sem_init((idptr), 0, 1)))
#define etcpal_signal_wait(idptr)                   ((bool)(!k_sem_take((idptr), K_FOREVER)))
#define etcpal_signal_try_wait(idptr)               ((bool)(!k_sem_take((idptr), K_NO_WAIT)))
#define etcpal_signal_timed_wait(idptr, timeout_ms) ((bool)(!k_sem_take((idptr), ms_to_zephyr_timeout((timeout_ms)))))
#define etcpal_signal_post(idptr)                   (k_sem_give((idptr)))
#define etcpal_signal_post_from_isr(idptr)          (etcpal_signal_post((idptr)))
void etcpal_signal_destroy(etcpal_signal_t* id);

#ifdef __cplusplus
}
#endif

#endif /* ETCPAL_OS_SIGNAL_H_ */
