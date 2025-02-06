#pragma once

#include <algorithm>
#include <array>
#include <functional>
#include <stdexcept>
#include <type_traits>

#include <etcpal/cpp/common.h>
#include <etcpal/cpp/optional.h>
#include <etcpal/cpp/overload.h>

namespace etcpal
{

/// @brief Tag type indicating the given type should be engaged into the variant.
/// @tparam T The type to engage.
template <typename T>
struct InPlaceType_t
{
};

/// @brief Tag type indicating the type at the given index should be engaged into the variant.
/// @tparam I The index of the type to engage.
template <std::size_t I>
struct InPlaceIndex_t
{
};

/// @brief Tag value indicating an invalid variant type index.
ETCPAL_INLINE_VARIABLE constexpr auto variant_npos = static_cast<std::size_t>(-1);

/// @brief Exception type indicating the variant does not contain the requested type.
class BadVariantAccess : public std::exception
{
};

namespace detail
{

template <typename T>
struct ConstructFrom
{
  template <typename... Args, typename = std::enable_if_t<std::is_constructible<T, Args...>::value>>
  [[nodiscard]] constexpr auto operator()(Args&&... args) const
      noexcept(std::is_nothrow_constructible<T, Args...>::value) -> T
  {
    return T{std::forward<Args>(args)...};
  }
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

/// @brief A type-safe tagged union type with more monadic operations than the `std::variant` in `C++17`.
/// @tparam T The types that the variant can possibly contain.
template <typename... T>
class Variant
{
public:
  /// @brief Construct a variant holding a default-constructed value of the type with index `0`.
  template <typename U = detail::TypeAtIndex_t<0, T...>,
            typename   = std::enable_if_t<std::is_default_constructible<U>::value>>
  constexpr Variant() noexcept(std::is_nothrow_default_constructible<U>::value);
  /// @brief Construct a variant using overload resolution to select the best-matching converting constructor.
  /// @tparam U The type of value to initialize this variant with.
  /// @param init The value to initialize this variant with.
  template <typename U, typename V = decltype(make_overload(detail::ConstructFrom<T>{}...)(std::declval<U>()))>
  constexpr Variant(U&& init) noexcept(noexcept(make_overload(detail::ConstructFrom<T>{}...)(std::forward<U>(init))));
  /// @brief Construct a variant holding a value of the given type constructed using the given arguments.
  /// @tparam U    The type of value to initialize this variant to hold.
  /// @tparam Args The types of arguments to initialize the wrapped value with.
  /// @param tag  Tag inidicating what type the variant should hold.
  /// @param args The arguments to initialize the wrapped value with.
  template <typename U, typename... Args, typename = std::enable_if_t<detail::is_unique_in<U, T...>()>>
  explicit constexpr Variant(InPlaceType_t<U> tag,
                             Args&&... args) noexcept(std::is_nothrow_constructible<U, Args...>::value);
  /// @brief Construct a variant holding a value of the given type constructed using the given arguments.
  /// @tparam U    The type of value to initialize this variant to hold.
  /// @tparam V    The type of values to list-initialize the wrapped value with.
  /// @tparam Args The types of arguments to initialize the wrapped value with.
  /// @param tag   Tag inidicating what type the variant should hold.
  /// @param ilist The initializer list to initialize the wrapped value with.
  /// @param args  The arguments to initialize the wrapped value with.
  template <typename U, typename V, typename... Args, typename = std::enable_if_t<detail::is_unique_in<U, T...>()>>
  explicit constexpr Variant(InPlaceType_t<U> tag, std::initializer_list<V> list, Args&&... args) noexcept(
      std::is_nothrow_constructible<U, std::initializer_list<V>, Args...>::value);
  /// @brief Construct a variant holding a value of the type at the given index constructed using the given arguments.
  /// @tparam I    The index of the type of value to initialize this variant to hold.
  /// @tparam Args The types of arguments to initialize the wrapped value with.
  /// @param tag  Tag inidicating what type the variant should hold.
  /// @param args The arguments to initialize the wrapped value with.
  template <std::size_t I,
            typename... Args,
            typename = std::enable_if_t<std::less<>{}(I, sizeof...(T)) &&
                                        std::is_constructible<detail::TypeAtIndex_t<I, T...>, Args...>::value>>
  explicit constexpr Variant(InPlaceIndex_t<I> tag, Args&&... args) noexcept(
      std::is_nothrow_constructible<detail::TypeAtIndex_t<I, T...>, Args...>::value);
  /// @brief Construct a variant holding a value of the type at the given index constructed using the given arguments.
  /// @tparam I    The index of the type of value to initialize this variant to hold.
  /// @tparam V    The type of values to list-initialize the wrapped value with.
  /// @tparam Args The types of arguments to initialize the wrapped value with.
  /// @param tag   Tag inidicating what type the variant should hold.
  /// @param ilist The initializer list to initialize the wrapped value with.
  /// @param args  The arguments to initialize the wrapped value with.
  template <std::size_t I,
            typename V,
            typename... Args,
            typename = std::enable_if_t<
                std::less<>{}(I, sizeof...(T)) &&
                std::is_constructible<detail::TypeAtIndex_t<I, T...>, std::initializer_list<V>, Args...>::value>>
  explicit constexpr Variant(InPlaceIndex_t<I> tag, std::initializer_list<V> list, Args&&... args) noexcept(
      std::is_nothrow_constructible<detail::TypeAtIndex_t<I, T...>, std::initializer_list<V>, Args...>::value);

