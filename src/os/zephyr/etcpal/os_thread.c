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

#include "etcpal/thread.h"

#if !defined(ETCPAL_BUILDING_MOCK_LIB)

static void thread_func_internal(void* pvParameters)
{

}

etcpal_error_t etcpal_thread_create(etcpal_thread_t*          id,
                                    const EtcPalThreadParams* params,
                                    void (*thread_fn)(void*),
                                    void* thread_arg)
{

}

etcpal_error_t etcpal_thread_join(etcpal_thread_t* id)
{

}

etcpal_error_t etcpal_thread_timed_join(etcpal_thread_t* id, int timeout_ms)
{

}

etcpal_error_t etcpal_thread_terminate(etcpal_thread_t* id)
{

}

etcpal_thread_os_handle_t etcpal_thread_get_os_handle(etcpal_thread_t* id)
{
}

#endif  // !defined(ETCPAL_BUILDING_MOCK_LIB)
