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

#include "etcpal/common.h"

#include <stdbool.h>
#include "etcpal/private/log.h"
#include "etcpal/private/timer.h"

#ifndef ETCPAL_NO_NETWORKING_SUPPORT
#include "etcpal/private/netint.h"
#include "etcpal/private/socket.h"
#endif

/****************************** Private types ********************************/

typedef struct EtcPalModuleInit
{
  bool initted;
  etcpal_error_t (*init_fn)();
  void (*deinit_fn)();
} EtcPalModuleInit;

/* clang-format off */

#define ETCPAL_MODULE_INIT(module_name) { false, module_name##_init, module_name##_deinit }

#ifdef ETCPAL_NO_NETWORKING_SUPPORT
#define ETCPAL_MODULE_INIT_ARRAY       \
  {                                    \
    ETCPAL_MODULE_INIT(etcpal_timer),  \
    ETCPAL_MODULE_INIT(etcpal_log)     \
  }
#define MODULE_INIT_ARRAY_SIZE 2
#else
#define ETCPAL_MODULE_INIT_ARRAY       \
  {                                    \
    ETCPAL_MODULE_INIT(etcpal_socket), \
    ETCPAL_MODULE_INIT(etcpal_netint), \
    ETCPAL_MODULE_INIT(etcpal_timer),  \
    ETCPAL_MODULE_INIT(etcpal_log)     \
  }
#define MODULE_INIT_ARRAY_SIZE 4
#endif

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

  EtcPalModuleInit init_array[MODULE_INIT_ARRAY_SIZE] = ETCPAL_MODULE_INIT_ARRAY;

  etcpal_error_t    init_res = kEtcPalErrOk;
  etcpal_features_t feature_mask = 1u;

  // Initialize each module in turn.
  for (EtcPalModuleInit* init_struct = init_array; init_struct < init_array + MODULE_INIT_ARRAY_SIZE; ++init_struct)
  {
    if (features & feature_mask)
    {
      init_res = init_struct->init_fn();
      if (init_res == kEtcPalErrOk)
        init_struct->initted = true;
      else
        break;
    }
    feature_mask <<= 1;
  }

  if (init_res != kEtcPalErrOk)
  {
    // Clean up on failure.
    for (EtcPalModuleInit* init_struct = init_array; init_struct < init_array + MODULE_INIT_ARRAY_SIZE; ++init_struct)
    {
      if (init_struct->initted)
        init_struct->deinit_fn();
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
  EtcPalModuleInit init_array[MODULE_INIT_ARRAY_SIZE] = ETCPAL_MODULE_INIT_ARRAY;

  etcpal_features_t feature_mask = 1u;

  // Deinitialize each module in turn.
  for (EtcPalModuleInit* init_struct = init_array; init_struct < init_array + MODULE_INIT_ARRAY_SIZE; ++init_struct)
  {
    if (features & feature_mask)
    {
      init_struct->deinit_fn();
    }
    feature_mask <<= 1;
  }
}
