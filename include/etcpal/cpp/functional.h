#pragma once

#include <utility>

#include <etcpal/cpp/common.h>

namespace etcpal
{

template <typename Signature, typename Allocator = DefaultAllocator>
class MoveOnlyFunction;

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
    virtual ~FunBase() noexcept                                                                        = default;      \
    virtual R operator()(Args... args) ETCPAL_CALLABLE_CV ETCPAL_CALLABLE_REF ETCPAL_CALLABLE_NOEXCEPT = 0;            \
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
  private:                                                                                                             \
    F fun_;                                                                                                            \
  };                                                                                                                   \
                                                                                                                       \
  template <typename R, typename... Args, typename Allocator>                                                          \
  class etcpal::MoveOnlyFunction<R(Args...) ETCPAL_CALLABLE_CV ETCPAL_CALLABLE_REF ETCPAL_CALLABLE_NOEXCEPT,           \
                                 Allocator>                                                                            \
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
    template <                                                                                                         \
        typename F,                                                                                                    \
        typename = std::enable_if_t<                                                                                   \
            !std::is_same<std::remove_cv_t<std::remove_reference_t<F>>, MoveOnlyFunction>::value,                      \
            void_t<decltype(std::declval<ETCPAL_CALLABLE_CV F ETCPAL_CALLABLE_REF>()(std::declval<Args>()...))>>>      \
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
    R operator()(Args... args) ETCPAL_CALLABLE_CV ETCPAL_CALLABLE_REF ETCPAL_CALLABLE_NOEXCEPT                         \
    {                                                                                                                  \
      if (!buffer_)                                                                                                    \
      {                                                                                                                \
        throw std::bad_function_call{};                                                                                \
      }                                                                                                                \
                                                                                                                       \
      return std::forward<ETCPAL_CALLABLE_CV FunBase ETCPAL_CALLABLE_REF>(*buffer_)(std::forward<Args>(args)...);      \
    }                                                                                                                  \
                                                                                                                       \
    [[nodiscard]] explicit operator bool() const noexcept                                                              \
    {                                                                                                                  \
      return ptr_ != nullptr;                                                                                          \
    }                                                                                                                  \
                                                                                                                       \
  private:                                                                                                             \
    std::unique_ptr<FunBase, detail::DeleteUsingAlloc<Allocator>> ptr_ = {};                                           \
  }

#define ETCPAL_CALLABLE_NOEXCEPT

#define ETCPAL_CALLABLE_CV

#define ETCPAL_CALLABLE_REF
ETCPAL_IMPLEMENT_VIRTUAL_CALLABLE;
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
