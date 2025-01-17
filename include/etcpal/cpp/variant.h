#pragma once

#include <algorithm>
#include <array>
#include <functional>
#include <stdexcept>
#include <type_traits>

#include <etcpal/cpp/common.h>
#include <etcpal/cpp/overload.h>

namespace etcpal
{

template <typename T>
struct InPlaceType_t
{
};

template <std::size_t I>
struct InPlaceIndex_t
{
};

ETCPAL_INLINE_VARIABLE constexpr auto variant_npos = static_cast<std::size_t>(-1);

namespace detail
{

template <typename T>
struct ConstructFrom
{
  template <typename... Args, typename = std::enable_if_t<std::is_constructible<T, Args...>::value>>
  [[nodiscard]] constexpr auto operator()(Args&&... args) const
      noexcept(std::is_nothrow_constructible<T, Args...>::value) -> T;
};

template <typename... T>
struct MaxSizeOf;
template <typename T>
struct MaxSizeOf<T> : public std::integral_constant<std::size_t, sizeof(T)>
{
};
template <typename First, typename Second, typename... Rest>
struct MaxSizeOf<First, Second, Rest...>
    : public std::integral_constant<std::size_t, std::max(sizeof(First), MaxSizeOf<Second, Rest...>::value)>
{
};

template <std::size_t I, typename = void, typename... T>
struct TypeAtIndex;

template <typename T, typename... U>
struct TypeAtIndex<0, void, T, U...>
{
  using type = T;
};

template <std::size_t I, typename First, typename Second, typename... Rest>
struct TypeAtIndex<I, std::enable_if_t<I != 0>, First, Second, Rest...>
    : public TypeAtIndex<I - 1, void, Second, Rest...>
{
};

template <typename... T>
struct AllNothrowCopyConstructible : public std::false_type
{
};

template <typename T>
struct AllNothrowCopyConstructible<T> : public std::is_nothrow_copy_constructible<T>
{
};

template <typename First, typename Second, typename... Rest>
struct AllNothrowCopyConstructible<First, Second, Rest...>
    : public std::integral_constant<bool,
                                    AllNothrowCopyConstructible<First>::value &&
                                        AllNothrowCopyConstructible<Second, Rest...>::value>
{
};

template <typename... T>
struct AllNothrowCopyAssignable : public std::false_type
{
};

template <typename T>
struct AllNothrowCopyAssignable<T> : public std::is_nothrow_copy_assignable<T>
{
};

template <typename First, typename Second, typename... Rest>
struct AllNothrowCopyAssignable<First, Second, Rest...>
    : public std::integral_constant<bool,
                                    AllNothrowCopyAssignable<First>::value &&
                                        AllNothrowCopyAssignable<Second, Rest...>::value>
{
};

template <std::size_t I, typename... T>
using TypeAtIndex_t = typename TypeAtIndex<I, void, T...>::type;

template <typename T, typename U>
[[nodiscard]] constexpr auto index_of(std::size_t curr = 0) noexcept
{
  return std::is_same<T, U>::value ? curr : variant_npos;
}

template <typename T, typename First, typename Second, typename... Rest>
[[nodiscard]] constexpr auto index_of(std::size_t curr = 0) noexcept
{
  return std::is_same<T, First>::value ? curr : index_of<T, Second, Rest...>(curr + 1);
}

template <typename T, typename U>
[[nodiscard]] constexpr auto count_occurances_of(InPlaceType_t<U> tag) noexcept
{
  return std::is_same<T, U>::value ? 1 : 0;
}

template <typename T, typename First, typename Second, typename... Rest>
[[nodiscard]] constexpr auto count_occurances_of(InPlaceType_t<First>  first,
                                                 InPlaceType_t<Second> second,
                                                 InPlaceType_t<Rest>... rest) noexcept
{
  return count_occurances_of<T>(first) + count_occurances_of<T>(second, rest...);
}

template <typename T, typename... U>
[[nodiscard]] constexpr auto is_unique_in() noexcept
{
  return count_occurances_of<T>(InPlaceType_t<U>{}...);
}

struct EmptyType
{
};

template <typename... T>
union VariadicUnion;

}  // namespace detail

template <typename... T>
class Variant
{
public:
  template <typename U = detail::TypeAtIndex_t<0, T...>,
            typename   = std::enable_if_t<std::is_default_constructible<U>::value>>
  constexpr Variant() noexcept(std::is_nothrow_default_constructible<U>::value);
  template <typename U, typename V = decltype(make_overload(detail::ConstructFrom<T>{}...)(std::declval<U>()))>
  constexpr Variant(U&& init) noexcept(noexcept(make_overload(detail::ConstructFrom<T>{}...)(std::forward<U>(init))));
  template <typename U, typename... Args, typename = std::enable_if_t<detail::is_unique_in<U, T...>()>>
  explicit constexpr Variant(InPlaceType_t<U> tag,
                             Args&&... args) noexcept(std::is_nothrow_constructible<U, Args...>::value);
  template <typename U, typename V, typename... Args, typename = std::enable_if_t<detail::is_unique_in<U, T...>()>>
  explicit constexpr Variant(InPlaceType_t<U> tag, std::initializer_list<V> list, Args&&... args) noexcept(
      std::is_nothrow_constructible<U, std::initializer_list<V>, Args...>::value);
  template <std::size_t I,
            typename... Args,
            typename = std::enable_if_t<std::less<>{}(I, sizeof...(T)) &&
                                        std::is_constructible<detail::TypeAtIndex_t<I, T...>, Args...>::value>>
  explicit constexpr Variant(InPlaceIndex_t<I> tag, Args&&... args) noexcept(
      std::is_nothrow_constructible<detail::TypeAtIndex_t<I, T...>, Args...>::value);
  template <std::size_t I,
            typename V,
            typename... Args,
            typename = std::enable_if_t<
                std::less<>{}(I, sizeof...(T)) &&
                std::is_constructible<detail::TypeAtIndex_t<I, T...>, std::initializer_list<V>, Args...>::value>>
  explicit constexpr Variant(InPlaceIndex_t<I> tag, std::initializer_list<V> list, Args&&... args) noexcept(
      std::is_nothrow_constructible<detail::TypeAtIndex_t<I, T...>, std::initializer_list<V>, Args...>::value);

