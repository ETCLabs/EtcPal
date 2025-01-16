#pragma once

#include <array>
#include <chrono>
#include <queue>

#include <etcpal/cpp/common.h>
#include <etcpal/cpp/event_group.h>
#include <etcpal/cpp/functional.h>
#include <etcpal/cpp/optional.h>
#include <etcpal/cpp/rwlock.h>
#include <etcpal/cpp/synchronized.h>
#include <etcpal/cpp/thread.h>

namespace etcpal
{

enum class FutureStatus : unsigned int
{
  ready     = 1 << 0,  //!< promise has been fulfilled with either a value or an exception
  timeout   = 1 << 1,  //!< timed out waiting for the future
  deferred  = 1 << 2,  //!< promise fulfillment has been deferred
  abandoned = 1 << 3,  //!< promise has been abandoned before being fulfilled
  continued = 1 << 4,  //!< future has had its continuation invoked
  consumed  = 1 << 5   //!< future value has been obtained
};

[[nodiscard]] constexpr auto operator~(FutureStatus value) noexcept
{
  return static_cast<FutureStatus>(~static_cast<unsigned int>(value));
}
[[nodiscard]] constexpr auto operator&(FutureStatus lhs, FutureStatus rhs) noexcept
{
  return static_cast<FutureStatus>(static_cast<unsigned int>(lhs) & static_cast<unsigned int>(rhs));
}
[[nodiscard]] constexpr auto operator|(FutureStatus lhs, FutureStatus rhs) noexcept
{
  return static_cast<FutureStatus>(static_cast<unsigned int>(lhs) | static_cast<unsigned int>(rhs));
}
[[nodiscard]] constexpr auto operator^(FutureStatus lhs, FutureStatus rhs) noexcept
{
  return static_cast<FutureStatus>(static_cast<unsigned int>(lhs) ^ static_cast<unsigned int>(rhs));
}
template <typename IntType>
[[nodiscard]] constexpr auto operator<<(FutureStatus lhs, IntType rhs) noexcept
{
  return static_cast<FutureStatus>(static_cast<unsigned int>(lhs) << rhs);
}
template <typename IntType>
[[nodiscard]] constexpr auto operator>>(FutureStatus lhs, IntType rhs) noexcept
{
  return static_cast<FutureStatus>(static_cast<unsigned int>(lhs) >> rhs);
}
[[nodiscard]] constexpr auto& operator&=(FutureStatus& lhs, FutureStatus rhs) noexcept
{
  return lhs = lhs & rhs;
}
[[nodiscard]] constexpr auto& operator|=(FutureStatus& lhs, FutureStatus rhs) noexcept
{
  return lhs = lhs | rhs;
}
[[nodiscard]] constexpr auto& operator^=(FutureStatus& lhs, FutureStatus rhs) noexcept
{
  return lhs = lhs ^ rhs;
}
template <typename IntType>
[[nodiscard]] constexpr auto& operator<<=(FutureStatus& lhs, IntType rhs) noexcept
{
  return lhs = lhs << rhs;
}
template <typename IntType>
[[nodiscard]] constexpr auto& operator>>=(FutureStatus& lhs, IntType rhs) noexcept
{
  return lhs = lhs >> rhs;
}

template <typename T, typename Allocator = DefaultAllocator>
class Promise;
template <typename T, typename Allocator = DefaultAllocator>
class Future;

class FutureError : public std::logic_error
{
public:
  FutureError() : std::logic_error{"future error"} {}

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
  using Task = MoveOnlyFunction<void(FutureStatus, Optional<T>&, std::exception_ptr), Allocator>;

  struct StateChangeResult
  {
    FutureActionResult result;
    FutureStatus       status;
    Optional<T>&       value;
    std::exception_ptr exception;
    Task               continuation;
  };

  explicit FutureSharedState(const Allocator& alloc) noexcept : allocator_{alloc} {}

  [[nodiscard]] auto set_value(T&& value) noexcept -> StateChangeResult;
  [[nodiscard]] auto set_exception(std::exception_ptr exception) noexcept -> StateChangeResult;
  [[nodiscard]] auto set_continuation(Task&& continuation) noexcept -> StateChangeResult;
  template <typename Rep, typename Period>
  [[nodiscard]] auto get_value(const std::chrono::duration<Rep, Period>& timeout) noexcept -> StateChangeResult;
  template <typename Rep, typename Period>
  [[nodiscard]] auto await_value(const std::chrono::duration<Rep, Period>& timeout) noexcept -> StateChangeResult;
  void               abandon() noexcept;

