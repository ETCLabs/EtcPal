#pragma once

#include <utility>
#include <functional>

#include <etcpal/cpp/common.h>
#include <etcpal/cpp/memory.h>
#include <etcpal/cpp/optional.h>

namespace etcpal
{

/// @brief A move-only type-erased function object with allocator support and multi-signature support.
///
/// A function object of this type can type-earse any object invocable with all of the given signatures. For example,
/// the type `MoveOnlyFunction<void(), void(int)>` can be invoked with either no arguments, or a single argument of type
/// `int`. A custom allocator type may also be specified as the last template argument, as in
/// `MoveOnlyFunction<void(), void(int), std::allocator<unsigned char>>`.
///
/// The constructor of a `MoveOnlyFunction` takes one or two arguments. The first argument is the callable object to
/// type-erase, and the second argument is the allocator to use. Initializing a `MoveOnlyFunction` with a `nullptr`
/// creates an object that throws `std::bad_function_call` when invoked.
///
/// @tparam Signatures The call signatures that this object supports.
template <typename... Signatures>
class MoveOnlyFunction;

/// @brief A non-owning reference to a callable object, with multi-signature support.
///
/// A function reference of this type can type-erase any object invocable with all of the given signatures. For example,
/// the type `FunctionRef<void(), void(int)>` can be invoked with either no arguments, or a single argument of type
/// `int`.
///
/// The constructor of a `FunctionRef` takes a single argument, the callable object to form a type-erased reference to.
/// Initializing a `FunctionRef` with a `nullptr` creates a `FunctionRef` with call operators that invoke undefined
/// behavior.
///
/// @tparam Signatures The call sugnatures that this object supports.
template <typename... Signatures>
class FunctionRef;

namespace detail
{

template <typename Signature>
class FunBase;
template <typename Signature, typename F>
class Callable;

}  // namespace detail

}  // namespace etcpal

