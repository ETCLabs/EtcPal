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

#ifndef ETCPAL_OS_THREAD_H_
#define ETCPAL_OS_THREAD_H_

#include <stdbool.h>
#include <zephyr/kernel.h>
#include "etcpal/common.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ETCPAL_THREAD_DEFAULT_PRIORITY 14
#define ETCPAL_THREAD_DEFAULT_STACK    4096
#define ETCPAL_THREAD_DEFAULT_NAME     "etcpal_thread"
#define ETCPAL_THREAD_NAME_MAX_LENGTH  CONFIG_THREAD_MAX_NAME_LEN

#define ETCPAL_THREAD_HAS_TIMED_JOIN 1

typedef k_tid_t etcpal_thread_os_handle_t;
#define ETCPAL_THREAD_OS_HANDLE_INVALID NULL

typedef struct {
  void (*fn)(void*);
  void*             arg;
  //etcpal_sem_t sig;
  etcpal_thread_os_handle_t      tid;
  struct k_thread thread;
  k_thread_stack_t* stack;
  
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



#define etcpal_thread_get_current_os_handle k_current_get

#ifdef __cplusplus
}
#endif

#endif /* ETCPAL_OS_THREAD_H_ */
