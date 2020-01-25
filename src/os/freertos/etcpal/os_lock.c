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
 * This file is a part of EtcPal. For more information, go to:
 * https://github.com/ETCLabs/EtcPal
 ******************************************************************************/

#include "etcpal/lock.h"
#include <task.h>

/*********************** Private function prototypes *************************/

static void reader_atomic_increment(etcpal_rwlock_t* id);
static void reader_atomic_decrement(etcpal_rwlock_t* id);

/*************************** Function definitions ****************************/

bool etcpal_mutex_create(etcpal_mutex_t* id)
{
  if (id)
  {
    return ((*id = (etcpal_mutex_t)xSemaphoreCreateMutex()) != NULL);
  }
  return false;
}

bool etcpal_mutex_lock(etcpal_mutex_t* id)
{
  if (id && *id)
  {
    return (pdTRUE == xSemaphoreTake((SemaphoreHandle_t)*id, portMAX_DELAY));
  }
  return false;
}

bool etcpal_mutex_try_lock(etcpal_mutex_t* id)
{
  if (id && *id)
  {
    return (pdTRUE == xSemaphoreTake((SemaphoreHandle_t)*id, 0));
  }
  return false;
}

bool etcpal_mutex_timed_lock(etcpal_mutex_t* id, int timeout_ms)
{
  if (id && *id)
  {
    return (pdTRUE == xSemaphoreTake((SemaphoreHandle_t)*id, pdMS_TO_TICKS(timeout_ms)));
  }
  return false;
}

void etcpal_mutex_unlock(etcpal_mutex_t* id)
{
  if (id && *id)
    xSemaphoreGive((SemaphoreHandle_t)*id);
}

void etcpal_mutex_destroy(etcpal_mutex_t* id)
{
  if (id && *id)
  {
    vSemaphoreDelete((SemaphoreHandle_t)*id);
    *id = (etcpal_mutex_t)NULL;
  }
}

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
    return (pdTRUE == xSemaphoreTake((SemaphoreHandle_t)*id, pdMS_TO_TICKS(timeout_ms)));
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

bool etcpal_rwlock_create(etcpal_rwlock_t* id)
{
  if (id && (NULL != (id->sem = xSemaphoreCreateMutex())))
  {
    id->reader_count = 0;
    id->valid = true;
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

  if (pdTRUE == xSemaphoreTake(id->sem, pdMS_TO_TICKS(timeout_ms)))
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
    id->sem = NULL;
    id->valid = false;
  }
}

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
