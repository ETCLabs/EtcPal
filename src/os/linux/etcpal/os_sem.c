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

#include "etcpal/sem.h"

#include <time.h>

enum
{
  kMsPerSec = 1000,
  kNsPerMs  = 1000000
};

bool etcpal_sem_timed_wait(etcpal_sem_t* id, int timeout_ms)
{
  if (timeout_ms == ETCPAL_WAIT_FOREVER)
  {
    return etcpal_sem_wait(id);
  }

  struct timespec ts;
  if (clock_gettime(CLOCK_REALTIME, &ts) == -1)
  {
    return false;
  }
  const time_t seconds      = (time_t)timeout_ms / kMsPerSec;
  const time_t nano_seconds = ((time_t)timeout_ms - (seconds * kMsPerSec)) * kNsPerMs;
  ts.tv_sec += seconds;
  ts.tv_nsec += nano_seconds;

  return !sem_timedwait(id, &ts);
}
