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

#ifndef ETCPAL_OS_EVENT_GROUP_H_
#define ETCPAL_OS_EVENT_GROUP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include <zephyr/kernel.h>
#include "etcpal/common.h"

typedef uint32_t etcpal_event_bits_t;
#define ETCPAL_EVENT_BITS_INIT 0

typedef struct
{
  bool                valid;
  struct k_condvar    cond;
  struct k_mutex      mutex;
  etcpal_event_bits_t bits;
} etcpal_event_group_t;
#define ETCPAL_EVENT_GROUP_INIT \
  {                             \
  }

#define ETCPAL_EVENT_GROUP_HAS_TIMED_WAIT         1
#define ETCPAL_EVENT_GROUP_HAS_ISR_FUNCTIONS      1
#define ETCPAL_EVENT_GROUP_WAKES_MULTIPLE_THREADS 1
#define ETCPAL_EVENT_GROUP_NUM_USABLE_BITS        32

bool etcpal_event_group_create(etcpal_event_group_t* id);
#define etcpal_event_group_wait(idptr, bits, flags) \
  (etcpal_event_group_timed_wait((idptr), (bits), (flags), ETCPAL_WAIT_FOREVER))
etcpal_event_bits_t etcpal_event_group_timed_wait(etcpal_event_group_t* id,
                                                  etcpal_event_bits_t   bits,
                                                  int                   flags,
                                                  int                   timeout_ms);
void                etcpal_event_group_set_bits(etcpal_event_group_t* id, etcpal_event_bits_t bits_to_set);
etcpal_event_bits_t etcpal_event_group_get_bits(etcpal_event_group_t* id);
void                etcpal_event_group_clear_bits(etcpal_event_group_t* id, etcpal_event_bits_t bits_to_clear);
void                etcpal_event_group_destroy(etcpal_event_group_t* id);

// Non-ISR calls are ISR safe
#define etcpal_event_group_set_bits_from_isr   etcpal_event_group_set_bits
#define etcpal_event_group_get_bits_from_isr   etcpal_event_group_get_bits
#define etcpal_event_group_clear_bits_from_isr etcpal_event_group_clear_bits

#ifdef __cplusplus
}
#endif

#endif /* ETCPAL_OS_EVENT_GROUP_H_ */
