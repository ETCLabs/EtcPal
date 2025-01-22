#pragma once

#include <utility>
#include <functional>

#include <etcpal/cpp/common.h>

namespace etcpal
{

template <typename... Signatures>
class MoveOnlyFunction;
template <typename... Signatures>
class FunctionRef;

namespace detail
{

template <typename Allocator>
struct DeleteUsingAlloc
{
  Allocator* alloc;

  template <typename T>
  void operator()(T* ptr) const noexcept
  {
    ptr->~T();
    typename std::allocator_traits<Allocator>::template rebind_alloc<T>{*alloc}.deallocate(ptr, 1);
  }
};

template <typename Signature, typename Allocator>
class FunBase;
template <typename Signature, typename Allocator, typename F>
class Callable;

}  // namespace detail

}  // namespace etcpal

#define ETCPAL_IMPLEMENT_VIRTUAL_CALLABLE                                                                              \
  template <typename R, typename... Args, typename Allocator>                                                          \
  class etcpal::detail::FunBase<R(Args...) ETCPAL_CALLABLE_CV ETCPAL_CALLABLE_REF ETCPAL_CALLABLE_NOEXCEPT, Allocator> \
  {                                                                                                                    \
  public:                                                                                                              \
    explicit FunBase(const Allocator& alloc) noexcept : allocator_{alloc}                                              \
    {                                                                                                                  \
    }                                                                                                                  \
    FunBase(const FunBase& rhs)                                                                        = delete;       \
    FunBase(FunBase&& rhs)                                                                             = delete;       \
    virtual ~FunBase() noexcept                                                                        = default;      \
    auto      operator=(const FunBase& rhs) -> FunBase&                                                = delete;       \
    auto      operator=(FunBase&& rhs) -> FunBase&                                                     = delete;       \
    virtual R operator()(Args... args) ETCPAL_CALLABLE_CV ETCPAL_CALLABLE_REF ETCPAL_CALLABLE_NOEXCEPT = 0;            \
    [[nodiscard]] virtual auto target_ptr() const noexcept -> const void*                              = 0;            \
    [[nodiscard]] virtual auto target_ptr() noexcept -> void*                                          = 0;            \
                                                                                                                       \
    [[nodiscard]] auto get_allocator() const noexcept -> const Allocator&                                              \
    {                                                                                                                  \
      return allocator_;                                                                                               \
    }                                                                                                                  \
    [[nodiscard]] auto get_allocator() noexcept -> Allocator&                                                          \
    {                                                                                                                  \
      return allocator_;                                                                                               \
    }                                                                                                                  \
                                                                                                                       \
  private:                                                                                                             \
    Allocator allocator_;                                                                                              \
  };                                                                                                                   \
                                                                                                                       \
  template <typename R, typename... Args, typename Allocator, typename F>                                              \
  class etcpal::detail::Callable<R(Args...) ETCPAL_CALLABLE_CV ETCPAL_CALLABLE_REF ETCPAL_CALLABLE_NOEXCEPT,           \
                                 Allocator, F>                                                                         \
      : public FunBase<R(Args...) ETCPAL_CALLABLE_CV ETCPAL_CALLABLE_REF ETCPAL_CALLABLE_NOEXCEPT, Allocator>          \
  {                                                                                                                    \
  public:                                                                                                              \
    Callable(const F& fun, const Allocator& alloc) noexcept(noexcept(F{fun}))                                          \
        : FunBase<R(Args...) ETCPAL_CALLABLE_CV ETCPAL_CALLABLE_REF ETCPAL_CALLABLE_NOEXCEPT, Allocator>{alloc}        \
        , fun_{fun}                                                                                                    \
    {                                                                                                                  \
    }                                                                                                                  \
    Callable(F&& fun, const Allocator& alloc) noexcept                                                                 \
        : FunBase<R(Args...) ETCPAL_CALLABLE_CV ETCPAL_CALLABLE_REF ETCPAL_CALLABLE_NOEXCEPT, Allocator>{alloc}        \
        , fun_{std::move(fun)}                                                                                         \
    {                                                                                                                  \
    }                                                                                                                  \
                                                                                                                       \
    ~Callable() noexcept override = default;                                                                           \
                                                                                                                       \
    R operator()(Args... args) ETCPAL_CALLABLE_CV ETCPAL_CALLABLE_REF ETCPAL_CALLABLE_NOEXCEPT override                \
    {                                                                                                                  \
      return std::forward<ETCPAL_CALLABLE_CV F ETCPAL_CALLABLE_REF>(fun_)(std::forward<Args>(args)...);                \
    }                                                                                                                  \
                                                                                                                       \
    [[nodiscard]] auto target_ptr() const noexcept -> const void* override                                             \
    {                                                                                                                  \
      return reinterpret_cast<const void*>(std::addressof(fun_));                                                      \
    }                                                                                                                  \
                                                                                                                       \
    [[nodiscard]] auto target_ptr() noexcept -> void* override                                                         \
    {                                                                                                                  \
      return reinterpret_cast<void*>(std::addressof(fun_));                                                            \
    }                                                                                                                  \
                                                                                                                       \
  private:                                                                                                             \
    F fun_;                                                                                                            \
  };                                                                                                                   \
                                                                                                                       \
  template <typename R, typename... Args>                                                                              \
  class etcpal::MoveOnlyFunction<R(Args...) ETCPAL_CALLABLE_CV ETCPAL_CALLABLE_REF ETCPAL_CALLABLE_NOEXCEPT>           \
      : public MoveOnlyFunction<R(Args...) ETCPAL_CALLABLE_CV ETCPAL_CALLABLE_REF ETCPAL_CALLABLE_NOEXCEPT,            \
                                DefaultAllocator>                                                                      \
  {                                                                                                                    \
  public:                                                                                                              \
    using Parent = MoveOnlyFunction<R(Args...) ETCPAL_CALLABLE_CV ETCPAL_CALLABLE_REF ETCPAL_CALLABLE_NOEXCEPT,        \
                                    DefaultAllocator>;                                                                 \
    using Parent::Parent;                                                                                              \
  };                                                                                                                   \
                                                                                                                       \
  template <typename R, typename... Args, typename Allocator>                                                          \
  class etcpal::MoveOnlyFunction<                                                                                      \
      R(Args...) ETCPAL_CALLABLE_CV ETCPAL_CALLABLE_REF ETCPAL_CALLABLE_NOEXCEPT,                                      \
      etcpal::detail::FirstOf<Allocator, decltype(Allocator{}.deallocate(Allocator{}.allocate(1), 1))>>                \
  {                                                                                                                    \
  private:                                                                                                             \
    template <typename F>                                                                                              \
    using Callable = detail::Callable<R(Args...) ETCPAL_CALLABLE_CV ETCPAL_CALLABLE_REF ETCPAL_CALLABLE_NOEXCEPT,      \
                                      Allocator,                                                                       \
                                      std::remove_cv_t<std::remove_reference_t<F>>>;                                   \
    using FunBase =                                                                                                    \
        detail::FunBase<R(Args...) ETCPAL_CALLABLE_CV ETCPAL_CALLABLE_REF ETCPAL_CALLABLE_NOEXCEPT, Allocator>;        \
                                                                                                                       \
  public:                                                                                                              \
    using result_type = R;                                                                                             \
                                                                                                                       \
    MoveOnlyFunction() noexcept = default;                                                                             \
    MoveOnlyFunction(std::nullptr_t tag) noexcept : MoveOnlyFunction{}                                                 \
    {                                                                                                                  \
    }                                                                                                                  \
    template <typename F,                                                                                              \
              typename = std::enable_if_t<                                                                             \
                  !std::is_same<std::remove_cv_t<std::remove_reference_t<F>>, MoveOnlyFunction>::value &&              \
                  std::is_convertible<                                                                                 \
                      decltype(std::declval<ETCPAL_CALLABLE_CV F ETCPAL_CALLABLE_REF>()(std::declval<Args>()...)),     \
                      R>::value>>                                                                                      \
    MoveOnlyFunction(F&& f, const Allocator& alloc = {})                                                               \
    {                                                                                                                  \
      auto* const fun =                                                                                                \
          typename std::allocator_traits<Allocator>::template rebind_alloc<Callable<F>>{alloc}.allocate(1);            \
      try                                                                                                              \
      {                                                                                                                \
        new (fun) Callable<F>{std::forward<F>(f), alloc};                                                              \
        ptr_ = std::unique_ptr<FunBase, detail::DeleteUsingAlloc<Allocator>>{                                          \
            fun, detail::DeleteUsingAlloc<Allocator>{std::addressof(fun->get_allocator())}};                           \
      }                                                                                                                \
      catch (...)                                                                                                      \
      {                                                                                                                \
        typename std::allocator_traits<Allocator>::template rebind_alloc<Callable<F>>{alloc}.deallocate(fun, 1);       \
        throw;                                                                                                         \
      }                                                                                                                \
    }                                                                                                                  \
                                                                                                                       \
    R operator()(Args... args) ETCPAL_CALLABLE_CV ETCPAL_CALLABLE_REF                                                  \
    {                                                                                                                  \
      return ptr_ ? std::forward<ETCPAL_CALLABLE_CV FunBase ETCPAL_CALLABLE_REF>(*ptr_)(std::forward<Args>(args)...)   \
                  : throw std::bad_function_call{};                                                                    \
    }                                                                                                                  \
                                                                                                                       \
    [[nodiscard]] explicit operator bool() const noexcept                                                              \
    {                                                                                                                  \
      return ptr_ != nullptr;                                                                                          \
    }                                                                                                                  \
                                                                                                                       \
  protected:                                                                                                           \
    [[nodiscard]] auto* target_ptr() const noexcept                                                                    \
    {                                                                                                                  \
      return ptr_->target_ptr();                                                                                       \
    }                                                                                                                  \
                                                                                                                       \
    [[nodiscard]] auto* target_ptr() noexcept                                                                          \
    {                                                                                                                  \
      return ptr_->target_ptr();                                                                                       \
    }                                                                                                                  \
                                                                                                                       \
  private:                                                                                                             \
    std::unique_ptr<FunBase, detail::DeleteUsingAlloc<Allocator>> ptr_ = {};                                           \
  };                                                                                                                   \
                                                                                                                       \
  template <typename R, typename... Args, typename FirstSignature, typename... Rest>                                   \
  class etcpal::MoveOnlyFunction<R(Args...) ETCPAL_CALLABLE_CV ETCPAL_CALLABLE_REF ETCPAL_CALLABLE_NOEXCEPT,           \
                                 FirstSignature, Rest...> : public MoveOnlyFunction<FirstSignature, Rest...>           \
  {                                                                                                                    \
  private:                                                                                                             \
    using Parent = MoveOnlyFunction<FirstSignature, Rest...>;                                                          \
                                                                                                                       \
  public:                                                                                                              \
    MoveOnlyFunction() noexcept = default;                                                                             \
    MoveOnlyFunction(std::nullptr_t tag) noexcept : Parent{tag}                                                        \
    {                                                                                                                  \
    }                                                                                                                  \
    template <typename F,                                                                                              \
              typename Allocator,                                                                                      \
              typename = std::enable_if_t<                                                                             \
                  !std::is_same<std::remove_cv_t<std::remove_reference_t<F>>, MoveOnlyFunction>::value &&              \
                  detail::IsCallableR<R, F, Args...>::value>>                                                          \
    MoveOnlyFunction(F&& f, const Allocator& alloc)                                                                    \
        : Parent{std::forward<F>(f), alloc}, thunk{[](ETCPAL_CALLABLE_CV void* obj, Args... args) -> R {               \
          return static_cast<ETCPAL_CALLABLE_CV F ETCPAL_CALLABLE_REF>(*reinterpret_cast<ETCPAL_CALLABLE_CV F*>(obj))( \
              std::forward<Args>(args)...);                                                                            \
        }}                                                                                                             \
    {                                                                                                                  \
    }                                                                                                                  \
    template <typename F,                                                                                              \
              typename = std::enable_if_t<                                                                             \
                  !std::is_same<std::remove_cv_t<std::remove_reference_t<F>>, MoveOnlyFunction>::value &&              \
                  detail::IsCallableR<R, F, Args...>::value>>                                                          \
    MoveOnlyFunction(F&& f)                                                                                            \
        : Parent{std::forward<F>(f)}, thunk{[](ETCPAL_CALLABLE_CV void* obj, Args... args) -> R {                      \
          return static_cast<ETCPAL_CALLABLE_CV F ETCPAL_CALLABLE_REF>(*reinterpret_cast<ETCPAL_CALLABLE_CV F*>(obj))( \
              std::forward<Args>(args)...);                                                                            \
        }}                                                                                                             \
    {                                                                                                                  \
    }                                                                                                                  \
                                                                                                                       \
    R operator()(Args... args) ETCPAL_CALLABLE_CV ETCPAL_CALLABLE_REF                                                  \
    {                                                                                                                  \
      return target_ptr() ? thunk(target_ptr(), std::forward<Args>(args)...) : throw std::bad_function_call{};         \
    }                                                                                                                  \
    template <typename... T, typename = std::enable_if_t<detail::IsCallable<const Parent, T...>::value>>               \
    constexpr decltype(auto) operator()(T&&... args) const&& noexcept(                                                 \
        detail::IsNothrowCallable<const Parent, T...>::value)                                                          \
    {                                                                                                                  \
      return static_cast<const Parent&&>(*this)(std::forward<T>(args)...);                                             \
    }                                                                                                                  \
    template <typename... T, typename = std::enable_if_t<detail::IsCallable<const Parent&, T...>::value>>              \
    constexpr decltype(auto) operator()(T&&... args) const& noexcept(                                                  \
        detail::IsNothrowCallable<const Parent&, T...>::value)                                                         \
    {                                                                                                                  \
      return static_cast<const Parent&>(*this)(std::forward<T>(args)...);                                              \
    }                                                                                                                  \
    template <typename... T, typename = std::enable_if_t<detail::IsCallable<Parent, T...>::value>>                     \
    constexpr decltype(auto) operator()(T&&... args) && noexcept(detail::IsNothrowCallable<Parent, T...>::value)       \
    {                                                                                                                  \
      return static_cast<Parent&&>(*this)(std::forward<T>(args)...);                                                   \
    }                                                                                                                  \
    template <typename... T, typename = std::enable_if_t<detail::IsCallable<Parent&, T...>::value>>                    \
    constexpr decltype(auto) operator()(T&&... args) & noexcept(detail::IsNothrowCallable<Parent&, T...>::value)       \
    {                                                                                                                  \
      return static_cast<Parent&>(*this)(std::forward<T>(args)...);                                                    \
    }                                                                                                                  \
                                                                                                                       \
    [[nodiscard]] explicit operator bool() const noexcept                                                              \
    {                                                                                                                  \
      return bool{static_cast<const Parent&>(*this)};                                                                  \
    }                                                                                                                  \
                                                                                                                       \
  protected:                                                                                                           \
    [[nodiscard]] auto* target_ptr() const noexcept                                                                    \
    {                                                                                                                  \
      return Parent::target_ptr();                                                                                     \
    }                                                                                                                  \
                                                                                                                       \
    [[nodiscard]] auto* target_ptr() noexcept                                                                          \
    {                                                                                                                  \
      return Parent::target_ptr();                                                                                     \
    }                                                                                                                  \
                                                                                                                       \
  private:                                                                                                             \
    R (*thunk)(ETCPAL_CALLABLE_CV void*, Args...) = nullptr;                                                           \
  }

