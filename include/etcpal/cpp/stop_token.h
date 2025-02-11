#pragma once

#include <algorithm>
#include <atomic>
#include <functional>
#include <memory>
#include <mutex>
#include <vector>

#include <etcpal/cpp/common.h>
#include <etcpal/cpp/functional.h>
#include <etcpal/cpp/rwlock.h>
#include <etcpal/cpp/synchronized.h>

namespace etcpal
{

/// @brief Tag type used to initialize an empty stop source.
struct NoStopState_t
{
};
/// @brief Tag value used to initialize an empty stop source.
ETCPAL_INLINE_VARIABLE constexpr auto NoStopState = NoStopState_t{};

template <typename Allocator = polymorphic_allocator<>>
class StopToken;

template <typename Callback, typename Allocator = polymorphic_allocator<>>
class StopCallback;

/// @brief A single-shot cancellation token with allocator support.
/// @tparam Allocator The type of allocator to allocate the shared state and callback registrations with.
template <typename Allocator = polymorphic_allocator<>>
class StopSource
{
private:
  struct StopContext;

public:
  /// @brief Construct a source with a shared cancellation state.
  StopSource() = default;
  /// @brief Construct an empty source with no shared cancellation state.
  /// @param nss Tag value indicating the source should be empty.
  explicit StopSource([[maybe_unused]] NoStopState_t nss) noexcept : ctrl_block_{} {}
  /// @brief Construct a source using the given allocator to allocate the shared cancellation state.
  /// @param alloc The allocator to allocate memory with.
  explicit StopSource(const Allocator& alloc) : ctrl_block_{std::allocate_shared<StopContext>(alloc, alloc)} {}
  StopSource(const StopSource& rhs) noexcept;                                         //!< Copy a stop source.
  StopSource(StopSource&& rhs) noexcept : ctrl_block_{std::move(rhs.ctrl_block_)} {}  //!< Move a stop source.

  auto operator=(const StopSource& rhs) noexcept -> StopSource&;  //!< Copy a stop source.
  auto operator=(StopSource&& rhs) noexcept -> StopSource&;       //!< Move a stop source.

  ~StopSource() noexcept;  //!< Destroy a stop source, abandoning any outstanding tokens.

  /// @brief Issue a cancellation request through this stop source.
  ///
  /// Issueing a stop request causes all existing registered callbacks to be invoked synchronously, and causes future
  /// callback registrations to immediately invoke their callbacks in the same execution context of the registration. A
  /// request will not be issued if one has already been issued on the same cancellation state, or if a shared
  /// cancellation state does not exist.
  ///
  /// @return `true` if a request was issued, or `false` otherwise.
  bool request_stop() noexcept;

  /// @name Observers
  ///
  /// @brief Obtain a token associated with the shared cancellation state, or information about the state itself.
  ///
  /// A `stop_possible` returns `true` simply if a shared cancellation state exists, whether or not a cancellation has
  /// already been issued to that shared state.
  ///
  /// @return The requested token or state information.
  ///
  /// @{
  [[nodiscard]] auto get_token() const noexcept { return StopToken<Allocator>{ctrl_block_}; }
  [[nodiscard]] bool stop_requested() const noexcept { return ctrl_block_ && ctrl_block_->stop_requested; }
  [[nodiscard]] bool stop_possible() const noexcept { return ctrl_block_.get(); }
  /// @}

private:
  friend class StopToken<Allocator>;
  template <typename Callback, typename Alloc>
  friend class StopCallback;

  std::shared_ptr<StopContext> ctrl_block_ = std::allocate_shared<StopContext>(Allocator{});
};

/// @brief A token to receive single-shot cancellation requests from a source.
/// @tparam Allocator The type of allocator the shared state was allocated with.
template <typename Allocator>
class StopToken
{
public:
  StopToken() noexcept = default;  ///!< Construct a token with no shared cancellation state.

  /// @name Observers
  ///
  /// @brief Obtain information about the shared cancellation state.
  ///
  /// `stop_possible` returns `true` if an associated cancellation state exists, at least one source is still associated
  /// with that shared state, and a stop request has not already been issued to that shared state.
  ///
  /// @return The requested cancellation state information.
  ///
  /// @{
  [[nodiscard]] bool stop_requested() const noexcept { return ctrl_block_ && ctrl_block_->stop_requested; }
  [[nodiscard]] bool stop_possible() const noexcept;
  /// @}

private:
  using StopContext = typename StopSource<Allocator>::StopContext;

  friend class StopSource<Allocator>;
  template <typename Callback, typename Alloc>
  friend class StopCallback;

  explicit StopToken(const std::shared_ptr<StopContext>& init) noexcept : ctrl_block_{init} {}

  std::shared_ptr<StopContext> ctrl_block_ = {};
};

/// @brief An RAII registration for a callback to be invoked on a cancellation event.
/// @tparam Callback The type of operator to invoke on cancellation.
/// @tparam Allocator The type of allocator to use to allocate the registration.
template <typename Callback, typename Allocator>
class StopCallback
{
public:
  using callback_type = Callback;  //!< The wrapped callback type.

