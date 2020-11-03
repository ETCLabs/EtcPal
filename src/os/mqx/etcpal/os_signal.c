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

#include "etcpal/signal.h"
#include "etcpal_mqx_common.h"

bool etcpal_signal_timed_wait(etcpal_signal_t* id, int timeout_ms)
{
  if (timeout_ms < 0)
  {
    return (MQX_OK == _lwevent_wait_ticks(id, 1u, true, 0u));
  }
  else
  {
    MQX_TICK_STRUCT ticks_to_wait;
    if (milliseconds_to_ticks(timeout_ms, &ticks_to_wait))
      return (MQX_OK == _lwevent_wait_for(id, 1u, true, &ticks_to_wait));
    return false;
  }
}
