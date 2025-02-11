#pragma once

#include <array>
#include <chrono>
#include <queue>

#include <etcpal/cpp/common.h>
#include <etcpal/cpp/event_group.h>
#include <etcpal/cpp/functional.h>
#include <etcpal/cpp/optional.h>
#include <etcpal/cpp/overload.h>
#include <etcpal/cpp/rwlock.h>
#include <etcpal/cpp/synchronized.h>
#include <etcpal/cpp/thread.h>
#include <etcpal/cpp/variant.h>

namespace etcpal
{

/// @brief Status of an operation on a future or promised value.
enum class FutureStatus : unsigned int
{
  ready     = 1 << 0,  //!< promise has been fulfilled with either a value or an exception
  timeout   = 1 << 1,  //!< timed out waiting for the future
  deferred  = 1 << 2,  //!< promise fulfillment has been deferred
  abandoned = 1 << 3,  //!< promise has been abandoned before being fulfilled
  continued = 1 << 4,  //!< future has had its continuation invoked
  consumed  = 1 << 5,  //!< future value has been obtained
  no_state  = 1 << 6   //!< future has no associated state
};

ETCPAL_NODISCARD constexpr auto operator~(FutureStatus value) noexcept
{
  return static_cast<FutureStatus>(~static_cast<unsigned int>(value));
}
ETCPAL_NODISCARD constexpr auto operator&(FutureStatus lhs, FutureStatus rhs) noexcept
{
  return static_cast<FutureStatus>(static_cast<unsigned int>(lhs) & static_cast<unsigned int>(rhs));
}
ETCPAL_NODISCARD constexpr auto operator|(FutureStatus lhs, FutureStatus rhs) noexcept
{
  return static_cast<FutureStatus>(static_cast<unsigned int>(lhs) | static_cast<unsigned int>(rhs));
}
ETCPAL_NODISCARD constexpr auto operator^(FutureStatus lhs, FutureStatus rhs) noexcept
{
  return static_cast<FutureStatus>(static_cast<unsigned int>(lhs) ^ static_cast<unsigned int>(rhs));
}
template <typename IntType>
ETCPAL_NODISCARD constexpr auto operator<<(FutureStatus lhs, IntType rhs) noexcept
{
  return static_cast<FutureStatus>(static_cast<unsigned int>(lhs) << rhs);
}
template <typename IntType>
ETCPAL_NODISCARD constexpr auto operator>>(FutureStatus lhs, IntType rhs) noexcept
{
  return static_cast<FutureStatus>(static_cast<unsigned int>(lhs) >> rhs);
}
ETCPAL_NODISCARD constexpr auto& operator&=(FutureStatus& lhs, FutureStatus rhs) noexcept
{
  return lhs = lhs & rhs;
}
ETCPAL_NODISCARD constexpr auto& operator|=(FutureStatus& lhs, FutureStatus rhs) noexcept
{
  return lhs = lhs | rhs;
}
ETCPAL_NODISCARD constexpr auto& operator^=(FutureStatus& lhs, FutureStatus rhs) noexcept
{
  return lhs = lhs ^ rhs;
}
template <typename IntType>
ETCPAL_NODISCARD constexpr auto& operator<<=(FutureStatus& lhs, IntType rhs) noexcept
{
  return lhs = lhs << rhs;
}
template <typename IntType>
ETCPAL_NODISCARD constexpr auto& operator>>=(FutureStatus& lhs, IntType rhs) noexcept
{
  return lhs = lhs >> rhs;
}

/// @brief Errors that may happen to an operation on a future or promised value.
enum class FutureErrc : unsigned int
{
  no_state,                   //!< no associated shared state
  timeout,                    //!< timed out waiting for an operation to complete
  broken_promise,             //!< the associated promise has been abandoned
  future_already_promised,    //!< a future value has already been promised to a caller
  future_already_retrieved,   //!< a future value has already been retrieved by a caller
  promise_already_satisfied,  //!< the promise has already been fulfilled with a value or exception
  continuation_already_set    //!< the promise has already had a continuation assigned
};

template <typename T, typename Allocator = polymorphic_allocator<>>
class Promise;
template <typename T, typename Allocator = polymorphic_allocator<>>
class Future;

/// @brief An exceptional condition that occurred while fulfilling a promise or obtaining a future value.
class FutureError : public std::logic_error
{
public:
  /// @brief Construct a generic future error.
  FutureError() : std::logic_error{"future error"} {}
  /// @brief Construct an exception represending the given future-related error condition.
  /// @param status The type of exceptional condition that occurred.
  FutureError(FutureErrc status)
      : std::logic_error{(status == FutureErrc::no_state)                    ? "no associated future state"
                         : (status == FutureErrc::future_already_promised)   ? "broken promise"
                         : (status == FutureErrc::future_already_retrieved)  ? "future value has already been retrieved"
                         : (status == FutureErrc::promise_already_satisfied) ? "promise has already been satisfied"
                         : (status == FutureErrc::continuation_already_set)
                             ? "promise has already had a continuation assigned"
                             : "future error"}
  {
  }

private:
  template <typename T, typename Allocator>
  friend class Promise;
  template <typename T, typename Allocator>
  friend class Future;

  FutureError(const std::string& what) : std::logic_error{what} {}
  FutureError(const char* what) : std::logic_error{what} {}
};

namespace detail
{

enum class FutureActionResult
{
  broken_promise,
  future_retrieve_succeeeded,
  future_already_retrieved,
  value_already_set,
  value_already_obtained,
  value_set_succeeded,
  value_get_succeeded,
  value_get_timeout,
  exception_already_set,
  exception_already_thrown,
  exception_throwing_required,
  exception_set_suceeded,
  continuation_already_set,
  continuation_already_invoked,
  continuation_invocation_required,
  continuation_set_suceeded
};

template <typename T, typename Allocator>
class FutureSharedState
{
public:
  using Type = std::conditional_t<std::is_same<T, void>::value, std::nullptr_t, T>;
  using Task = MoveOnlyFunction<void(FutureStatus, Optional<Type>&, std::exception_ptr), Allocator>;

  struct StateChangeResult
  {
    FutureActionResult result;
    FutureStatus       status;
    Optional<Type>&    value;
    std::exception_ptr exception;
    Task               continuation;
  };