#define ETCPAL_IMPLEMENT_FUNCTION_REF                                                                                  \
  template <typename R, typename... Args>                                                                              \
  class etcpal::FunctionRef<R(Args...) ETCPAL_CALLABLE_CV ETCPAL_CALLABLE_NOEXCEPT>                                    \
  {                                                                                                                    \
  public:                                                                                                              \
    constexpr FunctionRef() noexcept = default;                                                                        \
    constexpr FunctionRef(std::nullptr_t tag) noexcept : FunctionRef{}                                                 \
    {                                                                                                                  \
    }                                                                                                                  \
    constexpr FunctionRef(R (*f)(Args...) ETCPAL_CALLABLE_NOEXCEPT) noexcept : fun_{f}                                 \
    {                                                                                                                  \
    }                                                                                                                  \
    template <                                                                                                         \
        typename F,                                                                                                    \
        typename = std::enable_if_t<!std::is_same<std::remove_cv_t<std::remove_reference_t<F>>, FunctionRef>::value && \
                                    detail::IsCallableR<R, F, Args...>::value>>                                        \
    FunctionRef(F&& f) noexcept                                                                                        \
        : fun_{[](ETCPAL_CALLABLE_CV void* obj, Args... args) -> R {                                                   \
          return (*reinterpret_cast<ETCPAL_CALLABLE_CV std::remove_reference_t<F>*>(obj))(                             \
              std::forward<Args>(args)...);                                                                            \
        }}                                                                                                             \
        , obj_{const_cast<void*>(reinterpret_cast<ETCPAL_CALLABLE_CV void*>(std::addressof(f)))}                       \
    {                                                                                                                  \
    }                                                                                                                  \
                                                                                                                       \
    constexpr R operator()(Args... args) const ETCPAL_CALLABLE_NOEXCEPT                                                \
    {                                                                                                                  \
      return obj_ ? fun_.thunk(obj_, std::forward<Args>(args)...) : fun_.actual(std::forward<Args>(args)...);          \
    }                                                                                                                  \
                                                                                                                       \
    [[nodiscard]] explicit constexpr operator bool() const noexcept                                                    \
    {                                                                                                                  \
      return obj_ ? (fun_.thunk != nullptr) : (fun_.actual != nullptr);                                                \
    }                                                                                                                  \
                                                                                                                       \
    [[nodiscard]] auto target_address() const noexcept -> ETCPAL_CALLABLE_CV void*                                     \
    {                                                                                                                  \
      return obj_ ? obj_ : reinterpret_cast<ETCPAL_CALLABLE_CV void*>(fun_.actual);                                    \
    }                                                                                                                  \
                                                                                                                       \
    [[nodiscard]] constexpr auto target() const noexcept -> R (*)(Args...) ETCPAL_CALLABLE_NOEXCEPT                    \
    {                                                                                                                  \
      return obj_ ? nullptr : fun_.actual;                                                                             \
    }                                                                                                                  \
                                                                                                                       \
  private:                                                                                                             \
    union                                                                                                              \
    {                                                                                                                  \
      R (*thunk)(ETCPAL_CALLABLE_CV void*, Args...) ETCPAL_CALLABLE_NOEXCEPT;                                          \
      R (*actual)(Args...) ETCPAL_CALLABLE_NOEXCEPT;                                                                   \
    } fun_     = {};                                                                                                   \
    void* obj_ = nullptr;                                                                                              \
  };                                                                                                                   \
                                                                                                                       \
  template <typename R, typename... Args, typename FirstSignature, typename... OtherSignatures>                        \
  class etcpal::FunctionRef<R(Args...) ETCPAL_CALLABLE_CV ETCPAL_CALLABLE_NOEXCEPT, FirstSignature,                    \
                            OtherSignatures...> : public FunctionRef<FirstSignature, OtherSignatures...>               \
  {                                                                                                                    \
  private:                                                                                                             \
    using Parent = FunctionRef<FirstSignature, OtherSignatures...>;                                                    \
                                                                                                                       \
  public:                                                                                                              \
    template <typename F, typename = std::enable_if_t<detail::IsCallable<F, Args...>::value>>                          \
    constexpr FunctionRef(F&& fun) noexcept                                                                            \
        : Parent{std::forward<F>(fun)}, thunk{[](ETCPAL_CALLABLE_CV void* obj, Args... args) -> R {                    \
          return *reinterpret_cast<ETCPAL_CALLABLE_CV std::remove_reference_t<F>*>(obj)(std::forward<Args>(args)...);  \
        }}                                                                                                             \
    {                                                                                                                  \
    }                                                                                                                  \
                                                                                                                       \
    constexpr R operator()(Args... args) ETCPAL_CALLABLE_CV ETCPAL_CALLABLE_NOEXCEPT                                   \
    {                                                                                                                  \
      return thunk(target_address(), std::forward<Args>(args)...);                                                     \
    }                                                                                                                  \
    template <typename... T, typename = std::enable_if_t<detail::IsCallable<const Parent, T...>::value>>               \
    constexpr decltype(auto) operator()(T&&... args) const&& noexcept(                                                 \
        detail::IsNothrowCallable<const Parent, T...>::value)                                                          \
    {                                                                                                                  \
      return static_cast<const Parent&&>(*this)(std::forward<T>(args)...);                                             \
    }                                                                                                                  \
    template <typename... T, typename = std::enable_if_t<detail::IsCallable<const Parent&, T...>::value>>              \
    constexpr decltype(auto) operator()(T&&... args) const& noexcept(                                                  \
        detail::IsNothrowCallable<const Parent&, T...>::value)                                                         \
    {                                                                                                                  \
      return static_cast<const Parent&>(*this)(std::forward<T>(args)...);                                              \
    }                                                                                                                  \
    template <typename... T, typename = std::enable_if_t<detail::IsCallable<Parent, T...>::value>>                     \
    constexpr decltype(auto) operator()(T&&... args) && noexcept(detail::IsNothrowCallable<Parent, T...>::value)       \
    {                                                                                                                  \
      return static_cast<Parent&&>(*this)(std::forward<T>(args)...);                                                   \
    }                                                                                                                  \
    template <typename... T, typename = std::enable_if_t<detail::IsCallable<Parent&, T...>::value>>                    \
    constexpr decltype(auto) operator()(T&&... args) & noexcept(detail::IsNothrowCallable<Parent&, T...>::value)       \
    {                                                                                                                  \
      return static_cast<Parent&>(*this)(std::forward<T>(args)...);                                                    \
    }                                                                                                                  \
                                                                                                                       \
  protected:                                                                                                           \
    [[nodiscard]] constexpr auto* target_address() const noexcept                                                      \
    {                                                                                                                  \
      return Parent::target_address();                                                                                 \
    }                                                                                                                  \
                                                                                                                       \
  private:                                                                                                             \
    R (*thunk)(Args...) = nullptr;                                                                                     \
  }

