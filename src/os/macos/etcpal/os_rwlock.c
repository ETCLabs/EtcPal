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

#include <pthread.h>
#include <time.h>

#include <etcpal/thread.h>

#include "etcpal/rwlock.h"

static bool timed_out(struct timespec* timeout_spec)
{
  struct timespec curr_time;
  clock_gettime(CLOCK_REALTIME, &curr_time);

  return (curr_time.tv_sec > timeout_spec->tv_sec) ||
         ((curr_time.tv_sec == timeout_spec->tv_sec) && (curr_time.tv_nsec >= timeout_spec->tv_nsec));
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

  const int             timeout_sec = timeout_ms / 1000;
  const struct timespec time        = {timeout_sec, 1000000 * (timeout_ms - (timeout_sec * 1000))};

  struct timespec timeout_spec;
  clock_gettime(CLOCK_REALTIME, &timeout_spec);
  timeout_spec.tv_sec += time.tv_sec;
  timeout_spec.tv_nsec += time.tv_nsec;
  if (timeout_spec.tv_nsec >= 1000000000)
  {
    timeout_spec.tv_sec += 1;
    timeout_spec.tv_nsec -= 1000000000;
  }

  while (!timed_out(&timeout_spec))
  {
    if (etcpal_rwlock_try_readlock(id))
    {
      return true;
    }
    if (kEtcPalErrOk != etcpal_thread_sleep(1))
    {
      break;
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

  const int             timeout_sec = timeout_ms / 1000;
  const struct timespec time        = {timeout_sec, 1000000 * (timeout_ms - (timeout_sec * 1000))};

  struct timespec timeout_spec;
  clock_gettime(CLOCK_REALTIME, &timeout_spec);
  timeout_spec.tv_sec += time.tv_sec;
  timeout_spec.tv_nsec += time.tv_nsec;
  if (timeout_spec.tv_nsec >= 1000000000)
  {
    timeout_spec.tv_sec += 1;
    timeout_spec.tv_nsec -= 1000000000;
  }

  while (!timed_out(&timeout_spec))
  {
    if (etcpal_rwlock_try_writelock(id))
    {
      return true;
    }
    if (kEtcPalErrOk != etcpal_thread_sleep(1))
    {
      break;
    }
  }

  return false;
}
