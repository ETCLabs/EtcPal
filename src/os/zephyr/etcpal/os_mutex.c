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

#include "etcpal/mutex.h"
#include "etcpal/private/common.h"

bool etcpal_mutex_timed_lock(etcpal_mutex_t* id, int timeout_ms)
{
  if (!id)
  {
    return false;
  }

  int err = k_mutex_lock(id, ms_to_zephyr_timeout(timeout_ms));
  if (err)
  {
    return false;
  }

  /*
   * Zephyr does not support true mutexes, only recursive mutexes. In the case that a true mutex is locked twice from
   * the same thread, EtcPal expects the lock to deadlock until the timeout is complete. Since Zephyr's mutexes are
   * recursive, we need to spoof this behavior by sleeping the thread for the given timeout. We assume this
   * implementation to be okay since it is not guaranteed by all platforms that a lock can be released by a thread other
   * than the one holding it leaving no way for the lock to be released while the thread sleeps.
   *
   * *NOTE* If we hit this case from a etcpal_mutex_lock the thread will sleep forever.
   * */
  if (id->lock_count > 1)
  {
    k_mutex_unlock(id);
    k_sleep(ms_to_zephyr_timeout(timeout_ms));
    return false;
  }

  return true;
}

void etcpal_mutex_destroy(etcpal_mutex_t* id)
{
  ETCPAL_UNUSED_ARG(id);
  // Not needed
}
