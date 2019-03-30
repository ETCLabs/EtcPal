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

#include "lwpa_lock.h"
#include "lwpa_int.h"

static _mqx_uint msec_per_tick = 1;
static bool msec_per_tick_initted = false;

static _mqx_uint convert_ms_to_ticks(int ms)
{
  _mqx_uint ticks;
  if (ms == LWPA_WAIT_FOREVER)
    ticks = 0; /* 0 means infinite wait in MQX land */
  else
  {
    ticks = ms / msec_per_tick;
    /* Round up to the nearest tick */
    if (ms % msec_per_tick != 0 || ms == 0)
      ticks += 1;
  }
  return ticks;
}

static bool sem_create(LWSEM_STRUCT_PTR id, _mqx_int count)
{
  if (!msec_per_tick_initted)
  {
    msec_per_tick = _time_get_resolution();
    msec_per_tick_initted = true;
  }
  return (MQX_OK == _lwsem_create(id, count));
}

static bool sem_take(LWSEM_STRUCT_PTR id, int wait_ms)
{
  _mqx_uint ticks = convert_ms_to_ticks(wait_ms);
  return (MQX_OK == _lwsem_wait_ticks(id, ticks));
}

bool lwpa_mutex_create(lwpa_mutex_t *id)
{
  return sem_create((LWSEM_STRUCT_PTR)id, 1);
}

bool lwpa_mutex_take(lwpa_mutex_t *id, int wait_ms)
{
  return sem_take(id, wait_ms);
}

bool lwpa_signal_create(lwpa_signal_t *id)
{
  if (!msec_per_tick_initted)
  {
    msec_per_tick = _time_get_resolution();
    msec_per_tick_initted = true;
  }

  return (MQX_OK == _lwevent_create((LWEVENT_STRUCT_PTR)id, LWEVENT_AUTO_CLEAR));
}

bool lwpa_signal_wait(lwpa_signal_t *id, int wait_ms)
{
  _mqx_uint ticks = convert_ms_to_ticks(wait_ms);
  if (MQX_OK == _lwevent_wait_ticks((LWEVENT_STRUCT_PTR)id, 1u, true, ticks))
  {
    _lwevent_clear((LWEVENT_STRUCT_PTR)id, 1u);
    return true;
  }
  return false;
}

static void atomic_inc(unsigned int *count)
{
  _int_disable();
  (*count)++;
  _int_enable();
}

static void atomic_dec(unsigned int *count)
{
  _int_disable();
  (*count)--;
  _int_enable();
}

bool lwpa_rwlock_create(lwpa_rwlock_t *id)
{
  if (id && sem_create(&id->sem, 1))
  {
    id->reader_count = 0;
    return true;
  }
  return false;
}

bool lwpa_rwlock_readlock(lwpa_rwlock_t *id, int wait_ms)
{
  if (!id)
    return false;

  if (sem_take(&id->sem, wait_ms))
  {
    /* Add one to the reader count. */
    atomic_inc(&id->reader_count);
    /* Allow other readers to access. */
    _lwsem_post(&id->sem);
    return true;
  }
  return false;
}

void lwpa_rwlock_readunlock(lwpa_rwlock_t *id)
{
  if (id)
    atomic_dec(&id->reader_count);
}

bool lwpa_rwlock_writelock(lwpa_rwlock_t *id, int wait_ms)
{
  uint32_t initial_time;
  TIME_STRUCT tm;

  if (!id)
    return false;

  /* Start our timer here */
  _time_get_elapsed(&tm);
  initial_time = tm.SECONDS * 1000 + tm.MILLISECONDS;

  if (sem_take(&id->sem, wait_ms))
  {
    /* Wait until there are no readers, keeping the lock so that no new readers
     * can get in. */
    while (id->reader_count > 0)
    {
      _time_delay(1);
      if (wait_ms >= 0)
      {
        _time_get_elapsed(&tm);
        if ((uint32_t)(tm.SECONDS * 1000 + tm.MILLISECONDS) - initial_time > (uint32_t)wait_ms)
        {
          break;
        }
      }
    }
    if (id->reader_count > 0)
    {
      /* Timed out waiting for readers to leave. Bail. */
      _lwsem_post(&id->sem);
      return false;
    }
    /* Hold on to the lock until writeunlock() is called */
    return true;
  }
  return false;
}

void lwpa_rwlock_writeunlock(lwpa_rwlock_t *id)
{
  _lwsem_post(&id->sem);
}
