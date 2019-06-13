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

#include "lwpa/bool.h"
#include "lwpa/private/common.h"
#include "lwpa/private/log.h"
#include "lwpa/private/netint.h"

lwpa_error_t lwpa_init(lwpa_features_t features)
{
  bool os_initted = false;
  bool logging_initted = false;

  // Step 1: OS-specific init
  lwpa_error_t res = lwpa_os_init(features);
  os_initted = (res == kLwpaErrOk);

  // Step 2: OS-neutral init
  if (res == kLwpaErrOk)
  {
    if (features & LWPA_FEATURE_LOGGING)
    {
      logging_initted = ((res = lwpa_log_init()) == kLwpaErrOk);
    }
  }

  if (features & LWPA_FEATURE_NETINTS)
  {
    res = lwpa_netint_init();
  }

  if (res != kLwpaErrOk)
  {
    // Clean up on failure
    if (logging_initted)
      lwpa_log_deinit();
    if (os_initted)
      lwpa_os_deinit(features);
  }
  return res;
}

void lwpa_deinit(lwpa_features_t features)
{
  // Step 1: OS-neutral deinit
  if (features & LWPA_FEATURE_NETINTS)
  {
    lwpa_netint_deinit();
  }
  if (features & LWPA_FEATURE_LOGGING)
  {
    lwpa_log_deinit();
  }

  lwpa_os_deinit(features);
}