  /// @brief Copy construct a variant.
  /// @param rhs The variant to copy from.
  constexpr Variant(const Variant& rhs) noexcept(detail::AllNothrowCopyConstructible<T...>::value);
  /// @brief Move construct a variant.
  /// @param rhs The variant to move from.
  constexpr Variant(Variant&& rhs) noexcept;

  ~Variant() noexcept;  //!< Destroy the variant, destroying the wrapped value.

  /// @brief Copy assign to this variant.
  /// @param rhs The variant to copy from.
  /// @return Self-reference.
  ETCPAL_CONSTEXPR_20 auto operator=(const Variant& rhs) noexcept(detail::AllNothrowCopyAssignable<T...>::value)
      -> Variant&;
  /// @brief Move assign to this variant.
  /// @param rhs The variant to move from.
  /// @return Self-reference.
  ETCPAL_CONSTEXPR_20 auto operator=(Variant&& rhs) noexcept -> Variant&;

  /// @name Observers
  /// @brief Inspect what type the variant currently holds.
  /// @return The index of the currently-engaged type, or the valueless-by-exception status.
  /// @{
  [[nodiscard]] constexpr auto index() const noexcept { return type_; }
  [[nodiscard]] constexpr bool valueless_by_exception() const noexcept { return index() == variant_npos; }
  /// @}

  /// @name Emplacement
  /// @brief In-place construct a value into this variant.
  /// @tparam U    The type of value to place into this variant.
  /// @tparam I    The index of the type to place into this variant.
  /// @tparam V    The type of arguments to list-initialize the wrapped value with.
  /// @tparam Args The types of arguments to initialize the wrapped value with.
  /// @param ilist The initializer list to initialize the wrapped value with.
  /// @param args  The arguments to initialize the wrapped value with.
  /// @return Reference to the newly-emplaced value.
  /// @{
  template <typename U, typename... Args, typename = std::enable_if_t<detail::is_unique_in<U, T...>()>>
  auto& emplace(Args&&... args) noexcept(std::is_nothrow_constructible<U, Args...>::value);
  template <typename U, typename V, typename... Args, typename = std::enable_if_t<detail::is_unique_in<U, T...>()>>
  auto& emplace(std::initializer_list<V> ilist,
                Args&&... args) noexcept(std::is_nothrow_constructible<U, std::initializer_list<V>, Args...>::value);
  template <std::size_t I, typename... Args, typename = std::enable_if_t<std::less<>{}(I, sizeof...(T))>>
  auto& emplace(Args&&... args) noexcept(std::is_nothrow_constructible<detail::TypeAtIndex_t<I, T...>, Args...>::value);
  template <std::size_t I, typename V, typename... Args, typename = std::enable_if_t<std::less<>{}(I, sizeof...(T))>>
  auto& emplace(std::initializer_list<V> ilist, Args&&... args) noexcept(
      std::is_nothrow_constructible<detail::TypeAtIndex_t<I, T...>, std::initializer_list<V>, Args...>::value);
  /// @}

