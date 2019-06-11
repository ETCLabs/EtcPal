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
 * This file is a part of lwpa. For more information, go to:
 * https://github.com/ETCLabs/lwpa
 ******************************************************************************/

#include "lwpa/thread.h"

/*********************** Private function prototypes *************************/

static void* thread_func_internal(void* arg);

/*************************** Function definitions ****************************/

bool lwpa_thread_create(lwpa_thread_t* id, const LwpaThreadParams* params, void (*thread_fn)(void*), void* thread_arg)
{
  if (!id || !params || !thread_fn)
    return false;

  pthread_attr_t thread_attr;
  pthread_attr_t* p_thread_attr = NULL;

  if (params && params->stack_size != LWPA_THREAD_DEFAULT_STACK)
  {
    pthread_attr_init(&thread_attr);
    pthread_attr_setstacksize(&thread_attr, params->stack_size);
    p_thread_attr = &thread_attr;
  }

  id->fn = thread_fn;
  id->arg = thread_arg;
  bool res = (0 == pthread_create(&id->handle, p_thread_attr, thread_func_internal, id));

  if (p_thread_attr)
    pthread_attr_destroy(p_thread_attr);

  return res;
}

bool lwpa_thread_stop(lwpa_thread_t* id)
{
  if (id)
  {
    return (0 == pthread_join(id->handle, NULL));
  }
  return false;
}

void* thread_func_internal(void* arg)
{
  lwpa_thread_t* thread_data = (lwpa_thread_t*)arg;
  if (thread_data && thread_data->fn)
  {
    thread_data->fn(thread_data->arg);
  }
  return NULL;
}
