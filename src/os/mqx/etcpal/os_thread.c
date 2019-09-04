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

#include "etcpal/thread.h"

static void thread_func_internal(uint32_t initial_data)
{
  lwpa_thread_t* thread_data = (lwpa_thread_t*)initial_data;
  if (thread_data)
  {
    thread_data->fn(thread_data->arg);
    _lwsem_post(&thread_data->sig);
  }
}

bool lwpa_thread_create(lwpa_thread_t* id, const LwpaThreadParams* params, void (*thread_fn)(void*), void* thread_arg)
{
  if (!id || !params || !thread_fn || (MQX_OK != _lwsem_create(&id->sig, 0)))
    return false;

  TASK_TEMPLATE_STRUCT template;
  template.TASK_ADDRESS = thread_func_internal;
  template.TASK_NAME = (char*)(params->thread_name ? params->thread_name : "lwpa_thread");
  template.TASK_PRIORITY = params->thread_priority;
  template.TASK_STACKSIZE = params->stack_size;
  if (params->platform_data)
  {
    LwpaThreadParamsMqx* platform_params = (LwpaThreadParamsMqx*)params->platform_data;
    template.TASK_ATTRIBUTES = platform_params->task_attributes;
    template.DEFAULT_TIME_SLICE = platform_params->time_slice;
  }
  else
  {
    template.TASK_ATTRIBUTES = (_mqx_uint)LWPA_THREAD_MQX_DEFAULT_ATTRIBUTES;
    template.DEFAULT_TIME_SLICE = LWPA_THREAD_MQX_DEFAULT_TIME_SLICE;
  }
  id->fn = thread_fn;
  id->arg = thread_arg;
  /* This cast is OK for now, because we only use MQX on 32-bit processors.
   * It may need to be reexamined in the future. */
  template.CREATION_PARAMETER = (uint32_t)id;

  _task_id t_id = _task_create(0, 0, (uint32_t) & template);
  if (t_id != MQX_NULL_TASK_ID)
  {
    id->tid = t_id;
    return true;
  }
  return false;
}

bool lwpa_thread_join(lwpa_thread_t* id)
{
  if (!id)
    return false;

  _mqx_uint res = _lwsem_wait(&id->sig);
  if (res == MQX_OK)
  {
    _lwsem_destroy(&id->sig);
    return true;
  }
  return false;
}
