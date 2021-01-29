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
  etcpal_features_t feature_mask;
  etcpal_error_t (*init_fn)();
  void (*deinit_fn)();
} EtcPalModule;

#define ETCPAL_MODULE(module_name, feature_mask)           \
  {                                                        \
    feature_mask, module_name##_init, module_name##_deinit \
  }

/* clang-format off */

static const EtcPalModule kEtcPalModules[] = {
  ETCPAL_MODULE(etcpal_log, ETCPAL_FEATURE_LOGGING),
#if !ETCPAL_NO_OS_SUPPORT
  ETCPAL_MODULE(etcpal_timer, ETCPAL_FEATURE_TIMERS),
#endif
#if !ETCPAL_NO_NETWORKING_SUPPORT
  ETCPAL_MODULE(etcpal_socket, ETCPAL_FEATURE_SOCKETS),
  ETCPAL_MODULE(etcpal_netint, ETCPAL_FEATURE_NETINTS),
#endif
};
#define MODULE_ARRAY_SIZE (sizeof(kEtcPalModules) / sizeof(kEtcPalModules[0]))

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
 * This function can be called multiple times from the same application. Each call to etcpal_init()
 * must be paired with a call to etcpal_deinit() with the same argument for features.
 *
 * If you are using a library that depends on EtcPal, and not using EtcPal directly, that library
 * will call this function for you with the features it needs; you do not need to call it
 * explicitly.
 *
 * etcpal_init() and etcpal_deinit() are not thread-safe; you should make sure your init-time and
 * deinit-time code is serialized.
 *
 * @param[in] features Mask of EtcPal features required.
 * @return #kEtcPalErrOk: EtcPal library initialized successfully.
 * @return Various error codes possible from initialization of feature modules.
 */
etcpal_error_t etcpal_init(etcpal_features_t features)
{
  // In this function and the deinit() function below, we create an array of structs for each EtcPal
  // module that must be initialized, using the macros defined in etcpal/common.h and above in .his
  // file. The structs contain the init and deinit functions for each module that is enabled by the
  // feature macros.
  //
  // If any init fails, each struct contains a flag indicating whether it has already been
  // initialized, so it can be cleaned up.

  bool modules_initialized[MODULE_ARRAY_SIZE] = {false};

  etcpal_error_t init_res = kEtcPalErrOk;

  // Initialize each module in turn.
  for (size_t i = 0; i < MODULE_ARRAY_SIZE; ++i)
  {
    const EtcPalModule* module = &kEtcPalModules[i];
    if (features & module->feature_mask)
    {
      init_res = module->init_fn();
      if (init_res == kEtcPalErrOk)
        modules_initialized[i] = true;
      else
        break;
    }
  }

  if (init_res != kEtcPalErrOk)
  {
    // Clean up on failure.
    for (size_t i = 0; i < MODULE_ARRAY_SIZE; ++i)
    {
      if (modules_initialized[i])
        kEtcPalModules[i].deinit_fn();
    }
  }

  return init_res;
}

/**
 * @brief Deinitialize the EtcPal library.
 *
 * This must be called with the same argument as the corresponding call to etcpal_init() to clean up
 * resources held by the feature modules.
 *
 * @param[in] features Feature mask that was previously passed to etcpal_init().
 */
void etcpal_deinit(etcpal_features_t features)
{
  // Deinitialize each module in turn.
  for (const EtcPalModule* module = kEtcPalModules; module < kEtcPalModules + MODULE_ARRAY_SIZE; ++module)
  {
    if (features & module->feature_mask)
      module->deinit_fn();
  }
}
