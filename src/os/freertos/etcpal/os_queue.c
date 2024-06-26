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

#include "etcpal/queue.h"
#include "freertos_timed_wait.h"

/*************************** Function definitions ****************************/
bool etcpal_queue_create_static(etcpal_queue_t* id, size_t size, size_t item_size, uint8_t* buffer)
{
  // Not implemented
  return false;
}

bool etcpal_queue_create(etcpal_queue_t* id, size_t size, size_t item_size)
{
  if (id)
  {
    return ((*id = (etcpal_queue_t)xQueueCreate(size, item_size)) != NULL);
  }
  return false;
}

void etcpal_queue_destroy(etcpal_queue_t* id)
{
  if (id)
  {
    vQueueDelete(*id);
  }
}

bool etcpal_queue_send(etcpal_queue_t* id, const void* data)
{
  if (id && data)
  {
    BaseType_t status = xQueueSendToBack(*id, data, portMAX_DELAY);
    return (status == pdPASS);
  }
  return false;
}

bool etcpal_queue_timed_send(etcpal_queue_t* id, const void* data, int timeout_ms)
{
  if (id && data)
  {
    BaseType_t status = xQueueSendToBack(*id, data, ETCPAL_TIMEOUT_TO_FREERTOS(timeout_ms));
    return (status == pdPASS);
  }
  return false;
}

bool etcpal_queue_send_from_isr(etcpal_queue_t* id, const void* data)
{
  if (id && data)
  {
    BaseType_t higherPriorityTaskWoken;
    BaseType_t status = xQueueSendToBackFromISR(*id, data, &higherPriorityTaskWoken);
    portYIELD_FROM_ISR(higherPriorityTaskWoken);
    return (status == pdPASS);
  }
  return false;
}

bool etcpal_queue_receive(etcpal_queue_t* id, void* data)
{
  if (id && data)
  {
    BaseType_t status = xQueueReceive(*id, data, portMAX_DELAY);
    return (status == pdPASS);
  }
  return false;
}

bool etcpal_queue_timed_receive(etcpal_queue_t* id, void* data, int timeout_ms)
{
  if (id && data)
  {
    BaseType_t status = xQueueReceive(*id, data, ETCPAL_TIMEOUT_TO_FREERTOS(timeout_ms));
    return (status == pdPASS);
  }
  return false;
}

bool etcpal_queue_receive_from_isr(etcpal_queue_t* id, void* data)
{
  BaseType_t higherPrioTaskWoken = pdFALSE;
  if (id && data)
  {
    BaseType_t status = xQueueReceiveFromISR(*id, data, &higherPrioTaskWoken);
    // if 'higherPrioTaskWoken' is pdTRUE, it indicates to
    // portYIELD_FROM_ISR() that the scheduler should run to allow
    // the new thread to execute when the interrupt handler completes:
    portYIELD_FROM_ISR(higherPrioTaskWoken);
    return (status == pdPASS);
  }
  return false;
}

bool etcpal_queue_reset(etcpal_queue_t* id)
{
  if (id)
  {
    return xQueueReset(*id);
  }
  return false;
}

bool etcpal_queue_is_empty(const etcpal_queue_t* id)
{
  if (id)
  {
    return (uxQueueMessagesWaiting(*id) == 0);
  }
  return false;
}

bool etcpal_queue_is_empty_from_isr(const etcpal_queue_t* id)
{
  if (id)
  {
    return xQueueIsQueueEmptyFromISR(*id);
  }
  return false;
}

bool etcpal_queue_is_full(const etcpal_queue_t* id)
{
  if (id)
  {
    return (uxQueueSpacesAvailable(*id) == 0);
  }
  return false;
}

bool etcpal_queue_is_full_from_isr(const etcpal_queue_t* id)
{
  if (id)
  {
    return xQueueIsQueueFullFromISR(*id);
  }
  return false;
}

size_t etcpal_queue_slots_used(const etcpal_queue_t* id)
{
  if (id)
  {
    return uxQueueMessagesWaiting(*id);
  }
  return 0;
}

size_t etcpal_queue_slots_used_from_isr(const etcpal_queue_t* id)
{
  if (id)
  {
    return uxQueueMessagesWaitingFromISR(*id);
  }
  return 0;
}

size_t etcpal_queue_slots_available(const etcpal_queue_t* id)
{
  if (id)
  {
    return uxQueueSpacesAvailable(*id);
  }
  return 0;
}
