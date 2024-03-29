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

#include "etcpal/common.h"
#include "etcpal/etcpal_zephyr_common.h"

k_timeout_t ms_to_zephyr_timeout(int timeout_ms)
{
  switch (timeout_ms)
  {
    case ETCPAL_WAIT_FOREVER:
      return K_FOREVER;
    case ETCPAL_NO_WAIT:
      return K_NO_WAIT;
    default:
      return K_MSEC(timeout_ms);
  }
}