  [[nodiscard]] auto get_allocator() const noexcept { return allocator_; }

private:
  struct State
  {
    Optional<T>        value_;
    Task               continuation_;
    std::exception_ptr exception_;
  };

  EventGroup                  status_;
  Synchronized<State, RwLock> state_;
  Allocator                   allocator_;
};

}  // namespace detail

template <typename T, typename Allocator>
class Promise
{
public:
  Promise() = default;
  explicit Promise(const Allocator& alloc);

  Promise(const Promise& rhs)     = delete;
  Promise(Promise&& rhs) noexcept = default;

  ~Promise() noexcept;

  auto operator=(const Promise& rhs) -> Promise&     = delete;
  auto operator=(Promise&& rhs) noexcept -> Promise& = default;

  [[nodiscard]] auto get_future() const noexcept { return Future<T, Allocator>{state_}; }

  void set_value(const T& value);
  void set_value(T&& value);

private:
  std::shared_ptr<detail::FutureSharedState<T, Allocator>> state_ =
      std::allocate_shared<detail::FutureSharedState<T, Allocator>>(Allocator{}, Allocator{});
};

template <typename T, typename Allocator>
class Future
{
public:
  Future(const Future& rhs)     = delete;
  Future(Future&& rhs) noexcept = default;

  auto operator=(const Future& rhs) -> Future&     = delete;
  auto operator=(Future&& rhs) noexcept -> Future& = default;

  [[nodiscard]] auto get() -> T;
  template <typename F>
  [[nodiscard]] auto and_then(F&& cont)
      -> Future<decltype(std::forward<F>(cont)(FutureStatus{}, std::declval<Optional<T>&>(), std::exception_ptr{}))>;

  [[nodiscard]] bool valid() const noexcept;
  [[nodiscard]] auto wait() const noexcept -> FutureStatus;
  template <typename Rep, typename Period>
  [[nodiscard]] auto wait_for(const std::chrono::duration<Rep, Period>& timeout) const noexcept -> FutureStatus;
  template <typename Clock, typename Duration>
  [[nodiscard]] auto wait_until(const std::chrono::time_point<Clock, Duration>& timeout) const noexcept -> FutureStatus;

private:
  friend class Promise<T, Allocator>;

  explicit Future(const std::shared_ptr<detail::FutureSharedState<T, Allocator>>& state) noexcept : state_{state} {}

  std::shared_ptr<detail::FutureSharedState<T, Allocator>> state_ = {};
};

struct UseFuture
{
};

ETCPAL_INLINE_VARIABLE constexpr auto use_future = UseFuture{};

template <std::size_t N, typename Allocator = DefaultAllocator>
class ThreadPool
{
public:
  class Executor;

  ThreadPool() : ThreadPool{Allocator{}} {}
  ThreadPool(const Allocator& alloc);

  ~ThreadPool() noexcept;

  template <typename Alloc = Allocator, typename F>
  [[nodiscard]] auto post(UseFuture tag, F&& fun, const Alloc& alloc)
      -> Future<decltype(std::forward<F>(fun)()), Alloc>;
  template <typename F>
  [[nodiscard]] auto post(UseFuture tag, F&& fun) -> Future<decltype(std::forward<F>(fun)()), Allocator>;
  template <typename F>
  auto post(F&& fun);

  [[nodiscard]] auto get_executor() noexcept { return Executor{*this}; }

private:
  using Task          = MoveOnlyFunction<void(), Allocator>;
  using TaskAllocator = typename std::allocator_traits<Allocator>::template rebind_alloc<Task>;
  using TaskQueue     = std::queue<Task, std::deque<Task, TaskAllocator>>;

  EventGroup                      queue_status_ = {};
  Synchronized<TaskQueue, RwLock> queue_        = {};
  std::array<JThread, N>          threads_      = {};
  Allocator                       allocator_    = {};
};