  constexpr Variant(const Variant& rhs) noexcept(detail::AllNothrowCopyConstructible<T...>::value);
  constexpr Variant(Variant&& rhs) noexcept;

  ~Variant() noexcept;

  constexpr auto operator=(const Variant& rhs) noexcept(detail::AllNothrowCopyAssignable<T...>::value) -> Variant&;
  constexpr auto operator=(Variant&& rhs) noexcept -> Variant&;

  [[nodiscard]] constexpr auto index() const noexcept { return type_; }
  [[nodiscard]] constexpr bool valueless_by_exception() const noexcept { return index() == variant_npos; }

  template <typename Visitor>
  constexpr decltype(auto) visit(Visitor&& visitor) const&&;
  template <typename Visitor>
  constexpr decltype(auto) visit(Visitor&& visitor) const&;
  template <typename Visitor>
  constexpr decltype(auto) visit(Visitor&& visitor) &&;
  template <typename Visitor>
  constexpr decltype(auto) visit(Visitor&& visitor) &;
  template <typename R, typename Visitor>
  constexpr auto visit(Visitor&& visitor) const&& -> R;
  template <typename R, typename Visitor>
  constexpr auto visit(Visitor&& visitor) const& -> R;
  template <typename R, typename Visitor>
  constexpr auto visit(Visitor&& visitor) && -> R;
  template <typename R, typename Visitor>
  constexpr auto visit(Visitor&& visitor) & -> R;
  template <typename U, typename = std::enable_if_t<detail::is_unique_in<U, T...>()>>
  [[nodiscard]] constexpr auto get() const&& noexcept -> const U&&;
  template <typename U, typename = std::enable_if_t<detail::is_unique_in<U, T...>()>>
  [[nodiscard]] constexpr auto get() const& noexcept -> const U&;
  template <typename U, typename = std::enable_if_t<detail::is_unique_in<U, T...>()>>
  [[nodiscard]] constexpr auto get() && noexcept -> U&&;
  template <typename U, typename = std::enable_if_t<detail::is_unique_in<U, T...>()>>
  [[nodiscard]] constexpr auto get() & noexcept -> U&;
  template <std::size_t I, typename = std::enable_if_t<std::less<>{}(I, sizeof...(T))>>
  [[nodiscard]] constexpr auto get() const&& noexcept -> const detail::TypeAtIndex_t<I, T...>&&;
  template <std::size_t I, typename = std::enable_if_t<std::less<>{}(I, sizeof...(T))>>
  [[nodiscard]] constexpr auto get() const& noexcept -> const detail::TypeAtIndex_t<I, T...>&;
  template <std::size_t I, typename = std::enable_if_t<std::less<>{}(I, sizeof...(T))>>
  [[nodiscard]] constexpr auto get() && noexcept -> detail::TypeAtIndex_t<I, T...>&&;
  template <std::size_t I, typename = std::enable_if_t<std::less<>{}(I, sizeof...(T))>>
  [[nodiscard]] constexpr auto get() & noexcept -> detail::TypeAtIndex_t<I, T...>&;

