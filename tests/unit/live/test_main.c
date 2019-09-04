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
#include "unity_fixture.h"
#include "fff.h"

DEFINE_FFF_GLOBALS;

void run_all_tests(void)
{
  RUN_TEST_GROUP(lwpa_common);
  RUN_TEST_GROUP(lwpa_inet);
  RUN_TEST_GROUP(lwpa_lock);
  RUN_TEST_GROUP(lwpa_log);
  RUN_TEST_GROUP(lwpa_mempool);
  RUN_TEST_GROUP(lwpa_netint);
  RUN_TEST_GROUP(lwpa_pack);
  RUN_TEST_GROUP(lwpa_rbtree);
  RUN_TEST_GROUP(lwpa_socket);
  RUN_TEST_GROUP(lwpa_thread);
  RUN_TEST_GROUP(lwpa_timer);
  RUN_TEST_GROUP(lwpa_uuid);
}
