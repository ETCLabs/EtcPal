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

#include "etcpal/timer.h"
#include "etcpal/private/timer.h"

#include <windows.h>

#define ETCPAL_WINDOWS_TIMER_RESOLUTION 1  // ms

#if !defined(ETCPAL_BUILDING_MOCK_LIB)

etcpal_error_t etcpal_timer_init(void)
{
  if (TIMERR_NOERROR == timeBeginPeriod(ETCPAL_WINDOWS_TIMER_RESOLUTION))
    return kEtcPalErrOk;
  else
    return kEtcPalErrSys;
}

void etcpal_timer_deinit(void)
{
  timeEndPeriod(ETCPAL_WINDOWS_TIMER_RESOLUTION);
}

uint32_t etcpal_getms(void)
{
  return timeGetTime();
}

#endif  // !defined(ETCPAL_BUILDING_MOCK_LIB)