#define ETCPAL_IMPLEMENT_VIRTUAL_CALLABLE                                                                              \
  template <typename R, typename... Args>                                                                              \
  class etcpal::detail::FunBase<R(Args...) ETCPAL_CALLABLE_CV ETCPAL_CALLABLE_REF ETCPAL_CALLABLE_NOEXCEPT>            \
  {                                                                                                                    \
  public:                                                                                                              \
    FunBase() noexcept                                                                                 = default;      \
    FunBase(const FunBase& rhs)                                                                        = delete;       \
    FunBase(FunBase&& rhs)                                                                             = delete;       \
    virtual ~FunBase() noexcept                                                                        = default;      \
    auto      operator=(const FunBase& rhs) -> FunBase&                                                = delete;       \
    auto      operator=(FunBase&& rhs) -> FunBase&                                                     = delete;       \
    virtual R operator()(Args... args) ETCPAL_CALLABLE_CV ETCPAL_CALLABLE_REF ETCPAL_CALLABLE_NOEXCEPT = 0;            \
    [[nodiscard]] virtual auto target_ptr() const noexcept -> const void*                              = 0;            \
    [[nodiscard]] virtual auto target_ptr() noexcept -> void*                                          = 0;            \
  };                                                                                                                   \
                                                                                                                       \
  template <typename R, typename... Args, typename F>                                                                  \
  class etcpal::detail::Callable<R(Args...) ETCPAL_CALLABLE_CV ETCPAL_CALLABLE_REF ETCPAL_CALLABLE_NOEXCEPT, F>        \
      : public FunBase<R(Args...) ETCPAL_CALLABLE_CV ETCPAL_CALLABLE_REF ETCPAL_CALLABLE_NOEXCEPT>                     \
  {                                                                                                                    \
  public:                                                                                                              \
    Callable(const F& fun) noexcept(noexcept(F{fun})) : fun_{fun}                                                      \
    {                                                                                                                  \
    }                                                                                                                  \
    Callable(F&& fun) noexcept : fun_{std::move(fun)}                                                                  \
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
                                polymorphic_allocator<>>                                                               \
  {                                                                                                                    \
  public:                                                                                                              \
    using Parent = MoveOnlyFunction<R(Args...) ETCPAL_CALLABLE_CV ETCPAL_CALLABLE_REF ETCPAL_CALLABLE_NOEXCEPT,        \
                                    polymorphic_allocator<>>;                                                          \
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
                                      RemoveCVRef_t<F>>;                                                               \
    using FunBase  = detail::FunBase<R(Args...) ETCPAL_CALLABLE_CV ETCPAL_CALLABLE_REF ETCPAL_CALLABLE_NOEXCEPT>;      \
                                                                                                                       \
  public:                                                                                                              \
    using result_type = R;                                                                                             \
                                                                                                                       \
    MoveOnlyFunction() noexcept = default;                                                                             \
    MoveOnlyFunction(std::nullptr_t tag) noexcept : MoveOnlyFunction{}                                                 \
    {                                                                                                                  \
    }                                                                                                                  \
    template <                                                                                                         \
        typename F,                                                                                                    \
        typename = std::enable_if_t<!std::is_same<RemoveCVRef_t<F>, MoveOnlyFunction>::value &&                        \
                                    detail::IsCallableR<R, ETCPAL_CALLABLE_CV F ETCPAL_CALLABLE_REF, Args...>::value>> \
    MoveOnlyFunction(F&& f, const Allocator& alloc = {})                                                               \
        : ptr_{allocate_unique<Callable<F>>(alloc, std::forward<F>(f))}                                                \
    {                                                                                                                  \
    }                                                                                                                  \
    template <typename T, typename U>                                                                                  \
    MoveOnlyFunction(T U::* f, const Allocator& alloc = {})                                                            \
        : MoveOnlyFunction{[f](auto&& obj, auto&&... args) {                                                           \
                             return ((*std::forward<decltype(obj)>(obj)).*f)(std::forward<decltype(args)>(args)...);   \
                           },                                                                                          \
                           alloc}                                                                                      \
    {                                                                                                                  \
    }                                                                                                                  \
                                                                                                                       \
    template <typename Enable = R, std::enable_if_t<!std::is_void<Enable>::value>* = nullptr>                          \
    R operator()(Args... args) ETCPAL_CALLABLE_CV ETCPAL_CALLABLE_REF                                                  \
    {                                                                                                                  \
      return ETCPAL_TERNARY_THROW(                                                                                     \
          ptr_, std::forward<ETCPAL_CALLABLE_CV FunBase ETCPAL_CALLABLE_REF>(*ptr_)(std::forward<Args>(args)...),      \
          std::bad_function_call{});                                                                                   \
    }                                                                                                                  \
                                                                                                                       \
    template <typename Enable = R, std::enable_if_t<std::is_void<Enable>::value>* = nullptr>                           \
    R operator()(Args... args) ETCPAL_CALLABLE_CV ETCPAL_CALLABLE_REF                                                  \
    {                                                                                                                  \
      if (ptr_)                                                                                                        \
      {                                                                                                                \
        std::forward<ETCPAL_CALLABLE_CV FunBase ETCPAL_CALLABLE_REF> (*ptr_)(std::forward<Args>(args)...);             \
      }                                                                                                                \
                                                                                                                       \
      ETCPAL_THROW(std::bad_function_call{});                                                                          \
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
    std::unique_ptr<FunBase, DeleteUsingAlloc<FunBase, Allocator>> ptr_ = {};                                          \
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
    template <                                                                                                         \
        typename F,                                                                                                    \
        typename Allocator,                                                                                            \
        typename = std::enable_if_t<!std::is_same<RemoveCVRef_t<F>, MoveOnlyFunction>::value &&                        \
                                    detail::IsCallableR<R, ETCPAL_CALLABLE_CV F ETCPAL_CALLABLE_REF, Args...>::value>> \
    MoveOnlyFunction(F&& f, const Allocator& alloc)                                                                    \
        : Parent{std::forward<F>(f), alloc}, thunk{[](ETCPAL_CALLABLE_CV void* obj, Args... args) -> R {               \
          return static_cast<ETCPAL_CALLABLE_CV F ETCPAL_CALLABLE_REF>(*reinterpret_cast<ETCPAL_CALLABLE_CV F*>(obj))( \
              std::forward<Args>(args)...);                                                                            \
        }}                                                                                                             \
    {                                                                                                                  \
    }                                                                                                                  \
    template <                                                                                                         \
        typename F,                                                                                                    \
        typename = std::enable_if_t<!std::is_same<RemoveCVRef_t<F>, MoveOnlyFunction>::value &&                        \
                                    detail::IsCallableR<R, ETCPAL_CALLABLE_CV F ETCPAL_CALLABLE_REF, Args...>::value>> \
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
      return ETCPAL_TERNARY_THROW(target_ptr(), thunk(target_ptr(), std::forward<Args>(args)...),                      \
                                  std::bad_function_call{});                                                           \
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

#define ETCPAL_IMPLEMENT_FUNCTION_REF                                                                                 \
  template <typename R, typename... Args>                                                                             \
  class etcpal::FunctionRef<R(Args...) ETCPAL_CALLABLE_CV ETCPAL_CALLABLE_NOEXCEPT>                                   \
  {                                                                                                                   \
  public:                                                                                                             \
    constexpr FunctionRef() noexcept = default;                                                                       \
    constexpr FunctionRef(std::nullptr_t tag) noexcept : FunctionRef{}                                                \
    {                                                                                                                 \
    }                                                                                                                 \
    constexpr FunctionRef(R (*f)(Args...) ETCPAL_CALLABLE_NOEXCEPT) noexcept : fun_{f}                                \
    {                                                                                                                 \
    }                                                                                                                 \
    template <typename F,                                                                                             \
              typename = std::enable_if_t<!std::is_same<RemoveCVRef_t<F>, FunctionRef>::value &&                      \
                                          detail::IsCallableR<R, F, Args...>::value>>                                 \
    FunctionRef(F&& f) noexcept                                                                                       \
        : fun_{[](ETCPAL_CALLABLE_CV void* obj, Args... args) -> R {                                                  \
          return (*reinterpret_cast<ETCPAL_CALLABLE_CV std::remove_reference_t<F>*>(obj))(                            \
              std::forward<Args>(args)...);                                                                           \
        }}                                                                                                            \
        , obj_{const_cast<void*>(reinterpret_cast<ETCPAL_CALLABLE_CV void*>(std::addressof(f)))}                      \
    {                                                                                                                 \
    }                                                                                                                 \
                                                                                                                      \
    constexpr R operator()(Args... args) const ETCPAL_CALLABLE_NOEXCEPT                                               \
    {                                                                                                                 \
      return obj_ ? fun_.thunk(obj_, std::forward<Args>(args)...) : fun_.actual(std::forward<Args>(args)...);         \
    }                                                                                                                 \
                                                                                                                      \
    [[nodiscard]] explicit constexpr operator bool() const noexcept                                                   \
    {                                                                                                                 \
      return obj_ ? (fun_.thunk != nullptr) : (fun_.actual != nullptr);                                               \
    }                                                                                                                 \
                                                                                                                      \
    [[nodiscard]] auto target_address() const noexcept -> ETCPAL_CALLABLE_CV void*                                    \
    {                                                                                                                 \
      return obj_ ? obj_ : reinterpret_cast<ETCPAL_CALLABLE_CV void*>(fun_.actual);                                   \
    }                                                                                                                 \
                                                                                                                      \
    [[nodiscard]] constexpr auto target() const noexcept -> R (*)(Args...) ETCPAL_CALLABLE_NOEXCEPT                   \
    {                                                                                                                 \
      return obj_ ? nullptr : fun_.actual;                                                                            \
    }                                                                                                                 \
                                                                                                                      \
  private:                                                                                                            \
    union                                                                                                             \
    {                                                                                                                 \
      R (*thunk)(ETCPAL_CALLABLE_CV void*, Args...) ETCPAL_CALLABLE_NOEXCEPT;                                         \
      R (*actual)(Args...) ETCPAL_CALLABLE_NOEXCEPT;                                                                  \
    } fun_     = {};                                                                                                  \
    void* obj_ = nullptr;                                                                                             \
  };                                                                                                                  \
                                                                                                                      \
  template <typename R, typename... Args, typename FirstSignature, typename... OtherSignatures>                       \
  class etcpal::FunctionRef<R(Args...) ETCPAL_CALLABLE_CV ETCPAL_CALLABLE_NOEXCEPT, FirstSignature,                   \
                            OtherSignatures...> : public FunctionRef<FirstSignature, OtherSignatures...>              \
  {                                                                                                                   \
  private:                                                                                                            \
    using Parent = FunctionRef<FirstSignature, OtherSignatures...>;                                                   \
                                                                                                                      \
  public:                                                                                                             \
    template <typename F, typename = std::enable_if_t<detail::IsCallable<F, Args...>::value>>                         \
    constexpr FunctionRef(F&& fun) noexcept                                                                           \
        : Parent{std::forward<F>(fun)}, thunk{[](ETCPAL_CALLABLE_CV void* obj, Args... args) -> R {                   \
          return *reinterpret_cast<ETCPAL_CALLABLE_CV std::remove_reference_t<F>*>(obj)(std::forward<Args>(args)...); \
        }}                                                                                                            \
    {                                                                                                                 \
    }                                                                                                                 \
                                                                                                                      \
    constexpr R operator()(Args... args) ETCPAL_CALLABLE_CV ETCPAL_CALLABLE_NOEXCEPT                                  \
    {                                                                                                                 \
      return thunk(target_address(), std::forward<Args>(args)...);                                                    \
    }                                                                                                                 \
    template <typename... T, typename = std::enable_if_t<detail::IsCallable<const Parent, T...>::value>>              \
    constexpr decltype(auto) operator()(T&&... args) const&& noexcept(                                                \
        detail::IsNothrowCallable<const Parent, T...>::value)                                                         \
    {                                                                                                                 \
      return static_cast<const Parent&&>(*this)(std::forward<T>(args)...);                                            \
    }                                                                                                                 \
    template <typename... T, typename = std::enable_if_t<detail::IsCallable<const Parent&, T...>::value>>             \
    constexpr decltype(auto) operator()(T&&... args) const& noexcept(                                                 \
        detail::IsNothrowCallable<const Parent&, T...>::value)                                                        \
    {                                                                                                                 \
      return static_cast<const Parent&>(*this)(std::forward<T>(args)...);                                             \
    }                                                                                                                 \
    template <typename... T, typename = std::enable_if_t<detail::IsCallable<Parent, T...>::value>>                    \
    constexpr decltype(auto) operator()(T&&... args) && noexcept(detail::IsNothrowCallable<Parent, T...>::value)      \
    {                                                                                                                 \
      return static_cast<Parent&&>(*this)(std::forward<T>(args)...);                                                  \
    }                                                                                                                 \
    template <typename... T, typename = std::enable_if_t<detail::IsCallable<Parent&, T...>::value>>                   \
    constexpr decltype(auto) operator()(T&&... args) & noexcept(detail::IsNothrowCallable<Parent&, T...>::value)      \
    {                                                                                                                 \
      return static_cast<Parent&>(*this)(std::forward<T>(args)...);                                                   \
    }                                                                                                                 \
                                                                                                                      \
  protected:                                                                                                          \
    [[nodiscard]] constexpr auto* target_address() const noexcept                                                     \
    {                                                                                                                 \
      return Parent::target_address();                                                                                \
    }                                                                                                                 \
                                                                                                                      \
  private:                                                                                                            \
    R (*thunk)(Args...) = nullptr;                                                                                    \
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
