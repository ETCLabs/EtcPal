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
#include <unistd.h>
#include <semaphore.h>

/**************************** Private constants ******************************/

#define MAX_READERS 20000

/*********************** Private function prototypes *************************/

static void reader_atomic_increment(etcpal_rwlock_t* id);
static void reader_atomic_decrement(etcpal_rwlock_t* id);

/*************************** Function definitions ****************************/

bool etcpal_mutex_timed_lock(etcpal_mutex_t* id, int timeout_ms)
{
  return (timeout_ms == 0 ? etcpal_mutex_try_lock(id) : etcpal_mutex_lock(id));
}

bool etcpal_signal_create(etcpal_signal_t* id)
{
  if (id)
  {
    if (0 == pthread_mutex_init(&id->mutex, NULL))
    {
      if (0 == pthread_cond_init(&id->cond, NULL))
      {
        id->valid = true;
        id->signaled = false;
        return true;
      }
      else
      {
        pthread_mutex_destroy(&id->mutex);
      }
    }
  }
  return false;
}

bool etcpal_signal_wait(etcpal_signal_t* id)
{
  if (id && id->valid)
  {
    if (0 == pthread_mutex_lock(&id->mutex))
    {
      while (!id->signaled)
      {
        if (0 != pthread_cond_wait(&id->cond, &id->mutex))
        {
          // On error, the mutex is not locked
        }
      }
      id->signaled = false;
      pthread_mutex_unlock(&id->mutex);
      return true;
    }
  }
  return false;
}

bool etcpal_signal_try_wait(etcpal_signal_t* id)
{
  bool res = false;
  if (id && id->valid)
  {
    if (0 == pthread_mutex_lock(&id->mutex))
    {
      if (id->signaled)
      {
        res = true;
        id->signaled = false;
      }
      pthread_mutex_unlock(&id->mutex);
    }
  }
  return res;
}

bool etcpal_signal_timed_wait(etcpal_signal_t* id, int timeout_ms)
{
  return (timeout_ms == 0 ? etcpal_signal_try_wait(id) : etcpal_signal_wait(id));
}

void etcpal_signal_post(etcpal_signal_t* id)
{
  if (id && id->valid)
  {
    if (0 == pthread_mutex_lock(&id->mutex))
    {
      id->signaled = true;
      pthread_cond_signal(&id->cond);
      pthread_mutex_unlock(&id->mutex);
    }
  }
}

void etcpal_signal_destroy(etcpal_signal_t* id)
{
  if (id && id->valid)
  {
    pthread_cond_destroy(&id->cond);
    pthread_mutex_destroy(&id->mutex);
    id->valid = false;
  }
}

bool etcpal_rwlock_create(etcpal_rwlock_t* id)
{
  if (id)
  {
    if (0 == pthread_mutex_init(&id->mutex, NULL))
    {
      if (0 == pthread_mutex_init(&id->readcount_mutex, NULL))
      {
        id->valid = true;
        id->reader_count = 0;
        return true;
      }
      else
      {
        pthread_mutex_destroy(&id->mutex);
      }
    }
  }
  return false;
}

bool etcpal_rwlock_readlock(etcpal_rwlock_t* id)
{
  bool res = false;
  if (id && id->valid)
  {
    if (0 == pthread_mutex_lock(&id->mutex))
    {
      if (id->reader_count < MAX_READERS)
      {
        reader_atomic_increment(id);
        res = true;
      }
      pthread_mutex_unlock(&id->mutex);
    }
  }
  return res;
}

bool etcpal_rwlock_try_readlock(etcpal_rwlock_t* id)
{
  bool res = false;
  if (id && id->valid)
  {
    if (0 == pthread_mutex_trylock(&id->mutex))
    {
      if (id->reader_count < MAX_READERS)
      {
        reader_atomic_increment(id);
        res = true;
      }
      pthread_mutex_unlock(&id->mutex);
    }
  }
  return res;
}

bool etcpal_rwlock_timed_readlock(etcpal_rwlock_t* id, int timeout_ms)
{
  return (timeout_ms == 0 ? etcpal_rwlock_try_readlock(id) : etcpal_rwlock_readlock(id));
}

void etcpal_rwlock_readunlock(etcpal_rwlock_t* id)
{
  if (id && id->valid)
    reader_atomic_decrement(id);
}

bool etcpal_rwlock_writelock(etcpal_rwlock_t* id)
{
  if (id && id->valid)
  {
    if (0 == pthread_mutex_lock(&id->mutex))
    {
      // Wait until there are no readers, keeping the lock so that no new readers can get in.
      while (id->reader_count > 0)
      {
        usleep(1000);
      }
      // Hold on to the lock until writeunlock() is called
      return true;
    }
  }
  return false;
}

bool etcpal_rwlock_try_writelock(etcpal_rwlock_t* id)
{
  if (id && id->valid)
  {
    if (0 == pthread_mutex_trylock(&id->mutex))
    {
      // Just check once to see if there are still readers
      if (id->reader_count > 0)
      {
        // Readers are present, give up the lock and return false below
        pthread_mutex_unlock(&id->mutex);
      }
      else
      {
        // Return, holding the lock
        return true;
      }
    }
  }
  return false;
}

bool etcpal_rwlock_timed_writelock(etcpal_rwlock_t* id, int timeout_ms)
{
  return (timeout_ms == 0 ? etcpal_rwlock_try_writelock(id) : etcpal_rwlock_writelock(id));
}

void etcpal_rwlock_writeunlock(etcpal_rwlock_t* id)
{
  if (id && id->valid)
    pthread_mutex_unlock(&id->mutex);
}

void etcpal_rwlock_destroy(etcpal_rwlock_t* id)
{
  if (id && id->valid)
  {
    pthread_mutex_destroy(&id->mutex);
    pthread_mutex_destroy(&id->readcount_mutex);
    id->valid = false;
  }
}

bool etcpal_sem_timed_wait(etcpal_sem_t* id, int timeout_ms)
{
  return (timeout_ms == 0 ? etcpal_sem_try_wait(id) : etcpal_sem_wait(id));
}

// TODO investigate C11 atomics for this
void reader_atomic_increment(etcpal_rwlock_t* id)
{
  if (0 == pthread_mutex_lock(&id->readcount_mutex))
  {
    ++id->reader_count;
    pthread_mutex_unlock(&id->readcount_mutex);
  }
}

void reader_atomic_decrement(etcpal_rwlock_t* id)
{
  if (0 == pthread_mutex_lock(&id->readcount_mutex))
  {
    --id->reader_count;
    pthread_mutex_unlock(&id->readcount_mutex);
  }
}