#define ETCPAL_CALLABLE_NOEXCEPT

#define ETCPAL_CALLABLE_CV

#define ETCPAL_CALLABLE_REF
ETCPAL_IMPLEMENT_VIRTUAL_CALLABLE;
ETCPAL_IMPLEMENT_FUNCTION_REF;
#undef ETCPAL_CALLABLE_REF

#define ETCPAL_CALLABLE_REF &
ETCPAL_IMPLEMENT_VIRTUAL_CALLABLE;
#undef ETCPAL_CALLABLE_REF

#define ETCPAL_CALLABLE_REF &&
ETCPAL_IMPLEMENT_VIRTUAL_CALLABLE;
#undef ETCPAL_CALLABLE_REF

#undef ETCPAL_CALLABLE_CV

#define ETCPAL_CALLABLE_CV const

#define ETCPAL_CALLABLE_REF
ETCPAL_IMPLEMENT_VIRTUAL_CALLABLE;
ETCPAL_IMPLEMENT_FUNCTION_REF;
#undef ETCPAL_CALLABLE_REF

#define ETCPAL_CALLABLE_REF &
ETCPAL_IMPLEMENT_VIRTUAL_CALLABLE;
#undef ETCPAL_CALLABLE_REF

#define ETCPAL_CALLABLE_REF &&
ETCPAL_IMPLEMENT_VIRTUAL_CALLABLE;
#undef ETCPAL_CALLABLE_REF

