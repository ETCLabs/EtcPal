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

  return 0 == pthread_mutex_timedlock(id, &timeout_spec);
}
