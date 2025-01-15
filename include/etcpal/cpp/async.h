#pragma once

#include <array>
#include <chrono>
#include <queue>

#include <etcpal/cpp/common.h>
#include <etcpal/cpp/event_group.h>
#include <etcpal/cpp/functional.h>
#include <etcpal/cpp/mutex.h>
#include <etcpal/cpp/optional.h>
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
  template <typename Continuation>
  [[nodiscard]] auto set_continuation(Continuation&& continuation) noexcept -> StateChangeResult;
  [[nodiscard]] auto set_exception(std::exception_ptr exception) noexcept -> StateChangeResult;
  template <typename Rep, typename Period>
  [[nodiscard]] auto get_value(const std::chrono::duration<Rep, Period>& timeout) noexcept -> StateChangeResult;
  template <typename Rep, typename Period>
  [[nodiscard]] auto await_value(const std::chrono::duration<Rep, Period>& timeout) noexcept -> StateChangeResult;

  [[nodiscard]] auto get_allocator() const noexcept { return allocator_; }

private:
  Mutex              mutex_;
  EventGroup         status_;
  Optional<T>        value_;
  Task               continuation_;
  std::exception_ptr exception_;
  Allocator          allocator_;
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
  template <typename Continuation>
  [[nodiscard]] auto and_then(Continuation&& continuation)
      -> Future<decltype(std::forward<Continuation>(
                    continuation)(FutureStatus{}, std::declval<Optional<T>&>(), std::exception_ptr{})),
                Allocator>;
  template <typename Continuation, typename Executor>
  [[nodiscard]] auto and_then(Continuation&& continuation, const Executor& executor)
      -> Future<decltype(std::forward<Continuation>(
                    continuation)(FutureStatus{}, std::declval<Optional<T>&>(), std::exception_ptr{})),
                Allocator>;

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

private:
  using Task          = MoveOnlyFunction<void(), Allocator>;
  using TaskAllocator = typename std::allocator_traits<Allocator>::template rebind_alloc<Task>;
  using TaskQueue     = std::queue<Task, std::deque<Task, TaskAllocator>>;

  EventGroup             queue_status_ = {};
  Mutex                  queue_mutex_  = {};
  TaskQueue              queue_        = {};
  std::array<JThread, N> threads_      = {};
  Allocator              allocator_    = {};
};

