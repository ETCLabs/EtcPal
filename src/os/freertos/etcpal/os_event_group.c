/******************************************************************************
 * Copyright 2021 ETC Inc.
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
#include "freertos_timed_wait.h"

bool etcpal_event_group_create(etcpal_event_group_t* id)
{
  if (id)
  {
    return ((*id = xEventGroupCreate()) != NULL);
  }
  return false;
}

etcpal_event_bits_t etcpal_event_group_wait(etcpal_event_group_t* id, etcpal_event_bits_t bits, int flags)
{
  if (id && *id)
  {
    return xEventGroupWaitBits(*id, bits, (flags & ETCPAL_EVENT_GROUP_AUTO_CLEAR) ? pdTRUE : pdFALSE,
                               (flags & ETCPAL_EVENT_GROUP_WAIT_FOR_ALL) ? pdTRUE : pdFALSE, portMAX_DELAY);
  }
  return 0;
}

etcpal_event_bits_t etcpal_event_group_timed_wait(etcpal_event_group_t* id,
                                                  etcpal_event_bits_t   bits,
                                                  int                   flags,
                                                  int                   timeout_ms)
{
  if (id && *id)
  {
    return xEventGroupWaitBits(*id, bits, (flags & ETCPAL_EVENT_GROUP_AUTO_CLEAR) ? pdTRUE : pdFALSE,
                               (flags & ETCPAL_EVENT_GROUP_WAIT_FOR_ALL) ? pdTRUE : pdFALSE,
                               ETCPAL_TIMEOUT_TO_FREERTOS(timeout_ms));
  }
  return 0;
}

void etcpal_event_group_set_bits(etcpal_event_group_t* id, etcpal_event_bits_t bits_to_set)
{
  if (id && *id)
    xEventGroupSetBits(*id, bits_to_set);
}

etcpal_event_bits_t etcpal_event_group_get_bits(etcpal_event_group_t* id)
{
  if (id && *id)
    return xEventGroupGetBits(*id);
  return 0;
}

void etcpal_event_group_clear_bits(etcpal_event_group_t* id, etcpal_event_bits_t bits_to_clear)
{
  if (id && *id)
    xEventGroupClearBits(*id, bits_to_clear);
}

void etcpal_event_group_destroy(etcpal_event_group_t* id)
{
  if (id && *id)
  {
    vEventGroupDelete(*id);
    *id = NULL;
  }
}

void etcpal_event_group_set_bits_from_isr(etcpal_event_group_t* id, etcpal_event_bits_t bits_to_set)
{
  if (id && *id)
  {
    BaseType_t higherPriorityTaskWoken;
    if (xEventGroupSetBitsFromISR(*id, bits_to_set, &higherPriorityTaskWoken) == pdPASS)
      portYIELD_FROM_ISR(higherPriorityTaskWoken);
  }
}

etcpal_event_bits_t etcpal_event_group_get_bits_from_isr(etcpal_event_group_t* id)
{
  if (id && *id)
    return xEventGroupGetBitsFromISR(*id);
  return 0;
}

void etcpal_event_group_clear_bits_from_isr(etcpal_event_group_t* id, etcpal_event_bits_t bits_to_clear)
{
  if (id && *id)
    xEventGroupClearBitsFromISR(*id, bits_to_clear);
}
