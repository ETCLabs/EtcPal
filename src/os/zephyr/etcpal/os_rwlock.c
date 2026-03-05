/******************************************************************************
 * Copyright 2026 ETC Inc.
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

/******************************************************************************
 * Copyright (c) 2018 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 ******************************************************************************
 * Implementation taken from Zephyr RTOS and modified by ETC for use in EtcPal.
 * zephyr/lib/posix/options/rwlock.c
 ******************************************************************************/

#include "etcpal/rwlock.h"
#include "etcpal/etcpal_zephyr_common.h"

bool etcpal_rwlock_create(etcpal_rwlock_t* id)
{
  if (!id)
  {
    return false;
  }

  k_sem_init(&id->write_sem, 1, 1);
  k_sem_init(&id->reader_active, 1, 1);
  atomic_set(&id->reader_count, 0);
  id->initialized = true;

  return true;
}

bool etcpal_rwlock_destroy(etcpal_rwlock_t* id)
{
  if (!id || !id->initialized)
  {
    return false;
  }

  // Nothing to actually destroy. Just set initialized to false.
  id->initialized = false;
  return true;
}

bool etcpal_rwlock_timed_readlock(etcpal_rwlock_t* id, int timeout_ms)
{
  if (!id || !id->initialized)
  {
    return false;
  }

  // Wait for release of write lock
  if (k_sem_take(&id->write_sem, ms_to_zephyr_timeout(timeout_ms)) != 0)
  {
    return false;
  }

  if (atomic_inc(&id->reader_count) == 0)
  {
    (void)k_sem_take(&id->reader_active, K_NO_WAIT);
  }
  (void)k_sem_give(&id->write_sem);

  return true;
}

bool etcpal_rwlock_readunlock(etcpal_rwlock_t* id)
{
  if (!id || !id->initialized)
  {
    return false;
  }

  if (atomic_dec(&id->reader_count) == 1)
  {
    k_sem_give(&id->reader_active);
  }
  return true;
}

bool etcpal_rwlock_timed_writelock(etcpal_rwlock_t* id, int timeout_ms)
{
  if (!id || !id->initialized)
  {
    return false;
  }

  int64_t start_time = k_uptime_get();

  // Wait for release of write lock
  if (k_sem_take(&id->write_sem, ms_to_zephyr_timeout(timeout_ms)) != 0)
  {
    return false;
  }

  // Update timeout for remaining time
  if (timeout_ms != ETCPAL_WAIT_FOREVER)
  {
    int64_t elapsed_time = k_uptime_get() - start_time;
    timeout_ms           = timeout_ms <= elapsed_time ? 0 : timeout_ms - elapsed_time;
  }

  // Wait for all readers to be done
  if (k_sem_take(&id->reader_active, ms_to_zephyr_timeout(timeout_ms)) != 0)
  {
    (void)k_sem_give(&id->write_sem);
    return false;
  }

  return true;
}

bool etcpal_rwlock_writeunlock(etcpal_rwlock_t* id)
{
  if (!id || !id->initialized)
  {
    return false;
  }

  (void)k_sem_give(&id->reader_active);
  (void)k_sem_give(&id->write_sem);
  return true;
}
