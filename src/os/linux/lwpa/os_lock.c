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