  [[nodiscard]] friend constexpr bool operator==(const Variant& lhs, const Variant& rhs) noexcept
  {
    if ((lhs.index() != rhs.index()) || (lhs.valueless_by_exception() && rhs.valueless_by_exception()))
    {
      return false;
    }

    return lhs.visit([&](const auto& l) {
      return rhs.visit(make_selection([&](const decltype(l)& r) { return l == r; },
                                      [](const auto& r) -> bool {
                                        throw std::logic_error{"variant relational operator comparing different types"};
                                      }));
    });
  }
  [[nodiscard]] friend constexpr bool operator!=(const Variant& lhs, const Variant& rhs) noexcept
  {
    if (lhs.index() != rhs.index())
    {
      return true;
    }
    if (lhs.valueless_by_exception())
    {
      return false;
    }

    return lhs.visit([&](const auto& l) {
      return rhs.visit(make_selection([&](const decltype(l)& r) { return l != r; },
                                      [](const auto& r) -> bool {
                                        throw std::logic_error{"variant relational operator comparing different types"};
                                      }));
    });
  }
  [[nodiscard]] friend constexpr bool operator<(const Variant& lhs, const Variant& rhs) noexcept
  {
    if ((lhs.index() < rhs.index()) || (lhs.valueless_by_exception() && !rhs.valueless_by_exception()))
    {
      return true;
    }
    if (lhs.index() > rhs.index())
    {
      return false;
    }

    return lhs.visit([&](const auto& l) {
      return rhs.visit(make_selection([&](const decltype(l)& r) { return l < r; },
                                      [](const auto& r) -> bool {
                                        throw std::logic_error{"variant relational operator comparing different types"};
                                      }));
    });
  }
  [[nodiscard]] friend constexpr bool operator>(const Variant& lhs, const Variant& rhs) noexcept
  {
    if ((lhs.index() > rhs.index()) || (!lhs.valueless_by_exception() && rhs.valueless_by_exception()))
    {
      return true;
    }
    if (lhs.index() < rhs.index())
    {
      return false;
    }

    return lhs.visit([&](const auto& l) {
      return rhs.visit(make_selection([&](const decltype(l)& r) { return l > r; },
                                      [](const auto& r) -> bool {
                                        throw std::logic_error{"variant relational operator comparing different types"};
                                      }));
    });
  }
  [[nodiscard]] friend constexpr bool operator<=(const Variant& lhs, const Variant& rhs) noexcept
  {
    if ((lhs.index() < rhs.index()) || lhs.valueless_by_exception())
    {
      return true;
    }
    if (lhs.index() > rhs.index())
    {
      return false;
    }

    return lhs.visit([&](const auto& l) {
      return rhs.visit(make_selection([&](const decltype(l)& r) { return l <= r; },
                                      [](const auto& r) -> bool {
                                        throw std::logic_error{"variant relational operator comparing different types"};
                                      }));
    });
  }
  [[nodiscard]] friend constexpr bool operator>=(const Variant& lhs, const Variant& rhs) noexcept
  {
    if ((lhs.index() > rhs.index()) || rhs.valueless_by_exception())
    {
      return true;
    }
    if (lhs.index() < rhs.index())
    {
      return false;
    }

    return lhs.visit([&](const auto& l) {
      return rhs.visit(make_selection([&](const decltype(l)& r) { return l >= r; },
                                      [](const auto& r) -> bool {
                                        throw std::logic_error{"variant relational operator comparing different types"};
                                      }));
    });
  }

private:
  std::size_t                 type_;
  detail::VariadicUnion<T...> storage_;
};

}  // namespace etcpal

namespace etcpal
{

namespace detail
{

template <typename T, typename... Args>
constexpr void construct_if_possible(std::true_type tag,
                                     T*             ptr,
                                     Args&&... args) noexcept(std::is_nothrow_constructible<T, Args...>::value)
{
  new (ptr) T(std::forward<Args>(args)...);
}

template <typename T, typename... Args>
constexpr void construct_if_possible(std::false_type tag, T* ptr, Args&&... args) noexcept
{
}

}  // namespace detail

}  // namespace etcpal

