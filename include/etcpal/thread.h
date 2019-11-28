/******************************************************************************
 * Copyright 2019 ETC Inc.
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

#include "etcpal/os_thread.h"

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * \defgroup etcpal_thread Threading (thread)
 * \ingroup etcpal
 * \brief Platform-neutral threads.
 *
 * ```c
 * #include "etcpal/thread.h"
 * ```
 *
 * @{
 */

/*! A set of parameters for an etcpal_thread. */
typedef struct EtcPalThreadParams
{
  /*! The priority of the thread. Note that thread priority is not valid on all platforms. */
  unsigned int thread_priority;
  /*! The stack size of the thread. Note that thread stack size is not valid on all platforms. */
  unsigned int stack_size;
  /*! A name for the thread, maximum length #ETCPAL_THREAD_NAME_MAX_LENGTH. */
  const char* thread_name;
  /*! Pointer to a platform-specific parameter structure. */
  void* platform_data;
} EtcPalThreadParams;

/*! Set the platform-default values for the EtcPalThreadParams struct. */
#define ETCPAL_THREAD_SET_DEFAULT_PARAMS(threadparamsptr)                \
  do                                                                     \
  {                                                                      \
    (threadparamsptr)->thread_priority = ETCPAL_THREAD_DEFAULT_PRIORITY; \
    (threadparamsptr)->stack_size = ETCPAL_THREAD_DEFAULT_STACK;         \
    (threadparamsptr)->thread_name = ETCPAL_THREAD_DEFAULT_NAME;         \
    (threadparamsptr)->platform_data = NULL;                             \
  } while (0)

bool etcpal_thread_create(etcpal_thread_t* id, const EtcPalThreadParams* params, void (*thread_fn)(void*),
                          void* thread_arg);
bool etcpal_thread_join(etcpal_thread_t* id);

#if !defined(etcpal_thread_sleep) || DOXYGEN
void etcpal_thread_sleep(int sleep_ms);
#endif

/*!
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* ETCPAL_THREAD_H_ */
