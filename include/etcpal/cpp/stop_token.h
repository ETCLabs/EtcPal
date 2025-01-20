#pragma once

#include <algorithm>
#include <atomic>
#include <functional>
#include <memory>
#include <mutex>
#include <vector>

#include <etcpal/cpp/common.h>
#include <etcpal/cpp/rwlock.h>
#include <etcpal/cpp/synchronized.h>

namespace etcpal
{

struct NoStopState_t
{
};

ETCPAL_INLINE_VARIABLE constexpr auto NoStopState = NoStopState_t{};

template <typename Allocator = DefaultAllocator>
class StopToken;

template <typename Callback, typename Allocator = DefaultAllocator>
class StopCallback;

template <typename Allocator = DefaultAllocator>
class StopSource
{
private:
  struct StopContext;

public:
  StopSource() = default;
  explicit StopSource(NoStopState_t nss, const Allocator& alloc = {}) noexcept : ctrl_block_{} {}
  explicit StopSource(const Allocator& alloc) : ctrl_block_{std::allocate_shared<StopContext>(alloc, alloc)} {}
  StopSource(const StopSource& rhs) noexcept;
  StopSource(StopSource&& rhs) noexcept : ctrl_block_{std::move(rhs.ctrl_block_)} {}

  auto operator=(const StopSource& rhs) noexcept -> StopSource&;
  auto operator=(StopSource&& rhs) noexcept -> StopSource&;

  ~StopSource() noexcept;

  bool request_stop() noexcept;

  [[nodiscard]] auto get_token() const noexcept { return StopToken<Allocator>{ctrl_block_}; }
  [[nodiscard]] bool stop_requested() const noexcept { return ctrl_block_ && ctrl_block_->stop_requested; }
  [[nodiscard]] bool stop_possible() const noexcept { return ctrl_block_.get(); }

private:
  friend class StopToken<Allocator>;
  template <typename Callback, typename Alloc>
  friend class StopCallback;

  std::shared_ptr<StopContext> ctrl_block_ = std::allocate_shared<StopContext>(Allocator{});
};

template <typename Allocator>
class StopToken
{
public:
  StopToken() noexcept = default;

  [[nodiscard]] bool stop_requested() const noexcept { return ctrl_block_ && ctrl_block_->stop_requested; }
  [[nodiscard]] bool stop_possible() const noexcept;

private:
  using StopContext = typename StopSource<Allocator>::StopContext;

  friend class StopSource<Allocator>;
  template <typename Callback, typename Alloc>
  friend class StopCallback;

  explicit StopToken(const std::shared_ptr<StopContext>& init) noexcept : ctrl_block_{init} {}

  std::shared_ptr<StopContext> ctrl_block_ = {};
};

namespace detail
{

class StopCallbackBase
{
public:
  virtual void invoke() const = 0;
};

}  // namespace detail

template <typename Callback, typename Allocator>
class StopCallback : private detail::StopCallbackBase
{
public:
  using callback_type = Callback;

  template <typename C>
  explicit StopCallback(const StopToken<Allocator>& token,
                        C&&                         cb) noexcept(std::is_nothrow_constructible<Callback, C>::value);
  template <typename C>
  explicit StopCallback(StopToken<Allocator>&& token,
                        C&&                    cb) noexcept(std::is_nothrow_constructible<Callback, C>::value);

  StopCallback(const StopCallback& rhs) = delete;
  StopCallback(StopCallback&& rhs)      = delete;

  auto operator=(const StopCallback& rhs) -> StopCallback& = delete;
  auto operator=(StopCallback&& rhs) -> StopCallback&      = delete;

  ~StopCallback() noexcept;

private:
  void invoke() const override { callback_(); }

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
  using CallbackAllocator =
      typename std::allocator_traits<Allocator>::template rebind_alloc<const detail::StopCallbackBase*>;
  using CallbackStorage = std::vector<const detail::StopCallbackBase*, CallbackAllocator>;

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

  for (const auto* const var : *ctrl_block_->callbacks.lock())
  {
    if (var)
    {
      var->invoke();
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
  for (const auto* fun : *callbacks)
  {
    if (!fun)
    {
      fun = this;
      return;
    }
  }

  callbacks->push_back(this);
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
  const auto position =
      std::find(std::begin(*callbacks), std::end(*callbacks), static_cast<detail::StopCallbackBase*>(this));
  if (position == std::end(*callbacks))
  {
    return;
  }

  *position = nullptr;
}
