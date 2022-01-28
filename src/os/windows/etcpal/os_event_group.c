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

#include "etcpal/event_group.h"

/*********************** Private function prototypes *************************/

static bool check_and_clear_bits(etcpal_event_bits_t* bits, etcpal_event_bits_t bits_requested, int flags);

/*************************** Function definitions ****************************/

bool etcpal_event_group_create(etcpal_event_group_t* id)
{
  if (id)
  {
    InitializeSRWLock(&id->lock);
    InitializeConditionVariable(&id->cond);
    id->bits  = 0;
    id->valid = true;
    return true;
  }
  return false;
}

etcpal_event_bits_t etcpal_event_group_wait(etcpal_event_group_t* id, etcpal_event_bits_t bits, int flags)
{
  if (!id || !bits || !id->valid)
    return 0;

  AcquireSRWLockExclusive(&id->lock);
  etcpal_event_bits_t result = id->bits;
  while (!check_and_clear_bits(&id->bits, bits, flags))
  {
    SleepConditionVariableSRW(&id->cond, &id->lock, INFINITE, 0);
    result = id->bits;
  }
  ReleaseSRWLockExclusive(&id->lock);
  return result;
}

etcpal_event_bits_t etcpal_event_group_timed_wait(etcpal_event_group_t* id,
                                                  etcpal_event_bits_t   bits,
                                                  int                   flags,
                                                  int                   timeout_ms)
{
  if (!id || !bits || !id->valid)
    return 0;
  if (timeout_ms != 0)
    return etcpal_event_group_wait(id, bits, flags);

  // Just check once and return the state of the bits
  AcquireSRWLockExclusive(&id->lock);
  etcpal_event_bits_t result = id->bits;
  check_and_clear_bits(&id->bits, bits, flags);
  ReleaseSRWLockExclusive(&id->lock);
  return result;
}

void etcpal_event_group_set_bits(etcpal_event_group_t* id, etcpal_event_bits_t bits_to_set)
{
  if (!id || !bits_to_set || !id->valid)
    return;

  AcquireSRWLockExclusive(&id->lock);
  id->bits |= bits_to_set;
  WakeConditionVariable(&id->cond);
  ReleaseSRWLockExclusive(&id->lock);
}

etcpal_event_bits_t etcpal_event_group_get_bits(etcpal_event_group_t* id)
{
  if (!id || !id->valid)
    return 0;

  AcquireSRWLockExclusive(&id->lock);
  etcpal_event_bits_t result = id->bits;
  ReleaseSRWLockExclusive(&id->lock);
  return result;
}

void etcpal_event_group_clear_bits(etcpal_event_group_t* id, etcpal_event_bits_t bits_to_clear)
{
  if (!id || !bits_to_clear || !id->valid)
    return;

  AcquireSRWLockExclusive(&id->lock);
  id->bits &= (~bits_to_clear);
  ReleaseSRWLockExclusive(&id->lock);
}

void etcpal_event_group_destroy(etcpal_event_group_t* id)
{
  if (id && id->valid)
  {
    // SRWLocks and condition variables do not need to be destroyed.
    id->valid = false;
  }
}

bool check_and_clear_bits(etcpal_event_bits_t* bits, etcpal_event_bits_t bits_requested, int flags)
{
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
