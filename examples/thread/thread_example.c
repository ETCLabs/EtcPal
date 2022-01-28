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

/*
 * A platform-neutral example application showing usage of the etcpal_thread functions. Starts a
 * few threads that loop at different rates, then waits for them all to finish.
 */

#include <stdio.h>
#include "etcpal/thread.h"

// Iterates once and exits.
void thread_1_func(void* arg)
{
  int* pval = (int*)arg;
  printf("Thread 1 executing\n");
  (*pval)++;
}

// Iterates 5 times, sleeping for half a second between each iteration.
void thread_2_func(void* arg)
{
  int* pval = (int*)arg;
  for (int i = 0; i < 5; ++i)
  {
    printf("Thread 2 executing\n");
    (*pval)++;
    etcpal_thread_sleep(500);
  }
}

// Iterates 10 times, sleeping for a quarter of a second between each iteration.
void thread_3_func(void* arg)
{
  int* pval = (int*)arg;
  for (int i = 0; i < 10; ++i)
  {
    printf("Thread 3 executing\n");
    (*pval)++;
    etcpal_thread_sleep(250);
  }
}

int main(void)
{
  // No initialization is necessary for the thread module.

  int thread_1_count = 0;
  int thread_2_count = 0;
  int thread_3_count = 0;

  EtcPalThreadParams thread_params = ETCPAL_THREAD_PARAMS_INIT;

  // Start thread 1
  etcpal_thread_t thread_1_handle;
  thread_params.thread_name = "Thread 1";
  etcpal_error_t res        = etcpal_thread_create(&thread_1_handle, &thread_params, thread_1_func, &thread_1_count);
  if (res != kEtcPalErrOk)
  {
    printf("Error starting thread 1: '%s'\n", etcpal_strerror(res));
    return 1;
  }

  // Start thread 2
  etcpal_thread_t thread_2_handle;
  thread_params.thread_name = "Thread 2";
  res                       = etcpal_thread_create(&thread_2_handle, &thread_params, thread_2_func, &thread_2_count);
  if (res != kEtcPalErrOk)
  {
    printf("Error starting thread 2: '%s'\n", etcpal_strerror(res));
    return 1;
  }

  // Start thread 3
  etcpal_thread_t thread_3_handle;
  thread_params.thread_name = "Thread 3";
  res                       = etcpal_thread_create(&thread_3_handle, &thread_params, thread_3_func, &thread_3_count);
  if (res != kEtcPalErrOk)
  {
    printf("Error starting thread 3: '%s'\n", etcpal_strerror(res));
    return 1;
  }

  // Wait for all threads to finish
  etcpal_thread_join(&thread_1_handle);
  etcpal_thread_join(&thread_2_handle);
  etcpal_thread_join(&thread_3_handle);

  // Print results
  printf("Thread 1 ran %d times.\n", thread_1_count);
  printf("Thread 2 ran %d times.\n", thread_2_count);
  printf("Thread 3 ran %d times.\n", thread_3_count);

  return 0;
}
