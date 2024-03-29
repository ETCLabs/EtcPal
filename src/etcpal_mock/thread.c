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

#include "etcpal_mock/thread.h"

DEFINE_FAKE_VALUE_FUNC(etcpal_error_t,
                       etcpal_thread_create,
                       etcpal_thread_t*,
                       const EtcPalThreadParams*,
                       EtcPalThreadFunc,
                       void*);
DEFINE_FAKE_VALUE_FUNC(etcpal_error_t, etcpal_thread_sleep, unsigned int);
DEFINE_FAKE_VALUE_FUNC(etcpal_error_t, etcpal_thread_join, etcpal_thread_t*);
DEFINE_FAKE_VALUE_FUNC(etcpal_error_t, etcpal_thread_timed_join, etcpal_thread_t*, int);
DEFINE_FAKE_VALUE_FUNC(etcpal_error_t, etcpal_thread_terminate, etcpal_thread_t*);
DEFINE_FAKE_VALUE_FUNC(etcpal_thread_os_handle_t, etcpal_thread_get_os_handle, etcpal_thread_t*);

void etcpal_thread_reset_all_fakes(void)
{
  RESET_FAKE(etcpal_thread_create);
  RESET_FAKE(etcpal_thread_sleep);
  RESET_FAKE(etcpal_thread_join);
  RESET_FAKE(etcpal_thread_timed_join);
  RESET_FAKE(etcpal_thread_terminate);
  RESET_FAKE(etcpal_thread_get_os_handle);
}
