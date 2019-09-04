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

static void thread_func_internal(void* pvParameters)
{
  lwpa_thread_t* thread_data = (lwpa_thread_t*)pvParameters;
  if (thread_data)
  {
    thread_data->fn(thread_data->arg);
    xSemaphoreGive(thread_data->sig);
  }
  vTaskDelete(NULL);
}

bool lwpa_thread_create(lwpa_thread_t* id, const LwpaThreadParams* params, void (*thread_fn)(void*), void* thread_arg)
{
  if (!id || !params || !thread_fn)
    return false;

  id->sig = xSemaphoreCreateBinary();
  if (!id->sig)
    return false;

  id->fn = thread_fn;
  id->arg = thread_arg;
  if (pdPASS == xTaskCreate(thread_func_internal, params->thread_name ? params->thread_name : "lwpa_thread",
                            (uint16_t)params->stack_size, id, params->thread_priority, &id->tid))
  {
    return true;
  }
  return false;
}

bool lwpa_thread_join(lwpa_thread_t* id)
{
  bool res;

  if (!id)
    return false;

  res = (pdTRUE == xSemaphoreTake(id->sig, portMAX_DELAY));
  if (res)
  {
    vSemaphoreDelete(id->sig);
    id->sig = NULL;
    id->tid = NULL;
  }
  return res;
}
