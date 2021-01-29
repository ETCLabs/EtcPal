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

#ifndef ETCPAL_OS_SIGNAL_H_
#define ETCPAL_OS_SIGNAL_H_

#include <stdbool.h>
#include <mqx.h>
#include <lwevent.h>
#include "etcpal/common.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef LWEVENT_STRUCT etcpal_signal_t;

#define ETCPAL_SIGNAL_HAS_TIMED_WAIT 1
#define ETCPAL_SIGNAL_HAS_POST_FROM_ISR 0

#define etcpal_signal_create(idptr) (MQX_OK == _lwevent_create((idptr), LWEVENT_AUTO_CLEAR))
#define etcpal_signal_wait(idptr) (MQX_OK == _lwevent_wait_ticks((idptr), 1u, true, 0u))
#define etcpal_signal_try_wait(idptr) (MQX_OK == _lwevent_wait_ticks((idptr), 1u, true, 1u))
bool etcpal_signal_timed_wait(etcpal_signal_t* id, int timeout_ms);
#define etcpal_signal_post(idptr) ((void)_lwevent_set((idptr), 1u))
#define etcpal_signal_post_from_isr etcpal_signal_post
#define etcpal_signal_destroy(idptr) ((void)_lwevent_destroy(idptr))

#ifdef __cplusplus
}
#endif

#endif /* ETCPAL_OS_SIGNAL_H_ */