  explicit FutureSharedState(const Allocator& alloc) noexcept : allocator_{alloc} {}

  ETCPAL_NODISCARD auto set_future_retrieved() noexcept -> FutureActionResult;
  template <typename U, typename = std::enable_if_t<std::is_convertible<U, Type>::value>>
  ETCPAL_NODISCARD auto set_value(U&& value) noexcept -> StateChangeResult;
  ETCPAL_NODISCARD auto set_exception(std::exception_ptr exception) noexcept -> StateChangeResult;
  template <typename Func>
  ETCPAL_NODISCARD auto set_continuation(Func&& continuation) noexcept -> StateChangeResult;
  template <typename Rep, typename Period>
  ETCPAL_NODISCARD auto get_value(const std::chrono::duration<Rep, Period>& timeout) noexcept -> StateChangeResult;
  template <typename Rep, typename Period>
  ETCPAL_NODISCARD auto await_value(const std::chrono::duration<Rep, Period>& timeout) noexcept -> StateChangeResult;
  void               abandon() noexcept;

  ETCPAL_NODISCARD auto get_allocator() const noexcept { return allocator_; }

  template <typename U = T, typename Arg, typename = std::enable_if_t<!std::is_same<U, void>::value>>
  ETCPAL_NODISCARD static auto return_value(Arg&& value) noexcept -> T;
  template <typename U = T, typename = std::enable_if_t<std::is_same<U, void>::value>>
  static void return_value(std::nullptr_t value) noexcept;

private:
  struct State
  {
    Optional<Type>     value_;
    Task               continuation_;
    std::exception_ptr exception_;
  };

  EventGroup                  status_;
  Synchronized<State, RwLock> state_;
  Allocator                   allocator_;
  std::atomic<bool>           future_retrieved_{false};
};

}  // namespace detail

/// @brief A type representing a promise to produce a value in the future.
///
/// A promise/future pair acts like a single-shot mailbox for some asynchronous task to eventually deliver a result of
/// type @a T. This implementation supports custom allocators. The custom allocator allocates the shared mailbox
/// storage, and any allocations required for future continuations.
///
/// @tparam T         The type of value being promised.
/// @tparam Allocator The type of allocator to allocate memory with.
template <typename T, typename Allocator>
class Promise
{
public:
  /// @brief Construct a promise using a default-constructed allocator.
  Promise() = default;
  /// @brief Construct a promise using the given allocator.
  /// @param alloc The allocator to use to perform any required memory allocations.
  explicit Promise(const Allocator& alloc);

  /// @name Move-Only
  /// @brief Mark promises as move-only.
  /// @param rhs The promise object to move from.
  /// @{
  Promise(const Promise& rhs)                        = delete;
  Promise(Promise&& rhs) noexcept                    = default;
  auto operator=(const Promise& rhs) -> Promise&     = delete;
  auto operator=(Promise&& rhs) noexcept -> Promise& = default;
  /// @}

  /// @brief Destroy a fulfilled promise, or abaondon an unfilled one.
  ~Promise() noexcept;

  /// @brief Obtain a handle to the promised future value.
  /// @return Return the promise of a future value.
  ETCPAL_NODISCARD auto get_future();

  /// @name Promise Fulfillment
  /// @brief Fulfill this promise with the given value.
  /// @param value The value to fulfill the promise with.
  /// @{
  template <typename U, typename = std::enable_if_t<std::is_convertible<U, T>::value>>
  void set_value(U&& value);
  template <typename U = T, typename = std::enable_if_t<std::is_same<U, void>::value>>
  void set_value();
  /// @}

private:
  std::shared_ptr<detail::FutureSharedState<T, Allocator>> state_ =
      std::allocate_shared<detail::FutureSharedState<T, Allocator>>(Allocator{}, Allocator{});
};

/// @brief A handle to a promised future value.
/// @tparam T         The type of future value being promised.
/// @tparam Allocator The type of allocator to allocate memory with.
template <typename T, typename Allocator>
class Future
{
public:
  /// @name Move-Only
  /// @brief Mark futures as move-only.
  /// @param rhs The future object to move from.
  /// @{
  Future(const Future& rhs)                        = delete;
  Future(Future&& rhs) noexcept                    = default;
  auto operator=(const Future& rhs) -> Future&     = delete;
  auto operator=(Future&& rhs) noexcept -> Future& = default;
  /// @}

  /// @name Value Access
  /// @brief Obtain the promised future value.
  /// @param timeout The maximum amount of time to wait for the future value.
  /// @return The future value itself, the exception the promise holder reported, or an error in awaiting the value.
  /// @{
  ETCPAL_NODISCARD auto get() -> T;
  ETCPAL_NODISCARD auto get_if() noexcept -> Variant<T, FutureErrc, std::exception_ptr>;
  template <typename Rep, typename Period>
  ETCPAL_NODISCARD auto get_if(const std::chrono::duration<Rep, Period>& timeout = std::chrono::milliseconds{
                                   ETCPAL_WAIT_FOREVER}) noexcept -> Variant<T, FutureErrc, std::exception_ptr>;
  template <typename Clock, typename Duration>
  ETCPAL_NODISCARD auto get_if(const std::chrono::time_point<Clock, Duration>& timeout) noexcept
      -> Variant<T, FutureErrc, std::exception_ptr>;
  /// @}

