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

#include "etcpal/handle_manager.h"

/**
 * @brief Initialize an IntHandleManager instance.
 *
 * Once an IntHandleManager has been instantiated, it should be passed to this function to initialize its state.
 *
 * @param[in,out] manager The handle manager instance to initialize.
 * @param[in] max_value Determines a custom wrap-around point. It will be ignored if it's negative. Otherwise it
 * represents the highest allowed handle value.
 * @param[in] value_in_use_func The function that tests if a given handle value is currently assigned to a resource.
 * @param[in,out] context An optional pointer that will be passed to the value-in-use function. This can be NULL.
 */
void init_int_handle_manager(IntHandleManager*        manager,
                             int                      max_value,
                             HandleValueInUseFunction value_in_use_func,
                             void*                    context)
{
  manager->last_handle = -1;
  manager->max_value = max_value;
  manager->check_value_in_use = false;
  manager->value_in_use = value_in_use_func;
  manager->context = context;
}

/**
 * @brief Once the int handle manager is initialized, this is used to get the next handle value.
 *
 * This function provides the next handle value that is not currently assigned to a resource.
 *
 * @param[in,out] manager The handle manager to get the new handle from.
 * @return The new handle, or -1 if all handles are used.
 */
int get_next_int_handle(IntHandleManager* manager)
{
  int new_handle = manager->last_handle;
  bool is_used = false;
  do
  {
    ++new_handle;

    if ((new_handle < 0) || ((manager->max_value >= 0) && (new_handle > manager->max_value)))
    {
      new_handle = 0;
      manager->check_value_in_use = true;
    }

    if (manager->check_value_in_use)
      is_used = manager->value_in_use(new_handle, manager->context);
  } while (is_used && (new_handle != manager->last_handle));

  if (is_used)
    new_handle = -1;
  else
    manager->last_handle = new_handle;

  return new_handle;
}
