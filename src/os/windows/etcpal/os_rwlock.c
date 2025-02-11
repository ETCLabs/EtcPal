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

/**************************** Private constants ******************************/

#define MAX_READERS 20000

/*************************** Function definitions ****************************/

bool etcpal_rwlock_create(etcpal_rwlock_t* id)
{
  if (id)
  {
    InitializeSRWLock(&id->lock);
    id->valid        = true;
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
    AcquireSRWLockExclusive(&id->lock);
    if (id->reader_count < MAX_READERS)
    {
      InterlockedIncrement(&id->reader_count);  // Add one to the reader count
      res = true;
    }
    ReleaseSRWLockExclusive(&id->lock);
  }
  return res;
}

bool etcpal_rwlock_try_readlock(etcpal_rwlock_t* id)
{
  bool res = false;
  if (id && id->valid)
  {
    if (TryAcquireSRWLockExclusive(&id->lock))
    {
      if (id->reader_count < MAX_READERS)
      {
        InterlockedIncrement(&id->reader_count);
        res = true;
      }
      ReleaseSRWLockExclusive(&id->lock);
    }
  }
  return res;
}

bool etcpal_rwlock_timed_readlock(etcpal_rwlock_t* id, int timeout_ms)
{
  if (timeout_ms == ETCPAL_WAIT_FOREVER)
  {
    return etcpal_rwlock_readlock(id);
  }
  if (timeout_ms == 0)
  {
    return etcpal_rwlock_try_readlock(id);
  }

  for (DWORD curr_time = GetTickCount(), timeout_time = curr_time + timeout_ms; curr_time < timeout_time;
       curr_time = GetTickCount())
  {
    if (etcpal_rwlock_try_readlock(id))
    {
      return true;
    }

    Sleep((DWORD)((timeout_time - curr_time + 1) / 2));
  }

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
    AcquireSRWLockExclusive(&id->lock);
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
    if (TryAcquireSRWLockExclusive(&id->lock))
    {
      // Just check once to see if there are still readers
      if (id->reader_count > 0)
      {
        // Readers are present, give up the lock and return false below
        ReleaseSRWLockExclusive(&id->lock);
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
  if (timeout_ms == ETCPAL_WAIT_FOREVER)
  {
    return etcpal_rwlock_writelock(id);
  }
  if (timeout_ms == 0)
  {
    return etcpal_rwlock_try_writelock(id);
  }

  for (DWORD curr_time = GetTickCount(), timeout_time = curr_time + timeout_ms; curr_time < timeout_time;
       curr_time = GetTickCount())
  {
    if (etcpal_rwlock_try_writelock(id))
    {
      return true;
    }

    Sleep((DWORD)((timeout_time - curr_time + 1) / 2));
  }

  return false;
}

void etcpal_rwlock_writeunlock(etcpal_rwlock_t* id)
{
  if (id && id->valid)
    ReleaseSRWLockExclusive(&id->lock);
}

void etcpal_rwlock_destroy(etcpal_rwlock_t* id)
{
  if (id && id->valid)
  {
    // SRWLocks do not need to be destroyed.
    id->valid = false;
  }
}
