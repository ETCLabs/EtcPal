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

/**************************** Private constants ******************************/

#define MAX_READERS 20000

/*************************** Function definitions ****************************/

bool etcpal_mutex_create(etcpal_mutex_t* id)
{
  if (id)
  {
    InitializeCriticalSection(&id->cs);
    id->valid = true;
    return true;
  }
  return false;
}

bool etcpal_mutex_take(etcpal_mutex_t* id)
{
  if (id && id->valid)
  {
    EnterCriticalSection(&id->cs);
    return true;
  }
  return false;
}

bool etcpal_mutex_try_take(etcpal_mutex_t* id)
{
  if (id && id->valid)
  {
    return TryEnterCriticalSection(&id->cs);
  }
  return false;
}

bool etcpal_mutex_timed_take(etcpal_mutex_t* id, int timeout_ms)
{
  // TODO
  return false;
}

void etcpal_mutex_give(etcpal_mutex_t* id)
{
  if (id && id->valid)
  {
    LeaveCriticalSection(&id->cs);
  }
}

void etcpal_mutex_destroy(etcpal_mutex_t* id)
{
  if (id && id->valid)
  {
    DeleteCriticalSection(&id->cs);
    id->valid = false;
  }
}

bool etcpal_signal_create(etcpal_signal_t* id)
{
  if (id)
  {
    *id = CreateEvent(NULL, FALSE, FALSE, NULL);
    if (*id)
      return true;
  }
  return false;
}

bool etcpal_signal_wait(etcpal_signal_t* id)
{
  if (id)
    return (WAIT_OBJECT_0 == WaitForSingleObject(*id, INFINITE));
  return false;
}

bool etcpal_signal_try_wait(etcpal_signal_t* id)
{
  if (id)
    return (WAIT_OBJECT_0 == WaitForSingleObject(*id, 0));
  return false;
}

bool etcpal_signal_timed_wait(etcpal_signal_t* id, int timeout_ms)
{
  if (id)
  {
    DWORD wait_time = (timeout_ms < 0 ? INFINITE : timeout_ms);
    return (WAIT_OBJECT_0 == WaitForSingleObject(*id, wait_time));
  }
  return false;
}

void etcpal_signal_post(etcpal_signal_t* id)
{
  if (id)
    SetEvent(*id);
}

void etcpal_signal_destroy(etcpal_signal_t* id)
{
  if (id)
    CloseHandle(*id);
}

bool etcpal_rwlock_create(etcpal_rwlock_t* id)
{
  if (id)
  {
    InitializeCriticalSection(&id->cs);
    id->valid = true;
    id->reader_count = 0;
    return true;
  }
  return false;
}

bool etcpal_rwlock_readlock(etcpal_rwlock_t* id)
{
  bool res = false;
  if (id && id->valid)
  {
    EnterCriticalSection(&id->cs);
    if (id->reader_count < MAX_READERS)
    {
      InterlockedIncrement(&id->reader_count);  // Add one to the reader count
      res = true;
    }
    LeaveCriticalSection(&id->cs);
  }
  return res;
}

bool etcpal_rwlock_try_readlock(etcpal_rwlock_t* id)
{
  bool res = false;
  if (id && id->valid)
  {
    if (TryEnterCriticalSection(&id->cs))
    {
      if (id->reader_count < MAX_READERS)
      {
        InterlockedIncrement(&id->reader_count);
        res = true;
      }
      LeaveCriticalSection(&id->cs);
    }
  }
  return res;
}

bool etcpal_rwlock_timed_readlock(etcpal_rwlock_t* id, int timeout_ms)
{
  // TODO
  return false;
}

void etcpal_rwlock_readunlock(etcpal_rwlock_t* id)
{
  if (id && id->valid)
    InterlockedDecrement(&id->reader_count);
}

bool etcpal_rwlock_writelock(etcpal_rwlock_t* id)
{
  if (id && id->valid)
  {
    EnterCriticalSection(&id->cs);
    // Wait until there are no readers, keeping the lock so that no new readers can get in.
    while (id->reader_count > 0)
    {
      Sleep(1);
    }
    // Hold on to the lock until writeunlock() is called
    return true;
  }
  return false;
}

bool etcpal_rwlock_try_writelock(etcpal_rwlock_t* id)
{
  if (id && id->valid)
  {
    if (TryEnterCriticalSection(&id->cs))
    {
      // Just check once to see if there are still readers
      if (id->reader_count > 0)
      {
        // Readers are present, give up the lock and return false below
        LeaveCriticalSection(&id->cs);
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
  // TODO
  return false;
}

void etcpal_rwlock_writeunlock(etcpal_rwlock_t* id)
{
  if (id && id->valid)
    LeaveCriticalSection(&id->cs);
}

void etcpal_rwlock_destroy(etcpal_rwlock_t* id)
{
  if (id && id->valid)
  {
    DeleteCriticalSection(&id->cs);
    id->valid = false;
  }
}

bool etcpal_sem_create(etcpal_sem_t* id, unsigned int initial_count, unsigned int max_count)
{
  // TODO
  return false;
}

bool etcpal_sem_wait(etcpal_sem_t* id)
{
  // TODO
  return false;
}

bool etcpal_sem_try_wait(etcpal_sem_t* id)
{
  // TODO
  return false;
}

bool etcpal_sem_timed_wait(etcpal_sem_t* id, int timeout_ms)
{
  // TODO
  return false;
}

bool etcpal_sem_post(etcpal_sem_t* id)
{
  // TODO
  return false;
}

bool etcpal_sem_post_from_isr(etcpal_sem_t* id)
{
  // TODO
  return false;
}

void etcpal_sem_destroy(etcpal_sem_t* id)
{
  // TODO
}