  /// @name Visitation
  /// @brief Visit the variant with a callable object invocable with any of the types in `T...` as an argument.
  /// @tparam Visitor The type of visitor to visit the variant with.
  /// @param visitor The visitor to visit the variant with.
  /// @return The visitation result.
  /// @{
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
  /// @}

  /// @name Typed Access
  /// @brief Access the variant assuming the engaged type.
  /// @tparam U The type to assume the variant holds.
  /// @tparam I The index of the type to assume the variant holds.
  /// @return Reference to the requested type if it exists, or an empty optional otherwise.
  /// @throws BadVariantAccess if the variant does not acutally contain the requested type.
  /// @{
  template <typename U, typename = std::enable_if_t<detail::is_unique_in<U, T...>()>>
  [[nodiscard]] constexpr auto get() const&& -> const U&&;
  template <typename U, typename = std::enable_if_t<detail::is_unique_in<U, T...>()>>
  [[nodiscard]] constexpr auto get() const& -> const U&;
  template <typename U, typename = std::enable_if_t<detail::is_unique_in<U, T...>()>>
  [[nodiscard]] constexpr auto get() && -> U&&;
  template <typename U, typename = std::enable_if_t<detail::is_unique_in<U, T...>()>>
  [[nodiscard]] constexpr auto get() & -> U&;
  template <std::size_t I, typename = std::enable_if_t<std::less<>{}(I, sizeof...(T))>>
  [[nodiscard]] constexpr auto get() const&& -> const detail::TypeAtIndex_t<I, T...>&&;
  template <std::size_t I, typename = std::enable_if_t<std::less<>{}(I, sizeof...(T))>>
  [[nodiscard]] constexpr auto get() const& -> const detail::TypeAtIndex_t<I, T...>&;
  template <std::size_t I, typename = std::enable_if_t<std::less<>{}(I, sizeof...(T))>>
  [[nodiscard]] constexpr auto get() && -> detail::TypeAtIndex_t<I, T...>&&;
  template <std::size_t I, typename = std::enable_if_t<std::less<>{}(I, sizeof...(T))>>
  [[nodiscard]] constexpr auto get() & -> detail::TypeAtIndex_t<I, T...>&;
  template <typename U, typename = std::enable_if_t<detail::is_unique_in<U, T...>()>>
  [[nodiscard]] constexpr auto get_if() const&& noexcept -> Optional<const U&&>;
  template <typename U, typename = std::enable_if_t<detail::is_unique_in<U, T...>()>>
  [[nodiscard]] constexpr auto get_if() const& noexcept -> Optional<const U&>;
  template <typename U, typename = std::enable_if_t<detail::is_unique_in<U, T...>()>>
  [[nodiscard]] constexpr auto get_if() && noexcept -> Optional<U&&>;
  template <typename U, typename = std::enable_if_t<detail::is_unique_in<U, T...>()>>
  [[nodiscard]] constexpr auto get_if() & noexcept -> Optional<U&>;
  template <std::size_t I, typename = std::enable_if_t<std::less<>{}(I, sizeof...(T))>>
  [[nodiscard]] constexpr auto get_if() const&& noexcept -> Optional<const detail::TypeAtIndex_t<I, T...>&&>;
  template <std::size_t I, typename = std::enable_if_t<std::less<>{}(I, sizeof...(T))>>
  [[nodiscard]] constexpr auto get_if() const& noexcept -> Optional<const detail::TypeAtIndex_t<I, T...>&>;
  template <std::size_t I, typename = std::enable_if_t<std::less<>{}(I, sizeof...(T))>>
  [[nodiscard]] constexpr auto get_if() && noexcept -> Optional<detail::TypeAtIndex_t<I, T...>&&>;
  template <std::size_t I, typename = std::enable_if_t<std::less<>{}(I, sizeof...(T))>>
  [[nodiscard]] constexpr auto get_if() & noexcept -> Optional<detail::TypeAtIndex_t<I, T...>&>;
  /// @}

