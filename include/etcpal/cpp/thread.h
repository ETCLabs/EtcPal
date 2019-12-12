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
#include <cassert>
#include <chrono>
#include <functional>
#include <limits>
#include <memory>
#include <string>
#include <system_error>
#include <type_traits>
#include <utility>
#include "etcpal/thread.h"
#include "etcpal/cpp/common.h"
#include "etcpal/cpp/error.h"

namespace etcpal
{
/// \defgroup etcpal_cpp_thread thread (Threading)
/// \ingroup etcpal_cpp
/// \brief C++ utilities for the \ref etcpal_thread module.

/// \brief A thread class, modeled after std::thread.
/// \ingroup etcpal_cpp_thread
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
/// * The thread has a Start() function - a previously default-constructed thread can be assigned
///   some operation parameters, then started, without being reassigned to another instance.
///
/// This is one of the few EtcPal wrappers that does heap allocation, as the thread function and
/// arguments need to be stored.
class Thread
{
public:
  /// The thread identifier type.
  using Id = etcpal_thread_id_t;
  /// An invalid value for Id.
  constexpr static Id kInvalidId = ETCPAL_THREAD_ID_INVALID;

  ETCPAL_CONSTEXPR_14 Thread() noexcept;
  template <class Function, class... Args>
  Thread(Function&& func, Args&&... args);
  virtual ~Thread();

  Thread(Thread&& other) noexcept;
  Thread& operator=(Thread&& other) noexcept;

  /// Deleted copy constructor - threads are not copyable
  Thread(const Thread& other) = delete;
  /// Deleted copy assignment operator - threads are not copyable
  Thread& operator=(const Thread& other) = delete;

  bool joinable() const noexcept;
  Id id() const noexcept;

  constexpr unsigned int priority() const noexcept;
  constexpr unsigned int stack_size() const noexcept;
  constexpr const char* name() const noexcept;
  constexpr void* platform_data() const noexcept;
  constexpr const EtcPalThreadParams& params() const noexcept;

  Thread& SetPriority(unsigned int priority) noexcept;
  Thread& SetStackSize(unsigned int stack_size) noexcept;
  Thread& SetName(const char* name) noexcept;
  Thread& SetName(const std::string& name) noexcept;
  Thread& SetPlatformData(void* platform_data) noexcept;

  template <class Function, class... Args>
  Result Start(Function&& func, Args&&... args);
  Result Join() noexcept;

  static void Sleep(unsigned int ms) noexcept;
  template <typename Rep, typename Period>
  static void Sleep(const std::chrono::duration<Rep, Period>& sleep_duration) noexcept;