namespace detail
{

enum class QueueStatus : EventBits
{
  shut_down  = 1,                             // reserve lowest bit for shutdown flag
  count_mask = ~EventBits{} & ~EventBits{1},  // all other bits are the queue item count
  all_bits   = ~EventBits{}
};

[[nodiscard]] constexpr auto operator~(QueueStatus value) noexcept
{
  return static_cast<QueueStatus>(~static_cast<EventBits>(value));
}
[[nodiscard]] constexpr auto operator&(QueueStatus lhs, QueueStatus rhs) noexcept
{
  return static_cast<QueueStatus>(static_cast<EventBits>(lhs) & static_cast<EventBits>(rhs));
}
[[nodiscard]] constexpr auto operator|(QueueStatus lhs, QueueStatus rhs) noexcept
{
  return static_cast<QueueStatus>(static_cast<EventBits>(lhs) | static_cast<EventBits>(rhs));
}
[[nodiscard]] constexpr auto operator^(QueueStatus lhs, QueueStatus rhs) noexcept
{
  return static_cast<QueueStatus>(static_cast<EventBits>(lhs) ^ static_cast<EventBits>(rhs));
}
template <typename IntType>
[[nodiscard]] constexpr auto operator<<(QueueStatus lhs, IntType rhs) noexcept
{
  return static_cast<QueueStatus>(static_cast<EventBits>(lhs) << rhs);
}
template <typename IntType>
[[nodiscard]] constexpr auto operator>>(QueueStatus lhs, IntType rhs) noexcept
{
  return static_cast<QueueStatus>(static_cast<EventBits>(lhs) >> rhs);
}
[[nodiscard]] constexpr auto& operator&=(QueueStatus& lhs, QueueStatus rhs) noexcept
{
  return lhs = lhs & rhs;
}
[[nodiscard]] constexpr auto& operator|=(QueueStatus& lhs, QueueStatus rhs) noexcept
{
  return lhs = lhs | rhs;
}
[[nodiscard]] constexpr auto& operator^=(QueueStatus& lhs, QueueStatus rhs) noexcept
{
  return lhs = lhs ^ rhs;
}
template <typename IntType>
[[nodiscard]] constexpr auto& operator<<=(QueueStatus& lhs, IntType rhs) noexcept
{
  return lhs = lhs << rhs;
}
template <typename IntType>
[[nodiscard]] constexpr auto& operator>>=(QueueStatus& lhs, IntType rhs) noexcept
{
  return lhs = lhs >> rhs;
}

[[nodiscard]] constexpr auto to_queue_status(std::size_t queue_size) noexcept
{
  return static_cast<QueueStatus>(queue_size << 1);
}

template <typename T, typename Series, typename Lock>
[[nodiscard]] auto pop_one(Synchronized<std::queue<T, Series>, Lock>& sync_queue, EventGroup& queue_status) noexcept
    -> Optional<T>
{
  const auto queue = sync_queue.lock();
  if (queue->empty())
  {
    return {};
  }

  auto value = std::move(queue->front());
  queue->pop();
  queue_status.SetBits(static_cast<EventBits>(to_queue_status(queue->size())));

  return value;
}

[[nodiscard]] constexpr bool has_value(FutureStatus status) noexcept
{
  return (status & FutureStatus::ready) == FutureStatus::ready;
}

[[nodiscard]] constexpr bool is_abandoned(FutureStatus status) noexcept
{
  return (status & FutureStatus::abandoned) == FutureStatus::abandoned;
}

[[nodiscard]] constexpr bool is_consumed(FutureStatus status) noexcept
{
  return (status & (FutureStatus::consumed | FutureStatus::continued)) != FutureStatus{};
}

[[nodiscard]] constexpr bool has_unconsumed_value(FutureStatus status) noexcept
{
  return has_value(status) && !is_consumed(status);
}

}  // namespace detail

}  // namespace etcpal

template <std::size_t N, typename Allocator>
class etcpal::ThreadPool<N, Allocator>::Executor
{
public:
  explicit Executor(ThreadPool& pool) noexcept : pool_{std::addressof(pool)} {}

  template <typename Alloc = Allocator, typename F>
  [[nodiscard]] auto post(UseFuture tag, F&& fun, const Alloc& alloc) -> Future<decltype(std::forward<F>(fun)()), Alloc>
  {
    return pool_->post(tag, std::forward<F>(fun), alloc);
  }

  template <typename F>
  [[nodiscard]] auto post(UseFuture tag, F&& fun) -> Future<decltype(std::forward<F>(fun)()), Allocator>
  {
    return pool_->post(tag, std::forward<F>(fun));
  }