#undef ETCPAL_CALLABLE_CV

#undef ETCPAL_CALLABLE_NOEXCEPT

#if (__cplusplus >= 201703L)

#define ETCPAL_CALLABLE_NOEXCEPT noexcept

#define ETCPAL_CALLABLE_CV

#define ETCPAL_CALLABLE_REF
ETCPAL_IMPLEMENT_VIRTUAL_CALLABLE;
ETCPAL_IMPLEMENT_FUNCTION_REF;
#undef ETCPAL_CALLABLE_REF

#define ETCPAL_CALLABLE_REF &
ETCPAL_IMPLEMENT_VIRTUAL_CALLABLE;
#undef ETCPAL_CALLABLE_REF

#define ETCPAL_CALLABLE_REF &&
ETCPAL_IMPLEMENT_VIRTUAL_CALLABLE;
#undef ETCPAL_CALLABLE_REF

#undef ETCPAL_CALLABLE_CV

#define ETCPAL_CALLABLE_CV const

#define ETCPAL_CALLABLE_REF
ETCPAL_IMPLEMENT_VIRTUAL_CALLABLE;
ETCPAL_IMPLEMENT_FUNCTION_REF;
#undef ETCPAL_CALLABLE_REF

#define ETCPAL_CALLABLE_REF &
ETCPAL_IMPLEMENT_VIRTUAL_CALLABLE;
#undef ETCPAL_CALLABLE_REF

#define ETCPAL_CALLABLE_REF &&
ETCPAL_IMPLEMENT_VIRTUAL_CALLABLE;
#undef ETCPAL_CALLABLE_REF

#undef ETCPAL_CALLABLE_CV

#undef ETCPAL_CALLABLE_NOEXCEPT

#endif  // #if (__cplusplus >= 201703L)

#undef ETCPAL_IMPLEMENT_VIRTUAL_CALLABLE
#undef ETCPAL_IMPLEMENT_FUNCTION_REF
