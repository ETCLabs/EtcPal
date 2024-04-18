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

#ifndef ETCPAL_THREAD_H_
#define ETCPAL_THREAD_H_

#include "etcpal/error.h"
#include "etcpal/os_thread.h"

#ifdef __cplusplus
extern "C" {
#endif
/**
 * @defgroup etcpal_thread thread (Threading)
 * @ingroup etcpal_os
 * @brief Platform-neutral threads.
 *
 * ```c
 * #include "etcpal/thread.h"
 * ```
 *
 * Provides a platform-neutral threading API with an interface similar to pthread.
 *
 * @code
 * void thread_function(void* arg)
 * {
 *   int val = *(int*)arg;
 *   printf("Hello from a thread! The value is %d.\n", val);
 *   etcpal_thread_sleep(10); // Sleep for 10 milliseconds
 * }
 *
 * EtcPalThreadParams params = ETCPAL_THREAD_PARAMS_INIT;
 * int arg = 42;
 * etcpal_thread_t handle;
 * etcpal_error_t result = etcpal_thread_create(&handle, &params, thread_function, &arg);
 * if (result == kEtcPalErrOk)
 * {
 *   // The thread is running and prints "Hello from a thread! The value is 42."
 *   etcpal_thread_join(&handle);
 *   // The thread is no longer running.
 * }
 * @endcode
 *
 * On some platforms you can wait a finite time for a thread to exit:
 *
 * @code
 * if (etcpal_thread_timed_join(&handle, 1000) == kEtcPalErrOk)
 * {
 *   // The thread is no longer running.
 * }
 * else
 * {
 *   // If etcpal_thread_timed_join() returns kEtcPalErrTimedOut, the thread failed to stop within
 *   // the timeout given.
 * }
 * @endcode
 *
 * This behavior is platform-dependent:
 *
 * Platform | #ETCPAL_THREAD_HAS_TIMED_JOIN |
 * ---------|-------------------------------|
 * FreeRTOS | Yes                           |
 * Linux    | No                            |
 * macOS    | No                            |
 * MQX      | No                            |
 * Windows  | Yes                           |
 * Zephyr   | Yes                           |
 *
 * Manipulate the EtcPalThreadParams structure to change parameters about the thread to be created:
 *
 * @code
 * EtcPalThreadParams params;
 *
 * // The meaning of these numbers is platform-defined, and they are not honored on all platforms. See the description
 * // of the EtcPalThreadParams struct for more information.
 * params.priority = 8;
 * params.stack_size = 1500;
 *
 * params.thread_name = "My Thread";
 * @endcode
 *
 * You can also use the etcpal_thread_sleep() function to sleep for a given number of milliseconds
 * from any valid thread context:
 *
 * @code
 * etcpal_thread_sleep(200); // Sleep for 200 milliseconds
 * @endcode
 *
 * **IMPORTANT NOTE FOR RTOS USERS:** The EtcPal threading API does not initialize the scheduler on
 * real-time systems (e.g. it does not call vTaskStartScheduler() on FreeRTOS or _mqx() on MQX).
 * The application writer is responsible for managing the interaction between starting the
 * scheduler and starting EtcPal threads, just like when starting native RTOS tasks.
 *
 * **IMPORTANT NOTES FOR ZEPHYR USERS:** Some EtcPal features are dependent on Zephyr configuration see: <a
 * href="targeting_zephyr.html">Targeting Zephyr</a>
 * @{
 */

/**
 * @brief A set of parameters for an etcpal_thread.
 *
 * The members of this structure are not all honored on all platforms. Here is a breakdown:
 *
 * Platform | Priority Honored | Stack Size Honored | Thread Name Honored | Platform Data Available    |
 * ---------|------------------|--------------------|---------------------|----------------------------|
 * FreeRTOS | Yes              | Yes                | Yes                 | No                         |
 * Linux    | No               | Yes                | Yes                 | No                         |
 * macOS    | No               | Yes                | Yes                 | No                         |
 * MQX      | Yes              | Yes                | Yes                 | Yes, EtcPalThreadParamsMqx |
 * Windows  | Yes              | Yes                | Yes                 | No                         |
 * Zephyr   | Yes              | Yes                | Yes                 | No                         |
 */
typedef struct EtcPalThreadParams
{
  /** The priority of the thread. Note that thread priority is not valid on all platforms. */
  unsigned int priority;
  /**
   * @brief The stack size of the thread in bytes.
   *
   * This value is always in bytes and will be translated as appropriate for the underlying
   * platform (e.g. converted to words for FreeRTOS).
   *
   * Note that thread stack size is not valid on all platforms.
   */
  unsigned int stack_size;
  /** A name for the thread, maximum length #ETCPAL_THREAD_NAME_MAX_LENGTH. */
  const char* thread_name;
  /**
   * @brief Pointer to a platform-specific parameter structure.
   *
   * This is used to set thread attributes that are not shared between platforms. Currently the
   * only platform that has a valid value for this member is MQX, which has the following
   * structure:
   *
   * @code
   * typedef struct EtcPalThreadParamsMqx
   * {
   *   _mqx_uint task_attributes; // Corresponds to the TASK_ATTRIBUTES member of TASK_TEMPLATE_STRUCT
   *   _mqx_uint time_slice; // Corresponds to the DEFAULT_TIME_SLICE member of TASK_TEMPLATE_STRUCT
   * } EtcPalThreadParamsMqx;
   * @endcode
   */
  void* platform_data;
} EtcPalThreadParams;

/** Set the platform-default values for the EtcPalThreadParams struct. */
#define ETCPAL_THREAD_SET_DEFAULT_PARAMS(threadparamsptr)              \
  do                                                                   \
  {                                                                    \
    (threadparamsptr)->priority      = ETCPAL_THREAD_DEFAULT_PRIORITY; \
    (threadparamsptr)->stack_size    = ETCPAL_THREAD_DEFAULT_STACK;    \
    (threadparamsptr)->thread_name   = ETCPAL_THREAD_DEFAULT_NAME;     \
    (threadparamsptr)->platform_data = NULL;                           \
  } while (0)

/**
 * @brief The set of default values for an EtcPalThreadParamsStructure.
 *
 * Suitable for using in a bracket initializer, e.g.:
 * @code
 * EtcPalThreadParams params = ETCPAL_THREAD_PARAMS_INIT_VALUES;
 * @endcode
 */
#define ETCPAL_THREAD_PARAMS_INIT_VALUES \
  ETCPAL_THREAD_DEFAULT_PRIORITY, ETCPAL_THREAD_DEFAULT_STACK, ETCPAL_THREAD_DEFAULT_NAME, NULL

/**
 * @brief A default initializer value for an EtcPalThreadParams structure.
 *
 * Usage:
 * @code
 * EtcPalThreadParams params = ETCPAL_THREAD_PARAMS_INIT;
 * @endcode
 */
#define ETCPAL_THREAD_PARAMS_INIT    \
  {                                  \
    ETCPAL_THREAD_PARAMS_INIT_VALUES \
  }

etcpal_error_t            etcpal_thread_create(etcpal_thread_t*          id,
                                               const EtcPalThreadParams* params,
                                               void (*thread_fn)(void*),
                                               void* thread_arg);
etcpal_error_t            etcpal_thread_sleep(unsigned int sleep_ms);
etcpal_error_t            etcpal_thread_join(etcpal_thread_t* id);
etcpal_error_t            etcpal_thread_timed_join(etcpal_thread_t* id, int timeout_ms);
etcpal_error_t            etcpal_thread_terminate(etcpal_thread_t* id);
etcpal_thread_os_handle_t etcpal_thread_get_os_handle(etcpal_thread_t* id);

#if !defined(etcpal_thread_get_current_os_handle) || DOXYGEN
etcpal_thread_os_handle_t etcpal_thread_get_current_os_handle(void);

/**
 * @}
 */

#endif
#ifdef __cplusplus
}
#endif
#endif /* ETCPAL_THREAD_H_ */
