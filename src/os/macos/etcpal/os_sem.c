/******************************************************************************
 * Copyright 2021 ETC Inc.
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

#include "etcpal/sem.h"

bool etcpal_sem_create(etcpal_sem_t* id, unsigned int initial_count, unsigned int max_count)
{
  if (id)
  {
    id->sem = dispatch_semaphore_create(initial_count);
    id->valid = true;
    return true;
  }
  return false;
}

bool etcpal_sem_wait(etcpal_sem_t* id)
{
  if (id && id->valid)
    return !dispatch_semaphore_wait(id->sem, DISPATCH_TIME_FOREVER);
  return false;
}

bool etcpal_sem_try_wait(etcpal_sem_t* id)
{
  if (id && id->valid)
    return !dispatch_semaphore_wait(id->sem, DISPATCH_TIME_NOW);
  return false;
}

bool etcpal_sem_timed_wait(etcpal_sem_t* id, int timeout_ms)
{
  if (id && id->valid)
    return !dispatch_semaphore_wait(id->sem, dispatch_time(DISPATCH_TIME_NOW, timeout_ms * NSEC_PER_MSEC));
  return false;
}

bool etcpal_sem_post(etcpal_sem_t* id)
{
  if (id && id->valid)
  {
    dispatch_semaphore_signal(id->sem);
    return true;
  }
  return false;
}

void etcpal_sem_destroy(etcpal_sem_t* id)
{
  if (id && id->valid)
  {
    dispatch_release(id->sem);
    id->valid = false;
  }
}