namespace detail
{

enum QueueStatus : EventBits
{
  empty     = 1 << 0,
  has_tasks = 1 << 1,
  shut_down = 1 << 2
};

template <typename T, typename Series>
[[nodiscard]] auto pop_one(std::queue<T, Series>& queue, Mutex& mutex, EventGroup& queue_status) noexcept -> Optional<T>
{
  std::lock_guard<Mutex> guard{mutex};
  if (queue.empty())
  {
    return {};
  }

  auto value = std::move(queue.front());
  queue.pop();
  queue_status.SetBits(queue.size());

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
  std::lock_guard<Mutex> guard{mutex_};
  const auto             status = static_cast<FutureStatus>(status_.GetBits());
  if (exception_)
  {
    return {FutureActionResult::exception_already_set, status, value_, exception_, {}};
  }
  if (has_value(status))
  {
    return {FutureActionResult::value_already_set, status, value_, exception_, {}};
  }

  const auto new_status = continuation_ ? (FutureStatus::ready | FutureStatus::continued) : FutureStatus::ready;
  value_                = std::move(value);
  status_.SetBits(static_cast<EventBits>(new_status));

  return {
      continuation_ ? FutureActionResult::continuation_invocation_required : FutureActionResult::value_set_succeeded,
      new_status, value_, exception_, std::move(continuation_)};
}

template <typename T, typename Allocator>
template <typename Continuation>
[[nodiscard]] auto etcpal::detail::FutureSharedState<T, Allocator>::set_continuation(
    Continuation&& continuation) noexcept -> StateChangeResult
{
  std::lock_guard<Mutex> guard{mutex_};
  const auto             status = static_cast<FutureStatus>(status_.GetBits());
  if (continuation_)
  {
    return {FutureActionResult::continuation_already_set, status, value_, exception_, {}};
  }
  if (is_consumed(status))
  {
    return {FutureActionResult::value_already_obtained, status, value_, exception_, {}};
  }

  if (has_value(status))
  {
    const auto new_status = status | FutureStatus::continued | FutureStatus::consumed;
    status_.SetBits(static_cast<EventBits>(new_status));
    return {FutureActionResult::continuation_invocation_required, new_status, value_, exception_,
            std::forward<Continuation>(continuation)};
  }
  else
  {
    continuation_ = Task{std::forward<Continuation>(continuation), allocator_};
    return {FutureActionResult::continuation_set_suceeded, status, value_, exception_, {}};
  }
}

template <typename T, typename Allocator>
[[nodiscard]] auto etcpal::detail::FutureSharedState<T, Allocator>::set_exception(std::exception_ptr exception) noexcept
    -> StateChangeResult
{
  std::lock_guard<Mutex> guard{mutex_};
  const auto             status = static_cast<FutureStatus>(status_.GetBits());
  if (exception_)
  {
    return {FutureActionResult::exception_already_set, status, value_, exception_, {}};
  }
  if (has_value(status))
  {
    return {FutureActionResult::value_already_set, status, value_, exception_, {}};
  }

  const auto new_status = continuation_ ? (FutureStatus::ready | FutureStatus::continued) : FutureStatus::ready;
  exception_            = exception;
  status_.SetBits(static_cast<EventBits>(new_status));

  return {
      continuation_ ? FutureActionResult::continuation_invocation_required : FutureActionResult::exception_set_suceeded,
      new_status, value_, exception_, std::move(continuation_)};
}

template <typename T, typename Allocator>
template <typename Rep, typename Period>
[[nodiscard]] auto etcpal::detail::FutureSharedState<T, Allocator>::await_value(
    const std::chrono::duration<Rep, Period>& timeout) noexcept -> StateChangeResult
{
  const auto status = static_cast<FutureStatus>(
      status_.TryWait(static_cast<EventBits>(FutureStatus::ready), 0,
                      std::chrono::duration_cast<std::chrono::milliseconds>(timeout).count()));
  std::lock_guard<Mutex> guard{mutex_};
  if (is_abandoned(status))
  {
    return {FutureActionResult::broken_promise, status, value_, exception_, {}};
  }
  if (!has_value(status))
  {
    return {FutureActionResult::value_get_timeout, status, value_, exception_, {}};
  }
  if (is_consumed(status))
  {
    return {FutureActionResult::value_already_obtained, status, value_, exception_, {}};
  }
  if (continuation_)
  {
    return {FutureActionResult::continuation_already_set, status, value_, exception_, {}};
  }

  return {FutureActionResult::value_get_succeeded, status, value_, exception_, {}};
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

  const auto new_status =
      result.status | (continuation_ ? FutureStatus::consumed : (FutureStatus::consumed | FutureStatus::continued));
  status_.SetBits(static_cast<EventBits>(new_status));

  return {exception_ ? FutureActionResult::exception_throwing_required : FutureActionResult::value_get_succeeded,
          new_status, value_, exception_, std::move(continuation_)};
}

template <typename T, typename Allocator>
etcpal::Promise<T, Allocator>::Promise(const Allocator& alloc)
    : state_{std::allocate_shared<detail::FutureSharedState<T, Allocator>>(alloc, alloc)}
{
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
template <typename Continuation>
[[nodiscard]] auto etcpal::Future<T, Allocator>::and_then(Continuation&& continuation)
    -> Future<decltype(std::forward<Continuation>(
                  continuation)(FutureStatus{}, std::declval<Optional<T>&>(), std::exception_ptr{})),
              Allocator>
{
  if (!state_)
  {
    throw FutureError{"future has no shared state"};
  }

  auto promise = Promise<decltype(std::forward<Continuation>(continuation)(
      FutureStatus{}, std::declval<Optional<T>&>(), std::exception_ptr{}))>{state_->get_allocator()};
  auto future  = promise.get_future();
  auto status  = state_->set_continuation([fun = std::forward<Continuation>(continuation), prom = std::move(promise)](
                                             auto status, auto& value, auto exception) mutable {
    prom.set_value(std::forward<Continuation>(fun)(status, value, exception));
  });
  switch (status.result)
  {
    case detail::FutureActionResult::continuation_set_suceeded:
      break;
    case detail::FutureActionResult::continuation_invocation_required:
      status.continuation(status.status, status.value, status.exception);
      break;

    case detail::FutureActionResult::continuation_already_set:
      throw FutureError{"future already has a continuation"};
    case detail::FutureActionResult::value_already_obtained:
      throw FutureError{"future has already been consumed"};
    default:
      throw std::logic_error{"invalid future status"};
  }

  return future;
}

template <typename T, typename Allocator>
template <typename Continuation, typename Executor>
[[nodiscard]] auto etcpal::Future<T, Allocator>::and_then(Continuation&& continuation, const Executor& executor)
    -> Future<decltype(std::forward<Continuation>(
                  continuation)(FutureStatus{}, std::declval<Optional<T>&>(), std::exception_ptr{})),
              Allocator>
{
  if (!state_)
  {
    throw FutureError{"future has no shared state"};
  }

  return and_then(
      [cont = std::forward<Continuation>(continuation), exec = executor](auto status, auto& value, auto exception) {
        exec.post([fun = std::move(cont), status, val = std::move(value), exception]() mutable {
          fun(status, std::move(val), exception);
        });
      });
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
        queue_status_.Wait(std::numeric_limits<EventBits>::max());
        if (auto task = detail::pop_one(queue_, queue_mutex_, queue_status_))
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
  const std::lock_guard<Mutex> guard{queue_mutex_};
  for (auto& thread : threads_)
  {
    thread.request_stop();
  }
  queue_status_.SetBits(std::numeric_limits<EventBits>::max());
}

template <std::size_t N, typename Allocator>
template <typename F>
auto etcpal::ThreadPool<N, Allocator>::post(F&& fun)
{
  const std::lock_guard<Mutex> guard{queue_mutex_};
  queue_.push(Task{std::forward<F>(fun)});
  queue_status_.SetBits(queue_.size());

  return queue_.size();
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
