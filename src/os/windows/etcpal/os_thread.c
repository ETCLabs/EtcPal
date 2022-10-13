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

#include <process.h>
#include <string.h>
#include "etcpal/common.h"
#include "os_error.h"

#if !defined(ETCPAL_BUILDING_MOCK_LIB)

// THIS IS WINDOWS MAGIC to set the thread name on versions older than Win10 1607. It is copied
// directly from the sample code at Microsoft.
// Lasciate ogne speranza, voi ch'intrate
// Abandon all hope, ye who enter here.
const DWORD kMsVcException = 0x406D1388;
#pragma pack(push, 8)
typedef struct
{
  DWORD  dwType; /* Must be 0x1000. */                          // NOLINT
  LPCSTR szName; /* Pointer to name (in user addr space). */    // NOLINT
  DWORD  dwThreadID; /* Thread ID (-1=caller thread). */        // NOLINT
  DWORD  dwFlags; /* Reserved for future use, must be zero. */  // NOLINT
} ThreadNameInfo;
#pragma pack(pop)

#ifdef UNICODE
#define KERNEL32_DLL_NAME L"Kernel32.dll"
#else
#define KERNEL32_DLL_NAME "Kernel32.dll"
#endif

// Usage: SetThreadName ((DWORD)-1, "MainThread");
//
// Windows 10 version 1607 and Windows Server 2016 add an explicit thread name using the
// SetThreadDescription function. This API works even if no debugger is attached. This is used if
// available (the version of Windows is new enough); otherwise, we fall back to the old method that
// only works in debuggers.
void set_thread_name(DWORD thread_id, const char* thread_name)
{
  if (!ETCPAL_ASSERT_VERIFY(thread_name))
    return;

  // Find function pointer for Win10 1607 version
  // Kernel32 will definitely be loaded so directly ask for the handle
  typedef HRESULT(WINAPI * SetThreadDescriptionFunc)(HANDLE thread, PCWSTR thread_description);
  SetThreadDescriptionFunc set_thread_description_func =
      (SetThreadDescriptionFunc)GetProcAddress(GetModuleHandle(KERNEL32_DLL_NAME), "SetThreadDescription");
  if (set_thread_description_func)
  {
    // Convert to WCHAR
    wchar_t dest[100] = {0};
    MultiByteToWideChar(CP_UTF8, MB_PRECOMPOSED, thread_name, -1, dest, 100);
    set_thread_description_func(GetCurrentThread(), dest);
    return;
  }

  // Older versions of Windows
  ThreadNameInfo info;
  info.dwType     = 0x1000;
  info.szName     = thread_name;
  info.dwThreadID = thread_id;
  info.dwFlags    = 0;
#pragma warning(push)
#pragma warning(disable : 6320 6322)
  __try
  {
    RaiseException(kMsVcException, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR*)&info);
  }
  __except (EXCEPTION_EXECUTE_HANDLER)
  {
  }
#pragma warning(pop)
}

unsigned __stdcall thread_func_internal(void* pthread)
{
  etcpal_thread_t* thread_data = (etcpal_thread_t*)pthread;
  if (!ETCPAL_ASSERT_VERIFY(thread_data) || !ETCPAL_ASSERT_VERIFY(thread_data->fn))
    return 1;

  set_thread_name((DWORD)-1, thread_data->name);
  thread_data->fn(thread_data->arg);
  return 0;
}

etcpal_error_t etcpal_thread_create(etcpal_thread_t*          id,
                                    const EtcPalThreadParams* params,
                                    void (*thread_fn)(void*),
                                    void* thread_arg)
{
  if (!id || !params || !thread_fn)
    return kEtcPalErrInvalid;

  strncpy_s(id->name, ETCPAL_THREAD_NAME_MAX_LENGTH, params->thread_name, _TRUNCATE);
  id->arg = thread_arg;
  id->fn  = thread_fn;
  id->tid = (HANDLE)_beginthreadex(NULL, params->stack_size, thread_func_internal, id, CREATE_SUSPENDED, NULL);
  if (id->tid == 0)
    return err_os_to_etcpal(errno);

  SetThreadPriority(id->tid, (int)params->priority);
  ResumeThread(id->tid);
  return kEtcPalErrOk;
}

etcpal_error_t etcpal_thread_join(etcpal_thread_t* id)
{
  if (id)
  {
    if (WAIT_OBJECT_0 != WaitForSingleObject(id->tid, INFINITE))
    {
      return kEtcPalErrSys;
    }
    CloseHandle(id->tid);
    return kEtcPalErrOk;
  }
  return kEtcPalErrInvalid;
}

etcpal_error_t etcpal_thread_timed_join(etcpal_thread_t* id, int timeout_ms)
{
  if (id)
  {
    DWORD wait_time = (timeout_ms < 0 ? INFINITE : timeout_ms);
    if (WAIT_OBJECT_0 != WaitForSingleObject(id->tid, wait_time))
    {
      return kEtcPalErrTimedOut;
    }
    CloseHandle(id->tid);
    return kEtcPalErrOk;
  }
  return kEtcPalErrInvalid;
}

etcpal_error_t etcpal_thread_terminate(etcpal_thread_t* id)
{
  if (!id)
    return kEtcPalErrInvalid;

  if (TerminateThread(id->tid, 0) == 0)
    return err_os_to_etcpal(GetLastError());

  return kEtcPalErrOk;
}

etcpal_thread_os_handle_t etcpal_thread_get_os_handle(etcpal_thread_t* id)
{
  return (id ? GetThreadId(id->tid) : ETCPAL_THREAD_OS_HANDLE_INVALID);
}

#endif  // !defined(ETCPAL_BUILDING_MOCK_LIB)