  template <typename F>
  auto post(F&& fun)
  {
    return pool_->post(std::forward<F>(fun));
  }

private:
  ThreadPool* pool_;
};

template <typename T, typename Allocator>
[[nodiscard]] auto etcpal::detail::FutureSharedState<T, Allocator>::set_value(T&& value) noexcept -> StateChangeResult
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
  state->value_         = std::move(value);
  status_.SetBits(static_cast<EventBits>(new_status));

  return {state->continuation_ ? FutureActionResult::continuation_invocation_required
                               : FutureActionResult::value_set_succeeded,
          new_status, state->value_, state->exception_, std::move(state->continuation_)};
}

template <typename T, typename Allocator>
[[nodiscard]] auto etcpal::detail::FutureSharedState<T, Allocator>::set_exception(std::exception_ptr exception) noexcept
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
  state->exception_     = exception;
  status_.SetBits(static_cast<EventBits>(new_status));

  return {state->continuation_ ? FutureActionResult::continuation_invocation_required
                               : FutureActionResult::exception_set_suceeded,
          new_status, state->value_, state->exception_, std::move(state->continuation_)};
}

template <typename T, typename Allocator>
[[nodiscard]] auto etcpal::detail::FutureSharedState<T, Allocator>::set_continuation(Task&& continuation) noexcept
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
    state->continuation_ = std::move(continuation);
    return {FutureActionResult::continuation_set_suceeded, status, state->value_, state->exception_, {}};
  }

  const auto new_status = static_cast<FutureStatus>(status_.GetBits()) | FutureStatus::continued;
  status_.SetBits(static_cast<EventBits>(new_status));

  return {FutureActionResult::continuation_invocation_required, new_status, state->value_, state->exception_,
          std::forward<Task>(continuation)};
}

