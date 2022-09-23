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

#include "etcpal/signal.h"
#include <zephyr/kernel.h>

bool etcpal_signal_create(etcpal_signal_t* id)
{
  if (id)
  {
	  k_sem_init(id, 0, 1);
  }
  return false;
}

bool etcpal_signal_wait(etcpal_signal_t* id)
{
	if (id)
	{
		if (k_sem_take(id, K_FOREVER) == 0)
		{
			return true;
		}
	}
  return false;
}

bool etcpal_signal_try_wait(etcpal_signal_t* id)
{
	if (id)
	{
		if (k_sem_take(id, K_NO_WAIT) == 0)
		{
			return true;
		}
	}
	return false;
}

bool etcpal_signal_timed_wait(etcpal_signal_t* id, int timeout_ms)
{
  return (timeout_ms == 0 ? etcpal_signal_try_wait(id) : etcpal_signal_wait(id));
}

void etcpal_signal_post(etcpal_signal_t* id)
{
	if (id)
	{
	  k_sem_give(id);
	}
}

void etcpal_signal_destroy(etcpal_signal_t* id)
{
	// Not implemented
}
