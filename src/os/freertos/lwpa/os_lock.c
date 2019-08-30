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

#include "lwpa/lock.h"
#include <task.h>

/*********************** Private function prototypes *************************/

static void reader_atomic_increment(lwpa_rwlock_t* id);
static void reader_atomic_decrement(lwpa_rwlock_t* id);

/*************************** Function definitions ****************************/

bool lwpa_mutex_create(lwpa_mutex_t* id)
{
  return id ? ((*id = (lwpa_mutex_t)xSemaphoreCreateMutex()) != NULL) : false;
}

bool lwpa_mutex_take(lwpa_mutex_t* id)
{
  if (id && *id)
  {
    return (pdTRUE == xSemaphoreTake((SemaphoreHandle_t)*id, portMAX_DELAY));
  }
  return false;
}

bool lwpa_mutex_try_take(lwpa_mutex_t* id)
{
  if (id && *id)
  {
    return (pdTRUE == xSemaphoreTake((SemaphoreHandle_t)*id, 0));
  }
  return false;
}

void lwpa_mutex_give(lwpa_mutex_t* id)
{
  if (id && *id)
    xSemaphoreGive((SemaphoreHandle_t)*id);
}

void lwpa_mutex_destroy(lwpa_mutex_t* id)
{
  if (id && *id)
  {
    vSemaphoreDelete((SemaphoreHandle_t)*id);
    *id = (lwpa_mutex_t)NULL;
  }
}

bool lwpa_signal_create(lwpa_signal_t* id)
{
  return id ? ((*id = (lwpa_signal_t)xSemaphoreCreateBinary()) != NULL) : false;
}

bool lwpa_signal_wait(lwpa_signal_t* id)
{
  return (id && *id) ? (pdTRUE == xSemaphoreTake((SemaphoreHandle_t)*id, portMAX_DELAY)) : false;
}

bool lwpa_signal_poll(lwpa_signal_t* id)
{
  return (id && *id) ? (pdTRUE == xSemaphoreTake((SemaphoreHandle_t)*id, 0)) : false;
}

void lwpa_signal_post(lwpa_signal_t* id)
{
  if (id && *id)
    xSemaphoreGive((SemaphoreHandle_t)*id);
}

void lwpa_signal_destroy(lwpa_signal_t* id)
{
  if (id && *id)
  {
    vSemaphoreDelete((SemaphoreHandle_t)*id);
    *id = (lwpa_signal_t)NULL;
  }
}

bool lwpa_rwlock_create(lwpa_rwlock_t* id)
{
  if (id && (NULL != (id->sem = xSemaphoreCreateMutex())))
  {
    id->reader_count = 0;
    id->valid = true;
    return true;
  }
  return false;
}

bool lwpa_rwlock_readlock(lwpa_rwlock_t* id)
{
  if (!id || !id->valid)
    return false;

  if (pdTRUE == xSemaphoreTake(id->sem, portMAX_DELAY))
  {
    // Add one to the reader count.
    reader_atomic_increment(id);
    // Allow other readers to access.
    xSemaphoreGive(id->sem);
    return true;
  }
  return false;
}

bool lwpa_rwlock_try_readlock(lwpa_rwlock_t* id)
{
  if (!id || !id->valid)
    return false;

  // Poll the semaphore
  if (pdTRUE == xSemaphoreTake(id->sem, 0))
  {
    // Add one to the reader count
    reader_atomic_increment(id);
    // Allow other readers to access
    xSemaphoreGive(id->sem);
    return true;
  }
  return false;
}

void lwpa_rwlock_readunlock(lwpa_rwlock_t* id)
{
  if (id && id->valid)
    reader_atomic_decrement(id);
}

bool lwpa_rwlock_writelock(lwpa_rwlock_t* id)
{
  if (!id || !id->valid)
    return false;

  if (pdTRUE == xSemaphoreTake(id->sem, portMAX_DELAY))
  {
    // Wait until there are no readers, keeping the lock so that no new readers can get in.
    while (id->reader_count > 0)
    {
      vTaskDelay(1);  // Wait one tick at a time
    }
    // Hold on to the lock until writeunlock() is called
    return true;
  }
  return false;
}

bool lwpa_rwlock_try_writelock(lwpa_rwlock_t* id)
{
  if (!id || !id->valid)
    return false;

  if (pdTRUE == xSemaphoreTake(id->sem, 0))
  {
    // Just check once if there are still readers
    if (id->reader_count > 0)
    {
      // Readers are present, give up the lock and return false below.
      xSemaphoreGive(id->sem);
    }
    else
    {
      // Return, holding the lock
      return true;
    }
  }
  return false;
}

void lwpa_rwlock_writeunlock(lwpa_rwlock_t* id)
{
  if (id && id->valid)
    xSemaphoreGive(id->sem);
}

void lwpa_rwlock_destroy(lwpa_rwlock_t* id)
{
  if (id && id->valid)
  {
    vSemaphoreDelete(id->sem);
    id->sem = NULL;
    id->valid = false;
  }
}

void reader_atomic_increment(lwpa_rwlock_t* id)
{
  portENTER_CRITICAL();
  ++id->reader_count;
  portEXIT_CRITICAL();
}

void reader_atomic_decrement(lwpa_rwlock_t* id)
{
  portENTER_CRITICAL();
  --id->reader_count;
  portEXIT_CRITICAL();
}
