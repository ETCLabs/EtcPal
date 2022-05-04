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

#include "etcpal/common.h"

#include <stdbool.h>
#include <string.h>
#include "etcpal/private/log.h"
#include "etcpal/private/timer.h"

#ifndef ETCPAL_NO_NETWORKING_SUPPORT
#include "etcpal/private/netint.h"
#include "etcpal/private/socket.h"
#endif

/****************************** Private types ********************************/

typedef struct EtcPalModule
{
  etcpal_error_t (*init_fn)();
  void (*deinit_fn)();
  etcpal_features_t feature_mask;
  unsigned int      init_count;
} EtcPalModule;

#define ETCPAL_MODULE(module_name, feature_mask)              \
  {                                                           \
    module_name##_init, module_name##_deinit, feature_mask, 0 \
  }

/* clang-format off */

static EtcPalModule etcpal_modules[] = {
  ETCPAL_MODULE(etcpal_log, ETCPAL_FEATURE_LOGGING),
#if !ETCPAL_NO_OS_SUPPORT
  ETCPAL_MODULE(etcpal_timer, ETCPAL_FEATURE_TIMERS),
#endif
#if !ETCPAL_NO_NETWORKING_SUPPORT
  ETCPAL_MODULE(etcpal_socket, ETCPAL_FEATURE_SOCKETS),
  ETCPAL_MODULE(etcpal_netint, ETCPAL_FEATURE_NETINTS),
#endif
};
#define MODULE_ARRAY_SIZE (sizeof(etcpal_modules) / sizeof(etcpal_modules[0]))

/* clang-format on */

/*************************** Function definitions ****************************/

/**
 * @brief Initialize the EtcPal library.
 *
 * Use a bitwise OR to select from the set of modules which require initialization; for example:
 *
 * @code
 * etcpal_init(ETCPAL_FEATURE_SOCKETS | ETCPAL_FEATURE_NETINTS);
 * @endcode
 *
 * This function can be called multiple times from the same application. Each call to etcpal_init() must be paired with
 * a call to etcpal_deinit() with the same argument for features. Counters are tracked for each feature so that
 * etcpal_init() and etcpal_deinit() can be called multiple times for the same feature(s) (useful if running multiple
 * libraries that depend on common EtcPal features).
 *
 * If you are using a library that depends on EtcPal, and not using EtcPal directly, please refer to that library's
 * documentation to determine if you need to call this with the features the library needs.
 *
 * etcpal_init() and etcpal_deinit() are not thread-safe; you should make sure your init-time and deinit-time code is
 * serialized.
 *
 * @param[in] features Mask of EtcPal features required.
 * @return #kEtcPalErrOk: EtcPal library initialized successfully.
 * @return Various error codes possible from initialization of feature modules.
 */
etcpal_error_t etcpal_init(etcpal_features_t features)
{
  // In this function and the deinit() function below, we iterate the etcpal_modules array for each EtcPal module that
  // must be initialized. The array contains the init and deinit functions and an init counter for each supported
  // module.

  // If any init fails, use this to track what has already been initialized, so it can be cleaned up.
  bool modules_initialized[MODULE_ARRAY_SIZE] = {false};

  etcpal_error_t init_res = kEtcPalErrOk;

  // Initialize each module in turn.
  for (size_t i = 0; i < MODULE_ARRAY_SIZE; ++i)
  {
    EtcPalModule* module = &etcpal_modules[i];
    if (features & module->feature_mask)
    {
      init_res = (module->init_count == 0) ? module->init_fn() : kEtcPalErrOk;

      if (init_res == kEtcPalErrOk)
      {
        ++(module->init_count);
        modules_initialized[i] = true;
      }
      else
      {
        break;
      }
    }
  }

  if (init_res != kEtcPalErrOk)
  {
    // Clean up on failure.
    for (size_t i = 0; i < MODULE_ARRAY_SIZE; ++i)
    {
      if (modules_initialized[i])
      {
        --(etcpal_modules[i].init_count);

        if (etcpal_modules[i].init_count == 0)
          etcpal_modules[i].deinit_fn();
      }
    }
  }

  return init_res;
}

/**
 * @brief Deinitialize the EtcPal library.
 *
 * This must be called with the same argument as the corresponding call to etcpal_init() to clean up resources held by
 * the feature modules.
 *
 * For each feature, this must be called the same number of times as etcpal_init() was called for that feature. The
 * feature will be deinitialized on the final call.
 *
 * @param[in] features Feature mask that was previously passed to etcpal_init().
 */
void etcpal_deinit(etcpal_features_t features)
{
  // Deinitialize each module in turn.
  for (EtcPalModule* module = etcpal_modules; module < etcpal_modules + MODULE_ARRAY_SIZE; ++module)
  {
    if ((features & module->feature_mask) && (module->init_count > 0))
    {
      --(module->init_count);

      if (module->init_count == 0)
        module->deinit_fn();
    }
  }
}