  /// @name Continuation
  ///
  /// @brief Set a contionuation to invoke when the associated promise is fulfilled.
  ///
  /// The continuation may be a single callable object, or multiple callable objects that can handle all the possible
  /// result types when combined.
  ///
  /// The continuations must support one of the following:
  /// - the expression `cont(status, result, exception)` must be valid, given:
  ///     - `status` is a value of type `FutureStatus` indicating the future completion result
  ///     - `result` is a value of type `Optional<T>` holding the promised result if it was indeed delivered
  ///     - `exception` is a value of type `std::exception_ptr` holding holding the exception, if any, the promiser
  ///         reported
  /// - the following requirements are satisfied:
  ///     - given:
  ///         - `conts` is the pseudo-overload-set formed by all callable objects `cont...`, with any overload
  ///             resolution ambiguity favoring the first object in the variadic list
  ///         - `status` is a value of type `FutureStatus` indicating the future completion result
  ///         - `result` is a value of type `T` holding the promised result
  ///         - `exception` is a value of type `std::exception_ptr` holding holding the exception the promiser reported
  ///     - then all of the following expressions are valid, and return types implicitly convertible to each other:
  ///         - `conts(status)`
  ///         - `conts(result)`
  ///         - `conts(exception)`
  ///
  /// If `conts...` supports both the three-argument and the single-argument expressions, this library may choose to use
  /// any of the expressions. In any case, as long as the future was obtained from a promise, the continuation will be
  /// invoked exactly once, whether the promise was fulfilled or abandoned. In the three-argument version, only one of
  /// `result` and `exception` will hold a value. In the single-argument version, `status` is guaranteed to report some
  /// sort of failure, and `exception` is guaranteed to hold an exception.
  ///
  /// @tparam F        The types of callable objects to invoke when the associated promise is fulfilled or abandoned.
  /// @tparam Executor The type of executor to post continuation execution through.
  ///
  /// @param cont The callable objects to invoke when the associated promise is fulfilled or abandoned.
  /// @param exec The executor to post coninuation execution through.
  ///
  /// @return The future return value of the continuation.
  ///
  /// @throws FutureError if there is no associated shared state.
  ///
  /// @{
  template <typename... F,
            typename = std::enable_if_t<
                is_invocable<detail::Selection<F...>, FutureStatus, Optional<T>&, std::exception_ptr>::value &&
                !(is_invocable<detail::Selection<F...>, FutureStatus>::value &&
                  is_invocable<detail::Selection<F...>, T>::value &&
                  is_invocable<detail::Selection<F...>, std::exception_ptr>::value)>>
  ETCPAL_NODISCARD auto and_then(F&&... cont)
      -> Future<invoke_result_t<detail::Selection<F...>, FutureStatus, Optional<T>&, std::exception_ptr>, Allocator>;
  template <typename... F,
            typename = std::enable_if_t<is_invocable<detail::Selection<F...>, FutureStatus>::value &&
                                        is_invocable<detail::Selection<F...>, T>::value &&
                                        is_invocable<detail::Selection<F...>, std::exception_ptr>::value>>
  ETCPAL_NODISCARD auto and_then(F&&... cont)
      -> Future<detail::CommonCVRefType_t<invoke_result_t<detail::Selection<F...>, FutureStatus>,
                                          invoke_result_t<detail::Selection<F...>, T>,
                                          invoke_result_t<detail::Selection<F...>, std::exception_ptr>>,
                Allocator>;
  template <typename Executor,
            typename F,
            typename... Rest,
            typename = std::enable_if_t<
                is_invocable<detail::Selection<F, Rest...>, FutureStatus, Optional<T>&, std::exception_ptr>::value &&
                !(is_invocable<detail::Selection<F, Rest...>, FutureStatus>::value &&
                  is_invocable<detail::Selection<F, Rest...>, T>::value &&
                  is_invocable<detail::Selection<F, Rest...>, std::exception_ptr>::value)>>
  ETCPAL_NODISCARD auto and_then(const Executor& exec, F&& cont, Rest&&... rest)
      -> Future<invoke_result_t<detail::Selection<F, Rest...>, FutureStatus, Optional<T>&, std::exception_ptr>,
                Allocator>;
  template <typename Executor,
            typename F,
            typename... Rest,
            typename = std::enable_if_t<is_invocable<detail::Selection<F, Rest...>, FutureStatus>::value &&
                                        is_invocable<detail::Selection<F, Rest...>, T>::value &&
                                        is_invocable<detail::Selection<F, Rest...>, std::exception_ptr>::value>>
  ETCPAL_NODISCARD auto and_then(const Executor& exec, F&& cont, Rest&&... rest)
      -> Future<detail::CommonCVRefType_t<invoke_result_t<detail::Selection<F, Rest...>, FutureStatus>,
                                          invoke_result_t<detail::Selection<F, Rest...>, T>,
                                          invoke_result_t<detail::Selection<F, Rest...>, std::exception_ptr>>,
                Allocator>;
  /// @}

  ETCPAL_NODISCARD bool valid() const noexcept;
  ETCPAL_NODISCARD auto wait() const noexcept -> FutureStatus;
  template <typename Rep, typename Period>
  ETCPAL_NODISCARD auto wait_for(const std::chrono::duration<Rep, Period>& timeout) const noexcept -> FutureStatus;
  template <typename Clock, typename Duration>
  ETCPAL_NODISCARD auto wait_until(const std::chrono::time_point<Clock, Duration>& timeout) const noexcept
      -> FutureStatus;

private:
  friend class Promise<T, Allocator>;

  explicit Future(const std::shared_ptr<detail::FutureSharedState<T, Allocator>>& state) noexcept : state_{state} {}

  std::shared_ptr<detail::FutureSharedState<T, Allocator>> state_ = {};
};

/// @brief Tag type to indicate an asynchronous operation should return a future.
struct UseFuture
{
};
/// @brief Tag value to indicate an asynchronous operation should return a future.
ETCPAL_INLINE_VARIABLE constexpr auto use_future = UseFuture{};

/// @brief A statically-sized thread pool with allocator support and thread configuration.
/// @tparam Allocator The type of allocator to use to allocate memory.
/// @tparam N         The number of threads the pool has.
template <std::size_t N, typename Allocator = polymorphic_allocator<>>
class ThreadPool
{
public:
  class Executor;  //!< The pool's executor type.

  /// @brief Construct the thread pool using the given allocator and thread specifications.
  /// @param params Parameters to initialize all pool threads with.
  /// @param alloc  The allocator to use to allocate memory.
  explicit ThreadPool(const EtcPalThreadParams& params, const Allocator& alloc = {});
  /// @brief Construct the thread pool using the default thread parameters and the given allocator.
  /// @param alloc The allocator to use to allocate memory.
  explicit ThreadPool(const Allocator& alloc);
  /// @brief Construct a thread pool using the default thread parameters and a default-constructed allocator.
  ThreadPool() : ThreadPool{Allocator{}} {}

  /// @brief Shut down the thread pool, and join all the pool threads.
  ~ThreadPool() noexcept;

