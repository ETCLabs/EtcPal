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
 * This file is a part of EtcPal. For more information, go to:
 * https://github.com/ETCLabs/EtcPal
 ******************************************************************************/

/// \file etcpal/cpp/thread.h
/// \brief C++ wrapper and utilities for etcpal/thread.h

#ifndef ETCPAL_CPP_THREAD_H_
#define ETCPAL_CPP_THREAD_H_

#include <algorithm>
#include <chrono>
#include <functional>
#include <limits>
#include <memory>
#include <utility>
#include "etcpal/thread.h"

namespace etcpal
{
/// \defgroup etcpal_cpp_thread etcpal_cpp_thread
/// \ingroup etcpal_cpp
/// \brief C++ utilities for the \ref etcpal_thread module.

/// \ingroup etcpal_cpp_thread
/// \brief A thread class, modeled after std::thread.
///
/// This class is similar to std::thread, with the key advantage that it will work on any threaded
/// platform that EtcPal is ported for, including the embedded RTOS platforms. If your application
/// or library does not need to run on these platforms, consider using std::thread instead.
///
/// There are a few differences from std::thread:
/// * The thread is automatically joined on destruction (std::thread calls std::terminate if it is
///   destroyed while its associated thread is still running)
/// * The thread is not _detachable_; that is, once created, it _must_ be joined before it is
///   destroyed in all cases.
///
/// This is one of the few EtcPal wrappers that does heap allocation, as the thread function and
/// arguments need to be stored.
class Thread
{
public:
  using FunctionType = std::function<void()>;
  using Id = etcpal_thread_id_t;

  Thread() noexcept = default;
  template <class Function, class... Args>
  explicit Thread(Function&& func, Args&&... args);
  virtual ~Thread();

  Thread(Thread&& other) = default;
  Thread& operator=(Thread&& other) = default;

  Thread(const Thread& other) = delete;
  Thread& operator=(const Thread& other) = delete;

  bool joinable() const noexcept;
  Id id() const noexcept;

  void Join();

  static void Sleep(int ms);
  template <typename Rep, typename Period>
  static void Sleep(const std::chrono::duration<Rep, Period>& sleep_duration);

protected:
  std::unique_ptr<etcpal_thread_t> thread_;
  bool joinable_{false};
};

extern "C" inline void ThreadFn(void* arg)
{
  std::unique_ptr<Thread::FunctionType> p_f(static_cast<Thread::FunctionType*>(arg));
  (*p_f)();
}

template <class Function, class... Args>
inline Thread::Thread(Function&& func, Args&&... args)
{
  EtcPalThreadParams tparams;
  ETCPAL_THREAD_SET_DEFAULT_PARAMS(&tparams);

  thread_.reset(new etcpal_thread_t());

  auto new_f = std::unique_ptr<FunctionType>(
      new FunctionType(std::bind(std::forward<Function>(func), std::forward<Args>(args)...)));
  if (etcpal_thread_create(thread_.get(), &tparams, ThreadFn, new_f.get()))
  {
    new_f.release();
    joinable_ = true;
  }
}

inline Thread::~Thread()
{
  if (joinable_)
    etcpal_thread_join(thread_.get());
}

inline bool Thread::joinable() const noexcept
{
  return joinable_;
}

inline void Thread::Join()
{
  etcpal_thread_join(thread_.get());
  joinable_ = false;
}

inline void Thread::Sleep(int ms)
{
  etcpal_thread_sleep(ms);
}

template <typename Rep, typename Period>
void Thread::Sleep(const std::chrono::duration<Rep, Period>& sleep_duration)
{
  // This implementation cannot sleep longer than INT_MAX.
  int sleep_ms_clamped =
      static_cast<int>(std::min(std::chrono::milliseconds(sleep_duration).count(),
                                static_cast<std::chrono::milliseconds::rep>(std::numeric_limits<int>::max())));
  Sleep(sleep_ms_clamped);
}

};  // namespace etcpal

#endif  // ETCPAL_CPP_THREAD_H_
