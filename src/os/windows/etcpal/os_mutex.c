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

#include "etcpal/mutex.h"

bool etcpal_mutex_create(etcpal_mutex_t* id)
{
  if (id)
  {
    InitializeSRWLock(&id->lock);
    id->valid = true;
    return true;
  }
  return false;
}

bool etcpal_mutex_lock(etcpal_mutex_t* id)
{
  if (id && id->valid)
  {
    AcquireSRWLockExclusive(&id->lock);
    return true;
  }
  return false;
}

bool etcpal_mutex_try_lock(etcpal_mutex_t* id)
{
  if (id && id->valid)
  {
    return TryAcquireSRWLockExclusive(&id->lock);
  }
  return false;
}

bool etcpal_mutex_timed_lock(etcpal_mutex_t* id, int timeout_ms)
{
  if (timeout_ms == ETCPAL_WAIT_FOREVER)
  {
    return etcpal_mutex_lock(id);
  }
  if (timeout_ms == 0)
  {
    return etcpal_mutex_try_lock(id);
  }

  for (DWORD curr_time = GetTickCount(), timeout_time = curr_time + timeout_ms; curr_time < timeout_time;
       curr_time = GetTickCount())
  {
    if (etcpal_mutex_try_lock(id))
    {
      return true;
    }

    Sleep((DWORD)((timeout_time - curr_time + 1) / 2));
  }

  return false;
}

void etcpal_mutex_unlock(etcpal_mutex_t* id)
{
  if (id && id->valid)
  {
    ReleaseSRWLockExclusive(&id->lock);
  }
}

void etcpal_mutex_destroy(etcpal_mutex_t* id)
{
  if (id && id->valid)
  {
    // SRWLocks do not need to be destroyed.
    id->valid = false;
  }
}
