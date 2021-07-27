/******************************************************************************
 * Copyright 2021 ETC Inc.
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

#ifndef ETCPAL_FREERTOS_TIMED_WAIT_H_
#define ETCPAL_FREERTOS_TIMED_WAIT_H_

#define ETCPAL_TIMEOUT_TO_FREERTOS(timeout_ms) \
  (((timeout_ms) == ETCPAL_WAIT_FOREVER) ? portMAX_DELAY : pdMS_TO_TICKS(timeout_ms))

#endif  // ETCPAL_FREERTOS_TIMED_WAIT_H_
