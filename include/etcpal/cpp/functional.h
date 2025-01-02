#pragma once

#include <memory>
#include <memory_resource>

namespace etcpal
{

template <typename Signature, typename Allocator = std::pmr::polymorphic_allocator<std::byte>>
class move_only_function;

namespace detail
{

template <typename Allocator>
struct DeleteUsingAlloc
{
  Allocator alloc;

  template <typename T>
  void operator()(T* ptr) const noexcept
  {
    ptr->~T();
    typename std::allocator_traits<Allocator>::template rebind<T>{alloc}.deallocate(ptr, 1);
  }
};

template <typename Signature>
class FunBase;
template <typename Signature, typename F>
class Callable;

}  // namespace detail

}  // namespace etcpal

#define ETCPAL_IMPLEMENT_VIRTUAL_CALLABLE                                                                        \
  template <typename R, typename... Args>                                                                        \
  class etcpal::detail::FunBase<R(Args...) ETCPAL_CALLABLE_QUALIFIERS>                                           \
  {                                                                                                              \
  public:                                                                                                        \
    virtual ~FunBase() noexcept                                   = default;                                     \
    virtual R operator()(Args... args) ETCPAL_CALLABLE_QUALIFIERS = 0;                                           \
  };                                                                                                             \
                                                                                                                 \
  template <typename R, typename... Args, typename F>                                                            \
  class etcpal::detail::Callable<R(Args...) ETCPAL_CALLABLE_QUALIFIERS, F>                                       \
      : public FunBase<R(Args...) ETCPAL_CALLABLE_QUALIFIERS>                                                    \
  {                                                                                                              \
  public:                                                                                                        \
    Callable(const F& fun) noexcept(noexcept(F{fun})) : fun_{fun}                                                \
    {                                                                                                            \
    }                                                                                                            \
    Callable(F&& fun) noexcept : fun_{std::move(fun)}                                                            \
    {                                                                                                            \
    }                                                                                                            \
                                                                                                                 \
    ~Callable() noexcept override = default;                                                                     \
                                                                                                                 \
    R operator()(Args... args) ETCPAL_CALLABLE_QUALIFIERS override                                               \
    {                                                                                                            \
      return fun_(std::forward<Args>(args)...);                                                                  \
    }                                                                                                            \
                                                                                                                 \
  private:                                                                                                       \
    F fun_;                                                                                                      \
  };                                                                                                             \
                                                                                                                 \
  template <typename R, typename... Args, typename Allocator>                                                    \
  class etcpal::move_only_function<R(Args...) ETCPAL_CALLABLE_QUALIFIERS, Allocator>                             \
  {                                                                                                              \
  private:                                                                                                       \
    template <typename F>                                                                                        \
    using Callable = detail::Callable<R(Args...), std::remove_cv_t<std::remove_reference_t<F>>>;                 \
                                                                                                                 \
  public:                                                                                                        \
    using result_type = R;                                                                                       \
                                                                                                                 \
    move_only_function() noexcept = default;                                                                     \
    move_only_function(std::nullptr_t tag) noexcept : move_only_function{}                                       \
    {                                                                                                            \
    }                                                                                                            \
    template <typename F>                                                                                        \
    move_only_function(F&& f, const Allocator& alloc = {})                                                       \
        : ptr_{typename std::allocator_traits<Allocator>::template rebind<Callable<F>>{alloc}.allocate(1),       \
               detail::DeleteUsingAlloc<Allocator>{alloc}}                                                       \
    {                                                                                                            \
      new (ptr_.get()) Callable{std::forward<F>(f)};                                                             \
    }                                                                                                            \
                                                                                                                 \
    R operator()(Args... args) ETCPAL_CALLABLE_QUALIFIERS                                                        \
    {                                                                                                            \
      return ptr_->operator()(std::forward<Args>(args)...);                                                      \
    }                                                                                                            \
                                                                                                                 \
  private:                                                                                                       \
    std::unique_ptr<detail::FunBase<R(Args...) ETCPAL_CALLABLE_QUALIFIERS>, detail::DeleteUsingAlloc<Allocator>> \
        ptr_ = {};                                                                                               \
  }

#define ETCPAL_CALLABLE_QUALIFIERS
ETCPAL_IMPLEMENT_VIRTUAL_CALLABLE;
#undef ETCPAL_CALLABLE_QUALIFIERS

#define ETCPAL_CALLABLE_QUALIFIERS &
ETCPAL_IMPLEMENT_VIRTUAL_CALLABLE;
#undef ETCPAL_CALLABLE_QUALIFIERS

#define ETCPAL_CALLABLE_QUALIFIERS &&
ETCPAL_IMPLEMENT_VIRTUAL_CALLABLE;
#undef ETCPAL_CALLABLE_QUALIFIERS

#define ETCPAL_CALLABLE_QUALIFIERS const
ETCPAL_IMPLEMENT_VIRTUAL_CALLABLE;
#undef ETCPAL_CALLABLE_QUALIFIERS

#define ETCPAL_CALLABLE_QUALIFIERS const&
ETCPAL_IMPLEMENT_VIRTUAL_CALLABLE;
#undef ETCPAL_CALLABLE_QUALIFIERS

#define ETCPAL_CALLABLE_QUALIFIERS const&&
ETCPAL_IMPLEMENT_VIRTUAL_CALLABLE;
#undef ETCPAL_CALLABLE_QUALIFIERS

#if (__cplusplus >= 201703L)

#define ETCPAL_CALLABLE_QUALIFIERS noexcept
ETCPAL_IMPLEMENT_VIRTUAL_CALLABLE;
#undef ETCPAL_CALLABLE_QUALIFIERS

#define ETCPAL_CALLABLE_QUALIFIERS &noexcept
ETCPAL_IMPLEMENT_VIRTUAL_CALLABLE;
#undef ETCPAL_CALLABLE_QUALIFIERS

#define ETCPAL_CALLABLE_QUALIFIERS &&noexcept
ETCPAL_IMPLEMENT_VIRTUAL_CALLABLE;
#undef ETCPAL_CALLABLE_QUALIFIERS

#define ETCPAL_CALLABLE_QUALIFIERS const noexcept
ETCPAL_IMPLEMENT_VIRTUAL_CALLABLE;
#undef ETCPAL_CALLABLE_QUALIFIERS

#define ETCPAL_CALLABLE_QUALIFIERS const& noexcept
ETCPAL_IMPLEMENT_VIRTUAL_CALLABLE;
#undef ETCPAL_CALLABLE_QUALIFIERS

#define ETCPAL_CALLABLE_QUALIFIERS const&& noexcept
ETCPAL_IMPLEMENT_VIRTUAL_CALLABLE;
#undef ETCPAL_CALLABLE_QUALIFIERS

#endif  // #if (__cplusplus >= 201703L)

#undef ETCPAL_IMPLEMENT_VIRTUAL_CALLABLE
