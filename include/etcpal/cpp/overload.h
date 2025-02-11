#pragma once

#include <utility>

#include <etcpal/cpp/common.h>

namespace etcpal
{

/// @brief An overload set combining all of the given objects.
///
/// An `Overload<T...>{v...}` combines the call operators of all of the objects in `v...` into a single invocable
/// object. For example, and `Overload<void(*)(), std::function<int(int)>>` may be invoked using no arguments to return
/// nothing, or using an argument of type `int` to return an `int`. `Overload<T...>` forms a true overload set amongst
/// the types `T...`; therefore, any set of arguments that match multiple types in `T...`, including if `T...` contains
/// repeats, constitutes an overload resolution ambiguity error.
///
/// @tparam T The types of objects in this overload set.
template <typename... T>
class Overload;

template <typename T>
class Overload<T>
{
public:
  explicit constexpr Overload(const T& init) noexcept(noexcept(T{init})) : value_{init} {}
  explicit constexpr Overload(T&& init) noexcept : value_{std::move(init)} {}

  template <typename... Args, typename = std::enable_if_t<is_invocable<const T, Args...>::value>>
  constexpr decltype(auto) operator()(Args&&... args) const&& noexcept(is_nothrow_invocable<const T, Args...>::value)
  {
    return invoke(std::move(value_), std::forward<Args>(args)...);
  }
  template <typename... Args, typename = std::enable_if_t<is_invocable<const T&, Args...>::value>>
  constexpr decltype(auto) operator()(Args&&... args) const& noexcept(is_nothrow_invocable<const T&, Args...>::value)
  {
    return invoke(value_, std::forward<Args>(args)...);
  }
  template <typename... Args, typename = std::enable_if_t<is_invocable<T, Args...>::value>>
  constexpr decltype(auto) operator()(Args&&... args) && noexcept(is_nothrow_invocable<T, Args...>::value)
  {
    return invoke(std::move(value_), std::forward<Args>(args)...);
  }
  template <typename... Args, typename = std::enable_if_t<is_invocable<T&, Args...>::value>>
  constexpr decltype(auto) operator()(Args&&... args) & noexcept(is_nothrow_invocable<T&, Args...>::value)
  {
    return invoke(value_, std::forward<Args>(args)...);
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

  template <typename... Args, std::enable_if_t<is_invocable<const First, Args...>::value>* = nullptr>
  constexpr decltype(auto) operator()(Args&&... args) const&& noexcept(
      is_nothrow_invocable<const First, Args...>::value)
  {
    return invoke(std::move(value_), std::forward<Args>(args)...);
  }
  template <typename... Args, std::enable_if_t<is_invocable<const Parent, Args...>::value>* = nullptr>
  constexpr decltype(auto) operator()(Args&&... args) const&& noexcept(
      is_nothrow_invocable<const Parent, Args...>::value)
  {
    return invoke(static_cast<const Parent&&>(*this), std::forward<Args>(args)...);
  }
  template <typename... Args, std::enable_if_t<is_invocable<const First&, Args...>::value>* = nullptr>
  constexpr decltype(auto) operator()(Args&&... args) const& noexcept(
      is_nothrow_invocable<const First&, Args...>::value)
  {
    return invoke(value_, std::forward<Args>(args)...);
  }
  template <typename... Args, std::enable_if_t<is_invocable<const Parent&, Args...>::value>* = nullptr>
  constexpr decltype(auto) operator()(Args&&... args) const& noexcept(
      is_nothrow_invocable<const Parent&, Args...>::value)
  {
    return invoke(static_cast<const Parent&>(*this), std::forward<Args>(args)...);
  }
  template <typename... Args, std::enable_if_t<is_invocable<First, Args...>::value>* = nullptr>
  constexpr decltype(auto) operator()(Args&&... args) && noexcept(is_nothrow_invocable<First, Args...>::value)
  {
    return invoke(std::move(value_), std::forward<Args>(args)...);
  }
  template <typename... Args, std::enable_if_t<is_invocable<Parent, Args...>::value>* = nullptr>
  constexpr decltype(auto) operator()(Args&&... args) && noexcept(is_nothrow_invocable<Parent, Args...>::value)
  {
    return invoke(static_cast<Parent&&>(*this), std::forward<Args>(args)...);
  }
  template <typename... Args, std::enable_if_t<is_invocable<First&, Args...>::value>* = nullptr>
  constexpr decltype(auto) operator()(Args&&... args) & noexcept(is_nothrow_invocable<First&, Args...>::value)
  {
    return invoke(value_, std::forward<Args>(args)...);
  }
  template <typename... Args, std::enable_if_t<is_invocable<Parent&, Args...>::value>* = nullptr>
  constexpr decltype(auto) operator()(Args&&... args) & noexcept(is_nothrow_invocable<Parent&, Args...>::value)
  {
    return invoke(static_cast<Parent&>(*this), std::forward<Args>(args)...);
  }

private:
  First value_;
};

#if (__cplusplus >= 201703L)
template <typename... T>
Overload(T&&...) -> Overload<RemoveCVRef_t<T>...>;
#endif  // #if (__cplusplus >= 201703L)

/// @brief Construct an overload set from the given callable objects.
/// @tparam T The types of objects to form an overload set with.
/// @param args The objects to form an overload set with.
/// @return The new overload set object.
template <typename... T>
ETCPAL_NODISCARD constexpr auto make_overload(T&&... args) noexcept(
    std::is_nothrow_constructible<Overload<RemoveCVRef_t<T>...>, T...>::value)
{
  return Overload<RemoveCVRef_t<T>...>{std::forward<T>(args)...};
}

/// @brief A prioritized overload set combining all of the given objects.
///
/// `Select<T...>{v...}` forms an overload set like `Overload<T...>{v...}`, but resolves ambiguities between the types
/// `T...` in favor of the type that came earlier in the list.
///
/// @tparam T The types of objects in this prioritized overload set.
template <typename... T>
class Select;

template <typename T>
class Select<T>
{
public:
  explicit constexpr Select(const T& init) noexcept(noexcept(T{init})) : value_{init} {}
  explicit constexpr Select(T&& init) noexcept : value_{std::move(init)} {}

