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

#include "unity_fixture.h"

void run_all_tests(void)
{
  RUN_TEST_GROUP(mutex_integration);
  RUN_TEST_GROUP(recursive_mutex_integration);
  RUN_TEST_GROUP(rwlock_integration);
  RUN_TEST_GROUP(sem_integration);
  RUN_TEST_GROUP(signal_integration);
#if !DISABLE_SOCKET_INTEGRATION_TESTS
  RUN_TEST_GROUP(socket_integration_udp);
#endif
}