  /// @name Task Posting
  ///
  /// @brief Post a task to be executed on the thread pool.
  ///
  /// The task posted to the thread pool must be invocable with no arguments. If the first argument is of type
  /// `UseFuture`, then the return value of the task is returned as a future. Otherwise, the return value is discarded.
  ///
  /// @tparam Alloc The type of allocator to use to allocate memory for this new task.
  /// @tparam F     The type of callable object to execute on the thread pool.
  ///
  /// @param tag   Tag indicating the task's return value should be returned in a future.
  /// @param fun   The callable object to execute on the thread pool.
  /// @param alloc The allocator to use to allocate memory for this new task.
  ///
  /// @return The future return value of the given task, or the number of tasks currently queued for execution.
  ///
  /// @{
  template <typename Alloc = Allocator, typename F>
  ETCPAL_NODISCARD auto post(UseFuture tag, F&& fun, const Alloc& alloc)
      -> Future<decltype(std::forward<F>(fun)()), Alloc>;
  template <typename F>
  ETCPAL_NODISCARD auto post(UseFuture tag, F&& fun) -> Future<decltype(std::forward<F>(fun)()), Allocator>;
  template <typename Alloc = Allocator, typename F>
  auto post(F&& fun, const Alloc& alloc);
  template <typename F>
  auto post(F&& fun);
  /// @}

  /// @brief Obtain an executor associated with this thread pool.
  /// @return An executor associated with this thread pool.
  ETCPAL_NODISCARD auto get_executor() noexcept { return Executor{*this}; }

private:
  using Task          = MoveOnlyFunction<void(), Allocator>;
  using TaskAllocator = typename std::allocator_traits<Allocator>::template rebind_alloc<Task>;
  using TaskQueue     = std::queue<Task, std::deque<Task, TaskAllocator>>;

  EventGroup                        queue_status_ = {};
  Synchronized<TaskQueue, RwLock>   queue_        = {};
  std::array<JThread<Allocator>, N> threads_      = {};
  Allocator                         allocator_    = {};
};

namespace detail
{

enum class QueueStatus : EventBits
{
  shut_down  = 1,                             // reserve lowest bit for shutdown flag
  count_mask = ~EventBits{} & ~EventBits{1},  // all other bits are the queue item count
  all_bits   = ~EventBits{}
};

ETCPAL_NODISCARD constexpr auto operator~(QueueStatus value) noexcept
{
  return static_cast<QueueStatus>(~static_cast<EventBits>(value));
}
ETCPAL_NODISCARD constexpr auto operator&(QueueStatus lhs, QueueStatus rhs) noexcept
{
  return static_cast<QueueStatus>(static_cast<EventBits>(lhs) & static_cast<EventBits>(rhs));
}
ETCPAL_NODISCARD constexpr auto operator|(QueueStatus lhs, QueueStatus rhs) noexcept
{
  return static_cast<QueueStatus>(static_cast<EventBits>(lhs) | static_cast<EventBits>(rhs));
}
ETCPAL_NODISCARD constexpr auto operator^(QueueStatus lhs, QueueStatus rhs) noexcept
{
  return static_cast<QueueStatus>(static_cast<EventBits>(lhs) ^ static_cast<EventBits>(rhs));
}
template <typename IntType>
ETCPAL_NODISCARD constexpr auto operator<<(QueueStatus lhs, IntType rhs) noexcept
{
  return static_cast<QueueStatus>(static_cast<EventBits>(lhs) << rhs);
}
template <typename IntType>
ETCPAL_NODISCARD constexpr auto operator>>(QueueStatus lhs, IntType rhs) noexcept
{
  return static_cast<QueueStatus>(static_cast<EventBits>(lhs) >> rhs);
}
ETCPAL_NODISCARD constexpr auto& operator&=(QueueStatus& lhs, QueueStatus rhs) noexcept
{
  return lhs = lhs & rhs;
}
ETCPAL_NODISCARD constexpr auto& operator|=(QueueStatus& lhs, QueueStatus rhs) noexcept
{
  return lhs = lhs | rhs;
}
ETCPAL_NODISCARD constexpr auto& operator^=(QueueStatus& lhs, QueueStatus rhs) noexcept
{
  return lhs = lhs ^ rhs;
}
template <typename IntType>
ETCPAL_NODISCARD constexpr auto& operator<<=(QueueStatus& lhs, IntType rhs) noexcept
{
  return lhs = lhs << rhs;
}
template <typename IntType>
ETCPAL_NODISCARD constexpr auto& operator>>=(QueueStatus& lhs, IntType rhs) noexcept
{
  return lhs = lhs >> rhs;
}

ETCPAL_NODISCARD constexpr auto to_queue_status(std::size_t queue_size) noexcept
{
  return static_cast<QueueStatus>(queue_size << 1);
}

template <typename T, typename Series, typename Lock>
ETCPAL_NODISCARD auto pop_one(Synchronized<std::queue<T, Series>, Lock>& sync_queue, EventGroup& queue_status) noexcept
    -> Optional<T>
{
  const auto queue = sync_queue.lock();
  if (queue->empty() || ((static_cast<detail::QueueStatus>(queue_status.GetBits()) & detail::QueueStatus::shut_down) ==
                         detail::QueueStatus::shut_down))
  {
    return {};
  }

  auto value = std::move(queue->front());
  queue->pop();
  queue_status.SetBits(static_cast<EventBits>(to_queue_status(queue->size())));

  return value;
}

ETCPAL_NODISCARD constexpr bool has_value(FutureStatus status) noexcept
{
  return (status & FutureStatus::ready) == FutureStatus::ready;
}

ETCPAL_NODISCARD constexpr bool is_abandoned(FutureStatus status) noexcept
{
  return (status & FutureStatus::abandoned) == FutureStatus::abandoned;
}

ETCPAL_NODISCARD constexpr bool is_consumed(FutureStatus status) noexcept
{
  return (status & (FutureStatus::consumed | FutureStatus::continued)) != FutureStatus{};
}

template <typename T, typename Allocator, typename F, typename... Args>
void fulfill_promise(Promise<T, Allocator>& promise, F&& function, Args&&... args)
{
  promise.set_value(std::forward<F>(function)(std::forward<Args>(args)...));
}

template <typename Allocator, typename F, typename... Args>
void fulfill_promise(Promise<void, Allocator>& promise, F&& function, Args&&... args)
{
  std::forward<F>(function)(std::forward<Args>(args)...);
  promise.set_value();
}

}  // namespace detail

}  // namespace etcpal

