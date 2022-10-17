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
#include "etcpal/private/common.h"

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

// The order of this array determines the init & deinit order of the modules.
// They are initialized in order and deinitialized in reverse order.
static EtcPalModule etcpal_modules[] = {
  ETCPAL_MODULE(etcpal_log, ETCPAL_FEATURE_LOGGING),  // Logging is first for best EtcPal log coverage
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

/***************************** Global variables ******************************/

const EtcPalLogParams* etcpal_log_params  = NULL;
bool                   etcpal_init_called = false;

/**************************** Private variables ******************************/

static struct EtcPalState
{
  EtcPalLogParams log_params;
} etcpal_state;

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
 * If you've set up a handler for logs from the library via etcpal_init_log_handler(), make sure to enable the logging
 * feature in the first call to etcpal_init() - otherwise an error will be returned.
 *
 * It's highly recommended to call etcpal_init_log_handler() before this, since it will enable the application to log
 * critical assertions that may occur within EtcPal.
 *
 * etcpal_init() and etcpal_deinit() are not thread-safe; you should make sure your init-time and deinit-time code is
 * serialized.
 *
 * @param[in] features Mask of EtcPal features required.
 * @return #kEtcPalErrOk: EtcPal library initialized successfully.
 * @return #kEtcPalErrInvalid: A log handler was enabled via etcpal_init_log_handler(), but the logging feature is not
 * enabled and was not specified in the feature mask.
 * @return Various error codes possible from initialization of feature modules.
 */
etcpal_error_t etcpal_init(etcpal_features_t features)
{
  etcpal_init_called = true;

  // First check if the logging feature needs to be initialized due to the EtcPal log handler
  if (etcpal_log_params)
  {
    bool logging_enabled = false;
    for (size_t i = 0; i < MODULE_ARRAY_SIZE; ++i)
    {
      EtcPalModule* module = &etcpal_modules[i];
      if ((module->feature_mask & ETCPAL_FEATURE_LOGGING) && (module->init_count > 0))
      {
        logging_enabled = true;
        break;
      }
    }

    if (!logging_enabled && !(features & ETCPAL_FEATURE_LOGGING))
      return kEtcPalErrInvalid;
  }

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
    for (size_t i = MODULE_ARRAY_SIZE - 1; i >= 0; --i)  // Deinit in reverse order
    {
      EtcPalModule* module = &etcpal_modules[i];
      if (modules_initialized[i])
      {
        --(module->init_count);

        if (module->init_count == 0)
          module->deinit_fn();
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
  for (size_t i = MODULE_ARRAY_SIZE - 1; i >= 0; --i)  // Deinit in reverse order
  {
    EtcPalModule* module = &etcpal_modules[i];
    if ((features & module->feature_mask) && (module->init_count > 0))
    {
      --(module->init_count);

      if (module->init_count == 0)
        module->deinit_fn();
    }
  }
}

bool etcpal_assert_verify_fail(const char* exp, const char* file, const char* func, const int line)
{
#if !ETCPAL_LOGGING_ENABLED
  ETCPAL_UNUSED_ARG(exp);
  ETCPAL_UNUSED_ARG(file);
  ETCPAL_UNUSED_ARG(func);
  ETCPAL_UNUSED_ARG(line);
#endif
  ETCPAL_PRINT_LOG_CRIT("ASSERTION \"%s\" FAILED (FILE: \"%s\" FUNCTION: \"%s\" LINE: %d)", exp ? exp : "",
                        file ? file : "", func ? func : "", line);
  ETCPAL_ASSERT(false);
  return false;
}

bool set_etcpal_log_params(const EtcPalLogParams* params)
{
  if (etcpal_log_params)
    return false;

  if (params)
  {
    etcpal_state.log_params = *params;
    etcpal_log_params       = &etcpal_state.log_params;
  }

  return true;
}