template <typename T>
union etcpal::detail::VariadicUnion<T>
{
  EmptyType empty;
  T         value;

  constexpr VariadicUnion(InPlaceIndex_t<1> index) noexcept : empty{} {}
  template <typename... Args>
  constexpr VariadicUnion(InPlaceIndex_t<0> index,
                          Args&&... args) noexcept(std::is_nothrow_constructible<T, Args...>::value)
      : value{std::forward<Args>(args)...}
  {
  }

  ~VariadicUnion() noexcept {}

  template <typename... Args>
  constexpr auto& construct(InPlaceIndex_t<0> type_to_construct,
                            Args&&... args) noexcept(std::is_nothrow_constructible<T, Args...>::value)
  {
    new (std::addressof(value)) T(std::forward<Args>(args)...);
    return value;
  }

  template <typename... Args>
  constexpr auto& construct(InPlaceIndex_t<1> type_to_construct, Args&&... args) noexcept
  {
    new (std::addressof(empty)) EmptyType;
    return empty;
  }

  constexpr void destruct(std::size_t index) noexcept
  {
    switch (index)
    {
      case 0:
        value.~T();
        break;
      case 1:
        empty.~EmptyType();
        break;
      default:
        break;
    }
  }

  template <typename... Args>
  constexpr void construct(std::size_t type_to_construct,
                           Args&&... args) noexcept(std::is_nothrow_copy_constructible<T>::value)
  {
    if (type_to_construct == 0)
    {
      construct_if_possible<T>(std::is_constructible<T, Args...>{}, std::addressof(value), std::forward<Args>(args)...);
    }
  }

  template <typename V>
  [[nodiscard]] constexpr decltype(auto) visit(std::size_t index, V&& visitor) const&&
  {
    if (index != 0)
    {
      throw std::logic_error{"invalid variant index"};
    }

    return std::forward<V>(visitor)(std::move(value));
  }

  template <typename V>
  [[nodiscard]] constexpr decltype(auto) visit(std::size_t index, V&& visitor) const&
  {
    if (index != 0)
    {
      throw std::logic_error{"invalid variant index"};
    }

    return std::forward<V>(visitor)(value);
  }

  template <typename V>
  [[nodiscard]] constexpr decltype(auto) visit(std::size_t index, V&& visitor) &&
  {
    if (index != 0)
    {
      throw std::logic_error{"invalid variant index"};
    }

    return std::forward<V>(visitor)(std::move(value));
  }

  template <typename V>
  [[nodiscard]] constexpr decltype(auto) visit(std::size_t index, V&& visitor) &
  {
    if (index != 0)
    {
      throw std::logic_error{"invalid variant index"};
    }

    return std::forward<V>(visitor)(value);
  }
};

template <typename T, typename U, typename... Rest>
union etcpal::detail::VariadicUnion<T, U, Rest...>
{
  VariadicUnion<U, Rest...> rest;
  T                         value;

  template <std::size_t I, typename... Args, typename = std::enable_if_t<I != 0>>
  constexpr VariadicUnion(InPlaceIndex_t<I> index, Args&&... args) noexcept(
      std::is_nothrow_constructible<VariadicUnion<U, Rest...>, InPlaceIndex_t<I - 1>, Args...>::value)
      : rest{InPlaceIndex_t<I - 1>{}, std::forward<Args>(args)...}
  {
  }
  template <typename... Args>
  constexpr VariadicUnion(InPlaceIndex_t<0> index,
                          Args&&... args) noexcept(std::is_nothrow_constructible<T, Args...>::value)
      : value{std::forward<Args>(args)...}
  {
  }

  ~VariadicUnion() noexcept {}

  template <std::size_t I, typename... Args, typename = std::enable_if_t<I != 0>>
  constexpr auto& construct(InPlaceIndex_t<I> type_to_construct,
                            Args&&... args) noexcept(noexcept(rest.construct(InPlaceIndex_t<I - 1>{},
                                                                             std::forward<Args>(args)...)))
  {
    return rest.construct(InPlaceIndex_t<I - 1>{}, std::forward<Args>(args)...);
  }

  template <typename... Args>
  constexpr auto& construct(InPlaceIndex_t<0> type_to_construct,
                            Args&&... args) noexcept(std::is_nothrow_constructible<T, Args...>::value)
  {
    new (std::addressof(value)) T(std::forward<Args>(args)...);
    return value;
  }

