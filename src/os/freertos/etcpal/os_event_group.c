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

#include "etcpal/event_group.h"

bool etcpal_event_group_create(etcpal_event_group_t* id, int flags)
{
  if (id)
  {
    id->flags = flags;
    id->handle = xEventGroupCreate();
    return (id->handle != NULL);
  }
  return false;
}

etcpal_event_bits_t etcpal_event_group_wait(etcpal_event_group_t* id, etcpal_event_bits_t bits, int flags)
{
  if (id && id->handle)
  {
    return xEventGroupWaitBits(id->handle, bits, (id->flags & ETCPAL_EVENT_AUTO_CLEAR) ? pdTRUE : pdFALSE,
                               (flags & ETCPAL_EVENT_WAIT_FOR_ALL) ? pdTRUE : pdFALSE, portMAX_DELAY);
  }
  return 0;
}

etcpal_event_bits_t etcpal_event_group_timed_wait(etcpal_event_group_t* id,
                                                  etcpal_event_bits_t   bits,
                                                  int                   flags,
                                                  int                   timeout_ms)
{
  if (id && id->handle)
  {
    return xEventGroupWaitBits(id->handle, bits, (id->flags & ETCPAL_EVENT_AUTO_CLEAR) ? pdTRUE : pdFALSE,
                               (flags & ETCPAL_EVENT_WAIT_FOR_ALL) ? pdTRUE : pdFALSE, pdMS_TO_TICKS(timeout_ms));
  }
  return 0;
}

void etcpal_event_group_set_bits(etcpal_event_group_t* id, etcpal_event_bits_t bits_to_set)
{
  if (id && id->handle)
    xEventGroupSetBits(id->handle, bits_to_set);
}

etcpal_event_bits_t etcpal_event_group_get_bits(etcpal_event_group_t* id)
{
  if (id && id->handle)
    return xEventGroupGetBits(id->handle);
  return 0;
}

void etcpal_event_group_clear_bits(etcpal_event_group_t* id, etcpal_event_bits_t bits_to_clear)
{
  if (id && id->handle)
    xEventGroupClearBits(id->handle, bits_to_clear);
}

void etcpal_event_group_destroy(etcpal_event_group_t* id)
{
  if (id && id->handle)
    vEventGroupDelete(id->handle);
}

void etcpal_event_group_set_bits_from_isr(etcpal_event_group_t* id, etcpal_event_bits_t bits_to_set)
{
  if (id && id->handle)
  {
    BaseType_t higherPriorityTaskWoken;
    if (xEventGroupSetBitsFromISR(id->handle, bits_to_set, &higherPriorityTaskWoken) == pdPASS)
      portYIELD_FROM_ISR(higherPriorityTaskWoken);
  }
}

etcpal_event_bits_t etcpal_event_group_get_bits_from_isr(etcpal_event_group_t* id)
{
  if (id && id->handle)
    return xEventGroupGetBitsFromISR(id->handle);
  return 0;
}

void etcpal_event_group_clear_bits_from_isr(etcpal_event_group_t* id, etcpal_event_bits_t bits_to_clear)
{
  if (id && id->handle)
    xEventGroupClearBitsFromISR(id->handle, bits_to_clear);
}