template <std::size_t N, typename Allocator>
class etcpal::ThreadPool<N, Allocator>::Executor
{
public:
  explicit Executor(ThreadPool& pool) noexcept : pool_{std::addressof(pool)} {}

  template <typename Alloc = Allocator, typename F>
  ETCPAL_NODISCARD auto post(UseFuture tag, F&& fun, const Alloc& alloc) const
      -> Future<decltype(std::forward<F>(fun)()), Alloc>
  {
    return pool_->post(tag, std::forward<F>(fun), alloc);
  }

  template <typename F>
  ETCPAL_NODISCARD auto post(UseFuture tag, F&& fun) const -> Future<decltype(std::forward<F>(fun)()), Allocator>
  {
    return pool_->post(tag, std::forward<F>(fun));
  }

  template <typename F, typename Alloc = Allocator>
  auto post(F&& fun, const Alloc& alloc) const
  {
    return pool_->post(std::forward<F>(fun), alloc);
  }

  template <typename F>
  auto post(F&& fun) const
  {
    return pool_->post(std::forward<F>(fun));
  }

private:
  ThreadPool* pool_;
};

template <typename T, typename Allocator>
ETCPAL_NODISCARD auto etcpal::detail::FutureSharedState<T, Allocator>::set_future_retrieved() noexcept
    -> FutureActionResult
{
  return future_retrieved_.exchange(true) ? FutureActionResult::future_already_retrieved
                                          : FutureActionResult::future_retrieve_succeeeded;
}

template <typename T, typename Allocator>
template <typename U, typename>
ETCPAL_NODISCARD auto etcpal::detail::FutureSharedState<T, Allocator>::set_value(U&& value) noexcept
    -> StateChangeResult
{
  const auto state  = state_.lock();
  const auto status = static_cast<FutureStatus>(status_.GetBits());
  if (state->exception_)
  {
    return {FutureActionResult::exception_already_set, status, state->value_, state->exception_, {}};
  }
  if (has_value(status))
  {
    return {FutureActionResult::value_already_set, status, state->value_, state->exception_, {}};
  }

  const auto new_status = state->continuation_ ? (FutureStatus::ready | FutureStatus::continued) : FutureStatus::ready;
  state->value_         = std::forward<U>(value);
  status_.SetBits(static_cast<EventBits>(new_status));

  return {state->continuation_ ? FutureActionResult::continuation_invocation_required
                               : FutureActionResult::value_set_succeeded,
          new_status, state->value_, state->exception_, std::move(state->continuation_)};
}

template <typename T, typename Allocator>
ETCPAL_NODISCARD auto etcpal::detail::FutureSharedState<T, Allocator>::set_exception(
    std::exception_ptr exception) noexcept -> StateChangeResult
{
  const auto state  = state_.lock();
  const auto status = static_cast<FutureStatus>(status_.GetBits());
  if (state->exception_)
  {
    return {FutureActionResult::exception_already_set, status, state->value_, state->exception_, {}};
  }
  if (has_value(status))
  {
    return {FutureActionResult::value_already_set, status, state->value_, state->exception_, {}};
  }

  const auto new_status = state->continuation_ ? (FutureStatus::ready | FutureStatus::continued) : FutureStatus::ready;
  state->exception_     = exception;
  status_.SetBits(static_cast<EventBits>(new_status));

  return {state->continuation_ ? FutureActionResult::continuation_invocation_required
                               : FutureActionResult::exception_set_suceeded,
          new_status, state->value_, state->exception_, std::move(state->continuation_)};
}

template <typename T, typename Allocator>
template <typename Cont>
ETCPAL_NODISCARD auto etcpal::detail::FutureSharedState<T, Allocator>::set_continuation(Cont&& continuation) noexcept
    -> StateChangeResult
{
  const auto state  = state_.lock();
  const auto status = static_cast<FutureStatus>(status_.GetBits());
  if (state->continuation_)
  {
    return {FutureActionResult::continuation_already_set, status, state->value_, state->exception_, {}};
  }

  if (!state->exception_ && !has_value(status))
  {
    state->continuation_ = Task{std::forward<Cont>(continuation), allocator_};
    return {FutureActionResult::continuation_set_suceeded, status, state->value_, state->exception_, {}};
  }

  const auto new_status = static_cast<FutureStatus>(status_.GetBits()) | FutureStatus::continued;
  status_.SetBits(static_cast<EventBits>(new_status));

  return {FutureActionResult::continuation_invocation_required, new_status, state->value_, state->exception_,
          Task{std::forward<Cont>(continuation), allocator_}};
}

template <typename T, typename Allocator>
template <typename Rep, typename Period>
ETCPAL_NODISCARD auto etcpal::detail::FutureSharedState<T, Allocator>::await_value(
    const std::chrono::duration<Rep, Period>& timeout) noexcept -> StateChangeResult
{
  const auto status = static_cast<FutureStatus>(
      status_.TryWait(static_cast<EventBits>(FutureStatus::ready | FutureStatus::abandoned), 0,
                      std::chrono::duration_cast<std::chrono::duration<int, std::milli>>(timeout).count()));
  const auto state = state_.lock();
  if (is_abandoned(status))
  {
    return {FutureActionResult::broken_promise, status, state->value_, state->exception_, {}};
  }
  if (!has_value(status))
  {
    return {FutureActionResult::value_get_timeout, status, state->value_, state->exception_, {}};
  }
  if (is_consumed(status))
  {
    return {FutureActionResult::value_already_obtained, status, state->value_, state->exception_, {}};
  }
  if (state->continuation_)
  {
    return {FutureActionResult::continuation_already_set, status, state->value_, state->exception_, {}};
  }

  return {FutureActionResult::value_get_succeeded, status, state->value_, state->exception_, {}};
}

template <typename T, typename Allocator>
void etcpal::detail::FutureSharedState<T, Allocator>::abandon() noexcept
{
  const auto state  = state_.lock();
  const auto status = static_cast<FutureStatus>(status_.GetBits());
  if (has_value(status))
  {
    return;
  }

  status_.SetBits(static_cast<EventBits>(FutureStatus::abandoned));
  state->continuation_ = nullptr;
}

