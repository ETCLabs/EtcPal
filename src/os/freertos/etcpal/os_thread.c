/******************************************************************************
 * Copyright 2020 ETC Inc.
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

#if !defined(ETCPAL_BUILDING_MOCK_LIB)

static void thread_func_internal(void* pvParameters)
{
  etcpal_thread_t* thread_data = (etcpal_thread_t*)pvParameters;
  if (thread_data)
  {
    thread_data->fn(thread_data->arg);
    xSemaphoreGive(thread_data->sig);
  }
  vTaskDelete(NULL);
}

etcpal_error_t etcpal_thread_create(etcpal_thread_t*          id,
                                    const EtcPalThreadParams* params,
                                    void (*thread_fn)(void*),
                                    void* thread_arg)
{
  if (!id || !params || !thread_fn)
    return kEtcPalErrInvalid;

  id->sig = xSemaphoreCreateBinary();
  if (!id->sig)
  {
    // Per the FreeRTOS docs, NULL means there was not enough FreeRTOS heap available
    return kEtcPalErrNoMem;
  }

  id->fn = thread_fn;
  id->arg = thread_arg;
  if (pdPASS == xTaskCreate(thread_func_internal, params->thread_name ? params->thread_name : "etcpal_thread",
                            (uint16_t)params->stack_size, id, params->priority, &id->tid))
  {
    return kEtcPalErrOk;
  }
  // Per the FreeRTOS docs, the only failure mode for xTaskCreate is errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY
  return kEtcPalErrNoMem;
}

etcpal_error_t etcpal_thread_join(etcpal_thread_t* id)
{
  if (!id)
    return kEtcPalErrInvalid;

  if (pdTRUE == xSemaphoreTake(id->sig, portMAX_DELAY))
  {
    vSemaphoreDelete(id->sig);
    id->sig = NULL;
    id->tid = NULL;
    return kEtcPalErrOk;
  }
  // Should never happen
  return kEtcPalErrSys;
}

etcpal_error_t etcpal_thread_timed_join(etcpal_thread_t* id, unsigned ms_timeout)
{
  if (!id)
    return kEtcPalErrInvalid;

  if (pdTRUE == xSemaphoreTake(id->sig, pdMS_TO_TICKS(ms_timeout)))
  {
    vSemaphoreDelete(id->sig);
    id->sig = NULL;
    id->tid = NULL;
    return kEtcPalErrOk;
  }


  // The semaphore was not taken within the timeout period
  return kEtcPalErrTimedOut;
}

#endif // !defined(ETCPAL_BUILDING_MOCK_LIB)
