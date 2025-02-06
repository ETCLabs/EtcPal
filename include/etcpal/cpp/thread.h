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

/// @file etcpal/cpp/thread.h
/// @brief C++ wrapper and utilities for etcpal/thread.h

#ifndef ETCPAL_CPP_THREAD_H_
#define ETCPAL_CPP_THREAD_H_

#include <algorithm>
#include <cassert>
#include <chrono>
#include <functional>
#include <limits>
#include <memory>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>

#include <etcpal/common.h>
#include <etcpal/thread.h>

#include <etcpal/cpp/common.h>
#include <etcpal/cpp/error.h>
#include <etcpal/cpp/stop_token.h>

namespace etcpal
{
/// @defgroup etcpal_cpp_thread thread (Threading)
/// @ingroup etcpal_cpp
/// @brief C++ utilities for the @ref etcpal_thread module.
///
/// Provides a thread class Thread which is similar to std::thread, with the key advantage that it
/// will work on any threaded platform that EtcPal is ported for, including the embedded RTOS
/// platforms. If your application or library does not need to run on these platforms, consider
/// using std::thread instead (or in >= C++20, std::jthread, which more closely matches the
/// behavior of this class).
///
/// To start the thread with default parameters, you can use the constructor:
/// @code
/// void MyThreadFunction()
/// {
///   std::cout << "Hello from a thread!\n";
/// }
///
/// etcpal::Thread thread(MyThreadFunction); // Thread is now running
/// // Thread will be automatically joined when thread goes out of scope, or join it explicitly:
/// thread.Join();
/// @endcode
///
/// The value constructor will throw an etcpal::Error object if the thread fails to start for any
/// reason. To avoid an exception-handling approach, you can default-construct and use the Start()
/// function:
/// @code
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
/// @endcode
///
/// This approach also lets you set non-default thread parameters before starting; method-chaining
/// syntax is available on these setters:
/// @code
/// etcpal::Thread thread;
/// etcpal::Error start_result = thread.SetName("My Thread")
///                                     .SetPriority(4)
///                                     .SetStackSize(4000)
///                                     .Start(MyThreadFunction);
/// @endcode
///
/// Pass arguments to your thread function, which will be stored on the heap:
/// @code
/// void MyThreadFunction(int value)
/// {
///   std::cout << "The value is " << value << ".\n";
/// }
///
/// etcpal::Thread thread(MyThreadFunction, 5); // Thread will print "The value is 5."
/// @endcode
///
/// You can even use lambdas for quick-and-dirty operations:
/// @code
/// int value = 42;
/// etcpal::Thread thread([=]() { std::cout << "The value is " << value << '\n'; });
/// @endcode
///
/// The Thread class also provides static Sleep() functions, which can be used as a
/// platform-neutral sleep.
/// @code
/// etcpal::Thread::Sleep(20); // Sleep for 20 milliseconds
/// etcpal::Thread::Sleep(std::chrono::seconds(2)); // Sleep for 2 seconds
///
/// // In a C++14 or greater environment, you can also...
///
/// using namespace std::chrono_literals;
///
/// etcpal::Thread::Sleep(1min); // Sleep for 1 minute
/// @endcode
///
/// **IMPORTANT NOTE FOR RTOS USERS:** The EtcPal threading API does not initialize the scheduler
/// on real-time systems (e.g. it does not call vTaskStartScheduler() on FreeRTOS or _mqx() on
/// MQX). The application writer is responsible for managing the interaction between starting the
/// scheduler and starting EtcPal threads, just like when starting native RTOS tasks.
///
/// **IMPORTANT NOTES FOR ZEPHYR USERS:** Some EtcPal features are dependent on Zephyr configuration see: <a
/// href="targeting_zephyr.html">Targeting Zephyr</a>

/// @ingroup etcpal_cpp_thread
/// @brief A thread class, modeled after std::thread.
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
template <typename Allocator = polymorphic_allocator<>>
class BasicThread
{
public:
  /// @brief Create a new thread object which does not yet represent a thread.
  BasicThread() = default;
  template <class Function, class... Args, std::enable_if_t<detail::IsCallable<Function, Args...>::value>* = nullptr>
  BasicThread(Function&& func, Args&&... args);
  template <class Function, class... Args, std::enable_if_t<detail::IsCallable<Function, Args...>::value>* = nullptr>
  BasicThread(const Allocator& alloc, Function&& func, Args&&... args);
  BasicThread(unsigned int priority, unsigned int stack_size, const char* name, void* platform_data = nullptr);