  template <typename... Args>
  constexpr void construct(std::size_t type_to_construct,
                           Args&&... args) noexcept(std::is_nothrow_copy_constructible<T>::value)
  {
    if (type_to_construct == 0)
    {
      construct_if_possible<T>(std::is_constructible<T, Args...>{}, std::addressof(value), std::forward<Args>(args)...);
      return;
    }

    rest.construct(type_to_construct - 1, std::forward<Args>(args)...);
  }

  constexpr void destruct(std::size_t index) noexcept
  {
    if (index == 0)
    {
      value.~T();
      return;
    }

    rest.destruct(index - 1);
  }

  template <typename V>
  [[nodiscard]] constexpr decltype(auto) visit(std::size_t index, V&& visitor) const&&
  {
    return (index == 0) ? std::forward<V>(visitor)(std::move(value)) : rest.visit(index - 1, std::forward<V>(visitor));
  }

  template <typename V>
  [[nodiscard]] constexpr decltype(auto) visit(std::size_t index, V&& visitor) const&
  {
    return (index == 0) ? std::forward<V>(visitor)(value) : rest.visit(index - 1, std::forward<V>(visitor));
  }

  template <typename V>
  [[nodiscard]] constexpr decltype(auto) visit(std::size_t index, V&& visitor) &&
  {
    return (index == 0) ? std::forward<V>(visitor)(std::move(value)) : rest.visit(index - 1, std::forward<V>(visitor));
  }

  template <typename V>
  [[nodiscard]] constexpr decltype(auto) visit(std::size_t index, V&& visitor) &
  {
    return (index == 0) ? std::forward<V>(visitor)(value) : rest.visit(index - 1, std::forward<V>(visitor));
  }
};

template <typename... T>
template <typename U, typename>
constexpr etcpal::Variant<T...>::Variant() noexcept(std::is_nothrow_default_constructible<U>::value)
    : type_{0}, storage_{InPlaceIndex_t<0>{}}
{
}

template <typename... T>
template <typename U, typename V>
constexpr etcpal::Variant<T...>::Variant(U&& init) noexcept(
    noexcept(make_overload(detail::ConstructFrom<T>{}...)(std::forward<U>(init))))
    : type_{detail::index_of<V, T...>()}, storage_{InPlaceIndex_t<detail::index_of<V, T...>()>{}, std::forward<U>(init)}
{
}

template <typename... T>
template <typename U, typename... Args, typename>
constexpr etcpal::Variant<T...>::Variant(InPlaceType_t<U> tag,
                                         Args&&... args) noexcept(std::is_nothrow_constructible<U, Args...>::value)
    : type_{detail::index_of<U, T...>()}, storage_{InPlaceIndex_t<detail::index_of<U, T...>()>{}}
{
}

template <typename... T>
template <typename U, typename V, typename... Args, typename>
constexpr etcpal::Variant<T...>::Variant(InPlaceType_t<U> tag, std::initializer_list<V> list, Args&&... args) noexcept(
    std::is_nothrow_constructible<U, std::initializer_list<V>, Args...>::value)
    : type_{detail::index_of<U, T...>()}
    , storage_{InPlaceIndex_t<detail::index_of<U, T...>>{}, list, std::forward<Args>(args)...}
{
}

template <typename... T>
template <std::size_t I, typename... Args, typename>
constexpr etcpal::Variant<T...>::Variant(InPlaceIndex_t<I> tag, Args&&... args) noexcept(
    std::is_nothrow_constructible<detail::TypeAtIndex_t<I, T...>, Args...>::value)
    : type_{I}, storage_{InPlaceIndex_t<I>{}, std::forward<Args>(args)...}
{
}

template <typename... T>
template <std::size_t I, typename V, typename... Args, typename>
constexpr etcpal::Variant<T...>::Variant(InPlaceIndex_t<I> tag, std::initializer_list<V> list, Args&&... args) noexcept(
    std::is_nothrow_constructible<detail::TypeAtIndex_t<I, T...>, std::initializer_list<V>, Args...>::value)
    : type_{I}, storage_{InPlaceIndex_t<I>{}, list, std::forward<Args>(args)...}
{
}

