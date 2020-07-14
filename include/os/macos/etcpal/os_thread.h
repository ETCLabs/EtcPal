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

#ifndef ETCPAL_OS_THREAD_H_
#define ETCPAL_OS_THREAD_H_

#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ETCPAL_THREAD_DEFAULT_PRIORITY 0 /* Priority ignored on macOS */
#define ETCPAL_THREAD_DEFAULT_STACK 0    /* 0 means keep default */
#define ETCPAL_THREAD_DEFAULT_NAME NULL  /* Name ignored on macOS */
#define ETCPAL_THREAD_JOIN_CAN_TIMEOUT 0 /* Timeout unavailable on macOS */

#define ETCPAL_THREAD_NAME_MAX_LENGTH 0

typedef struct
{
  void (*fn)(void*);
  void*     arg;
  pthread_t handle;
} etcpal_thread_t;

#define etcpal_thread_sleep(sleep_ms) usleep(((useconds_t)sleep_ms) * 1000)

#ifdef __cplusplus
}
#endif

#endif /* ETCPAL_OS_THREAD_H_ */
