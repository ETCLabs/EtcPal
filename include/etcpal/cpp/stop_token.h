#pragma once

#include <atomic>
#include <functional>
#include <memory>
#include <memory_resource>
#include <variant>

#include <etcpal/cpp/mutex.h>

namespace etcpal
{

struct NoStopState_t
{
};

inline constexpr auto NoStopState = NoStopState_t{};

template <typename Allocator = std::pmr::polymorphic_allocator<std::byte>>
class StopToken;

template <typename Callback, typename Allocator = std::pmr::polymorphic_allocator<std::byte>>
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
  void operator()() override { std::invoke(fun_); }

private:
  Function fun_;
};
template <typename Function>
class Callback<Function, std::enable_if_t<std::is_invocable_v<const Function>>> : public ConstCallbackBase
{
public:
  Callback(Function&& fun) : fun_{std::move(fun)} {}
  void operator()() const override { std::invoke(fun_); }

private:
  Function fun_;
};

}  // namespace detail

template <typename Allocator = std::pmr::polymorphic_allocator<std::byte>>
class StopSource
{
private:
  struct StopContext
  {
    using CallbackRef       = std::variant<std::monostate,
                                           std::reference_wrapper<const detail::ConstCallbackBase>,
                                           std::reference_wrapper<detail::MutableCallbackBase>>;
    using CallbackAllocator = typename std::allocator_traits<Allocator>::template rebind_alloc<CallbackRef>;
    using CallbackStorage   = std::vector<CallbackRef, CallbackAllocator>;

    StopContext() = default;
    StopContext(const CallbackAllocator& alloc) : callbacks{alloc} {}

    Mutex             cb_mutex       = {};
    CallbackStorage   callbacks      = CallbackStorage{CallbackAllocator{}};
    std::atomic<int>  num_sources    = 1;
    std::atomic<bool> stop_requested = false;
  };

public:
  StopSource() = default;
  explicit StopSource(NoStopState_t nss, const Allocator& alloc = {}) noexcept : ctrl_block_{} {}
  explicit StopSource(const Allocator& alloc) : ctrl_block_{std::allocate_shared<StopContext>(alloc, alloc)} {}
  StopSource(const StopSource& rhs) noexcept : ctrl_block_{rhs.ctrl_block_}
  {
    if (ctrl_block_)
    {
      ctrl_block_->num_sources += 1;
    }
  }
  StopSource(StopSource&& rhs) noexcept : ctrl_block_{std::move(rhs.ctrl_block_)} {}

  auto operator=(const StopSource& rhs) noexcept -> StopSource&
  {
    StopSource{std::move(*this)};
    return *this = StopSource{rhs};
  }
  auto operator=(StopSource&& rhs) noexcept -> StopSource&
  {
    StopSource{std::move(*this)};
    ctrl_block_ = std::move(rhs.ctrl_block_);
    return *this;
  }

  ~StopSource() noexcept
  {
    if (ctrl_block_)
    {
      ctrl_block_->num_sources -= 1;
    }
  }

  bool request_stop() noexcept
  {
    if (!ctrl_block_)
    {
      return false;
    }

    const auto guard    = std::lock_guard{ctrl_block_->cb_mutex};
    bool       expected = false;
    if (!ctrl_block_->stop_requested.compare_exchange_weak(expected, true))
    {
      return false;
    }

    for (const auto& var : ctrl_block_->callbacks)
    {
      std::visit(
          [](const auto& fun) {
            if constexpr (!std::is_same_v<std::monostate, std::remove_cv_t<std::remove_reference_t<decltype(fun)>>>)
            {
              std::invoke(fun);
            }
          },
          var);
    }

    return true;
  }

  [[nodiscard]] auto get_token() const noexcept { return StopToken{ctrl_block_}; }
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
  [[nodiscard]] bool stop_possible() const noexcept
  {
    if (!ctrl_block_ || (!ctrl_block_->stop_requested && (ctrl_block_->num_sources == 0)))
    {
      return false;
    }

    return true;
  }

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
                        C&&                         cb) noexcept(std::is_nothrow_constructible_v<Callback, C>)
      : token_{token}, callback_{std::forward<C>(cb)}
  {
    if (!token_.ctrl_block_)
    {
      return;
    }
    if (token_.ctrl_block_->stop_requested)
    {
      std::invoke(callback_);
      return;
    }

    const auto guard = std::lock_guard{token_.ctrl_block_->cb_mutex};
    for (auto& fun : token_.ctrl_block_->callbacks)
    {
      if (std::holds_alternative<std::monostate>(fun))
      {
        fun = callback_;
        return;
      }
    }

    token_.ctrl_block_->callbacks.push_back(callback_);
  }
  template <typename C>
  explicit StopCallback(StopToken<Allocator>&& token, C&& cb) noexcept(std::is_nothrow_constructible_v<Callback, C>)
      : token_{std::move(token)}, callback_{std::forward<C>(cb)}
  {
  }

  StopCallback(const StopCallback& rhs) = delete;
  StopCallback(StopCallback&& rhs)      = delete;

  auto operator=(const StopCallback& rhs) -> StopCallback& = delete;
  auto operator=(StopCallback&& rhs) -> StopCallback&      = delete;

  ~StopCallback() noexcept
  {
    if (!token_.ctrl_block_)
    {
      return;
    }

    const auto guard    = std::lock_guard{token_.ctrl_block_->cb_mutex};
    const auto position = std::find_if(
        std::begin(token_.ctrl_block_->callbacks), std::end(token_.ctrl_block_->callbacks), [this](const auto& var) {
          return std::visit(
              [this](const auto& fun) {
                using fun_type = std::remove_cv_t<std::remove_reference_t<decltype(fun)>>;
                if constexpr (std::is_same_v<std::monostate, fun_type>)
                {
                  return false;
                }
                else if constexpr (std::is_same_v<typename fun_type::type, detail::Callback<callback_type>>)
                {
                  return fun.get() == callback_;
                }

                return false;
              },
              var);
        });
    if (position == std::end(token_.ctrl_block_->callbacks))
    {
      return;
    }

    *position = std::monostate{};
  }

private:
  StopToken<Allocator>                    token_;
  mutable detail::Callback<callback_type> callback_;
};

template <typename Callback, typename Allocator>
StopCallback(const StopToken<Allocator>&, Callback&&)
    -> StopCallback<std::remove_reference_t<std::remove_cv_t<Callback>>, Allocator>;
template <typename Callback, typename Allocator>
StopCallback(StopToken<Allocator>&&, Callback&&)
    -> StopCallback<std::remove_reference_t<std::remove_cv_t<Callback>>, Allocator>;

}  // namespace etcpal
