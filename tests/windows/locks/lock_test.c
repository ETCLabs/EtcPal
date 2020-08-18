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

/* lock_test.c: Rough benchmarking of Windows synchronization primitives.
 *
 * Windows has a bunch of mutual-exclusion APIs available. The purpose of this test is to get some
 * rough performance metrics from each of them to determine which one is most suitable for use
 * under the hood in the etcpal/lock module.
 *
 * The methodology of the test is:
 * - Make a global counter protected by a shared mutex
 * - Spawn a thread per core which all try to increment the counter within the mutex as fast as
 *   possible
 * - Sleep for 1 second, then join the threads
 * - Record the final value of the counter. The higher, the better.
 *
 * The last run of this test was done on 19 December 2019, with the following environment:
 * - Toolchain: Windows/Visual Studio 2019, with default CMake Release mode settings (/O2 /Ob2)
 * - System: Windows 10, 64-bit Intel Core i7-6700 with 8 logical processors
 * - Results:
 *  SRWLock    | Critical Section | Mutex
 * ------------|------------------|------------
 *    64023730 |          7159050 |     215022
 *
 * As SRWLocks are also non-recursive within the same thread (the desired behavior of
 * etcpal_mutex_t), they are the clear winner.
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>
#include <Windows.h>
#include <synchapi.h>

#include "etcpal/common.h"
#include "etcpal/thread.h"

// Suppress unused local variable warning in MSVC. We are using asserts which are disabled in
// release mode.

#ifdef _MSC_VER
#pragma warning(disable : 4189)
#endif

#define NUM_TEST_ITERATIONS 5

bool             keep_running;
unsigned long    shared_counter;
SRWLOCK          srw_lock;
CRITICAL_SECTION cs;
HANDLE           mutex;

void test_mutex_worker(void* arg)
{
  ETCPAL_UNUSED_ARG(arg);

  while (keep_running)
  {
    DWORD wait_res = WaitForSingleObject(mutex, INFINITE);
    assert(wait_res == WAIT_OBJECT_0);
    assert(shared_counter != LONG_MAX);
    ++shared_counter;
    ReleaseMutex(mutex);
  }
}

unsigned long test_mutex(DWORD core_count)
{
  printf("Starting Mutex test for %d cores...\n", core_count);

  mutex = CreateMutex(NULL, FALSE, NULL);
  shared_counter = 0;
  keep_running = true;

  EtcPalThreadParams thread_params = ETCPAL_THREAD_PARAMS_INIT;

  etcpal_thread_t* threads = (etcpal_thread_t*)calloc(core_count, sizeof(etcpal_thread_t));
  for (DWORD i = 0; i < core_count; ++i)
  {
    etcpal_error_t result = etcpal_thread_create(&threads[i], &thread_params, test_mutex_worker, NULL);
    assert(result == kEtcPalErrOk);
  }

  Sleep(1000);

  keep_running = false;
  for (DWORD i = 0; i < core_count; ++i)
  {
    etcpal_error_t result = etcpal_thread_join(&threads[i]);
    assert(result == kEtcPalErrOk);
  }

  free(threads);

  CloseHandle(mutex);
  printf("Mutex test done.\n");
  return shared_counter;
}

void test_critical_section_worker(void* arg)
{
  ETCPAL_UNUSED_ARG(arg);

  while (keep_running)
  {
    EnterCriticalSection(&cs);
    assert(shared_counter != LONG_MAX);
    ++shared_counter;
    LeaveCriticalSection(&cs);
  }
}

unsigned long test_critical_section(DWORD core_count)
{
  printf("Starting Critical Section test for %d cores...\n", core_count);

  InitializeCriticalSection(&cs);
  shared_counter = 0;
  keep_running = true;

  EtcPalThreadParams thread_params = ETCPAL_THREAD_PARAMS_INIT;

  etcpal_thread_t* threads = (etcpal_thread_t*)calloc(core_count, sizeof(etcpal_thread_t));
  for (DWORD i = 0; i < core_count; ++i)
  {
    etcpal_error_t result = etcpal_thread_create(&threads[i], &thread_params, test_critical_section_worker, NULL);
    assert(result == kEtcPalErrOk);
  }

  Sleep(1000);

  keep_running = false;
  for (DWORD i = 0; i < core_count; ++i)
  {
    etcpal_error_t result = etcpal_thread_join(&threads[i]);
    assert(result == kEtcPalErrOk);
  }

  free(threads);

  DeleteCriticalSection(&cs);
  printf("Critical Section test done.\n");
  return shared_counter;
}

void test_srw_lock_worker(void* arg)
{
  ETCPAL_UNUSED_ARG(arg);

  while (keep_running)
  {
    AcquireSRWLockExclusive(&srw_lock);
    assert(shared_counter != LONG_MAX);
    ++shared_counter;
    ReleaseSRWLockExclusive(&srw_lock);
  }
}

unsigned long test_srw_lock(DWORD core_count)
{
  printf("Starting SRWLock test for %d cores...\n", core_count);

  InitializeSRWLock(&srw_lock);
  shared_counter = 0;
  keep_running = true;

  EtcPalThreadParams thread_params = ETCPAL_THREAD_PARAMS_INIT;

  etcpal_thread_t* threads = (etcpal_thread_t*)calloc(core_count, sizeof(etcpal_thread_t));
  for (DWORD i = 0; i < core_count; ++i)
  {
    etcpal_error_t result = etcpal_thread_create(&threads[i], &thread_params, test_srw_lock_worker, NULL);
    assert(result == kEtcPalErrOk);
  }

  Sleep(1000);

  keep_running = false;
  for (DWORD i = 0; i < core_count; ++i)
  {
    etcpal_error_t result = etcpal_thread_join(&threads[i]);
    assert(result == kEtcPalErrOk);
  }

  free(threads);

  // Per the docs, SRWLocks do not need to be deinitialized.
  printf("SRWLock test done.\n");
  return shared_counter;
}

int main(void)
{
  SYSTEM_INFO system_info;
  GetSystemInfo(&system_info);
  DWORD core_count = system_info.dwNumberOfProcessors;

  uintmax_t mutex_average = 0;
  uintmax_t cs_average = 0;
  uintmax_t srw_average = 0;

  for (int i = 0; i < NUM_TEST_ITERATIONS; ++i)
  {
    printf("Starting test iteration %d...\n", i);
    mutex_average += test_mutex(core_count);
    cs_average += test_critical_section(core_count);
    srw_average += test_srw_lock(core_count);
    printf("End of test iteration %d.\n", i);
  }

  mutex_average /= NUM_TEST_ITERATIONS;
  cs_average /= NUM_TEST_ITERATIONS;
  srw_average /= NUM_TEST_ITERATIONS;

  // clang-format off
  printf("Test finished. Results:\n");
  printf(" SRWLock    | Critical Section | Mutex      \n");
  printf("------------|------------------|------------\n");
  printf(" %10llu"  " | %16llu"        " | %10llu\n", srw_average, cs_average, mutex_average);
  // clang-format on

  return 0;
}