  /// @brief Register a callback with the given stop token, invoking the callback if a cancellation is outstanding.
  /// @tparam C The type of callback to register.
  /// @param token The token to register the callback to.
  /// @param cb    The callback to invoke on cancellation.
  template <typename C>
  explicit StopCallback(const StopToken<Allocator>& token,
                        C&&                         cb) noexcept(std::is_nothrow_constructible<Callback, C>::value);
  /// @brief Register a callback with the given stop token, invoking the callback if a cancellation is outstanding.
  /// @tparam C The type of callback to register.
  /// @param token The token to register the callback to.
  /// @param cb    The callback to invoke on cancellation.
  template <typename C>
  explicit StopCallback(StopToken<Allocator>&& token,
                        C&&                    cb) noexcept(std::is_nothrow_constructible<Callback, C>::value);

  StopCallback(const StopCallback& rhs) = delete;  //!< Disable copying a callback.
  StopCallback(StopCallback&& rhs)      = delete;  //!< Disable moving a callack.

  auto operator=(const StopCallback& rhs) -> StopCallback& = delete;  //!< Disable copying a callack.
  auto operator=(StopCallback&& rhs) -> StopCallback&      = delete;  //!< Disable moving a callack.

  ~StopCallback() noexcept;  //!< De-register a callback.

private:
  StopToken<Allocator>  token_;
  mutable callback_type callback_;
};

#if (__cplusplus >= 201703L)
template <typename Callback, typename Allocator>
StopCallback(const StopToken<Allocator>&, Callback&&)
    -> StopCallback<std::remove_reference_t<std::remove_cv_t<Callback>>, Allocator>;
template <typename Callback, typename Allocator>
StopCallback(StopToken<Allocator>&&, Callback&&)
    -> StopCallback<std::remove_reference_t<std::remove_cv_t<Callback>>, Allocator>;
#endif  // #if (__cplusplus >= 201703L)

}  // namespace etcpal

template <typename Allocator>
struct etcpal::StopSource<Allocator>::StopContext
{
  using CallbackAllocator = typename std::allocator_traits<Allocator>::template rebind_alloc<FunctionRef<void()>>;
  using CallbackStorage   = std::vector<FunctionRef<void()>, CallbackAllocator>;

  StopContext() = default;
  StopContext(const CallbackAllocator& alloc) : callbacks{alloc} {}

  Synchronized<CallbackStorage, RwLock> callbacks{CallbackStorage{CallbackAllocator{}}};
  std::atomic<int>                      num_sources{1};
  std::atomic<bool>                     stop_requested{false};
};

template <typename Allocator>
etcpal::StopSource<Allocator>::StopSource(const StopSource& rhs) noexcept : ctrl_block_{rhs.ctrl_block_}
{
  if (ctrl_block_)
  {
    ctrl_block_->num_sources += 1;
  }
}

template <typename Allocator>
auto etcpal::StopSource<Allocator>::operator=(const StopSource& rhs) noexcept -> StopSource&
{
  StopSource{std::move(*this)};
  return *this = StopSource{rhs};
}

template <typename Allocator>
auto etcpal::StopSource<Allocator>::operator=(StopSource&& rhs) noexcept -> StopSource&
{
  StopSource{std::move(*this)};
  ctrl_block_ = std::move(rhs.ctrl_block_);
  return *this;
}

template <typename Allocator>
etcpal::StopSource<Allocator>::~StopSource() noexcept
{
  if (ctrl_block_)
  {
    ctrl_block_->num_sources -= 1;
  }
}

template <typename Allocator>
bool etcpal::StopSource<Allocator>::request_stop() noexcept
{
  if (!ctrl_block_)
  {
    return false;
  }

  bool expected = false;
  if (!ctrl_block_->stop_requested.compare_exchange_weak(expected, true))
  {
    return false;
  }

  for (const auto& var : *ctrl_block_->callbacks.lock())
  {
    if (var)
    {
      var();
    }
  }

  return true;
}

template <typename Allocator>
[[nodiscard]] bool etcpal::StopToken<Allocator>::stop_possible() const noexcept
{
  if (!ctrl_block_ || (!ctrl_block_->stop_requested && (ctrl_block_->num_sources == 0)))
  {
    return false;
  }

  return true;
}

template <typename Callback, typename Allocator>
template <typename C>
etcpal::StopCallback<Callback, Allocator>::StopCallback(const StopToken<Allocator>& token, C&& cb) noexcept(
    std::is_nothrow_constructible<Callback, C>::value)
    : token_{token}, callback_{std::forward<C>(cb)}
{
  if (!token_.ctrl_block_)
  {
    return;
  }
  if (token_.ctrl_block_->stop_requested)
  {
    callback_();
    return;
  }

  const auto callbacks = token_.ctrl_block_->callbacks.lock();
  for (auto& fun : *callbacks)
  {
    if (!fun)
    {
      fun = FunctionRef<void()>{callback_};
      return;
    }
  }

  callbacks->push_back(callback_);
}

template <typename Callback, typename Allocator>
template <typename C>
etcpal::StopCallback<Callback, Allocator>::StopCallback(StopToken<Allocator>&& token, C&& cb) noexcept(
    std::is_nothrow_constructible<Callback, C>::value)
    : token_{std::move(token)}, callback_{std::forward<C>(cb)}
{
}

template <typename Callback, typename Allocator>
etcpal::StopCallback<Callback, Allocator>::~StopCallback() noexcept
{
  if (!token_.ctrl_block_)
  {
    return;
  }

  const auto callbacks = token_.ctrl_block_->callbacks.lock();
  const auto position  = std::find_if(std::begin(*callbacks), std::end(*callbacks), [&](const auto& val) {
    return val.target_address() == std::addressof(callback_);
  });
  if (position == std::end(*callbacks))
  {
    return;
  }

  *position = nullptr;
}
