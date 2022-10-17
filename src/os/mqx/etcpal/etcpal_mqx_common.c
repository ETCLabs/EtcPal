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

#include "etcpal/private/common.h"
#include "etcpal_mqx_common.h"

bool milliseconds_to_ticks(int ms, MQX_TICK_STRUCT* tick_struct)
{
  if (!ETCPAL_ASSERT_VERIFY(tick_struct))
    return false;

  TIME_STRUCT ts;
  ts.SECONDS      = (ms < 0 ? 0 : (ms / 1000));
  ts.MILLISECONDS = (ms <= 0 ? 1 : (ms % 1000));
  return _time_to_ticks(&ts, tick_struct);
}

bool lwsem_timed_wait(LWSEM_STRUCT* sem, int timeout_ms)
{
  if (!ETCPAL_ASSERT_VERIFY(sem))
    return false;

  if (timeout_ms < 0)
  {
    return (MQX_OK == _lwsem_wait(sem));
  }
  else
  {
    MQX_TICK_STRUCT ticks_to_wait;
    if (milliseconds_to_ticks(timeout_ms, &ticks_to_wait))
      return (MQX_OK == _lwsem_wait_for(sem, &ticks_to_wait));
    return false;
  }
}