  /// Deleted copy constructor - threads are not copyable
  BasicThread(const BasicThread& other) = delete;
  BasicThread(BasicThread&& other) noexcept;

  virtual ~BasicThread();

  /// Deleted copy assignment operator - threads are not copyable
  BasicThread& operator=(const BasicThread& other) = delete;
  BasicThread& operator=(BasicThread&& other) noexcept;

  bool joinable() const noexcept;

  /// @name Getters
  /// @{
  unsigned int              priority() const noexcept;
  unsigned int              stack_size() const noexcept;
  const char*               name() const noexcept;
  void*                     platform_data() const noexcept;
  const EtcPalThreadParams& params() const noexcept;
  etcpal_thread_os_handle_t os_handle() const noexcept;
  /// @}

  /// @name Setters
  /// @{
  BasicThread& SetPriority(unsigned int priority) noexcept;
  BasicThread& SetStackSize(unsigned int stack_size) noexcept;
  BasicThread& SetName(const char* name) noexcept;
  BasicThread& SetName(const std::string& name) noexcept;
  BasicThread& SetPlatformData(void* platform_data) noexcept;
  BasicThread& SetParams(const EtcPalThreadParams& params) noexcept;
  /// @}

  template <class Function, class... Args, typename = std::enable_if_t<detail::IsCallable<Function, Args...>::value>>
  Error Start(const Allocator& alloc, Function&& func, Args&&... args);
  template <class Function, class... Args, typename = std::enable_if_t<detail::IsCallable<Function, Args...>::value>>
  Error Start(Function&& func, Args&&... args);
  Error Join(int timeout_ms = ETCPAL_WAIT_FOREVER) noexcept;
  Error Terminate() noexcept;

  static Error Sleep(unsigned int ms) noexcept;
  template <typename Rep, typename Period>
  static Error Sleep(const std::chrono::duration<Rep, Period>& sleep_duration) noexcept;

  /// @cond
  using FunctionType = MoveOnlyFunction<void(), Allocator>;
  /// @endcond

private:
  struct CtrlBlock
  {
    FunctionType    entry_point;
    etcpal_thread_t thread;
  };

  std::unique_ptr<CtrlBlock, DeleteUsingAlloc<CtrlBlock, Allocator>> thread_ = nullptr;
  EtcPalThreadParams                                                 params_{ETCPAL_THREAD_PARAMS_INIT_VALUES};
};

using Thread = BasicThread<>;

/// @brief A joining thread with cooperative cancellation and allocator support.
/// @tparam Allocator The type of allocator to allocate memory with.
template <typename Allocator = polymorphic_allocator<>>
class JThread
{
public:
  using native_handle_type = etcpal_thread_os_handle_t;  //!< The underlying native thread handle type.

  /// @name Constructors
  ///
  /// @brief Construct a thread that executes the given entry point function using the given arguments.
  ///
  /// If the function `fun` is capable of accepting a `StopToken` as its first argument, a token associated with this
  /// thread object will be passed to it. On destruction, the thread object issues a cancellation to that token,
  /// allowing cooperative cancellation.
  ///
  /// @tparam Fun  The type of entry point function to execute.
  /// @tparam Args The types of arguments to pass to the entry point function.
  ///
  /// @param params The paramters to create the thread using.
  /// @param alloc  The allocator to allocate memory using.
  /// @param fun    The entry point function to execute on the new thread.
  /// @param args   The arguments to pass to the entry point function.
  ///
  /// @{
  JThread() noexcept = default;
  template <typename Fun,
            typename... Args,
            std::enable_if_t<detail::IsCallable<Fun, const StopToken<Allocator>&, Args...>::value>* = nullptr>
  explicit JThread(const EtcPalThreadParams& params, const Allocator& alloc, Fun&& fun, Args&&... args);
  template <typename Fun,
            typename... Args,
            std::enable_if_t<detail::IsCallable<Fun, const StopToken<Allocator>&, Args...>::value>* = nullptr>
  explicit JThread(const EtcPalThreadParams& params, Fun&& fun, Args&&... args);
  template <typename Fun, typename... Args, std::enable_if_t<detail::IsCallable<Fun, Args...>::value>* = nullptr>
  explicit JThread(const EtcPalThreadParams& params, Fun&& fun, Args&&... args);
  template <typename Fun,
            typename... Args,
            std::enable_if_t<detail::IsCallable<Fun, const StopToken<Allocator>&, Args...>::value>* = nullptr>
  explicit JThread(const Allocator& alloc, Fun&& fun, Args&&... args);
  template <typename Fun,
            typename... Args,
            std::enable_if_t<detail::IsCallable<Fun, const StopToken<Allocator>&, Args...>::value>* = nullptr>
  explicit JThread(Fun&& fun, Args&&... args);
  template <typename Fun, typename... Args, std::enable_if_t<detail::IsCallable<Fun, Args...>::value>* = nullptr>
  explicit JThread(Fun&& fun, Args&&... args);
  /// @}