  template <typename... Args, typename = std::enable_if_t<is_invocable<const T, Args...>::value>>
  constexpr decltype(auto) operator()(Args&&... args) const&& noexcept(is_nothrow_invocable<const T, Args...>::value)
  {
    return invoke(std::move(value_), std::forward<Args>(args)...);
  }
  template <typename... Args, typename = std::enable_if_t<is_invocable<const T&, Args...>::value>>
  constexpr decltype(auto) operator()(Args&&... args) const& noexcept(is_nothrow_invocable<const T&, Args...>::value)
  {
    return invoke(value_, std::forward<Args>(args)...);
  }
  template <typename... Args, typename = std::enable_if_t<is_invocable<T, Args...>::value>>
  constexpr decltype(auto) operator()(Args&&... args) && noexcept(is_nothrow_invocable<T, Args...>::value)
  {
    return invoke(std::move(value_), std::forward<Args>(args)...);
  }
  template <typename... Args, typename = std::enable_if_t<is_invocable<T&, Args...>::value>>
  constexpr decltype(auto) operator()(Args&&... args) & noexcept(is_nothrow_invocable<T&, Args...>::value)
  {
    return invoke(value_, std::forward<Args>(args)...);
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

  template <typename... Args, std::enable_if_t<is_invocable<const First, Args...>::value>* = nullptr>
  constexpr decltype(auto) operator()(Args&&... args) const&& noexcept(
      is_nothrow_invocable<const First, Args...>::value)
  {
    return invoke(std::move(value_), std::forward<Args>(args)...);
  }
  template <typename... Args,
            std::enable_if_t<!is_invocable<const First, Args...>::value &&
                             is_invocable<const Parent, Args...>::value>* = nullptr>
  constexpr decltype(auto) operator()(Args&&... args) const&& noexcept(
      is_nothrow_invocable<const Parent, Args...>::value)
  {
    return invoke(static_cast<const Parent&&>(*this), std::forward<Args>(args)...);
  }
  template <typename... Args, std::enable_if_t<is_invocable<const First&, Args...>::value>* = nullptr>
  constexpr decltype(auto) operator()(Args&&... args) const& noexcept(
      is_nothrow_invocable<const First&, Args...>::value)
  {
    return invoke(value_, std::forward<Args>(args)...);
  }
  template <typename... Args,
            std::enable_if_t<!is_invocable<const First&, Args...>::value &&
                             is_invocable<const Parent&, Args...>::value>* = nullptr>
  constexpr decltype(auto) operator()(Args&&... args) const& noexcept(
      is_nothrow_invocable<const Parent&, Args...>::value)
  {
    return invoke(static_cast<const Parent&>(*this), std::forward<Args>(args)...);
  }
  template <typename... Args, std::enable_if_t<is_invocable<First, Args...>::value>* = nullptr>
  constexpr decltype(auto) operator()(Args&&... args) && noexcept(is_nothrow_invocable<First, Args...>::value)
  {
    return invoke(std::move(value_), std::forward<Args>(args)...);
  }
  template <typename... Args,
            std::enable_if_t<!is_invocable<First, Args...>::value && is_invocable<Parent, Args...>::value>* = nullptr>
  constexpr decltype(auto) operator()(Args&&... args) && noexcept(is_nothrow_invocable<Parent, Args...>::value)
  {
    return invoke(static_cast<Parent&&>(*this), std::forward<Args>(args)...);
  }
  template <typename... Args, std::enable_if_t<is_invocable<First&, Args...>::value>* = nullptr>
  constexpr decltype(auto) operator()(Args&&... args) & noexcept(is_nothrow_invocable<First&, Args...>::value)
  {
    return invoke(value_, std::forward<Args>(args)...);
  }
  template <typename... Args,
            std::enable_if_t<!is_invocable<First&, Args...>::value && is_invocable<Parent&, Args...>::value>* = nullptr>
  constexpr decltype(auto) operator()(Args&&... args) & noexcept(is_nothrow_invocable<Parent&, Args...>::value)
  {
    return invoke(static_cast<Parent&>(*this), std::forward<Args>(args)...);
  }

private:
  First value_;
};

#if (__cplusplus >= 201703L)
template <typename... T>
Select(T&&...) -> Select<RemoveCVRef_t<T>...>;
#endif  // #if (__cplusplus >= 201703L)

namespace detail
{

template <typename... T>
using Selection = Select<RemoveCVRef_t<T>...>;

}

/// @brief Construct a prioritized overload set from the given objects.
/// @tparam T The types of objects to create a prioritized overload set from.
/// @param args The objects to create a prioritized overload set from.
/// @return The new prioritized overload set.
template <typename... T>
ETCPAL_NODISCARD constexpr auto make_selection(T&&... args) noexcept(
    std::is_nothrow_constructible<detail::Selection<T...>, T...>::value)
{
  return detail::Selection<T...>{std::forward<T>(args)...};
}

}  // namespace etcpal
