#pragma once

#include <utility>

#include <etcpal/cpp/common.h>

namespace etcpal
{

template <typename... T>
class Overload;

namespace detail
{

template <typename F, typename, typename... Args>
struct IsCallable : public std::false_type
{
};

template <typename F, typename... Args>
struct IsCallable<F, void_t<decltype(std::declval<F>()(std::declval<Args>()...))>, Args...> : public std::true_type
{
};

template <typename F, typename, typename... Args>
struct IsNothrowCallable : public std::false_type
{
};

template <typename F, typename... Args>
struct IsNothrowCallable<F, std::enable_if_t<noexcept(std::declval<F>()(std::declval<Args>()...))>, Args...>
    : public std::true_type
{
};

}  // namespace detail

template <typename T>
class Overload<T>
{
public:
  explicit constexpr Overload(const T& init) noexcept(noexcept(T{init})) : value_{init} {}
  explicit constexpr Overload(T&& init) noexcept : value_{std::move(init)} {}

  template <typename... Args, typename = std::enable_if_t<detail::IsCallable<const T, void, Args...>::value>>
  constexpr decltype(auto) operator()(Args&&... args) const&& noexcept(
      detail::IsNothrowCallable<const T, void, Args...>::value)
  {
    return std::move(value_)(std::forward<Args>(args)...);
  }
  template <typename... Args, typename = std::enable_if_t<detail::IsCallable<const T&, void, Args...>::value>>
  constexpr decltype(auto) operator()(Args&&... args) const& noexcept(
      detail::IsNothrowCallable<const T&, void, Args...>::value)
  {
    return value_(std::forward<Args>(args)...);
  }
  template <typename... Args, typename = std::enable_if_t<detail::IsCallable<T, void, Args...>::value>>
  constexpr decltype(auto) operator()(Args&&... args) && noexcept(detail::IsNothrowCallable<T, void, Args...>::value)
  {
    return std::move(value_)(std::forward<Args>(args)...);
  }
  template <typename... Args, typename = std::enable_if_t<detail::IsCallable<T&, void, Args...>::value>>
  constexpr decltype(auto) operator()(Args&&... args) & noexcept(detail::IsNothrowCallable<T&, void, Args...>::value)
  {
    return value_(std::forward<Args>(args)...);
  }

private:
  T value_;
};

template <typename First, typename Second, typename... Rest>
class Overload<First, Second, Rest...> : public Overload<Second, Rest...>
{
private:
  using Parent = Overload<Second, Rest...>;

public:
  template <typename... Args>
  explicit constexpr Overload(const First& first, Args&&... args) noexcept(
      std::is_nothrow_copy_constructible<First>::value &&
      std::is_nothrow_constructible<Overload<Second, Rest...>, Args...>::value)
      : Overload<Second, Rest...>{std::forward<Args>(args)...}, value_{first}
  {
  }
  template <typename... Args>
  explicit constexpr Overload(First&& first, Args&&... args) noexcept(
      std::is_nothrow_constructible<Overload<Second, Rest...>, Args...>::value)
      : Overload<Second, Rest...>{std::forward<Args>(args)...}, value_{std::move(first)}
  {
  }