  /// @name Relational Operators
  ///
  /// @brief Compare two variants.
  ///
  /// If two variants have the same type indices engaged, their wrapped values are compared using the underlying
  /// relational operators. Otherwise, variants are compared via their engaged type indices.
  ///
  /// @param lhs The left-hand relational operand.
  /// @param rhs The right-hand relational operand.
  ///
  /// @return The relational comparision result.
  ///
  /// @{
  [[nodiscard]] friend constexpr bool operator==(const Variant& lhs, const Variant& rhs) noexcept
  {
    if ((lhs.index() != rhs.index()) || (lhs.valueless_by_exception() && rhs.valueless_by_exception()))
    {
      return false;
    }

    return lhs.visit([&](const auto& l) {
      return rhs.visit(make_selection([&](const decltype(l)& r) { return l == r; },
                                      [](const auto& r) -> bool { ETCPAL_THROW(BadVariantAccess{}); }));
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
                                      [](const auto& r) -> bool { ETCPAL_THROW(BadVariantAccess{}); }));
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
                                      [](const auto& r) -> bool { ETCPAL_THROW(BadVariantAccess{}); }));
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
                                      [](const auto& r) -> bool { ETCPAL_THROW(BadVariantAccess{}); }));
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
                                      [](const auto& r) -> bool { ETCPAL_THROW(BadVariantAccess{}); }));
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
                                      [](const auto& r) -> bool { ETCPAL_THROW(BadVariantAccess{}); }));
    });
  }
  /// @}

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

  constexpr VariadicUnion(const VariadicUnion& rhs) : empty{} {}

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

  template <typename... Args>
  constexpr auto& construct(InPlaceIndex_t<variant_npos> type_to_construct, Args&&... args) noexcept
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
      ETCPAL_THROW(BadVariantAccess{});
    }

    return std::forward<V>(visitor)(std::move(value));
  }

  template <typename V>
  [[nodiscard]] constexpr decltype(auto) visit(std::size_t index, V&& visitor) const&
  {
    if (index != 0)
    {
      ETCPAL_THROW(BadVariantAccess{});
    }

    return std::forward<V>(visitor)(value);
  }

  template <typename V>
  [[nodiscard]] constexpr decltype(auto) visit(std::size_t index, V&& visitor) &&
  {
    if (index != 0)
    {
      ETCPAL_THROW(BadVariantAccess{});
    }

    return std::forward<V>(visitor)(std::move(value));
  }

  template <typename V>
  [[nodiscard]] constexpr decltype(auto) visit(std::size_t index, V&& visitor) &
  {
    if (index != 0)
    {
      ETCPAL_THROW(BadVariantAccess{});
    }

    return std::forward<V>(visitor)(value);
  }

  [[nodiscard]] constexpr auto& get(InPlaceIndex_t<0> tag) const noexcept { return value; }
  [[nodiscard]] constexpr auto& get(InPlaceIndex_t<0> tag) noexcept { return value; }
  [[nodiscard]] constexpr auto& get(InPlaceIndex_t<1> tag) const noexcept { return empty; }
  [[nodiscard]] constexpr auto& get(InPlaceIndex_t<1> tag) noexcept { return empty; }
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

  [[nodiscard]] constexpr auto& get(InPlaceIndex_t<0> tag) const noexcept { return value; }
  [[nodiscard]] constexpr auto& get(InPlaceIndex_t<0> tag) noexcept { return value; }
  template <std::size_t I, typename = std::enable_if_t<I != 0>>
  [[nodiscard]] constexpr auto& get(InPlaceIndex_t<I> tag) const noexcept
  {
    return rest.get(InPlaceIndex_t<I - 1>{});
  }
  template <std::size_t I, typename = std::enable_if_t<I != 0>>
  [[nodiscard]] constexpr auto& get(InPlaceIndex_t<I> tag) noexcept
  {
    return rest.get(InPlaceIndex_t<I - 1>{});
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
ETCPAL_CONSTEXPR_20 auto etcpal::Variant<T...>::operator=(const Variant& rhs) noexcept(
    detail::AllNothrowCopyAssignable<T...>::value) -> Variant&
{
  if (type_ == rhs.type_)
  {
    storage_.visit(type_, [&](auto& lhs) {
      rhs.storage_.visit(rhs.type_, make_selection([&](const decltype(lhs)& arg) { lhs = arg; },
                                                   [](auto&& arg) { ETCPAL_THROW(BadVariantAccess{}); }));
    });
    return *this;
  }

  storage_.destruct((type_ == variant_npos) ? sizeof...(T) : type_);
  ETCPAL_TRY
  {
    rhs.storage_.visit(rhs.type_, [&](const auto& arg) { storage_.construct(rhs.type_, arg); });
    type_ = rhs.type_;
  }
  ETCPAL_CATCH(...)
  {
    type_ = variant_npos;
  }

  return *this;
}

template <typename... T>
ETCPAL_CONSTEXPR_20 auto etcpal::Variant<T...>::operator=(Variant&& rhs) noexcept -> Variant&
{
  if (type_ == rhs.type_)
  {
    storage_.visit(type_, [&](auto& lhs) {
      rhs.storage_.visit(rhs.type_, make_selection([&](decltype(lhs)& arg) { lhs = std::move(arg); },
                                                   [](auto&& arg) { ETCPAL_THROW(BadVariantAccess{}); }));
    });
    return *this;
  }

  storage_.destruct((type_ == variant_npos) ? sizeof...(T) : type_);
  ETCPAL_TRY
  {
    rhs.storage_.visit(rhs.type_, [&](auto&& arg) { storage_.construct(rhs.type_, std::move(arg)); });
    type_ = rhs.type_;
  }
  ETCPAL_CATCH(...)
  {
    type_ = variant_npos;
  }

  return *this;
}

template <typename... T>
template <typename U, typename... Args, typename>
auto& etcpal::Variant<T...>::emplace(Args&&... args) noexcept(std::is_nothrow_constructible<U, Args...>::value)
{
  storage_.destruct(index());
  storage_.construct(InPlaceIndex_t<detail::index_of<U, T...>()>{}, std::forward<Args>(args)...);

  return get<U>();
}

template <typename... T>
template <typename U, typename V, typename... Args, typename>
auto& etcpal::Variant<T...>::emplace(std::initializer_list<V> ilist, Args&&... args) noexcept(
    std::is_nothrow_constructible<U, std::initializer_list<V>, Args...>::value)
{
  storage_.destruct(index());
  storage_.construct(InPlaceIndex_t<detail::index_of<U, T...>()>{}, ilist, std::forward<Args>(args)...);

  return get<U>();
}

template <typename... T>
template <std::size_t I, typename... Args, typename>
auto& etcpal::Variant<T...>::emplace(Args&&... args) noexcept(
    std::is_nothrow_constructible<detail::TypeAtIndex_t<I, T...>, Args...>::value)
{
  storage_.destruct(index());
  storage_.construct(InPlaceIndex_t<I>{}, std::forward<Args>(args)...);

  return get<I>();
}

template <typename... T>
template <std::size_t I, typename V, typename... Args, typename>
auto& etcpal::Variant<T...>::emplace(std::initializer_list<V> ilist, Args&&... args) noexcept(
    std::is_nothrow_constructible<detail::TypeAtIndex_t<I, T...>, std::initializer_list<V>, Args...>::value)
{
  storage_.destruct(index());
  storage_.construct(InPlaceIndex_t<I>{}, ilist, std::forward<Args>(args)...);

  return get<I>();
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

template <typename... T>
template <typename U, typename>
[[nodiscard]] constexpr auto etcpal::Variant<T...>::get() const&& -> const U&&
{
  return std::move(get<detail::index_of<U, T...>()>());
}

template <typename... T>
template <typename U, typename>
[[nodiscard]] constexpr auto etcpal::Variant<T...>::get() const& -> const U&
{
  return get<detail::index_of<U, T...>()>();
}

template <typename... T>
template <typename U, typename>
[[nodiscard]] constexpr auto etcpal::Variant<T...>::get() && -> U&&
{
  return std::move(get<detail::index_of<U, T...>()>());
}

template <typename... T>
template <typename U, typename>
[[nodiscard]] constexpr auto etcpal::Variant<T...>::get() & -> U&
{
  return get<detail::index_of<U, T...>()>();
}

template <typename... T>
template <std::size_t I, typename>
[[nodiscard]] constexpr auto etcpal::Variant<T...>::get() const&& -> const detail::TypeAtIndex_t<I, T...>&&
{
  if (index() != I)
  {
    ETCPAL_THROW(BadVariantAccess{});
  }

  return std::move(storage_.get(InPlaceIndex_t<I>{}));
}

template <typename... T>
template <std::size_t I, typename>
[[nodiscard]] constexpr auto etcpal::Variant<T...>::get() const& -> const detail::TypeAtIndex_t<I, T...>&
{
  if (index() != I)
  {
    ETCPAL_THROW(BadVariantAccess{});
  }

  return storage_.get(InPlaceIndex_t<I>{});
}

template <typename... T>
template <std::size_t I, typename>
[[nodiscard]] constexpr auto etcpal::Variant<T...>::get() && -> detail::TypeAtIndex_t<I, T...>&&
{
  if (index() != I)
  {
    ETCPAL_THROW(BadVariantAccess{});
  }

  return std::move(storage_.get(InPlaceIndex_t<I>{}));
}

template <typename... T>
template <std::size_t I, typename>
[[nodiscard]] constexpr auto etcpal::Variant<T...>::get() & -> detail::TypeAtIndex_t<I, T...>&
{
  if (index() != I)
  {
    ETCPAL_THROW(BadVariantAccess{});
  }

  return storage_.get(InPlaceIndex_t<I>{});
}

template <typename... T>
template <typename U, typename>
[[nodiscard]] constexpr auto etcpal::Variant<T...>::get_if() const&& noexcept -> Optional<const U&&>
{
  return std::move(*this).template get_if<detail::index_of<U, T...>()>();
}

template <typename... T>
template <typename U, typename>
[[nodiscard]] constexpr auto etcpal::Variant<T...>::get_if() const& noexcept -> Optional<const U&>
{
  return get_if<detail::index_of<U, T...>()>();
}

template <typename... T>
template <typename U, typename>
[[nodiscard]] constexpr auto etcpal::Variant<T...>::get_if() && noexcept -> Optional<U&&>
{
  return std::move(*this).template get_if<detail::index_of<U, T...>()>();
}

template <typename... T>
template <typename U, typename>
[[nodiscard]] constexpr auto etcpal::Variant<T...>::get_if() & noexcept -> Optional<U&>
{
  return get_if<detail::index_of<U, T...>()>();
}

template <typename... T>
template <std::size_t I, typename>
[[nodiscard]] constexpr auto etcpal::Variant<T...>::get_if() const&& noexcept
    -> Optional<const detail::TypeAtIndex_t<I, T...>&&>
{
  if (index() != I)
  {
    return {};
  }

  return std::move(storage_.get(InPlaceIndex_t<I>{}));
}

template <typename... T>
template <std::size_t I, typename>
[[nodiscard]] constexpr auto etcpal::Variant<T...>::get_if() const& noexcept
    -> Optional<const detail::TypeAtIndex_t<I, T...>&>
{
  if (index() != I)
  {
    return {};
  }

  return storage_.get(InPlaceIndex_t<I>{});
}

template <typename... T>
template <std::size_t I, typename>
[[nodiscard]] constexpr auto etcpal::Variant<T...>::get_if() && noexcept -> Optional<detail::TypeAtIndex_t<I, T...>&&>
{
  if (index() != I)
  {
    return {};
  }

  return std::move(storage_.get(InPlaceIndex_t<I>{}));
}

template <typename... T>
template <std::size_t I, typename>
[[nodiscard]] constexpr auto etcpal::Variant<T...>::get_if() & noexcept -> Optional<detail::TypeAtIndex_t<I, T...>&>
{
  if (index() != I)
  {
    return {};
  }

  return Optional<detail::TypeAtIndex_t<I, T...>&>{storage_.get(InPlaceIndex_t<I>{})};
}
