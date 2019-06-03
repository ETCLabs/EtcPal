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

#include "lwpa/common.h"
#include "lwpa/private/common.h"
#include "lwpa/private/log.h"

lwpa_error_t lwpa_init(lwpa_features_t features)
{
  // Step 1: OS-specific init
  lwpa_error_t res = lwpa_os_init(features);
  if (res != kLwpaErrOk)
    return res;

  // Step 2: OS-neutral init
  if (features & LWPA_FEATURE_LOGGING)
  {
    res = lwpa_log_init();
    if (res != kLwpaErrOk)
    {
      lwpa_os_deinit(features);
    }
  }
  return res;
}

void lwpa_deinit(lwpa_features_t features)
{
  // Step 1: OS-neutral deinit
  if (features & LWPA_FEATURE_LOGGING)
  {
    lwpa_log_deinit();
  }

  lwpa_os_deinit(features);
}
