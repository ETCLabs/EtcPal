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

#include "etcpal/signal.h"
#include "freertos_timed_wait.h"

bool etcpal_signal_create(etcpal_signal_t* id)
{
  if (id)
  {
    return ((*id = (etcpal_signal_t)xSemaphoreCreateBinary()) != NULL);
  }
  return false;
}

bool etcpal_signal_wait(etcpal_signal_t* id)
{
  if (id && *id)
  {
    return (pdTRUE == xSemaphoreTake((SemaphoreHandle_t)*id, portMAX_DELAY));
  }
  return false;
}

bool etcpal_signal_try_wait(etcpal_signal_t* id)
{
  if (id && *id)
  {
    return (pdTRUE == xSemaphoreTake((SemaphoreHandle_t)*id, 0));
  }
  return false;
}

bool etcpal_signal_timed_wait(etcpal_signal_t* id, int timeout_ms)
{
  if (id && *id)
  {
    return (pdTRUE == xSemaphoreTake((SemaphoreHandle_t)*id, ETCPAL_TIMEOUT_TO_FREERTOS(timeout_ms)));
  }
  return false;
}

void etcpal_signal_post(etcpal_signal_t* id)
{
  if (id && *id)
    xSemaphoreGive((SemaphoreHandle_t)*id);
}

void etcpal_signal_post_from_isr(etcpal_signal_t* id)
{
  if (id && *id)
  {
    BaseType_t higherPriorityTaskWoken;
    xSemaphoreGiveFromISR((SemaphoreHandle_t)*id, &higherPriorityTaskWoken);
    portYIELD_FROM_ISR(higherPriorityTaskWoken);
  }
}

void etcpal_signal_destroy(etcpal_signal_t* id)
{
  if (id && *id)
  {
    vSemaphoreDelete((SemaphoreHandle_t)*id);
    *id = (etcpal_signal_t)NULL;
  }
}