  JThread(const JThread& rhs)     = delete;   //!< Disallow copying a thread.
  JThread(JThread&& rhs) noexcept = default;  //!< Move a thread.

  ~JThread() noexcept { request_stop(); }  //!< Destroy a thread, issueing a cancellation request and performing a join.

  auto operator=(const JThread& rhs) -> JThread& = delete;  //!< Disallow copying a thread.
  auto operator=(JThread&& rhs) noexcept -> JThread&;  //!< Move assignment, cancelling and joining any existing thread.

  /// @name Observers
  /// @brief Obtain state information about the managed thread.
  /// @return The requested information.
  /// @{
  [[nodiscard]] bool joinable() const noexcept { return thread_.joinable(); }
  [[nodiscard]] auto native_handle() const noexcept { return thread_.os_handle(); }
  /// @}

  /// @brief Wait for the thread to join with the caller.
  void join() noexcept { thread_.Join(); }

  /// @name Cancellation
  /// @brief Manage cooperative thread cancellation.
  /// @return The associated cancellation token or source, or the status of a stop request.
  /// @{
  [[nodiscard]] auto get_stop_source() const noexcept { return ssource_; }
  [[nodiscard]] auto get_stop_token() const noexcept { return ssource_.get_token(); }
  bool               request_stop() noexcept { return ssource_.request_stop(); }
  /// @}

private:
  StopSource<Allocator>  ssource_ = StopSource<Allocator>{NoStopState};
  BasicThread<Allocator> thread_  = {};
};

/// @brief Create a new thread object and associate it with a new thread of execution.
///
/// Default thread parameters will be used. See the Start() function for more information.
///
/// @param func Callable object to execute in the new thread.
/// @param args Arguments to pass to func.
/// @throw std::runtime_error if Start() returns an error code.
/// @post `joinable() == true`
template <typename Allocator>
template <class Function, class... Args, std::enable_if_t<etcpal::detail::IsCallable<Function, Args...>::value>*>
BasicThread<Allocator>::BasicThread(const Allocator& alloc, Function&& func, Args&&... args)
{
  auto result = Start(alloc, std::forward<Function>(func), std::forward<Args>(args)...);
  if (!result)
    ETCPAL_THROW(std::runtime_error("Error while starting EtcPal thread: " + result.ToString()));
}

template <typename Allocator>
template <class Function, class... Args, std::enable_if_t<etcpal::detail::IsCallable<Function, Args...>::value>*>
BasicThread<Allocator>::BasicThread(Function&& func, Args&&... args)
    : BasicThread{Allocator{}, std::forward<Function>(func), std::forward<Args>(args)...}
{
}

/// @brief Create a new thread object which does not yet represent a thread, passing explicit parameters.
///
/// The thread is not running until Start() is called.
///
/// @param priority Priority of the thread.
/// @param stack_size Stack size of the thread in bytes.
/// @param name A name for the thread, maximum length #ETCPAL_THREAD_NAME_MAX_LENGTH.
/// @param platform_data Pointer to a platform-specific parameter structure. See
///                      #EtcPalThreadParams for more information.
template <typename Allocator>
BasicThread<Allocator>::BasicThread(unsigned int priority,
                                    unsigned int stack_size,
                                    const char*  name,
                                    void*        platform_data)
    : params_{priority, stack_size, name, platform_data}
{
}

template <typename Allocator>
BasicThread<Allocator>::BasicThread(BasicThread&& rhs) noexcept
    : thread_{std::move(rhs.thread_)}
    , params_{std::exchange(rhs.params_, EtcPalThreadParams{ETCPAL_THREAD_PARAMS_INIT_VALUES})}
{
}

/// @brief Destroy the thread object.
///
/// If *this has a valid associated thread (`joinable() == true`), the thread is joined before the
/// destructor finishes.
template <typename Allocator>
BasicThread<Allocator>::~BasicThread()
{
  if (joinable())
  {
    Join();
  }
}

template <typename Allocator>
auto BasicThread<Allocator>::operator=(BasicThread&& rhs) noexcept -> BasicThread&
{
  thread_ = std::move(rhs.thread_);
  params_ = std::exchange(rhs.params_, EtcPalThreadParams{ETCPAL_THREAD_PARAMS_INIT_VALUES});
  return *this;
}

/// @brief Whether the thread object identifies an active thread of execution.
template <typename Allocator>
bool BasicThread<Allocator>::joinable() const noexcept
{
  return (bool)thread_;
}

/// @brief Get the priority of this thread (not valid on all platforms).
template <typename Allocator>
unsigned int BasicThread<Allocator>::priority() const noexcept
{
  return params_.priority;
}

/// @brief Get the stack size of this thread in bytes (not valid on all platforms).
template <typename Allocator>
unsigned int BasicThread<Allocator>::stack_size() const noexcept
{
  return params_.stack_size;
}

/// @brief Get the name of this thread.
template <typename Allocator>
const char* BasicThread<Allocator>::name() const noexcept
{
  return params_.thread_name;
}

/// @brief Get the platform-specific data of this thread.
template <typename Allocator>
void* BasicThread<Allocator>::platform_data() const noexcept
{
  return params_.platform_data;
}

/// @brief Get a reference the parameters of this thread.
template <typename Allocator>
const EtcPalThreadParams& BasicThread<Allocator>::params() const noexcept
{
  return params_;
}

/// @brief Get the native OS handle of this thread.
/// @return The thread's OS handle, or ETCPAL_THREAD_OS_HANDLE_INVALID if the thread is not running
///         (`joinable() == false`).
template <typename Allocator>
etcpal_thread_os_handle_t BasicThread<Allocator>::os_handle() const noexcept
{
  return thread_ ? etcpal_thread_get_os_handle(std::addressof(thread_->thread)) : ETCPAL_THREAD_OS_HANDLE_INVALID;
}

/// @brief Set the priority of this thread.
///
/// Priority is not valid on all platforms. This function does not have any effect on the
/// associated thread unless it is called on a default-constructed thread before Start() is called.
///
/// @param priority Priority to set.
/// @return A reference to this thread, for method chaining.
template <typename Allocator>
BasicThread<Allocator>& BasicThread<Allocator>::SetPriority(unsigned int priority) noexcept
{
  params_.priority = priority;
  return *this;
}

/// @brief Set the stack size of this thread in bytes.
///
/// Stack size is not valid on all platforms. This function does not have any effect on the
/// associated thread unless it is called on a default-constructed thread before Start() is called.
///
/// @param stack_size Stack size to set.
/// @return A reference to this thread, for method chaining.
template <typename Allocator>
BasicThread<Allocator>& BasicThread<Allocator>::SetStackSize(unsigned int stack_size) noexcept
{
  params_.stack_size = stack_size;
  return *this;
}

/// @brief Set the name of this thread.
///
/// This function does not have any effect on the associated thread unless it is called on a
/// default-constructed thread before Start() is called. The pointer passed to this function must
/// remain valid until after Start() is called.
///
/// @param name Name to set.
/// @return A reference to this thread, for method chaining.
template <typename Allocator>
BasicThread<Allocator>& BasicThread<Allocator>::SetName(const char* name) noexcept
{
  params_.thread_name = name;
  return *this;
}

/// @brief Set the name of this thread.
///
/// This function does not have any effect on the associated thread unless it is called on a
/// default-constructed thread before Start() is called. The string reference passed to this
/// function must remain valid until after Start() is called.
///
/// @param name Name to set.
/// @return A reference to this thread, for method chaining.
template <typename Allocator>
BasicThread<Allocator>& BasicThread<Allocator>::SetName(const std::string& name) noexcept
{
  params_.thread_name = name.c_str();
  return *this;
}

/// @brief Set the platform-specific parameter data.
///
/// This function does not have any effect on the associated thread unless it is called on a
/// default-constructed thread before Start() is called. The pointer passed to this function must
/// remain valid until after Start() is called.
///
/// @param platform_data Pointer to platform-specific data structure.
/// @return A reference to this thread, for method chaining.
template <typename Allocator>
BasicThread<Allocator>& BasicThread<Allocator>::SetPlatformData(void* platform_data) noexcept
{
  params_.platform_data = platform_data;
  return *this;
}

/// @brief Set this thread's parameters from an existing EtcPalThreadParams struct.
///
/// This function does not have any effect on the associated thread unless it is called on a
/// default-constructed thread before Start() is called.
///
/// @param params EtcPalThreadParams to use when starting the thread.
/// @return A reference to this thread, for method chaining.
template <typename Allocator>
BasicThread<Allocator>& BasicThread<Allocator>::SetParams(const EtcPalThreadParams& params) noexcept
{
  params_ = params;
  return *this;
}

/// @brief Associate this thread object with a new thread of execution.
///
/// The new thread of execution starts executing
/// @code
/// std::invoke(decay_copy(std::forward<Function>(f)),
///             decay_copy(std::forward<Args>(args))...);
/// @endcode
/// where decay_copy is defined as
/// @code
/// template <class T>
/// std::decay<T>::type decay_copy(T&& v) { return std::forward<T>(v); }
/// @endcode
/// Except that the calls to decay_copy are evaluated in the context of the caller, so that any
/// exceptions thrown during evaluation and copying/moving of the arguments are thrown in the
/// current thread, without starting the new thread.
///
/// @param func Callable object to execute in the new thread.
/// @param args Arguments to pass to func.
/// @return #kEtcPalErrOk: The thread started successfully, `joinable()` is now true.
/// @return #kEtcPalErrNoMem: Allocation failed while starting thread.
/// @return #kEtcPalErrInvalid: The thread was already running (`joinable() == true`).
/// @return Other codes translated from system error codes are possible.
template <typename Allocator>
template <class Function, class... Args, typename>
Error BasicThread<Allocator>::Start(const Allocator& alloc, Function&& func, Args&&... args)
{
  if (thread_)
    return kEtcPalErrInvalid;

  ETCPAL_TRY
  {
    thread_ = allocate_unique<CtrlBlock>(
        alloc, CtrlBlock{FunctionType{std::bind(std::forward<Function>(func), std::forward<Args>(args)...), alloc}});
    Error create_res = etcpal_thread_create(
        std::addressof(thread_->thread), &params_, [](void* arg) { (*reinterpret_cast<FunctionType*>(arg))(); },
        std::addressof(thread_->entry_point));
    if (!create_res)
    {
      thread_.reset();
    }

    return create_res;
  }
  ETCPAL_CATCH(const std::bad_alloc& exe, { return kEtcPalErrNoMem; })
}

template <typename Allocator>
template <class Function, class... Args, typename>
Error BasicThread<Allocator>::Start(Function&& func, Args&&... args)
{
  return Start(Allocator{}, std::forward<Function>(func), std::forward<Args>(args)...);
}

/// @brief Wait for the thread to finish execution.
///
/// If timeout_ms is not given, or if #ETCPAL_THREAD_HAS_TIMED_JOIN is false on this platform,
/// blocks until the thread has exited. Otherwise, blocks up to timeout_ms waiting for the thread
/// to exit.
///
/// @param timeout_ms How long to wait for the thread to exit (default forever).
/// @return #kEtcPalErrOk: The thread was stopped; joinable() is now false.
/// @return #kEtcPalErrTimedOut: The thread did not join within the specified timeout.
/// @return #kEtcPalErrInvalid: The thread was not running (`joinable() == false`).
/// @return Other codes translated from system error codes are possible.
template <typename Allocator>
Error BasicThread<Allocator>::Join(int timeout_ms) noexcept
{
  if (!thread_)
    return kEtcPalErrInvalid;

  Error join_res = etcpal_thread_timed_join(std::addressof(thread_->thread), timeout_ms);
  if (join_res)
    thread_.reset();
  return join_res;
}

/// @brief Forcefully kill the thread.
///
/// **Be careful when using this function.** Depending on the state of a thread when it is
/// terminated, shared resources or memory could not be freed, resulting in memory leaks or
/// deadlocks.
///
/// @return #kEtcPalErrOk: The thread was terminated; joinable() is now false.
/// @return #kEtcPalErrInvalid: The thread was not running (`joinable() == false`).
/// @return Other codes translated from system error codes are possible.
template <typename Allocator>
Error BasicThread<Allocator>::Terminate() noexcept
{
  if (!thread_)
    return kEtcPalErrInvalid;

  Error terminate_res = etcpal_thread_terminate(thread_.get());
  if (terminate_res)
    thread_.reset();
  return terminate_res;
}

/// @brief Blocks the current thread for the specified number of milliseconds.
///
/// @return #kEtcPalErrOk: The sleep completed.
/// @return #kEtcPalErrSys: The system call may have been interrupted and awoke early.
template <typename Allocator>
Error BasicThread<Allocator>::Sleep(unsigned int ms) noexcept
{
  return etcpal_thread_sleep(ms);
}

/// @brief Blocks the current thread for the specified duration.
///
/// Note: Duration will be clamped to [0, UINT_MAX] milliseconds.
template <typename Allocator>
template <typename Rep, typename Period>
Error BasicThread<Allocator>::Sleep(const std::chrono::duration<Rep, Period>& sleep_duration) noexcept
{
  // This implementation cannot sleep longer than UINT_MAX.
  unsigned int sleep_ms_clamped = static_cast<unsigned int>(
      std::min(std::chrono::milliseconds(sleep_duration).count(),
               static_cast<std::chrono::milliseconds::rep>(std::numeric_limits<unsigned int>::max())));
  return Sleep(sleep_ms_clamped);
}

};  // namespace etcpal

