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

#include "etcpal/rwlock.h"
#include <task.h>
#include "freertos_timed_wait.h"

/*********************** Private function prototypes *************************/

static void reader_atomic_increment(etcpal_rwlock_t* id);
static void reader_atomic_decrement(etcpal_rwlock_t* id);

/*************************** Function definitions ****************************/

bool etcpal_rwlock_create(etcpal_rwlock_t* id)
{
  if (id && (NULL != (id->sem = xSemaphoreCreateMutex())))
  {
    id->reader_count = 0;
    id->valid        = true;
    return true;
  }
  return false;
}

bool etcpal_rwlock_readlock(etcpal_rwlock_t* id)
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

bool etcpal_rwlock_try_readlock(etcpal_rwlock_t* id)
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

bool etcpal_rwlock_timed_readlock(etcpal_rwlock_t* id, int timeout_ms)
{
  if (!id || !id->valid)
    return false;

  if (pdTRUE == xSemaphoreTake(id->sem, ETCPAL_TIMEOUT_TO_FREERTOS(timeout_ms)))
  {
    // Add one to the reader count
    reader_atomic_increment(id);
    // Allow other readers to access
    xSemaphoreGive(id->sem);
    return true;
  }
  return false;
}

void etcpal_rwlock_readunlock(etcpal_rwlock_t* id)
{
  if (id && id->valid)
    reader_atomic_decrement(id);
}

bool etcpal_rwlock_writelock(etcpal_rwlock_t* id)
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

bool etcpal_rwlock_try_writelock(etcpal_rwlock_t* id)
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

bool etcpal_rwlock_timed_writelock(etcpal_rwlock_t* id, int timeout_ms)
{
  if (timeout_ms < 0)
  {
    return etcpal_rwlock_writelock(id);
  }
  else if (id && id->valid)
  {
    TickType_t start_time = xTaskGetTickCount();

    if (pdTRUE == xSemaphoreTake(id->sem, pdMS_TO_TICKS(timeout_ms)))
    {
      // Wait until there are no readers, keeping the lock so that no new readers can get in.
      while (id->reader_count > 0 && xTaskGetTickCount() - start_time < pdMS_TO_TICKS(timeout_ms))
      {
        vTaskDelay(1);  // Wait one tick at a time
      }
      if (id->reader_count == 0)
      {
        // Hold on to the lock until writeunlock() is called
        return true;
      }
      else
      {
        xSemaphoreGive(id->sem);
      }
    }
  }
  return false;
}

void etcpal_rwlock_writeunlock(etcpal_rwlock_t* id)
{
  if (id && id->valid)
    xSemaphoreGive(id->sem);
}

void etcpal_rwlock_destroy(etcpal_rwlock_t* id)
{
  if (id && id->valid)
  {
    vSemaphoreDelete(id->sem);
    id->sem   = NULL;
    id->valid = false;
  }
}

void reader_atomic_increment(etcpal_rwlock_t* id)
{
  portENTER_CRITICAL();
  ++id->reader_count;
  portEXIT_CRITICAL();
}

void reader_atomic_decrement(etcpal_rwlock_t* id)
{
  portENTER_CRITICAL();
  --id->reader_count;
  portEXIT_CRITICAL();
}
