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

/* etcpal/handle_manager.h: A utility to manage handing out integer handles to resources. */

#ifndef ETCPAL_HANDLE_MANAGER_H_
#define ETCPAL_HANDLE_MANAGER_H_

#include <stdbool.h>
#include <stdint.h>

/**
 * @defgroup etcpal_handle_manager handle_manager
 * @ingroup etcpal_core
 * @brief A utility to manage handing out integer handles to resources.
 *
 * ```c
 * #include "etcpal/handle_manager.h"
 * ```
 *
 * This struct and the accompanying functions are a utility to manage handing out integer handles
 * to resources. It first assigns monotonically-increasing positive values starting at 0 to handles;
 * after wraparound, it uses the value_in_use function to find holes where new handle values can be
 * assigned.
 *
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/** A pointer to a function that tests if a given handle value is currently assigned to a resource. */
typedef bool (*HandleValueInUseFunction)(int handle_val, void* context);

/** State for managing generic integer handle values. */
typedef struct IntHandleManager
{
  /** The last handle that was handed out, or -1 if none have been handed out yet. */
  int last_handle;
  /** Determines a custom wrap-around point. It will be ignored if it's negative. Otherwise it represents the highest
      allowed handle value. */
  int max_value;
  /** Optimize the handle generation algorithm by tracking whether the handle value has wrapped around. If not, this is
      false because there's no need to check if the new handle is in use. This becomes true when the handle value wraps
      around. */
  bool check_value_in_use;
  /** Function pointer to determine if a handle value is currently in use. Used only after the handle
      value has wrapped around once. */
  HandleValueInUseFunction value_in_use;
  /** A context passed to the value in use function. */
  void* context;
} IntHandleManager;

void init_int_handle_manager(IntHandleManager*        manager,
                             int                      max_value,
                             HandleValueInUseFunction value_in_use_func,
                             void*                    context);
int  get_next_int_handle(IntHandleManager* manager);

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif /* ETCPAL_HANDLE_MANAGER_H_ */
