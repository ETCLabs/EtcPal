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

#include "lwpa/private/log.h"
#include "lwpa/private/netint.h"
#include "lwpa/private/socket.h"
#include "lwpa/private/timer.h"

/****************************** Private types ********************************/

typedef struct LwpaModuleInit
{
  bool initted;
  lwpa_error_t (*init_fn)();
  void (*deinit_fn)();
} LwpaModuleInit;

/* clang-format off */

#define LWPA_MODULE_INIT(module_name) { false, module_name##_init, module_name##_deinit }

#define LWPA_MODULE_INIT_ARRAY     \
  {                                \
    LWPA_MODULE_INIT(lwpa_socket), \
    LWPA_MODULE_INIT(lwpa_netint), \
    LWPA_MODULE_INIT(lwpa_timer),  \
    LWPA_MODULE_INIT(lwpa_log)     \
  }

/* clang-format on */

/*************************** Function definitions ****************************/

/*! \brief Initialize the lwpa library.
 *
 *  This function can be called multiple times from the same application. Each call to lwpa_init()
 *  must be paired with a call to lwpa_deinit() with the same argument for features.
 *
 *  If you are using a library that depends on lwpa, and not using lwpa directly, that library will
 *  call this function for you with the features it needs; you do not need to call it explicitly.
 *
 *  lwpa_init() and lwpa_deinit() are not thread-safe; you should make sure your init-time and
 *  deinit-time code is serialized.
 *
 *  \param[in] features Mask of lwpa features required.
 *  \return #kLwpaErrOk: lwpa library initialized successfully.
 *  \return Various error codes possible from initialization of feature modules.
 */
lwpa_error_t lwpa_init(lwpa_features_t features)
{
  // In this function and the deinit() function below, we create an array of structs for each lwpa
  // module that must be initialized, using the macros defined in lwpa/common.h and above in this
  // file. The structs contain the init and deinit functions for each module that is enabled by the
  // feature macros.
  //
  // If any init fails, each struct contains a flag indicating whether it has already been
  // initialized, so it can be cleaned up.

  LwpaModuleInit init_array[LWPA_NUM_FEATURES] = LWPA_MODULE_INIT_ARRAY;

  lwpa_error_t init_res = kLwpaErrOk;
  lwpa_features_t feature_mask = 1u;

  // Initialize each module in turn.
  for (LwpaModuleInit* init_struct = init_array; init_struct < init_array + LWPA_NUM_FEATURES; ++init_struct)
  {
    if (features & feature_mask)
    {
      init_res = init_struct->init_fn();
      if (init_res == kLwpaErrOk)
        init_struct->initted = true;
      else
        break;
    }
    feature_mask <<= 1;
  }

  if (init_res != kLwpaErrOk)
  {
    // Clean up on failure.
    for (LwpaModuleInit* init_struct = init_array; init_struct < init_array + LWPA_NUM_FEATURES; ++init_struct)
    {
      if (init_struct->initted)
        init_struct->deinit_fn();
    }
  }

  return init_res;
}

/*! \brief Deinitialize the lwpa library.
 *
 *  This must be called with the same argument as the corresponding call to lwpa_init() to clean up
 *  resources held by the feature modules.
 *
 *  \param[in] features Feature mask that was previously passed to lwpa_init().
 */
void lwpa_deinit(lwpa_features_t features)
{
  LwpaModuleInit init_array[LWPA_NUM_FEATURES] = LWPA_MODULE_INIT_ARRAY;

  lwpa_features_t feature_mask = 1u;

  // Deinitialize each module in turn.
  for (LwpaModuleInit* init_struct = init_array; init_struct < init_array + LWPA_NUM_FEATURES; ++init_struct)
  {
    if (features & feature_mask)
    {
      init_struct->deinit_fn();
    }
    feature_mask <<= 1;
  }
}