template <typename T, typename Allocator>
template <typename U, typename Arg, typename>
ETCPAL_NODISCARD auto etcpal::detail::FutureSharedState<T, Allocator>::return_value(Arg&& value) noexcept -> T
{
  return std::move(value);
}

template <typename T, typename Allocator>
template <typename U, typename>
void etcpal::detail::FutureSharedState<T, Allocator>::return_value(std::nullptr_t value) noexcept
{
}

template <typename T, typename Allocator>
template <typename Rep, typename Period>
ETCPAL_NODISCARD auto etcpal::detail::FutureSharedState<T, Allocator>::get_value(
    const std::chrono::duration<Rep, Period>& timeout) noexcept -> StateChangeResult
{
  auto result = await_value(timeout);
  if (result.result != FutureActionResult::value_get_succeeded)
  {
    return result;
  }

  const auto state      = state_.lock();
  const auto new_status = result.status | (state->continuation_ ? FutureStatus::consumed
                                                                : (FutureStatus::consumed | FutureStatus::continued));
  status_.SetBits(static_cast<EventBits>(new_status));

  return {state->exception_ ? FutureActionResult::exception_throwing_required : FutureActionResult::value_get_succeeded,
          new_status, state->value_, state->exception_, std::move(state->continuation_)};
}

template <typename T, typename Allocator>
etcpal::Promise<T, Allocator>::Promise(const Allocator& alloc)
    : state_{std::allocate_shared<detail::FutureSharedState<T, Allocator>>(alloc, alloc)}
{
}

template <typename T, typename Allocator>
etcpal::Promise<T, Allocator>::~Promise() noexcept
{
  if (state_)
  {
    state_->abandon();
  }
}

template <typename T, typename Allocator>
ETCPAL_NODISCARD auto etcpal::Promise<T, Allocator>::get_future()
{
  if (!state_)
  {
    ETCPAL_THROW(FutureError{FutureErrc::no_state});
  }

  switch (state_->set_future_retrieved())
  {
    case detail::FutureActionResult::future_retrieve_succeeeded:
      return Future<T, Allocator>{state_};

    case detail::FutureActionResult::future_already_retrieved:
      ETCPAL_THROW(FutureError{FutureErrc::future_already_promised});
    default:
      ETCPAL_THROW(std::logic_error{"invalid promise status"});
  }
}

template <typename T, typename Allocator>
template <typename U, typename>
void etcpal::Promise<T, Allocator>::set_value(U&& value)
{
  if (!state_)
  {
    ETCPAL_THROW(FutureError{FutureErrc::no_state});
  }

  auto result = state_->set_value(std::forward<U>(value));
  switch (result.result)
  {
    case detail::FutureActionResult::value_set_succeeded:
      return;
    case detail::FutureActionResult::continuation_invocation_required:
      result.continuation(result.status, result.value, result.exception);
      return;

    case detail::FutureActionResult::value_already_set:
      ETCPAL_FALLTHROUGH;
    case detail::FutureActionResult::exception_already_set:
      ETCPAL_THROW(FutureError{FutureErrc::promise_already_satisfied});
    default:
      ETCPAL_THROW(std::logic_error{"invalid promise status"});
  }
}

template <typename T, typename Allocator>
template <typename U, typename>
void etcpal::Promise<T, Allocator>::set_value()
{
  if (!state_)
  {
    ETCPAL_THROW(FutureError{FutureErrc::no_state});
  }

  auto result = state_->set_value(nullptr);
  switch (result.result)
  {
    case detail::FutureActionResult::value_set_succeeded:
      return;
    case detail::FutureActionResult::continuation_invocation_required:
      result.continuation(result.status, result.value, result.exception);
      return;

    case detail::FutureActionResult::value_already_set:
      ETCPAL_FALLTHROUGH;
    case detail::FutureActionResult::exception_already_set:
      ETCPAL_THROW(FutureError{FutureErrc::promise_already_satisfied});
    default:
      ETCPAL_THROW(std::logic_error{"invalid promise status"});
  }
}

template <typename T, typename Allocator>
ETCPAL_NODISCARD auto etcpal::Future<T, Allocator>::get() -> T
{
  if (!state_)
  {
    ETCPAL_THROW(FutureError{FutureErrc::no_state});
  }

  auto result = state_->get_value(std::chrono::milliseconds{ETCPAL_WAIT_FOREVER});
  switch (result.result)
  {
    case detail::FutureActionResult::exception_throwing_required:
      std::rethrow_exception(result.exception);
    case detail::FutureActionResult::value_get_succeeded:
      return state_->return_value(std::move(*result.value));

    case detail::FutureActionResult::broken_promise:
      ETCPAL_THROW(FutureError{FutureErrc::broken_promise});
    case detail::FutureActionResult::value_already_obtained:
      ETCPAL_THROW(FutureError{FutureErrc::future_already_retrieved});
    case detail::FutureActionResult::continuation_already_set:
      ETCPAL_THROW(FutureError{FutureErrc::continuation_already_set});
    case detail::FutureActionResult::value_get_timeout:
      ETCPAL_THROW(std::logic_error{"timed out waiting forever for a future value"});
    default:
      ETCPAL_THROW(std::logic_error{"invalid future status"});
  }
}

template <typename T, typename Allocator>
ETCPAL_NODISCARD auto etcpal::Future<T, Allocator>::get_if() noexcept -> Variant<T, FutureErrc, std::exception_ptr>
{
  if (!state_)
  {
    return FutureErrc::no_state;
  }

  auto result = state_->get_value(std::chrono::milliseconds{ETCPAL_WAIT_FOREVER});
  switch (result.result)
  {
    case detail::FutureActionResult::exception_throwing_required:
      return result.exception;
    case detail::FutureActionResult::value_get_succeeded:
      return state_->return_value(std::move(*result.value));

    case detail::FutureActionResult::broken_promise:
      return FutureErrc::broken_promise;
    case detail::FutureActionResult::value_already_obtained:
      return FutureErrc::future_already_retrieved;
    case detail::FutureActionResult::continuation_already_set:
      return FutureErrc::continuation_already_set;
    default:
      std::abort();
  }
}

