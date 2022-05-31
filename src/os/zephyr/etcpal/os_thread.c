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

#include "etcpal/thread.h"
#include "pthread.h"
#include <kernel.h>
#include <assert.h>

#if !defined(ETCPAL_BUILDING_MOCK_LIB)

volatile int i = 0;
static void thread_func_internal(void *pvParameters, void *p2, void *p3) {
  etcpal_thread_t *thread_data = (etcpal_thread_t *)pvParameters;
  if (thread_data) {
    thread_data->fn(thread_data->arg);
  }
}

/*
 * From the Zephyr docs:
 *   A cooperative thread has a negative priority value. Once it becomes the
 *   current thread, a cooperative thread remains the current thread until it
 *   performs an action that makes it unready.
 *  
 *   A preemptible thread has a non-negative priority value. Once it becomes the
 *   current thread, a preemptible thread may be supplanted at any time if a
 *   cooperative thread, or a preemptible thread of higher or equal priority, becomes
 *   ready.
 **/
etcpal_error_t etcpal_thread_create(etcpal_thread_t*          id,
                                    const EtcPalThreadParams* params,
                                    void (*thread_fn)(void*),
                                    void* thread_arg)
{
  EtcPalThreadParams *tparams = (EtcPalThreadParams*)params;
  EtcPalThreadParamsZephyr *zparams = tparams->platform_data;

  // Depending on the scheduler mode, we may need to change around the priority
  int priority = 0;

  if (zparams->sched_mode == NOT_SPECIFIED) {

    return kEtcPalErrInvalid;

  } else if (zparams->sched_mode == PREEMPTIVE) {

    priority = params->priority;

    // CONFIG_NUM_PREEMPT_PRIORITIES should be specified with Kconfig
    if (priority >= CONFIG_NUM_PREEMPT_PRIORITIES) {
      return kEtcPalErrInvalid;
    }

  } else if (zparams->sched_mode == COOPERATIVE) {

    priority = -1 * (int)params->priority;

    // CONFIG_NUM_PREEMPT_PRIORITIES should be specified with Kconfig
    if (priority <= -1 * (int)CONFIG_NUM_COOP_PRIORITIES) {
      return kEtcPalErrInvalid;
    }

  } else if (zparams->sched_mode == META_IRQ) {

    return kEtcPalErrNotImpl;
  }

  // Thread delay controls how long to wait before starting the thread
  k_timeout_t thread_delay = {ETCPAL_NO_WAIT};

  id->fn = thread_fn;
  id->arg = thread_arg;
  
  // Check if the user has given us a buffer to use as a stack pointer

  if (zparams->stack_data_pointer == NULL) {

    // This stack is allocated from the calling thread's resource pool
    #ifdef NAM
    id->my_stack = (k_thread_stack_t *)calloc(params->stack_size, sizeof(k_thread_stack_t));
    id->tid = k_thread_create(&id->my_thread, id->my_stack,
                              params->stack_size,
                              thread_func_internal, id, NULL, NULL, priority, 0,
                              thread_delay);
    #endif // NAM
    // Dynamic creation not supported! (yet?)
    assert(false);

  } else {

    id->my_stack = zparams->stack_data_pointer;
    
    // Note K_THREAD_STACK_SIZEOF(&id->my_stack) does not necessarily equal
    // params->stack_size there is padding and alignment
    id->tid = k_thread_create(
        &id->my_thread,
        id->my_stack,
        params->stack_size, // Zephyr may want to allocate more data as padding.
        thread_func_internal,
        id, NULL, NULL, priority, 0,
                              thread_delay);
  }

  if (id->tid == 0) {
    return kEtcPalErrSys;
  }

  k_thread_name_set(id->tid, params->thread_name);
  return kEtcPalErrOk;
}

etcpal_error_t etcpal_thread_join(etcpal_thread_t* id)
{
  return kEtcPalErrNotImpl;
}

etcpal_error_t etcpal_thread_timed_join(etcpal_thread_t* id, int timeout_ms)
{
  return kEtcPalErrNotImpl;
}

etcpal_error_t etcpal_thread_terminate(etcpal_thread_t* id)
{
  k_thread_abort(id->tid);
  k_stack_cleanup(&id->my_stack);
  return kEtcPalErrOk;
}

etcpal_thread_os_handle_t etcpal_thread_get_os_handle(etcpal_thread_t* id)
{
  return kEtcPalErrNotImpl;
}

#endif  // !defined(ETCPAL_BUILDING_MOCK_LIB)