  /// \cond
  using FunctionType = std::function<void()>;
  /// \endcond

private:
  std::unique_ptr<etcpal_thread_t> thread_;
  EtcPalThreadParams params_;
};

extern "C" inline void ThreadFn(void* arg)
{
  std::unique_ptr<Thread::FunctionType> p_func(static_cast<Thread::FunctionType*>(arg));
  // Tear the roof off the sucker
  (*p_func)();
}

/// \brief Create a new thread object which does not yet represent a thread.
ETCPAL_CONSTEXPR_14_OR_INLINE Thread::Thread() noexcept
{
  ETCPAL_THREAD_SET_DEFAULT_PARAMS(&params_);
}

/// \brief Create a new thread object and associate it with a new thread of execution.
///
/// See the Start() function for more information.
///
/// \param func Callable object to execute in the new thread.
/// \param args Arguments to pass to func.
/// \throw etcpal::Result if Start() returns an error code.
/// \post `joinable() == true`
template <class Function, class... Args>
inline Thread::Thread(Function&& func, Args&&... args)
{
  ETCPAL_THREAD_SET_DEFAULT_PARAMS(&params_);
  auto result = Start(std::forward<Function>(func), std::forward<Args>(args)...);
  if (!result)
    throw result;
}

/// \brief Destroy the thread object.
///
/// If *this has a valid associated thread (`joinable() == true`), the thread is joined before the
/// destructor finishes.
inline Thread::~Thread()
{
  if (thread_)
    etcpal_thread_join(thread_.get());
}

/// \brief Move another thread into this thread.
///
/// If *this has a valid associated thread (`joinable() == true`), the behavior is undefined.
/// After this call, *this has the parameters and ID of other, and other is set to a
/// default-constructed state.
inline Thread::Thread(Thread&& other) noexcept
{
  *this = std::move(other);
}

/// \brief Move another thread into this thread.
///
/// If *this has a valid associated thread (`joinable() == true`), the behavior is undefined.
/// After this call, *this has the parameters and ID of other, and other is set to a
/// default-constructed state.
inline Thread& Thread::operator=(Thread&& other) noexcept
{
  thread_ = std::move(other.thread_);
  params_ = other.params_;
  ETCPAL_THREAD_SET_DEFAULT_PARAMS(&other.params_);
  return *this;
}

/// \brief Whether the thread object identifies an active thread of execution.
inline bool Thread::joinable() const noexcept
{
  return (bool)thread_;
}

/// \brief Get the ID of this thread.
/// \return The thread ID if joinable() is true, or Thread::kInvalidId if joinable() is false.
inline Thread::Id Thread::id() const noexcept
{
  return (thread_ ? ETCPAL_THREAD_GET_ID(thread_.get()) : kInvalidId);
}

/// \brief Get the priority of this thread (not valid on all platforms).
constexpr unsigned int Thread::priority() const noexcept
{
  return params_.priority;
}

/// \brief Get the stack size of this thread (not valid on all platforms).
constexpr unsigned int Thread::stack_size() const noexcept
{
  return params_.stack_size;
}

/// \brief Get the name of this thread.
constexpr const char* Thread::name() const noexcept
{
  return params_.thread_name;
}

/// \brief Get the platform-specific data of this thread.
constexpr void* Thread::platform_data() const noexcept
{
  return params_.platform_data;
}

/// \brief Get a reference the parameters of this thread.
constexpr const EtcPalThreadParams& Thread::params() const noexcept
{
  return params_;
}

/// \brief Set the priority of this thread.
///
/// Priority is not valid on all platforms. This function does not have any effect on the
/// associated thread unless it is called on a default-constructed thread before Start() is called.
///
/// \param priority Priority to set.
/// \return A reference to this thread, for method chaining.
inline Thread& Thread::SetPriority(unsigned int priority) noexcept
{
  params_.priority = priority;
  return *this;
}

/// \brief Set the stack size of this thread.
///
/// Stack size is not valid on all platforms. This function does not have any effect on the
/// associated thread unless it is called on a default-constructed thread before Start() is called.
///
/// \param stack_size Stack size to set.
/// \return A reference to this thread, for method chaining.
inline Thread& Thread::SetStackSize(unsigned int stack_size) noexcept
{
  params_.stack_size = stack_size;
  return *this;
}

/// \brief Set the name of this thread.
///
/// This function does not have any effect on the associated thread unless it is called on a
/// default-constructed thread before Start() is called. The pointer passed to this function must
/// remain valid until after Start() is called.
///
/// \param name Name to set.
/// \return A reference to this thread, for method chaining.
inline Thread& Thread::SetName(const char* name) noexcept
{
  params_.thread_name = name;
  return *this;
}

/// \brief Set the name of this thread.
///
/// This function does not have any effect on the associated thread unless it is called on a
/// default-constructed thread before Start() is called. The string reference passed to this
/// function must remain valid until after Start() is called.
///
/// \param name Name to set.
/// \return A reference to this thread, for method chaining.
inline Thread& Thread::SetName(const std::string& name) noexcept
{
  params_.thread_name = name.c_str();
  return *this;
}

/// \brief Set the platform-specific parameter data.
///
/// This function does not have any effect on the associated thread unless it is called on a
/// default-constructed thread before Start() is called. The pointer passed to this function must
/// remain valid until after Start() is called.
///
/// \param platform_data Pointer to platform-specific data structure.
/// \return A reference to this thread, for method chaining.
inline Thread& Thread::SetPlatformData(void* platform_data) noexcept
{
  params_.platform_data = platform_data;
  return *this;
}

/// \brief Associate this thread object with a new thread of execution.
///
/// The new thread of execution starts executing
/// \code
/// std::invoke(decay_copy(std::forward<Function>(f)),
///             decay_copy(std::forward<Args>(args))...);
/// \endcode
/// where decay_copy is defined as
/// \code
/// template <class T>
/// std::decay<T>::type decay_copy(T&& v) { return std::forward<T>(v); }
/// \endcode
/// Except that the calls to decay_copy are evaluated in the context of the caller, so that any
/// exceptions thrown during evaluation and copying/moving of the arguments are thrown in the
/// current thread, without starting the new thread.
///
/// \param func Callable object to execute in the new thread.
/// \param args Arguments to pass to func.
/// \return #kEtcPalErrOk: The thread started successfully, `joinable()` is now true.
/// \return #kEtcPalErrInvalid: The thread was already running (`joinable() == true`).
/// \return Other codes translated from system error codes are possible.
template <class Function, class... Args>
Result Thread::Start(Function&& func, Args&&... args)
{
  if (thread_)
    return kEtcPalErrInvalid;

  thread_.reset(new etcpal_thread_t);

  auto new_f = std::unique_ptr<FunctionType>(
      new FunctionType(std::bind(std::forward<Function>(func), std::forward<Args>(args)...)));
  Result create_res = etcpal_thread_create(thread_.get(), &params_, ThreadFn, new_f.get());
  if (create_res)
    new_f.release();
  else
    thread_.reset();
  return create_res;
}

/// \brief Wait for the thread to finish execution.
///
/// Blocks until the thread has exited.
///
/// \return #kEtcPalErrOk: The thread was stopped.
/// \return #kEtcPalErrInvalid: The thread was not running (`joinable() == false`).
/// \return Other codes translated from system error codes are possible.
/// \post joinable() is false.
inline Result Thread::Join() noexcept
{
  if (thread_)
  {
    Result join_res = etcpal_thread_join(thread_.get());
    if (join_res)
      thread_.reset();
    return join_res;
  }
  else
  {
    return kEtcPalErrInvalid;
  }
}

/// \brief Blocks the current thread for the specified number of milliseconds.
inline void Thread::Sleep(unsigned int ms) noexcept
{
  etcpal_thread_sleep(ms);
}

/// \brief Blocks the current thread for the specified duration.
template <typename Rep, typename Period>
void Thread::Sleep(const std::chrono::duration<Rep, Period>& sleep_duration) noexcept
{
  // This implementation cannot sleep longer than UINT_MAX.
  unsigned int sleep_ms_clamped = static_cast<unsigned int>(
      std::min(std::chrono::milliseconds(sleep_duration).count(),
               static_cast<std::chrono::milliseconds::rep>(std::numeric_limits<unsigned int>::max())));
  Sleep(sleep_ms_clamped);
}

};  // namespace etcpal

#endif  // ETCPAL_CPP_THREAD_H_
