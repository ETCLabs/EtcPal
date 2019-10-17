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

#include <functional>
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
/// This class is essentially a substitute for std::thread, with the key advantage that it will
/// work on any threaded platform that EtcPal is ported for, including the embedded RTOS platforms.
/// If your application or library does not need to run on these platforms, consider using
/// std::thread instead.
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
  explicit Thread(Function&& func, Args&&... Args);
  virtual ~Thread();

  Thread(Thread&& other) = default;
  Thread& operator=(Thread&& other) = default;

  Thread(const Thread& other) = delete;
  Thread& operator=(const Thread& other) = delete;

  bool joinable() const noexcept;
  Id id() const noexcept;

  void Join();

  static void Sleep(int ms);

protected:
  etcpal_thread_t thread_{};
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

  FunctionType* new_f = new FunctionType(std::bind(std::forward<Function>(func), std::forward<Args>(args)...));
  if (etcpal_thread_create(&thread_, &tparams, ThreadFn, new_f))
  {
    joinable_ = true;
  }
  else
  {
    delete new_f;
  }
}

inline Thread::~Thread()
{
  if (joinable_)
    etcpal_thread_join(&thread_);
}

inline bool Thread::joinable() const noexcept
{
  return joinable_;
}

inline void Thread::Sleep(int ms)
{
  etcpal_thread_sleep(ms);
}

};  // namespace etcpal

#endif  // ETCPAL_CPP_THREAD_H_