template <typename T, typename Allocator>
template <typename Rep, typename Period>
ETCPAL_NODISCARD auto etcpal::Future<T, Allocator>::get_if(const std::chrono::duration<Rep, Period>& timeout) noexcept
    -> Variant<T, FutureErrc, std::exception_ptr>
{
  if (!state_)
  {
    return FutureErrc::no_state;
  }

  auto result = state_->get_value(timeout);
  switch (result.result)
  {
    case detail::FutureActionResult::exception_throwing_required:
      return result.exception;
    case detail::FutureActionResult::value_get_succeeded:
      return state_->return_value(std::move(*result.value));

    case detail::FutureActionResult::broken_promise:
      return FutureErrc::broken_promise;
    case detail::FutureActionResult::value_already_obtained:
      return FutureErrc::future_already_retrieved;
    case detail::FutureActionResult::continuation_already_set:
      return FutureErrc::continuation_already_set;
    case detail::FutureActionResult::value_get_timeout:
      return FutureErrc::timeout;
    default:
      std::abort();
  }
}

template <typename T, typename Allocator>
template <typename... F, typename>
ETCPAL_NODISCARD auto etcpal::Future<T, Allocator>::and_then(F&&... cont)
    -> Future<invoke_result_t<detail::Selection<F...>, FutureStatus, Optional<T>&, std::exception_ptr>, Allocator>
{
  if (!state_)
  {
    ETCPAL_THROW(FutureError{FutureErrc::no_state});
  }

  auto promise =
      Promise<invoke_result_t<detail::Selection<F...>, FutureStatus, Optional<T>&, std::exception_ptr>, Allocator>{
          state_->get_allocator()};
  auto future = promise.get_future();
  auto result = state_->set_continuation(
      [fun = make_selection(std::forward<F>(cont)...), prom = std::move(promise)](
          auto status, auto& value, auto exception) mutable { prom.set_value(fun(status, value, exception)); });
  switch (result.result)
  {
    case detail::FutureActionResult::continuation_set_suceeded:
      break;
    case detail::FutureActionResult::continuation_invocation_required:
      result.continuation(result.status, result.value, result.exception);
      break;

    case detail::FutureActionResult::continuation_already_set:
      ETCPAL_THROW(FutureError{FutureErrc::continuation_already_set});
    default:
      ETCPAL_THROW(std::logic_error{"invalid promise status"});
  }

  return future;
}

template <typename T, typename Allocator>
template <typename... F, typename>
ETCPAL_NODISCARD auto etcpal::Future<T, Allocator>::and_then(F&&... cont)
    -> Future<detail::CommonCVRefType_t<invoke_result_t<detail::Selection<F...>, FutureStatus>,
                                        invoke_result_t<detail::Selection<F...>, T>,
                                        invoke_result_t<detail::Selection<F...>, std::exception_ptr>>,
              Allocator>
{
  if (!state_)
  {
    ETCPAL_THROW(FutureError{FutureErrc::no_state});
  }

  auto promise = Promise<detail::CommonCVRefType_t<invoke_result_t<detail::Selection<F...>, FutureStatus>,
                                                   invoke_result_t<detail::Selection<F...>, T>,
                                                   invoke_result_t<detail::Selection<F...>, std::exception_ptr>>,
                         Allocator>{state_->get_allocator()};
  auto future  = promise.get_future();
  auto result  = state_->set_continuation([fun = make_selection(std::forward<F>(cont)...), prom = std::move(promise)](
                                             auto status, auto& value, auto exception) mutable {
    if (value)
    {
      detail::fulfill_promise(prom, fun, *value);
    }
    else if (exception)
    {
      detail::fulfill_promise(prom, fun, exception);
    }
    else
    {
      detail::fulfill_promise(prom, fun, status);
    }
  });
  switch (result.result)
  {
    case detail::FutureActionResult::continuation_set_suceeded:
      break;
    case detail::FutureActionResult::continuation_invocation_required:
      result.continuation(result.status, result.value, result.exception);
      break;

    case detail::FutureActionResult::continuation_already_set:
      ETCPAL_THROW(FutureError{FutureErrc::continuation_already_set});
    default:
      ETCPAL_THROW(std::logic_error{"invalid promise status"});
  }

  return future;
}

template <typename T, typename Allocator>
template <typename Executor, typename F, typename... Rest, typename>
ETCPAL_NODISCARD auto etcpal::Future<T, Allocator>::and_then(const Executor& exec, F&& cont, Rest&&... rest)
    -> Future<invoke_result_t<detail::Selection<F, Rest...>, FutureStatus, Optional<T>&, std::exception_ptr>, Allocator>
{
  if (!state_)
  {
    ETCPAL_THROW(FutureError{FutureErrc::no_state});
  }

  auto promise = Promise<invoke_result_t<detail::Selection<F, Rest...>, FutureStatus, Optional<T>&, std::exception_ptr>,
                         Allocator>{state_->get_allocator()};
  auto future  = promise.get_future();
  auto result =
      state_->set_continuation([fun  = make_selection(std::forward<F>(cont), std::forward<Rest>(rest)...), exec,
                                prom = std::move(promise)](auto status, auto& value, auto exception) mutable {
        exec.post([f = std::move(fun), p = std::move(prom), status, val = std::move(value), exception]() mutable {
          detail::fulfill_promise(p, f, status, val, exception);
        });
      });
  switch (result.result)
  {
    case detail::FutureActionResult::continuation_set_suceeded:
      break;
    case detail::FutureActionResult::continuation_invocation_required:
      result.continuation(result.status, result.value, result.exception);
      break;

    case detail::FutureActionResult::continuation_already_set:
      ETCPAL_THROW(FutureError{FutureErrc::continuation_already_set});
    default:
      ETCPAL_THROW(std::logic_error{"invalid promise status"});
  }

  return future;
}

