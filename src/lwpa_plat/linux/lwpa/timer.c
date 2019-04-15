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

#include "lwpa/timer.h"

#include <time.h>

uint32_t lwpa_getms()
{
  struct timespec plat_time;
  if (0 == clock_gettime(CLOCK_MONOTONIC, &plat_time))
  {
    return (uint32_t)(plat_time.tv_sec * 1000 + (plat_time.tv_nsec / 1000000));
  }
  return 0;
}