  template <typename... Args, std::enable_if_t<detail::IsCallable<const First, void, Args...>::value>* = nullptr>
  constexpr decltype(auto) operator()(Args&&... args) const&& noexcept(
      detail::IsNothrowCallable<const First, void, Args...>::value)
  {
    return std::move(value_)(std::forward<Args>(args)...);
  }
  template <typename... Args, std::enable_if_t<detail::IsCallable<const Parent, void, Args...>::value>* = nullptr>
  constexpr decltype(auto) operator()(Args&&... args) const&& noexcept(
      detail::IsNothrowCallable<const Parent, void, Args...>::value)
  {
    return static_cast<const Parent&&>(*this)(std::forward<Args>(args)...);
  }
  template <typename... Args, std::enable_if_t<detail::IsCallable<const First&, void, Args...>::value>* = nullptr>
  constexpr decltype(auto) operator()(Args&&... args) const& noexcept(
      detail::IsNothrowCallable<const First&, void, Args...>::value)
  {
    return value_(std::forward<Args>(args)...);
  }
  template <typename... Args, std::enable_if_t<detail::IsCallable<const Parent&, void, Args...>::value>* = nullptr>
  constexpr decltype(auto) operator()(Args&&... args) const& noexcept(
      detail::IsNothrowCallable<const Parent&, void, Args...>::value)
  {
    return static_cast<const Parent&>(*this)(std::forward<Args>(args)...);
  }
  template <typename... Args, std::enable_if_t<detail::IsCallable<First, void, Args...>::value>* = nullptr>
  constexpr decltype(auto) operator()(Args&&... args) && noexcept(
      detail::IsNothrowCallable<First, void, Args...>::value)
  {
    return std::move(value_)(std::forward<Args>(args)...);
  }
  template <typename... Args, std::enable_if_t<detail::IsCallable<Parent, void, Args...>::value>* = nullptr>
  constexpr decltype(auto) operator()(Args&&... args) && noexcept(
      detail::IsNothrowCallable<Parent, void, Args...>::value)
  {
    return static_cast<Parent&&>(*this)(std::forward<Args>(args)...);
  }
  template <typename... Args, std::enable_if_t<detail::IsCallable<First&, void, Args...>::value>* = nullptr>
  constexpr decltype(auto) operator()(Args&&... args) & noexcept(
      detail::IsNothrowCallable<First&, void, Args...>::value)
  {
    return value_(std::forward<Args>(args)...);
  }
  template <typename... Args, std::enable_if_t<detail::IsCallable<Parent&, void, Args...>::value>* = nullptr>
  constexpr decltype(auto) operator()(Args&&... args) & noexcept(
      detail::IsNothrowCallable<Parent&, void, Args...>::value)
  {
    return static_cast<Parent&>(*this)(std::forward<Args>(args)...);
  }

private:
  First value_;
};

#if (__cplusplus >= 201703L)
template <typename... T>
Overload(T&&...) -> Overload<std::remove_cv_t<std::remove_reference_t<T>>...>;
#endif  // #if (__cplusplus >= 201703L)

template <typename... T>
[[nodiscard]] constexpr auto make_overload(T&&... args) noexcept(
    std::is_nothrow_constructible<Overload<std::remove_cv_t<std::remove_reference_t<T>>...>, T...>::value)
{
  return Overload<std::remove_cv_t<std::remove_reference_t<T>>...>{std::forward<T>(args)...};
}

template <typename... T>
class Select;

template <typename T>
class Select<T>
{
public:
  explicit constexpr Select(const T& init) noexcept(noexcept(T{init})) : value_{init} {}
  explicit constexpr Select(T&& init) noexcept : value_{std::move(init)} {}

  template <typename... Args, typename = std::enable_if_t<detail::IsCallable<const T, void, Args...>::value>>
  constexpr decltype(auto) operator()(Args&&... args) const&& noexcept(
      detail::IsNothrowCallable<const T, void, Args...>::value)
  {
    return std::move(value_)(std::forward<Args>(args)...);
  }
  template <typename... Args, typename = std::enable_if_t<detail::IsCallable<const T&, void, Args...>::value>>
  constexpr decltype(auto) operator()(Args&&... args) const& noexcept(
      detail::IsNothrowCallable<const T&, void, Args...>::value)
  {
    return value_(std::forward<Args>(args)...);
  }
  template <typename... Args, typename = std::enable_if_t<detail::IsCallable<T, void, Args...>::value>>
  constexpr decltype(auto) operator()(Args&&... args) && noexcept(detail::IsNothrowCallable<T, void, Args...>::value)
  {
    return std::move(value_)(std::forward<Args>(args)...);
  }
  template <typename... Args, typename = std::enable_if_t<detail::IsCallable<T&, void, Args...>::value>>
  constexpr decltype(auto) operator()(Args&&... args) & noexcept(detail::IsNothrowCallable<T&, void, Args...>::value)
  {
    return value_(std::forward<Args>(args)...);
  }

private:
  T value_;
};

template <typename First, typename Second, typename... Rest>
class Select<First, Second, Rest...> : public Select<Second, Rest...>
{
private:
  using Parent = Select<Second, Rest...>;

public:
  template <typename... Args>
  explicit constexpr Select(const First& first, Args&&... args) noexcept(
      std::is_nothrow_copy_constructible<First>::value &&
      std::is_nothrow_constructible<Select<Second, Rest...>, Args...>::value)
      : Select<Second, Rest...>{std::forward<Args>(args)...}, value_{first}
  {
  }
  template <typename... Args>
  explicit constexpr Select(First&& first, Args&&... args) noexcept(
      std::is_nothrow_constructible<Select<Second, Rest...>, Args...>::value)
      : Select<Second, Rest...>{std::forward<Args>(args)...}, value_{std::move(first)}
  {
  }

