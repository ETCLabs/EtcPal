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

#include "etcpal/sem.h"

bool etcpal_sem_create(etcpal_sem_t* id, unsigned int initial_count, unsigned int max_count)
{
  if (id)
  {
    return ((*id = (etcpal_sem_t)xSemaphoreCreateCounting(max_count, initial_count)) != NULL);
  }
  return false;
}

bool etcpal_sem_wait(etcpal_sem_t* id)
{
  if (id && *id)
  {
    return (pdTRUE == xSemaphoreTake((SemaphoreHandle_t)*id, portMAX_DELAY));
  }
  return false;
}

bool etcpal_sem_try_wait(etcpal_sem_t* id)
{
  if (id && *id)
  {
    return (pdTRUE == xSemaphoreTake((SemaphoreHandle_t)*id, 0));
  }
  return false;
}

bool etcpal_sem_timed_wait(etcpal_sem_t* id, int timeout_ms)
{
  if (id && *id)
  {
    return (pdTRUE == xSemaphoreTake((SemaphoreHandle_t)*id, pdMS_TO_TICKS(timeout_ms)));
  }
  return false;
}

bool etcpal_sem_post(etcpal_sem_t* id)
{
  if (id && *id)
  {
    return (pdTRUE == xSemaphoreGive((SemaphoreHandle_t)*id));
  }
  return false;
}

bool etcpal_sem_post_from_isr(etcpal_sem_t* id)
{
  bool result = false;
  if (id && *id)
  {
    BaseType_t higherPriorityTaskWoken;
    result = (pdTRUE == xSemaphoreGiveFromISR((SemaphoreHandle_t)*id, &higherPriorityTaskWoken));
    portYIELD_FROM_ISR(higherPriorityTaskWoken);
  }
  return result;
}

void etcpal_sem_destroy(etcpal_sem_t* id)
{
  if (id && *id)
  {
    vSemaphoreDelete((SemaphoreHandle_t)*id);
    *id = (etcpal_signal_t)NULL;
  }
}
