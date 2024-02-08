/******************************************************************************
 * Copyright 2024 ETC Inc.
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

#include "etcpal/event_group.h"
#include "etcpal/private/common.h"
#include "etcpal/etcpal_zephyr_common.h"

/*********************** Private function prototypes *************************/

static bool check_and_clear_bits(etcpal_event_bits_t* bits, etcpal_event_bits_t bits_requested, int flags);

/*************************** Function definitions ****************************/

bool etcpal_event_group_create(etcpal_event_group_t* id)
{
  if (id)
  {
    if (0 == k_mutex_init(&id->mutex))
    {
      if (0 == k_condvar_init(&id->cond))
      {
        id->valid = true;
        id->bits  = 0;
        return true;
      }
    }
  }
  return false;
}

etcpal_event_bits_t etcpal_event_group_wait(etcpal_event_group_t* id, etcpal_event_bits_t bits, int flags)
{
  // TODO: Format ifs properly
  if (!id || !bits || !id->valid)
    return 0;

  etcpal_event_bits_t result = 0;
  if (id && id->valid)
  {
    if (0 == k_mutex_lock(&id->mutex, K_FOREVER))
    {
      result = id->bits;
      while (!check_and_clear_bits(&id->bits, bits, flags))
      {
        if (0 != k_condvar_wait(&id->cond, &id->mutex, K_FOREVER))
        {
          // On error, the mutex is not locked
          return false;
        }
        result = id->bits;
      }
      k_mutex_unlock(&id->mutex);
    }
  }
  return result;
}

etcpal_event_bits_t etcpal_event_group_timed_wait(etcpal_event_group_t* id,
                                                  etcpal_event_bits_t   bits,
                                                  int                   flags,
                                                  int                   timeout_ms)
{
  if (!id || !bits || !id->valid)
    return 0;

  etcpal_event_bits_t result = 0;
  if (id && id->valid)
  {
    if (0 == k_mutex_lock(&id->mutex, ms_to_zephyr_timeout(timeout_ms)))
    {
      result = id->bits;
      while (!check_and_clear_bits(&id->bits, bits, flags))
      {
        if (0 != k_condvar_wait(&id->cond, &id->mutex, ms_to_zephyr_timeout(timeout_ms)))
        {
          // On error, the mutex is not locked
          return false;
        }
        result = id->bits;
      }
      k_mutex_unlock(&id->mutex);
    }
  }
  return result;
}

void etcpal_event_group_set_bits(etcpal_event_group_t* id, etcpal_event_bits_t bits_to_set)
{
  if (!id || !bits_to_set || !id->valid)
    return;

  if (0 == k_mutex_lock(&id->mutex, K_FOREVER))
  {
    id->bits |= bits_to_set;
    k_condvar_broadcast(&id->cond);
    k_mutex_unlock(&id->mutex);
  }
}

etcpal_event_bits_t etcpal_event_group_get_bits(etcpal_event_group_t* id)
{
  if (!id || !id->valid)
    return 0;

  etcpal_event_bits_t result = 0;
  if (0 == k_mutex_lock(&id->mutex, K_FOREVER))
  {
    result = id->bits;
    k_mutex_unlock(&id->mutex);
  }
  return result;
}

void etcpal_event_group_clear_bits(etcpal_event_group_t* id, etcpal_event_bits_t bits_to_clear)
{
  if (!id || !bits_to_clear || !id->valid)
    return;

  if (0 == k_mutex_lock(&id->mutex, K_FOREVER))
  {
    id->bits &= (~bits_to_clear);
    k_mutex_unlock(&id->mutex);
  }
}

void etcpal_event_group_destroy(etcpal_event_group_t* id)
{
  if (id && id->valid)
  {
    id->valid = false;
  }
}

bool check_and_clear_bits(etcpal_event_bits_t* bits, etcpal_event_bits_t bits_requested, int flags)
{
  if (!ETCPAL_ASSERT_VERIFY(bits))
    return false;

  bool result = false;

  if (flags & ETCPAL_EVENT_GROUP_WAIT_FOR_ALL)
  {
    if ((*bits & bits_requested) == bits_requested)
      result = true;
  }
  else
  {
    if (*bits & bits_requested)
      result = true;
  }

  if (result && (flags & ETCPAL_EVENT_GROUP_AUTO_CLEAR))
  {
    *bits &= (~(*bits & bits_requested));
  }

  return result;
}
