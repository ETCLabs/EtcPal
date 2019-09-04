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

#ifndef _ETCPAL_THREAD_H_
#define _ETCPAL_THREAD_H_

#include "etcpal/os_thread.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct LwpaThreadParams
{
  unsigned int thread_priority;
  unsigned int stack_size;
  const char* thread_name;
  void* platform_data;
} LwpaThreadParams;

#define ETCPAL_THREAD_SET_DEFAULT_PARAMS(threadparamsptr)                \
  do                                                                   \
  {                                                                    \
    (threadparamsptr)->thread_priority = ETCPAL_THREAD_DEFAULT_PRIORITY; \
    (threadparamsptr)->stack_size = ETCPAL_THREAD_DEFAULT_STACK;         \
    (threadparamsptr)->thread_name = ETCPAL_THREAD_DEFAULT_NAME;         \
    (threadparamsptr)->platform_data = NULL;                           \
  } while (0)

bool etcpal_thread_create(etcpal_thread_t* id, const LwpaThreadParams* params, void (*thread_fn)(void*), void* thread_arg);
bool etcpal_thread_join(etcpal_thread_t* id);

#ifndef etcpal_thread_sleep
void etcpal_thread_sleep(int sleep_ms);
#endif

#ifdef __cplusplus
}
#endif

#endif /* _ETCPAL_THREAD_H_ */