template <typename T, typename Allocator>
template <typename Rep, typename Period>
[[nodiscard]] auto etcpal::detail::FutureSharedState<T, Allocator>::await_value(
    const std::chrono::duration<Rep, Period>& timeout) noexcept -> StateChangeResult
{
  const auto status = static_cast<FutureStatus>(
      status_.TryWait(static_cast<EventBits>(FutureStatus::ready | FutureStatus::abandoned), 0,
                      std::chrono::duration_cast<std::chrono::milliseconds>(timeout).count()));
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
template <typename Rep, typename Period>
[[nodiscard]] auto etcpal::detail::FutureSharedState<T, Allocator>::get_value(
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
void etcpal::Promise<T, Allocator>::set_value(const T& value)
{
  if (!state_)
  {
    throw FutureError{"promise has no associated state"};
  }

  set_value(T{value});
}

template <typename T, typename Allocator>
void etcpal::Promise<T, Allocator>::set_value(T&& value)
{
  if (!state_)
  {
    throw FutureError{"promise has no associated state"};
  }

  auto result = state_->set_value(std::move(value));
  switch (result.result)
  {
    case detail::FutureActionResult::value_set_succeeded:
      return;
    case detail::FutureActionResult::continuation_invocation_required:
      result.continuation(result.status, result.value, result.exception);
      return;

    case detail::FutureActionResult::value_already_set:
      [[fallthrough]];
    case detail::FutureActionResult::exception_already_set:
      throw FutureError{"promise already fulfilled"};
    default:
      throw std::logic_error{"invalid promise status"};
  }
}

template <typename T, typename Allocator>
[[nodiscard]] auto etcpal::Future<T, Allocator>::get() -> T
{
  if (!state_)
  {
    throw FutureError{"future has no shared state"};
  }

  auto result = state_->get_value(std::chrono::milliseconds{ETCPAL_WAIT_FOREVER});
  switch (result.result)
  {
    case detail::FutureActionResult::exception_throwing_required:
      std::rethrow_exception(result.exception);
    case detail::FutureActionResult::value_get_succeeded:
      return std::move(*result.value);

    case detail::FutureActionResult::broken_promise:
      throw FutureError{"broken promise"};
    case detail::FutureActionResult::value_already_obtained:
      throw FutureError{"attempting to obtain a future value that has already been obtained"};
    case detail::FutureActionResult::continuation_already_set:
      throw FutureError{"attempting to obtain a future value when a continuation has already been set"};
    case detail::FutureActionResult::value_get_timeout:
      throw std::logic_error{"timed out waiting forever for a future value"};
    default:
      throw std::logic_error{"invalid future status"};
  }
}

template <typename T, typename Allocator>
template <typename F>
[[nodiscard]] auto etcpal::Future<T, Allocator>::and_then(F&& cont)
    -> Future<decltype(std::forward<F>(cont)(FutureStatus{}, std::declval<Optional<T>&>(), std::exception_ptr{}))>
{
  if (!state_)
  {
    throw FutureError{"promise has no associated state"};
  }

  auto promise =
      Promise<decltype(std::forward<F>(cont)(FutureStatus{}, std::declval<Optional<T>&>(), std::exception_ptr{}))>{
          state_->get_allocator()};
  auto future = promise.get_future();
  auto result = state_->set_continuation(
      [fun = std::forward<F>(cont), prom = std::move(promise)](auto status, auto& value, auto exception) mutable {
        prom.set_value(std::forward<F>(fun)(status, value, exception));
      });
  switch (result.result)
  {
    case detail::FutureActionResult::continuation_set_suceeded:
      break;
    case detail::FutureActionResult::continuation_invocation_required:
      result.continuation(result.status, result.value, result.exception);
      break;

    case detail::FutureActionResult::continuation_already_set:
      throw FutureError{"promise already has a continuation"};
    default:
      throw std::logic_error{"invalid promise status"};
  }

  return future;
}

template <typename T, typename Allocator>
[[nodiscard]] bool etcpal::Future<T, Allocator>::valid() const noexcept
{
  return state_ != nullptr;
}

template <typename T, typename Allocator>
[[nodiscard]] auto etcpal::Future<T, Allocator>::wait() const noexcept -> FutureStatus
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
[[nodiscard]] auto etcpal::Future<T, Allocator>::wait_for(
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
      throw std::logic_error{"invalid future status"};
  }
}

template <typename T, typename Allocator>
template <typename Clock, typename Duration>
[[nodiscard]] auto etcpal::Future<T, Allocator>::wait_until(
    const std::chrono::time_point<Clock, Duration>& timeout) const noexcept -> FutureStatus
{
  return wait_for(timeout - Clock::now());
}

template <std::size_t N, typename Allocator>
etcpal::ThreadPool<N, Allocator>::ThreadPool(const Allocator& alloc) : queue_{alloc}, allocator_{alloc}
{
  for (auto& thread : threads_)
  {
    thread = JThread{[&](const auto& token) {
      while (token.stop_possible() && !token.stop_requested())
      {
        queue_status_.Wait(static_cast<EventBits>(detail::QueueStatus::all_bits));
        if (auto task = detail::pop_one(queue_, queue_status_))
        {
          (*task)();
        }
      }
    }};
  }
}

template <std::size_t N, typename Allocator>
etcpal::ThreadPool<N, Allocator>::~ThreadPool() noexcept
{
  const auto queue = queue_.lock();
  for (auto& thread : threads_)
  {
    thread.request_stop();
  }
  queue_status_.SetBits(static_cast<EventBits>(detail::QueueStatus::shut_down));
}

template <std::size_t N, typename Allocator>
template <typename F>
auto etcpal::ThreadPool<N, Allocator>::post(F&& fun)
{
  const auto queue = queue_.lock();
  queue->push(Task{std::forward<F>(fun)});
  queue_status_.SetBits(static_cast<EventBits>(detail::to_queue_status(queue->size())));

  return queue->size();
}

template <std::size_t N, typename Allocator>
template <typename Alloc, typename F>
auto etcpal::ThreadPool<N, Allocator>::post(UseFuture tag, F&& fun, const Alloc& alloc)
    -> Future<decltype(std::forward<F>(fun)()), Alloc>
{
  using T = decltype(std::forward<F>(fun)());

  auto promise = Promise<T, Alloc>{alloc};
  auto future  = promise.get_future();
  post([f = std::forward<F>(fun), prom = std::move(promise)]() mutable { prom.set_value(std::forward<F>(f)); });

  return future;
}

template <std::size_t N, typename Allocator>
template <typename F>
auto etcpal::ThreadPool<N, Allocator>::post(UseFuture tag, F&& fun)
    -> Future<decltype(std::forward<F>(fun)()), Allocator>
{
  return post(tag, std::forward<F>(fun), allocator_);
}
