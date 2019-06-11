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
#include <unistd.h>

/**************************** Private constants ******************************/

#define MAX_READERS 20000

/*********************** Private function prototypes *************************/

static void reader_atomic_increment(lwpa_rwlock_t* id);
static void reader_atomic_decrement(lwpa_rwlock_t* id);

/*************************** Function definitions ****************************/

bool lwpa_signal_create(lwpa_signal_t* id)
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

bool lwpa_signal_wait(lwpa_signal_t* id)
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

bool lwpa_signal_poll(lwpa_signal_t* id)
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

void lwpa_signal_post(lwpa_signal_t* id)
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

void lwpa_signal_destroy(lwpa_signal_t* id)
{
  if (id && id->valid)
  {
    pthread_cond_destroy(&id->cond);
    pthread_mutex_destroy(&id->mutex);
    id->valid = false;
  }
}

bool lwpa_rwlock_create(lwpa_rwlock_t* id)
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

bool lwpa_rwlock_readlock(lwpa_rwlock_t* id)
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

bool lwpa_rwlock_try_readlock(lwpa_rwlock_t* id)
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

void lwpa_rwlock_readunlock(lwpa_rwlock_t* id)
{
  if (id && id->valid)
    reader_atomic_decrement(id);
}

bool lwpa_rwlock_writelock(lwpa_rwlock_t* id)
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

bool lwpa_rwlock_try_writelock(lwpa_rwlock_t* id)
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

void lwpa_rwlock_writeunlock(lwpa_rwlock_t* id)
{
  if (id && id->valid)
    pthread_mutex_unlock(&id->mutex);
}

void lwpa_rwlock_destroy(lwpa_rwlock_t* id)
{
  if (id && id->valid)
  {
    pthread_mutex_destroy(&id->mutex);
    pthread_mutex_destroy(&id->readcount_mutex);
    id->valid = false;
  }
}

// TODO investigate C11 atomics for this
void reader_atomic_increment(lwpa_rwlock_t* id)
{
  if (0 == pthread_mutex_lock(&id->readcount_mutex))
  {
    ++id->reader_count;
    pthread_mutex_unlock(&id->readcount_mutex);
  }
}

void reader_atomic_decrement(lwpa_rwlock_t* id)
{
  if (0 == pthread_mutex_lock(&id->readcount_mutex))
  {
    --id->reader_count;
    pthread_mutex_unlock(&id->readcount_mutex);
  }
}
