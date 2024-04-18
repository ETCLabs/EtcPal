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

#include "etcpal/thread.h"
#include "etcpal/os_error.h"
#include "etcpal/etcpal_zephyr_common.h"
#include "etcpal/private/common.h"
#include <string.h>

void zephyr_thread_entry(void* thread_fn, void* thread_arg, void* unused)
{
  if (ETCPAL_ASSERT_VERIFY(thread_fn))
  {
    ((void (*)(void*))thread_fn)(thread_arg);
  }
}

etcpal_error_t etcpal_thread_create(etcpal_thread_t*          id,
                                    const EtcPalThreadParams* params,
                                    void (*thread_fn)(void*),
                                    void* thread_arg)
{
  if (!id || !params || !thread_fn)
  {
    return kEtcPalErrInvalid;
  }

  id->stack = k_thread_stack_alloc(params->stack_size, IS_ENABLED(CONFIG_USERSPACE) ? K_USER : 0);
  if (!id->stack)
  {
    return kEtcPalErrNoMem;
  }

  etcpal_error_t err = kEtcPalErrOk;
  k_tid_t        thread_id =
      k_thread_create(&id->thread, id->stack, params->stack_size, zephyr_thread_entry, thread_fn, thread_arg, NULL,
                      params->priority, IS_ENABLED(CONFIG_USERSPACE) ? K_USER | K_INHERIT_PERMS : 0, K_NO_WAIT);
  if (thread_id == NULL)
  {
    err = kEtcPalErrInvalid;
  }

  if (err == kEtcPalErrOk)
  {
    switch (k_thread_name_set(&id->thread, params->thread_name))
    {
      case 0:
        break;
      case -EINVAL:
        err = kEtcPalErrInvalid;
        break;
      default:
        err = kEtcPalErrSys;
        break;
    }
  }

  if (err != kEtcPalErrOk)
  {
    if (thread_id != NULL)
    {
      k_thread_abort(thread_id);
    }
    ETCPAL_ASSERT_VERIFY(k_thread_stack_free(id->stack) == 0);
  }
  return err;
}

etcpal_error_t etcpal_thread_sleep(unsigned int sleep_ms)
{
  return !k_sleep(ms_to_zephyr_timeout(sleep_ms)) ? kEtcPalErrOk : kEtcPalErrSys;
}

etcpal_error_t etcpal_thread_join(etcpal_thread_t* id)
{
  return etcpal_thread_timed_join(id, ETCPAL_WAIT_FOREVER);
}

etcpal_error_t etcpal_thread_timed_join(etcpal_thread_t* id, int timeout_ms)
{
  if (!id)
  {
    return kEtcPalErrInvalid;
  }

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
  if (!ETCPAL_ASSERT_VERIFY(err == 0))
  {
    return errno_os_to_etcpal(err);
  }

  return kEtcPalErrOk;
}

etcpal_error_t etcpal_thread_terminate(etcpal_thread_t* id)
{
  if (!id)
  {
    return kEtcPalErrInvalid;
  }

  k_thread_abort(&id->thread);
  int err = k_thread_stack_free(id->stack);
  return errno_os_to_etcpal(err);
}

etcpal_thread_os_handle_t etcpal_thread_get_os_handle(etcpal_thread_t* id)
{
  if (!id)
  {
    return ETCPAL_THREAD_OS_HANDLE_INVALID;
  }

  return &id->thread;
}
