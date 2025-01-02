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

#include "unity_fixture.h"

extern "C" void run_all_tests(void)  // NOLINT
{
  RUN_TEST_GROUP(etcpal_cpp_stop_token);
  RUN_TEST_GROUP(etcpal_cpp_error);
  RUN_TEST_GROUP(etcpal_cpp_hash);
  RUN_TEST_GROUP(etcpal_cpp_uuid);
  RUN_TEST_GROUP(etcpal_cpp_opaque_id);
  RUN_TEST_GROUP(etcpal_cpp_optional);
#if !ETCPAL_NO_OS_SUPPORT
#if !DISABLE_EVENT_GROUP_TESTS
  RUN_TEST_GROUP(etcpal_cpp_event_group);
#endif
  RUN_TEST_GROUP(etcpal_cpp_log_timestamp);
  RUN_TEST_GROUP(etcpal_cpp_log);
  RUN_TEST_GROUP(etcpal_cpp_mutex);
#if !DISABLE_RECURSIVE_MUTEX_TESTS
  RUN_TEST_GROUP(etcpal_cpp_recursive_mutex);
#endif
  RUN_TEST_GROUP(etcpal_cpp_rwlock);
  RUN_TEST_GROUP(etcpal_cpp_sem);
  RUN_TEST_GROUP(etcpal_cpp_signal);
  RUN_TEST_GROUP(etcpal_cpp_thread);
  RUN_TEST_GROUP(etcpal_cpp_timer);

#if !DISABLE_QUEUE_TESTS
  RUN_TEST_GROUP(etcpal_cpp_queue);
#endif

#if !DISABLE_STATIC_QUEUE_TESTS
  RUN_TEST_GROUP(etcpal_cpp_queue_static);
#endif

#endif
#if !ETCPAL_NO_NETWORKING_SUPPORT
  RUN_TEST_GROUP(etcpal_cpp_inet);
#endif
}
