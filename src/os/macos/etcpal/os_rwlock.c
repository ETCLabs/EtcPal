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
#include "etcpal/private/common.h"
#include <unistd.h>

/**************************** Private constants ******************************/

#define MAX_READERS 20000

/*********************** Private function prototypes *************************/

static bool reader_atomic_increment(etcpal_rwlock_t* id);
static void reader_atomic_decrement(etcpal_rwlock_t* id);

/*************************** Function definitions ****************************/

bool etcpal_rwlock_create(etcpal_rwlock_t* id)
{
  if (id)
  {
    if (0 == pthread_mutex_init(&id->write_lock, NULL))
    {
      if (0 == pthread_mutex_init(&id->read_lock, NULL))
      {
        if (0 == pthread_mutex_init(&id->readcount_mutex, NULL))
        {
          id->valid        = true;
          id->reader_count = 0;
          return true;
        }
        pthread_mutex_destroy(&id->read_lock);
      }
      pthread_mutex_destroy(&id->write_lock);
    }
  }
  return false;
}

bool etcpal_rwlock_readlock(etcpal_rwlock_t* id)
{
  bool res = false;
  if (id && id->valid)
  {
    // Yield to write lock, it takes precedence
    if (0 == pthread_mutex_lock(&id->write_lock))
    {
      res = reader_atomic_increment(id);
      pthread_mutex_unlock(&id->write_lock);
    }
  }
  return res;
}

bool etcpal_rwlock_try_readlock(etcpal_rwlock_t* id)
{
  bool res = false;
  if (id && id->valid)
  {
    // Yield to write lock, it takes precedence
    if (0 == pthread_mutex_trylock(&id->write_lock))
    {
      res = reader_atomic_increment(id);
      pthread_mutex_unlock(&id->write_lock);
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
    // Yield to other write locks, block additional readers
    if (0 == pthread_mutex_lock(&id->write_lock))
    {
      // Wait until reader count returns to 0
      if (0 == pthread_mutex_lock(&id->read_lock))
      {
        pthread_mutex_unlock(&id->read_lock);

        // Hold on to the write lock until writeunlock() is called
        return true;
      }

      // Error case, unlock & return false below
      pthread_mutex_unlock(&id->write_lock);
    }
  }
  return false;
}

bool etcpal_rwlock_try_writelock(etcpal_rwlock_t* id)
{
  if (id && id->valid)
  {
    // Yield to other write locks, block additional readers
    if (0 == pthread_mutex_trylock(&id->write_lock))
    {
      // If read_lock can be taken successfully, then there are no readers
      if (0 == pthread_mutex_trylock(&id->read_lock))
      {
        pthread_mutex_unlock(&id->read_lock);

        // Hold on to the write lock until writeunlock() is called
        return true;
      }

      // Otherwise there are still readers, so release the write lock & return false below
      pthread_mutex_unlock(&id->write_lock);
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
    pthread_mutex_unlock(&id->write_lock);
}

void etcpal_rwlock_destroy(etcpal_rwlock_t* id)
{
  if (id && id->valid)
  {
    pthread_mutex_destroy(&id->write_lock);
    pthread_mutex_destroy(&id->read_lock);
    pthread_mutex_destroy(&id->readcount_mutex);
    id->valid = false;
  }
}

// TODO investigate C11 atomics for this
bool reader_atomic_increment(etcpal_rwlock_t* id)
{
  if (!ETCPAL_ASSERT_VERIFY(id))
    return false;

  bool res = false;
  if (0 == pthread_mutex_lock(&id->readcount_mutex))
  {
    if (id->reader_count < MAX_READERS)
    {
      // Only lock read_lock when reader_count increments to 1
      if ((id->reader_count > 0) || (0 == pthread_mutex_lock(&id->read_lock)))
      {
        ++id->reader_count;
        res = true;
      }
    }
    pthread_mutex_unlock(&id->readcount_mutex);
  }

  return res;
}

void reader_atomic_decrement(etcpal_rwlock_t* id)
{
  if (!ETCPAL_ASSERT_VERIFY(id))
    return;

  if (0 == pthread_mutex_lock(&id->readcount_mutex))
  {
    if (id->reader_count > 0)
    {
      --id->reader_count;

      // Only unlock read_lock when reader_count decrements to 0
      if (id->reader_count == 0)
        pthread_mutex_unlock(&id->read_lock);
    }

    pthread_mutex_unlock(&id->readcount_mutex);
  }
}