  template <typename... Args, std::enable_if_t<detail::IsCallable<const First, void, Args...>::value>* = nullptr>
  constexpr decltype(auto) operator()(Args&&... args) const&& noexcept(
      detail::IsNothrowCallable<const First, void, Args...>::value)
  {
    return std::move(value_)(std::forward<Args>(args)...);
  }
  template <typename... Args,
            std::enable_if_t<!detail::IsCallable<const First, void, Args...>::value &&
                             detail::IsCallable<const Parent, void, Args...>::value>* = nullptr>
  constexpr decltype(auto) operator()(Args&&... args) const&& noexcept(
      detail::IsNothrowCallable<const Parent, void, Args...>::value)
  {
    return static_cast<const Parent&&>(*this)(std::forward<Args>(args)...);
  }
  template <typename... Args, std::enable_if_t<detail::IsCallable<const First&, void, Args...>::value>* = nullptr>
  constexpr decltype(auto) operator()(Args&&... args) const& noexcept(
      detail::IsNothrowCallable<const First&, void, Args...>::value)
  {
    return value_(std::forward<Args>(args)...);
  }
  template <typename... Args,
            std::enable_if_t<!detail::IsCallable<const First&, void, Args...>::value &&
                             detail::IsCallable<const Parent&, void, Args...>::value>* = nullptr>
  constexpr decltype(auto) operator()(Args&&... args) const& noexcept(
      detail::IsNothrowCallable<const Parent&, void, Args...>::value)
  {
    return static_cast<const Parent&>(*this)(std::forward<Args>(args)...);
  }
  template <typename... Args, std::enable_if_t<detail::IsCallable<First, void, Args...>::value>* = nullptr>
  constexpr decltype(auto) operator()(Args&&... args) && noexcept(
      detail::IsNothrowCallable<First, void, Args...>::value)
  {
    return std::move(value_)(std::forward<Args>(args)...);
  }
  template <typename... Args,
            std::enable_if_t<!detail::IsCallable<First, void, Args...>::value &&
                             detail::IsCallable<Parent, void, Args...>::value>* = nullptr>
  constexpr decltype(auto) operator()(Args&&... args) && noexcept(
      detail::IsNothrowCallable<Parent, void, Args...>::value)
  {
    return static_cast<Parent&&>(*this)(std::forward<Args>(args)...);
  }
  template <typename... Args, std::enable_if_t<detail::IsCallable<First&, void, Args...>::value>* = nullptr>
  constexpr decltype(auto) operator()(Args&&... args) & noexcept(
      detail::IsNothrowCallable<First&, void, Args...>::value)
  {
    return value_(std::forward<Args>(args)...);
  }
  template <typename... Args,
            std::enable_if_t<!detail::IsCallable<First&, void, Args...>::value &&
                             detail::IsCallable<Parent&, void, Args...>::value>* = nullptr>
  constexpr decltype(auto) operator()(Args&&... args) & noexcept(
      detail::IsNothrowCallable<Parent&, void, Args...>::value)
  {
    return static_cast<Parent&>(*this)(std::forward<Args>(args)...);
  }

private:
  First value_;
};

#if (__cplusplus >= 201703L)
template <typename... T>
Select(T&&...) -> Select<std::remove_cv_t<std::remove_reference_t<T>>...>;
#endif  // #if (__cplusplus >= 201703L)

template <typename... T>
[[nodiscard]] constexpr auto make_selection(T&&... args) noexcept(
    std::is_nothrow_constructible<Select<std::remove_cv_t<std::remove_reference_t<T>>...>, T...>::value)
{
  return Select<std::remove_cv_t<std::remove_reference_t<T>>...>{std::forward<T>(args)...};
}
}  // namespace etcpal
