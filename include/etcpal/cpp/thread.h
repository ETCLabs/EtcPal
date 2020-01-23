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
///
/// Provides a thread class Thread which is similar to std::thread, with the key advantage that it
/// will work on any threaded platform that EtcPal is ported for, including the embedded RTOS
/// platforms. If your application or library does not need to run on these platforms, consider
/// using std::thread instead (or in >= C++20, std::jthread, which more closely matches the
/// behavior of this class).
///
/// To start the thread with default parameters, you can use the constructor:
/// \code
/// void MyThreadFunction()
/// {
///   std::cout << "Hello from a thread!\n";
/// }
///
/// etcpal::Thread thread(MyThreadFunction); // Thread is now running
/// // Thread will be automatically joined when thread goes out of scope, or join it explicitly:
/// thread.Join();
/// \endcode
///
/// The value constructor will throw an etcpal::Error object if the thread fails to start for any
/// reason. To avoid an exception-handling approach, you can default-construct and use the Start()
/// function:
/// \code
/// etcpal::Thread thread;
/// etcpal::Error start_result = thread.Start(MyThreadFunction);
/// if (start_result)
/// {
///   // Thread is running, yay!
/// }
/// else
/// {
///   std::cout << "Error starting thread: " << start_result.ToString() << '\n';
/// }
/// \endcode
///
/// This approach also lets you set non-default thread parameters before starting; method-chaining
/// syntax is available on these setters:
/// \code
/// etcpal::Thread thread;
/// etcpal::Error start_result = thread.SetName("My Thread")
///                                     .SetPriority(4)
///                                     .SetStackSize(4000)
///                                     .Start(MyThreadFunction);
/// \endcode
///
/// Pass arguments to your thread function, which will be stored on the heap:
/// \code
/// void MyThreadFunction(int value)
/// {
///   std::cout << "The value is " << value << ".\n";
/// }
///
/// etcpal::Thread thread(MyThreadFunction, 5); // Thread will print "The value is 5."
/// \endcode
///
/// You can even use lambdas for quick-and-dirty operations:
/// \code
/// etcpal::Thread thread([]() { std::cout << "Hello from a thread!\n"; });
/// \endcode
///
/// The Thread class also provides static Sleep() functions, which can be used as a
/// platform-neutral sleep.
/// \code
/// etcpal::Thread::Sleep(20); // Sleep for 20 milliseconds
/// etcpal::Thread::Sleep(std::chrono::seconds(2)); // Sleep for 2 seconds
///
/// // In a C++14 or greater environment, you can also...
///
/// using namespace std::chrono_literals;
///
/// etcpal::Thread::Sleep(1min); // Sleep for 1 minute
/// \endcode

/// \ingroup etcpal_cpp_thread
/// \brief A thread class, modeled after std::thread.
///
/// This class is similar to std::thread, with the key advantage that it will work on any threaded
/// platform that EtcPal is ported for, including the embedded RTOS platforms. If your application
/// or library does not need to run on these platforms, consider using std::thread instead.
///
/// There are a few differences from std::thread:
/// * The thread is automatically joined on destruction (this matches the behavior of C++20's
///   std::jthread)
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
  /// \brief Create a new thread object which does not yet represent a thread.
  Thread() = default;
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

  /// \name Getters
  /// @{
  unsigned int priority() const noexcept;
  unsigned int stack_size() const noexcept;
  const char* name() const noexcept;
  void* platform_data() const noexcept;
  const EtcPalThreadParams& params() const noexcept;
  /// @}

  /// \name Setters
  /// @{
  Thread& SetPriority(unsigned int priority) noexcept;
  Thread& SetStackSize(unsigned int stack_size) noexcept;
  Thread& SetName(const char* name) noexcept;
  Thread& SetName(const std::string& name) noexcept;
  Thread& SetPlatformData(void* platform_data) noexcept;
  /// @}

  template <class Function, class... Args>
  Error Start(Function&& func, Args&&... args);
  Error Join() noexcept;

  static void Sleep(unsigned int ms) noexcept;
  template <typename Rep, typename Period>
  static void Sleep(const std::chrono::duration<Rep, Period>& sleep_duration) noexcept;

  /// \cond
  using FunctionType = std::function<void()>;
  /// \endcond

private:
  std::unique_ptr<etcpal_thread_t> thread_;
  EtcPalThreadParams params_{ETCPAL_THREAD_DEFAULT_PARAMS_INIT};
};

/// \cond Internal thread function

extern "C" inline void CppThreadFn(void* arg)
{
  std::unique_ptr<Thread::FunctionType> p_func(static_cast<Thread::FunctionType*>(arg));
  // Tear the roof off the sucker
  (*p_func)();
}

/// \endcond

/// \brief Create a new thread object and associate it with a new thread of execution.
///
/// See the Start() function for more information.
///
/// \param func Callable object to execute in the new thread.
/// \param args Arguments to pass to func.
/// \throw etcpal::Error if Start() returns an error code.
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
/// After this call, *this has the parameters of other, and other is set to a default-constructed
/// state.
inline Thread::Thread(Thread&& other) noexcept
{
  *this = std::move(other);
}

/// \brief Move another thread into this thread.
///
/// If *this has a valid associated thread (`joinable() == true`), the behavior is undefined.
/// After this call, *this has the parameters of other, and other is set to a default-constructed
/// state.
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

/// \brief Get the priority of this thread (not valid on all platforms).
inline unsigned int Thread::priority() const noexcept
{
  return params_.priority;
}

/// \brief Get the stack size of this thread (not valid on all platforms).
inline unsigned int Thread::stack_size() const noexcept
{
  return params_.stack_size;
}

/// \brief Get the name of this thread.
inline const char* Thread::name() const noexcept
{
  return params_.thread_name;
}

/// \brief Get the platform-specific data of this thread.
inline void* Thread::platform_data() const noexcept
{
  return params_.platform_data;
}

/// \brief Get a reference the parameters of this thread.
inline const EtcPalThreadParams& Thread::params() const noexcept
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
Error Thread::Start(Function&& func, Args&&... args)
{
  if (thread_)
    return kEtcPalErrInvalid;

  thread_.reset(new etcpal_thread_t);

  auto new_f = std::unique_ptr<FunctionType>(
      new FunctionType(std::bind(std::forward<Function>(func), std::forward<Args>(args)...)));
  Error create_res = etcpal_thread_create(thread_.get(), &params_, CppThreadFn, new_f.get());
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
/// \return #kEtcPalErrOk: The thread was stopped; joinable() is now false.
/// \return #kEtcPalErrInvalid: The thread was not running (`joinable() == false`).
/// \return Other codes translated from system error codes are possible.
inline Error Thread::Join() noexcept
{
  if (thread_)
  {
    Error join_res = etcpal_thread_join(thread_.get());
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
///
/// Note: Duration will be clamped to [0, UINT_MAX] milliseconds.
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
