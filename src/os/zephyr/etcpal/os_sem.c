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

#include "etcpal/sem.h"
#include "etcpal/etcpal_zephyr_common.h"
#include "etcpal/private/common.h"
#include <zephyr/kernel.h>

bool etcpal_sem_create(etcpal_sem_t* id, unsigned int initial_count, unsigned int max_count)
{
  if (!id)
  {
    return false;
  }

  int err = k_sem_init(id, initial_count, max_count);
  return err == 0;
}

bool etcpal_sem_timed_wait(etcpal_sem_t* id, int timeout_ms)
{
  if (!id)
  {
    return false;
  }

  int err = k_sem_take(id, ms_to_zephyr_timeout(timeout_ms));
  return err == 0;
}

bool etcpal_sem_post(etcpal_sem_t* id)
{
  if (!id)
  {
    return false;
  }

  if (id->count >= id->limit)
  {
    return false;
  }

  k_sem_give(id);
  return true;
}

void etcpal_sem_destroy(etcpal_sem_t* id)
{
  ETCPAL_UNUSED_ARG(id);
  // Not needed
}
