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

namespace detail
{

class ConstCallbackBase
{
public:
  virtual ~ConstCallbackBase() noexcept = default;
  virtual void operator()() const       = 0;
};

class MutableCallbackBase
{
public:
  virtual ~MutableCallbackBase() noexcept = default;
  virtual void operator()()               = 0;
};

template <typename Function, typename = void>
class Callback : public MutableCallbackBase
{
public:
  Callback(Function&& fun) : fun_{std::move(fun)} {}
  void operator()() override { fun_(); }

private:
  Function fun_;
};
template <typename Function>
class Callback<Function, void_t<decltype(std::declval<const Function>()())>> : public ConstCallbackBase
{
public:
  Callback(Function&& fun) : fun_{std::move(fun)} {}
  void operator()() const override { fun_(); }

private:
  Function fun_;
};

}  // namespace detail

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

template <typename Callback, typename Allocator>
class StopCallback
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
  StopToken<Allocator>                    token_;
  mutable detail::Callback<callback_type> callback_;
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
  struct CallbackRef
  {
    bool is_const;
    bool has_value = false;
    union RefUnion
    {
      constexpr RefUnion() noexcept : null_callback{} {}
      constexpr RefUnion(const detail::ConstCallbackBase& init) noexcept : const_callback{init} {}
      constexpr RefUnion(detail::MutableCallbackBase& init) noexcept : mutable_callback{init} {}

      std::nullptr_t                                          null_callback;
      std::reference_wrapper<const detail::ConstCallbackBase> const_callback;
      std::reference_wrapper<detail::MutableCallbackBase>     mutable_callback;
    } fun;

    constexpr CallbackRef() = default;
    constexpr CallbackRef(const detail::ConstCallbackBase& f) noexcept : is_const{true}, has_value{true}, fun{f} {}
    constexpr CallbackRef(detail::MutableCallbackBase& f) noexcept : is_const{false}, has_value{true}, fun{f} {}

    ~CallbackRef() noexcept { deinit(); }

    constexpr auto operator=(std::nullptr_t rhs) noexcept -> CallbackRef&
    {
      deinit();
      has_value = false;
      new (std::addressof(fun.null_callback)) std::nullptr_t;

      return *this;
    }
    constexpr auto operator=(const detail::ConstCallbackBase& rhs) noexcept -> CallbackRef&
    {
      deinit();
      has_value = true;
      is_const  = true;
      new (std::addressof(fun.const_callback)) decltype(fun.const_callback){rhs};

      return *this;
    }
    constexpr auto operator=(detail::MutableCallbackBase& rhs) noexcept -> CallbackRef&
    {
      deinit();
      has_value = true;
      is_const  = false;
      new (std::addressof(fun.mutable_callback)) decltype(fun.mutable_callback){rhs};

      return *this;
    }

    void operator()() const noexcept
    {
      if (!has_value)
      {
        return;
      }

      if (is_const)
      {
        fun.const_callback();
      }
      else
      {
        fun.mutable_callback();
      }
    }

    constexpr void deinit() noexcept
    {
      if (!has_value)
      {
        using T = std::nullptr_t;
        fun.null_callback.~T();
      }
      else if (is_const)
      {
        using T = decltype(fun.const_callback);
        fun.const_callback.~T();
      }
      else
      {
        using T = decltype(fun.mutable_callback);
        fun.mutable_callback.~T();
      }
    }

    [[nodiscard]] constexpr bool operator==(std::nullptr_t rhs) const noexcept { return !has_value; }
    [[nodiscard]] constexpr bool operator==(const detail::ConstCallbackBase& rhs) const noexcept
    {
      return has_value && is_const && (std::addressof(fun.const_callback.get()) == std::addressof(rhs));
    }
    [[nodiscard]] constexpr bool operator==(const detail::MutableCallbackBase& rhs) const noexcept
    {
      return has_value && is_const && (std::addressof(fun.mutable_callback.get()) == std::addressof(rhs));
    }
  };
  using CallbackAllocator = typename std::allocator_traits<Allocator>::template rebind_alloc<CallbackRef>;
  using CallbackStorage   = std::vector<CallbackRef, CallbackAllocator>;

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

  bool                         expected = false;
  if (!ctrl_block_->stop_requested.compare_exchange_weak(expected, true))
  {
    return false;
  }

  for (const auto& var : *ctrl_block_->callbacks.lock())
  {
    if (!var.has_value)
    {
      continue;
    }

    if (var.is_const)
    {
      var.fun.const_callback();
    }
    else
    {
      var.fun.mutable_callback();
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
    if (!fun.has_value)
    {
      fun = callback_;
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
  const auto position  = std::find(std::begin(*callbacks), std::end(*callbacks), callback_);
  if (position == std::end(*callbacks))
  {
    return;
  }

  *position = nullptr;
}
