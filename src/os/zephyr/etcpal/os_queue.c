/******************************************************************************
 * Copyright 2024 ETC Inc.
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

bool etcpal_queue_create(etcpal_queue_t* id, size_t size, size_t item_size)
{
  id->buffer = malloc(size * item_size);
  if (id->buffer == NULL)
  {
    return false;
  }
  k_msgq_init(&id->queue, id->buffer, item_size, size);
  return true;
}

void etcpal_queue_destroy(etcpal_queue_t* id)
{
  k_msgq_purge(&id->queue);
  // k_msgq_cleanup frees the buffer allocated in the create call
  k_msgq_cleanup(&id->queue);
}

bool etcpal_queue_reset(etcpal_queue_t* id)
{
  k_msgq_purge(&id->queue);
  return true;
}

bool etcpal_queue_is_empty(const etcpal_queue_t* id)
{
  struct k_msgq_attrs attrs;
  k_msgq_get_attrs((struct k_msgq*)(&id->queue), &attrs);

  return attrs.used_msgs == 0;
}

bool etcpal_queue_is_full(const etcpal_queue_t* id)
{
  struct k_msgq_attrs attrs;
  k_msgq_get_attrs((struct k_msgq*)(&id->queue), &attrs);

  return attrs.used_msgs == attrs.max_msgs;
}

size_t etcpal_queue_slots_used(const etcpal_queue_t* id)
{
  struct k_msgq_attrs attrs;
  k_msgq_get_attrs((struct k_msgq*)(&id->queue), &attrs);

  return attrs.used_msgs;
}

size_t etcpal_queue_slots_available(const etcpal_queue_t* id)
{
  struct k_msgq_attrs attrs;
  k_msgq_get_attrs((struct k_msgq*)(&id->queue), &attrs);

  return attrs.max_msgs - attrs.used_msgs;
}
