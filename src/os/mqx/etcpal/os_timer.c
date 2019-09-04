/******************************************************************************
 * Copyright 2019 ETC Inc.
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
 * This file is a part of lwpa. For more information, go to:
 * https://github.com/ETCLabs/lwpa
 ******************************************************************************/

#include "etcpal/timer.h"
#include "etcpal/private/timer.h"

#include <mqx.h>

lwpa_error_t lwpa_timer_init()
{
  // No initialization necessary on this platform
  return kLwpaErrOk;
}

void lwpa_timer_deinit()
{
  // No deinitialization necessary on this platform
}

uint32_t lwpa_getms()
{
  TIME_STRUCT ts;
  _time_get_elapsed(&ts);
  return (ts.SECONDS * 1000 + ts.MILLISECONDS);
}