template <typename... T>
constexpr etcpal::Variant<T...>::Variant(const Variant& rhs) noexcept(detail::AllNothrowCopyConstructible<T...>::value)
    : type_{rhs.type_}, storage_{InPlaceIndex_t<sizeof...(T)>{}}
{
  rhs.storage_.visit(type_, [&](const auto& arg) { storage_.construct(type_, arg); });
}

template <typename... T>
constexpr etcpal::Variant<T...>::Variant(Variant&& rhs) noexcept
    : type_{rhs.type_}, storage_{InPlaceIndex_t<sizeof...(T)>{}}
{
  rhs.storage_.visit(type_, [&](auto&& arg) { storage_.construct(type_, std::move(arg)); });
}

template <typename... T>
etcpal::Variant<T...>::~Variant() noexcept
{
  storage_.destruct((type_ == variant_npos) ? sizeof...(T) : type_);
}

template <typename... T>
constexpr auto etcpal::Variant<T...>::operator=(const Variant& rhs) noexcept(
    detail::AllNothrowCopyAssignable<T...>::value) -> Variant&
{
  if (type_ == rhs.type_)
  {
    storage_.visit(type_, [&](auto& lhs) {
      rhs.storage_.visit(
          rhs.type_, make_selection([&](const decltype(lhs)& arg) { lhs = arg; },
                                    [](auto&& arg) {
                                      throw std::logic_error{"variants with matching indices not visited correctly"};
                                    }));
    });
    return *this;
  }

  storage_.destruct((type_ == variant_npos) ? sizeof...(T) : type_);
  try
  {
    rhs.storage_.visit(rhs.type_, [&](const auto& arg) { storage_.construct(rhs.type_, arg); });
    type_ = rhs.type_;
  }
  catch (...)
  {
    type_ = variant_npos;
  }

  return *this;
}

template <typename... T>
constexpr auto etcpal::Variant<T...>::operator=(Variant&& rhs) noexcept -> Variant&
{
  if (type_ == rhs.type_)
  {
    storage_.visit(type_, [&](auto& lhs) {
      rhs.storage_.visit(
          rhs.type_, make_selection([&](decltype(lhs)& arg) { lhs = std::move(arg); },
                                    [](auto&& arg) {
                                      throw std::logic_error{"variants with matching indices not visited correctly"};
                                    }));
    });
    return *this;
  }

  storage_.destruct((type_ == variant_npos) ? sizeof...(T) : type_);
  try
  {
    rhs.storage_.visit(rhs.type_, [&](auto&& arg) { storage_.construct(rhs.type_, std::move(arg)); });
    type_ = rhs.type_;
  }
  catch (...)
  {
    type_ = variant_npos;
  }

  return *this;
}

template <typename... T>
template <typename Visitor>
constexpr decltype(auto) etcpal::Variant<T...>::visit(Visitor&& visitor) const&&
{
  return std::move(storage_).visit(type_, std::forward<Visitor>(visitor));
}

template <typename... T>
template <typename Visitor>
constexpr decltype(auto) etcpal::Variant<T...>::visit(Visitor&& visitor) const&
{
  return storage_.visit(type_, std::forward<Visitor>(visitor));
}

template <typename... T>
template <typename Visitor>
constexpr decltype(auto) etcpal::Variant<T...>::visit(Visitor&& visitor) &&
{
  return std::move(storage_).visit(type_, std::forward<Visitor>(visitor));
}

template <typename... T>
template <typename Visitor>
constexpr decltype(auto) etcpal::Variant<T...>::visit(Visitor&& visitor) &
{
  return storage_.visit(type_, std::forward<Visitor>(visitor));
}

template <typename... T>
template <typename R, typename Visitor>
constexpr auto etcpal::Variant<T...>::visit(Visitor&& visitor) const&& -> R
{
  return std::move(storage_).visit(type_, std::forward<Visitor>(visitor));
}

template <typename... T>
template <typename R, typename Visitor>
constexpr auto etcpal::Variant<T...>::visit(Visitor&& visitor) const& -> R
{
  return storage_.visit(type_, std::forward<Visitor>(visitor));
}

template <typename... T>
template <typename R, typename Visitor>
constexpr auto etcpal::Variant<T...>::visit(Visitor&& visitor) && -> R
{
  return std::move(storage_).visit(type_, std::forward<Visitor>(visitor));
}

template <typename... T>
template <typename R, typename Visitor>
constexpr auto etcpal::Variant<T...>::visit(Visitor&& visitor) & -> R
{
  return storage_.visit(type_, std::forward<Visitor>(visitor));
}
