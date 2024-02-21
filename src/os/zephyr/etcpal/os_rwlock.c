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

#include "etcpal/rwlock.h"

#define MS_IN_S      1000
#define MS_TO_S(ms)  ((ms) / MS_IN_S)
#define MS_TO_NS(ms) ((ms)*1000000)

bool etcpal_rwlock_timed_readlock(etcpal_rwlock_t* id, int timeout_ms)
{
  struct timespec time = {
      .tv_sec  = MS_TO_S(timeout_ms),
      .tv_nsec = MS_TO_NS(timeout_ms % MS_IN_S),
  };
  return !pthread_rwlock_timedrdlock(id, &time);
}

bool etcpal_rwlock_timed_writelock(etcpal_rwlock_t* id, int timeout_ms)
{
  struct timespec time = {
      .tv_sec  = MS_TO_S(timeout_ms),
      .tv_nsec = MS_TO_NS(timeout_ms % MS_IN_S),
  };
  return !pthread_rwlock_timedwrlock(id, &time);
}
