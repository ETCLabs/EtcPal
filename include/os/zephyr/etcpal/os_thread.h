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

#ifndef ETCPAL_OS_THREAD_H_
#define ETCPAL_OS_THREAD_H_
#include "kernel.h"
#include "etcpal/common.h"
#include "etcpal/sem.h"
#include "etcpal/thread.h"
#include "zephyr/kernel.h"



#ifdef __cplusplus
extern "C" {
#endif

#define ETCPAL_THREAD_DEFAULT_PRIORITY (5)
#define ETCPAL_THREAD_DEFAULT_STACK    2000
#define ETCPAL_THREAD_DEFAULT_NAME     "etcpal_thread"
#define ETCPAL_THREAD_HAS_TIMED_JOIN   1
#define ETCPAL_THREAD_NAME_MAX_LENGTH  16

typedef k_tid_t etcpal_thread_os_handle_t;
#define ETCPAL_THREAD_OS_HANDLE_INVALID NULL

typedef struct {
  void (*fn)(void*);
  void*             arg;
  etcpal_sem_t sig;
  etcpal_thread_os_handle_t      tid;
  struct k_thread my_thread;
  k_thread_stack_t* my_stack;
  
 // stack_data_t *stack_data_pointer;
  

} etcpal_thread_t;

typedef enum {
  /// Will trigger an error
  NOT_SPECIFIED,
  /// Thread will be pre-emtpible
  PREEMPTIVE,
  /// Thread will only be marked unready on it's own terms, except by META_IRQ
  /// threads/
  COOPERATIVE,
  /// Not yet implemented in this port. Special, high-priority threads that can even
  /// pre-empt COOPERTIVE threads. These threads are second in priority only to IRQs themselves.
  /// The purpose of a META_IRQ thread is to be the 'back-half' of a hardware interrupt handler.
  META_IRQ       
  }scheduling_mode_t;

typedef struct {
  // If this is null, the current thread's resource pool
  // will be used to allocate the new thread's resource pool
  k_thread_stack_t* stack_data_pointer;
  scheduling_mode_t sched_mode;
} EtcPalThreadParamsZephyr;


static inline void zephyr_sleep_ms(int sleep_ms) {
  if (sleep_ms == ETCPAL_WAIT_FOREVER) {
    k_sleep(K_FOREVER);
  } else {
    k_msleep(sleep_ms);
    }
  }

#define etcpal_thread_sleep(sleep_ms) zephyr_sleep_ms(sleep_ms)
#define etcpal_thread_sleep_us(sleep_us) k_usleep(sleep_us)
#define etcpal_thread_get_current_os_handle xTaskGetCurrentTaskHandle

#ifdef __cplusplus
}
#endif

#endif /* ETCPAL_OS_THREAD_H_ */