template <typename Allocator>
template <typename Fun,
          typename... Args,
          std::enable_if_t<etcpal::detail::IsCallable<Fun, const etcpal::StopToken<Allocator>&, Args...>::value>*>
etcpal::JThread<Allocator>::JThread(const EtcPalThreadParams& params, const Allocator& alloc, Fun&& fun, Args&&... args)
    : ssource_{alloc}, thread_{params.priority, params.stack_size, params.thread_name, params.platform_data}
{
  const auto result = thread_.Start(alloc, std::forward<Fun>(fun), get_stop_token(), std::forward<Args>(args)...);
  if (!result)
  {
    ETCPAL_THROW(std::runtime_error("Error while starting EtcPal thread: " + result.ToString()));
  }
}

template <typename Allocator>
template <typename Fun,
          typename... Args,
          std::enable_if_t<etcpal::detail::IsCallable<Fun, const etcpal::StopToken<Allocator>&, Args...>::value>*>
etcpal::JThread<Allocator>::JThread(const EtcPalThreadParams& params, Fun&& fun, Args&&... args)
    : ssource_{}, thread_{params.priority, params.stack_size, params.thread_name, params.platform_data}
{
  const auto result = thread_.Start(std::forward<Fun>(fun), get_stop_token(), std::forward<Args>(args)...);
  if (!result)
  {
    ETCPAL_THROW(std::runtime_error("Error while starting EtcPal thread: " + result.ToString()));
  }
}

