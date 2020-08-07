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

#include "etcpal/rtos/pal_queue.h"
#include "projdefs.h"

/*************************** Function definitions ****************************/

bool etcpal_queue_create(etcpal_queue_t* id, unsigned size, unsigned item_size)
{
  if (id)
  {
    return ((*id = (etcpal_queue_t)xQueueCreate(size, item_size)) != NULL);
  }
  return false;
}

bool etcpal_queue_add_blocking(etcpal_queue_t* id, const void* data)
{
  if (id)
  {
    BaseType_t status = xQueueSendToBack(*id, data, portMAX_DELAY);
    return (status == pdPASS);
  }
  return false;
}

bool etcpal_queue_add(etcpal_queue_t* id, const void* data, unsigned timeout_ms)
{
  if (id)
  {
    TickType_t ticks_to_wait = pdMS_TO_TICKS(timeout_ms);
    BaseType_t status = xQueueSendToBack(*id, data, ticks_to_wait);
    return (status == pdPASS);
  }
  return false;
}

bool etcpal_queue_add_from_isr(etcpal_queue_t* id, const void* data)
{
  if (id)
  {
    BaseType_t higherPriorityTaskWoken;
    BaseType_t status = xQueueSendToBackFromISR(*id, data, &higherPriorityTaskWoken);
    portYIELD_FROM_ISR(higherPriorityTaskWoken);
    return (status == pdPASS);
  }
  return false;
}

bool etcpal_queue_get_blocking(etcpal_queue_t* id, void* data)
{
  if (id)
  {
    BaseType_t status = xQueueReceive(*id, data, portMAX_DELAY);
    return (status == pdPASS);
  }
  return false;
}

bool etcpal_queue_get(etcpal_queue_t* id, void* data, unsigned timeout_ms)
{
  if (id)
  {
    BaseType_t status = xQueueReceive(*id, data, pdMS_TO_TICKS(timeout_ms));
    return (status == pdPASS);
  }
  return false;
}

bool etcpal_queue_is_empty(const etcpal_queue_t* id)
{

  UBaseType_t numMessages = uxQueueMessagesWaiting(*id);

  return (numMessages == 0);
}

bool etcpal_queue_is_empty_from_isr(const etcpal_queue_t* id)
{
  UBaseType_t numMessages = uxQueueMessagesWaitingFromISR(*id);

  return (numMessages == 0);
}