template <typename T, typename Allocator>
template <typename Executor, typename F, typename... Rest, typename>
ETCPAL_NODISCARD auto etcpal::Future<T, Allocator>::and_then(const Executor& exec, F&& cont, Rest&&... rest)
    -> Future<detail::CommonCVRefType_t<invoke_result_t<detail::Selection<F, Rest...>, FutureStatus>,
                                        invoke_result_t<detail::Selection<F, Rest...>, T>,
                                        invoke_result_t<detail::Selection<F, Rest...>, std::exception_ptr>>,
              Allocator>
{
  if (!state_)
  {
    ETCPAL_THROW(FutureError{FutureErrc::no_state});
  }

  auto promise = Promise<detail::CommonCVRefType_t<invoke_result_t<detail::Selection<F, Rest...>, FutureStatus>,
                                                   invoke_result_t<detail::Selection<F, Rest...>, T>,
                                                   invoke_result_t<detail::Selection<F, Rest...>, std::exception_ptr>>,
                         Allocator>{state_->get_allocator()};
  auto future  = promise.get_future();
  auto result =
      state_->set_continuation([fun  = make_selection(std::forward<F>(cont), std::forward<Rest>(rest)...), exec,
                                prom = std::move(promise)](auto status, auto& value, auto exception) mutable {
        exec.post([f = move(fun), p = std::move(prom), status, val = std::move(value), exception]() mutable {
          if (val)
          {
            detail::fulfill_promise(p, f, *val);
          }
          else if (exception)
          {
            detail::fulfill_promise(p, f, exception);
          }
          else
          {
            detail::fulfill_promise(p, f, status);
          }
        });
      });
  switch (result.result)
  {
    case detail::FutureActionResult::continuation_set_suceeded:
      break;
    case detail::FutureActionResult::continuation_invocation_required:
      result.continuation(result.status, result.value, result.exception);
      break;

    case detail::FutureActionResult::continuation_already_set:
      ETCPAL_THROW(FutureError{FutureErrc::continuation_already_set});
    default:
      ETCPAL_THROW(std::logic_error{"invalid promise status"});
  }

  return future;
}

template <typename T, typename Allocator>
ETCPAL_NODISCARD bool etcpal::Future<T, Allocator>::valid() const noexcept
{
  return state_ != nullptr;
}

template <typename T, typename Allocator>
ETCPAL_NODISCARD auto etcpal::Future<T, Allocator>::wait() const noexcept -> FutureStatus
{
  const auto status = wait_for(std::chrono::milliseconds{ETCPAL_WAIT_FOREVER});
  if (status == FutureStatus::timeout)
  {
    return FutureStatus::abandoned;
  }

  return status;
}

template <typename T, typename Allocator>
template <typename Rep, typename Period>
ETCPAL_NODISCARD auto etcpal::Future<T, Allocator>::wait_for(
    const std::chrono::duration<Rep, Period>& timeout) const noexcept -> FutureStatus
{
  if (!state_)
  {
    return FutureStatus::abandoned;
  }

  const auto result = state_->await_value(timeout);
  switch (result.result)
  {
    case detail::FutureActionResult::value_get_succeeded:
      return FutureStatus::ready;
    case detail::FutureActionResult::value_get_timeout:
      return FutureStatus::timeout;

    case detail::FutureActionResult::broken_promise:
      return FutureStatus::abandoned;
    case detail::FutureActionResult::value_already_obtained:
      return FutureStatus::consumed;
    case detail::FutureActionResult::continuation_already_set:
      return FutureStatus::continued;

    default:
      std::abort();
  }
}

template <typename T, typename Allocator>
template <typename Clock, typename Duration>
ETCPAL_NODISCARD auto etcpal::Future<T, Allocator>::wait_until(
    const std::chrono::time_point<Clock, Duration>& timeout) const noexcept -> FutureStatus
{
  return wait_for(timeout - Clock::now());
}

template <std::size_t N, typename Allocator>
etcpal::ThreadPool<N, Allocator>::ThreadPool(const EtcPalThreadParams& params, const Allocator& alloc)
    : queue_{alloc}, allocator_{alloc}
{
  for (auto& thread : threads_)
  {
    thread = JThread<Allocator>{params, allocator_, [&](const auto& token) {
                                  while (token.stop_possible() && !token.stop_requested())
                                  {
                                    queue_status_.Wait(static_cast<EventBits>(detail::QueueStatus::all_bits));
                                    if (auto task = detail::pop_one(queue_, queue_status_))
                                    {
                                      (*task)();
                                    }
                                  }
                                  queue_status_.SetBits(static_cast<EventBits>(detail::QueueStatus::all_bits));
                                }};
  }
}

template <std::size_t N, typename Allocator>
etcpal::ThreadPool<N, Allocator>::ThreadPool(const Allocator& alloc)
    : ThreadPool{EtcPalThreadParams{ETCPAL_THREAD_PARAMS_INIT_VALUES}, alloc}
{
}

template <std::size_t N, typename Allocator>
etcpal::ThreadPool<N, Allocator>::~ThreadPool() noexcept
{
  const auto queue = queue_.lock();
  queue_status_.SetBits(static_cast<EventBits>(detail::QueueStatus::shut_down));
  for (auto& thread : threads_)
  {
    thread.request_stop();
  }
}

template <std::size_t N, typename Allocator>
template <typename Alloc, typename F>
auto etcpal::ThreadPool<N, Allocator>::post(F&& fun, const Alloc& alloc)
{
  const auto queue = queue_.lock();
  queue->push(Task{std::forward<F>(fun), alloc});
  queue_status_.SetBits(static_cast<EventBits>(detail::to_queue_status(queue->size())));

  return queue->size();
}

template <std::size_t N, typename Allocator>
template <typename F>
auto etcpal::ThreadPool<N, Allocator>::post(F&& fun)
{
  return post(std::forward<F>(fun), allocator_);
}

template <std::size_t N, typename Allocator>
template <typename Alloc, typename F>
auto etcpal::ThreadPool<N, Allocator>::post(UseFuture tag, F&& fun, const Alloc& alloc)
    -> Future<decltype(std::forward<F>(fun)()), Alloc>
{
  using T = decltype(std::forward<F>(fun)());

  auto promise = Promise<T, Alloc>{alloc};
  auto future  = promise.get_future();
  post([f = std::forward<F>(fun), prom = std::move(promise)]() mutable { prom.set_value(std::forward<F>(f)()); });

  return future;
}

template <std::size_t N, typename Allocator>
template <typename F>
auto etcpal::ThreadPool<N, Allocator>::post(UseFuture tag, F&& fun)
    -> Future<decltype(std::forward<F>(fun)()), Allocator>
{
  return post(tag, std::forward<F>(fun), allocator_);
}