template <typename Allocator>
template <typename Fun, typename... Args, std::enable_if_t<etcpal::detail::IsCallable<Fun, Args...>::value>*>
etcpal::JThread<Allocator>::JThread(const EtcPalThreadParams& params, Fun&& fun, Args&&... args)
    : thread_{params.priority, params.stack_size, params.thread_name, params.platform_data}
{
  const auto result = thread_.Start(std::forward<Fun>(fun), std::forward<Args>(args)...);
  if (!result)
  {
    ETCPAL_THROW(std::runtime_error("Error while starting EtcPal thread: " + result.ToString()));
  }
}

template <typename Allocator>
template <typename Fun,
          typename... Args,
          std::enable_if_t<etcpal::detail::IsCallable<Fun, const etcpal::StopToken<Allocator>&, Args...>::value>*>
etcpal::JThread<Allocator>::JThread(const Allocator& alloc, Fun&& fun, Args&&... args)
    : ssource_{alloc}, thread_{alloc, std::forward<Fun>(fun), ssource_.get_token(), std::forward<Args>(args)...}
{
}

template <typename Allocator>
template <typename Fun,
          typename... Args,
          std::enable_if_t<etcpal::detail::IsCallable<Fun, const etcpal::StopToken<Allocator>&, Args...>::value>*>
etcpal::JThread<Allocator>::JThread(Fun&& fun, Args&&... args)
    : ssource_{}, thread_{std::forward<Fun>(fun), ssource_.get_token(), std::forward<Args>(args)...}
{
}

template <typename Allocator>
template <typename Fun, typename... Args, std::enable_if_t<etcpal::detail::IsCallable<Fun, Args...>::value>*>
etcpal::JThread<Allocator>::JThread(Fun&& fun, Args&&... args)
    : thread_{std::forward<Fun>(fun), std::forward<Args>(args)...}
{
}

template <typename Allocator>
auto etcpal::JThread<Allocator>::operator=(JThread&& rhs) noexcept -> JThread&
{
  request_stop();
  if (joinable())
  {
    join();
  }

  ssource_ = std::move(rhs.ssource_);
  thread_  = std::move(rhs.thread_);

  return *this;
}

#endif  // ETCPAL_CPP_THREAD_H_
