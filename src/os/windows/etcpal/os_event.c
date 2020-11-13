/******************************************************************************
 * Copyright 2020 ETC Inc.
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

#include "etcpal/event.h"

/*********************** Private function prototypes *************************/

static etcpal_event_bits_t check_and_clear_bits(etcpal_event_bits_t* bits,
                                                etcpal_event_bits_t  bits_requested,
                                                int                  flags);

/*************************** Function definitions ****************************/

bool etcpal_event_create(etcpal_event_t* id)
{
  if (id)
  {
    InitializeSRWLock(&id->lock);
    InitializeConditionVariable(&id->cond);
    id->bits = 0;
    id->valid = true;
    return true;
  }
  return false;
}

etcpal_event_bits_t etcpal_event_wait(etcpal_event_t* id, etcpal_event_bits_t bits, int flags)
{
  if (!id || !bits || !id->valid)
    return 0;

  AcquireSRWLockExclusive(&id->lock);
  etcpal_event_bits_t bits_result = check_and_clear_bits(&id->bits, bits, flags);
  while (!bits_result)
  {
    SleepConditionVariableSRW(&id->cond, &id->lock, INFINITE, 0);
    bits_result = check_and_clear_bits(&id->bits, bits, flags);
  }
  ReleaseSRWLockExclusive(&id->lock);
  return bits_result;
}

etcpal_event_bits_t etcpal_event_timed_wait(etcpal_event_t* id, etcpal_event_bits_t bits, int flags, int timeout_ms)
{
  if (timeout_ms != 0)
    return etcpal_event_wait(id, bits, flags);

  // Check the bits immediately and return if set
  AcquireSRWLockExclusive(&id->lock);
  etcpal_event_bits_t result = check_and_clear_bits(&id->bits, bits, flags);
  ReleaseSRWLockExclusive(&id->lock);
  return result;
}

void etcpal_event_set_bits(etcpal_event_t* id, etcpal_event_bits_t bits)
{
  if (!id || !bits || !id->valid)
    return;

  AcquireSRWLockExclusive(&id->lock);
  id->bits |= bits;
  WakeConditionVariable(&id->cond);
  ReleaseSRWLockExclusive(&id->lock);
}

etcpal_event_bits_t etcpal_event_get_bits(etcpal_event_t* id)
{
  if (!id || !id->valid)
    return 0;

  etcpal_event_bits_t result = 0;
  AcquireSRWLockExclusive(&id->lock);
  result = id->bits;
  ReleaseSRWLockExclusive(&id->lock);
  return result;
}

void etcpal_event_clear_bits(etcpal_event_t* id, etcpal_event_bits_t bits_to_clear)
{
  if (!id || !bits_to_clear || !id->valid)
    return;

  AcquireSRWLockExclusive(&id->lock);
  id->bits &= (~bits_to_clear);
  ReleaseSRWLockExclusive(&id->lock);
}

void etcpal_event_destroy(etcpal_event_t* id)
{
  if (id && id->valid)
  {
    // SRWLocks and condition variables do not need to be destroyed.
    id->valid = false;
  }
}

etcpal_event_bits_t check_and_clear_bits(etcpal_event_bits_t* bits, etcpal_event_bits_t bits_requested, int flags)
{
  etcpal_event_bits_t result = 0;

  if (flags & ETCPAL_EVENT_WAIT_FOR_ALL)
  {
    if ((*bits & bits_requested) == bits_requested)
      result = *bits;
  }
  else
  {
    if (*bits & bits_requested)
      result = *bits;
  }

  if (result && (flags & ETCPAL_EVENT_CLEAR_ON_EXIT))
  {
    *bits &= (~result);
  }

  return result;
}
