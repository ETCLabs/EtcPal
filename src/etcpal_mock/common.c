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

#include "etcpal_mock/common.h"
#include "etcpal_mock/netint.h"
#include "etcpal_mock/socket.h"
#include "etcpal_mock/thread.h"
#include "etcpal_mock/timer.h"

const EtcPalLogParams* etcpal_log_params  = NULL;
bool                   etcpal_init_called = false;

DEFINE_FAKE_VALUE_FUNC(etcpal_error_t, etcpal_init, etcpal_features_t);
DEFINE_FAKE_VOID_FUNC(etcpal_deinit, etcpal_features_t);
DEFINE_FAKE_VALUE_FUNC(bool, set_etcpal_log_params, const EtcPalLogParams*);

bool etcpal_assert_verify_fail(const char* exp, const char* file, const char* func, int line)
{
  ETCPAL_UNUSED_ARG(exp);
  ETCPAL_UNUSED_ARG(file);
  ETCPAL_UNUSED_ARG(func);
  ETCPAL_UNUSED_ARG(line);
  return false;
}

void etcpal_reset_all_fakes(void)
{
  etcpal_netint_reset_all_fakes();
  etcpal_socket_reset_all_fakes();
  etcpal_thread_reset_all_fakes();
  etcpal_timer_reset_all_fakes();

  RESET_FAKE(etcpal_init);
  RESET_FAKE(etcpal_deinit);
  RESET_FAKE(set_etcpal_log_params);
}
