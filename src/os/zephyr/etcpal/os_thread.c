/******************************************************************************
 * Copyright 2024 ETC Inc.
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

#include "etcpal/private/opts.h"
#include "etcpal/thread.h"
#include "etcpal/os_error.h"
#include "etcpal/etcpal_zephyr_common.h"
#include <string.h>

void zephyr_thread_entry(void* thread_fn, void* thread_arg, void* unused)
{
  ((void (*)(void*))thread_fn)(thread_arg);
}

etcpal_error_t etcpal_thread_create(etcpal_thread_t*          id,
                                    const EtcPalThreadParams* params,
                                    void (*thread_fn)(void*),
                                    void* thread_arg)
{
  id->stack = k_thread_stack_alloc(params->stack_size, IS_ENABLED(CONFIG_USERSPACE) ? K_USER : 0);
  if (id->stack == NULL)
  {
    return kEtcPalErrInvalid;
  }

  // TODO: Looks like this doesn't do anything
  // need to use k_thread_name_set
  strncpy(id->thread.name, params->thread_name, ETCPAL_THREAD_NAME_MAX_LENGTH - 1);
  k_tid_t thread_id =
      k_thread_create(&id->thread, id->stack, params->stack_size, zephyr_thread_entry, thread_fn, thread_arg, NULL,
                      params->priority, IS_ENABLED(CONFIG_USERSPACE) ? K_USER | K_INHERIT_PERMS : 0, K_NO_WAIT);
  return thread_id != NULL ? kEtcPalErrOk : kEtcPalErrInvalid;
}

etcpal_error_t etcpal_thread_sleep(unsigned int sleep_ms)
{
  return !k_msleep(sleep_ms) ? kEtcPalErrOk : kEtcPalErrSys;
}

etcpal_error_t etcpal_thread_join(etcpal_thread_t* id)
{
  int err;
  err = k_thread_join(&id->thread, K_FOREVER);
  switch (err)
  {
    case -EBUSY:
    case -EAGAIN:
      return kEtcPalErrTimedOut;
    case -EDEADLK:
      return kEtcPalErrInvalid;
  }

  err = k_thread_stack_free(id->stack);
  ETCPAL_ASSERT(err == 0);
  if (err)
  {
    return errno_os_to_etcpal(err);
  }

  return kEtcPalErrOk;
}

etcpal_error_t etcpal_thread_timed_join(etcpal_thread_t* id, int timeout_ms)
{
  int err;
  err = k_thread_join(&id->thread, ms_to_zephyr_timeout(timeout_ms));
  switch (err)
  {
    case -EBUSY:
    case -EAGAIN:
      return kEtcPalErrTimedOut;
    case -EDEADLK:
      return kEtcPalErrInvalid;
  }

  err = k_thread_stack_free(id->stack);
  ETCPAL_ASSERT(err == 0);
  if (err)
  {
    return errno_os_to_etcpal(err);
  }

  return kEtcPalErrOk;
}

etcpal_error_t etcpal_thread_terminate(etcpal_thread_t* id)
{
  k_thread_abort(&id->thread);
  int err = k_thread_stack_free(id->stack);
  ETCPAL_ASSERT(err == 0);
  return errno_os_to_etcpal(err);
}

etcpal_thread_os_handle_t etcpal_thread_get_os_handle(etcpal_thread_t* id)
{
  return &id->thread;
}
