#pragma once

#include <initializer_list>
#include <memory>
#include <type_traits>
#include <utility>

#include <etcpal/cpp/common.h>

namespace etcpal
{

/// @brief Tag type indicating an empty optional.
struct Nullopt_t
{
};
/// @brief Tag value indicating an empty optional.
ETCPAL_INLINE_VARIABLE constexpr auto nullopt = Nullopt_t{};

/// @brief An exception indicating a checked attempt to access an empty optional.
class BadOptionalAccess : public std::exception
{
};

template <typename T>
class Optional;

namespace detail
{

template <typename T>
struct IsOptional : public std::false_type
{
};

template <typename T>
struct IsOptional<Optional<T>> : public std::true_type
{
};

template <typename T, typename = void>
struct StorageFor;

}  // namespace detail

/// @brief An implementation of `std::optional` with reference support, and more monadic operations.
/// @tparam T The contained value type.
template <typename T>
class Optional
{
public:
  constexpr Optional() noexcept = default;                                        //!< Create an empty optional.
  constexpr Optional(ETCPAL_MAYBE_UNUSED Nullopt_t tag) noexcept : Optional{} {}  //!< Create an empty optional.
  constexpr Optional(const Optional& rhs)     = default;                          //!< Copy an optional.
  constexpr Optional(Optional&& rhs) noexcept = default;                          //!< Move an optional.
  /// @brief Implicitly copy-convert an optional of a compatible type.
  /// @tparam U The contained type of the other optional.
  /// @param rhs The optional to copy-convert from.
  template <typename U,
            std::enable_if_t<std::is_convertible<U, T>::value>* = nullptr,
            typename Enable                                     = T,
            typename = std::enable_if_t<std::is_constructible<T, const U&>::value && !std::is_reference<Enable>::value>>
  constexpr Optional(const Optional<U>& rhs);
  /// @brief Explicitly copy-convert an optional of a compatible type.
  /// @tparam U The contained type of the other optional.
  /// @param rhs The optional to copy-convert from.
  template <typename U,
            std::enable_if_t<!std::is_convertible<U, T>::value>* = nullptr,
            typename Enable                                      = T,
            typename = std::enable_if_t<std::is_constructible<T, const U&>::value && !std::is_reference<Enable>::value>>
  explicit constexpr Optional(const Optional<U>& rhs);
  /// @brief Implicitly move-convert an optional of a compatible type.
  /// @tparam U The contained type of the other optional.
  /// @param rhs The optional to move-convert from.
  template <typename U,
            std::enable_if_t<std::is_convertible<U, T>::value>* = nullptr,
            typename Enable                                     = T,
            typename = std::enable_if_t<std::is_constructible<T, U>::value && !std::is_reference<Enable>::value>>
  constexpr Optional(Optional<U>&& rhs) noexcept;
  /// @brief Explicitly move-convert an optional of a compatible type.
  /// @tparam U The contained type of the other optional.
  /// @param rhs The optional to move-convert from.
  template <typename U,
            std::enable_if_t<!std::is_convertible<U, T>::value>* = nullptr,
            typename Enable                                      = T,
            typename = std::enable_if_t<std::is_constructible<T, U>::value && !std::is_reference<Enable>::value>>
  explicit constexpr Optional(Optional<U>&& rhs) noexcept;
  /// @brief Construct an engaged optional, in-place initializing the contained value using the given arguments.
  /// @tparam Args The types of arguments to initialize the contained value with.
  /// @param tag  Tag value indicating the contained value should be in-place initialized.
  /// @param args Arguments to initialize the contained value with.
  template <typename... Args,
            typename Enable = T,
            typename = std::enable_if_t<std::is_constructible<T, Args...>::value && !std::is_reference<Enable>::value>>
  explicit constexpr Optional(InPlace_t tag, Args&&... args) noexcept(std::is_nothrow_constructible<T, Args...>::value);
  /// @brief Construct an engaged optional, in-place initializing the contained value using the given arguments.
  /// @tparam U    The types of initialization values in the initializer list.
  /// @tparam Args The types of arguments to initialize the contained value with.
  /// @param tag   Tag value indicating the contained value should be in-place initialized.
  /// @param ilist An initializer list to initialize the contained value with.
  /// @param args  Arguments to initialize the contained value with.
  template <typename U,
            typename... Args,
            typename Enable = T,
            typename        = std::enable_if_t<std::is_constructible<T, std::initializer_list<U>, Args...>::value &&
                                               !std::is_reference<Enable>::value>>
  explicit constexpr Optional(InPlace_t tag, std::initializer_list<U> ilist, Args&&... args) noexcept(
      std::is_nothrow_constructible<T, std::initializer_list<U>, Args...>::value);
  /// @brief Implicitly initialize an engaged optional holding the given non-reference value.
  /// @tparam U The type of value to initialize the optional with.
  /// @param rhs The value to initialize the optional with.
  template <typename U                                            = T,
            std::enable_if_t<std::is_convertible<U&&, T>::value>* = nullptr,
            typename Enable                                       = T,
            typename                                              = std::enable_if_t<
                                                             std::is_constructible<T, U&&>::value && !std::is_same<RemoveCVRef_t<U>, InPlace_t>::value &&
                                                             !detail::IsOptional<RemoveCVRef_t<U>>::value && !std::is_reference<Enable>::value>>
  constexpr Optional(U&& rhs) noexcept(std::is_nothrow_constructible<T, U>::value);
  /// @brief Implicitly initialize an engaged optional holding the given reference.
  /// @param rhs The value to hold a reference to.
  template <typename U = T, typename = std::enable_if_t<std::is_reference<U>::value>>
  constexpr Optional(T rhs) noexcept;
  /// @brief Explicitly initialize an engaged optional holding the given non-reference value.
  /// @tparam U The type of value to initialize the optional with.
  /// @param rhs The value to initialize the optional with.
  template <typename U                                             = T,
            std::enable_if_t<!std::is_convertible<U&&, T>::value>* = nullptr,
            typename Enable                                        = T,
            typename                                               = std::enable_if_t<
                                                              std::is_constructible<T, U&&>::value && !std::is_same<RemoveCVRef_t<U>, InPlace_t>::value &&
                                                              !detail::IsOptional<RemoveCVRef_t<U>>::value && !std::is_reference<Enable>::value>>
  explicit constexpr Optional(U&& rhs) noexcept(std::is_nothrow_constructible<T, U>::value);

  ~Optional() noexcept = default;  //!< Destroy the optional, and any contained value.

  constexpr auto operator=(Nullopt_t tag) noexcept -> Optional&;             //!< Reset the optional.
  constexpr auto operator=(const Optional& rhs) -> Optional&     = default;  //!< Copy an optional value.
  constexpr auto operator=(Optional&& rhs) noexcept -> Optional& = default;  //!< Move an optional value.
  /// @brief Assign a compatible value to the optional.
  /// @tparam U The type of compatible value to assign.
  /// @param rhs The value to assign to this optional.
  /// @return Self-reference.
  template <typename U      = T,
            typename Enable = T,
            typename        = std::enable_if_t<!detail::IsOptional<U>::value && std::is_constructible<T, U>::value &&
                                               !std::is_same<std::decay_t<U>, T>::value && !std::is_reference<Enable>::value>>
  constexpr auto operator=(U&& rhs) noexcept(std::is_nothrow_constructible<T, U>::value &&
                                             std::is_nothrow_assignable<T, U>::value) -> Optional&;
  /// @brief Copy-convert another optional to this optional.
  /// @tparam U The wrapped type to convert from.
  /// @param rhs The wrapped value to convert from.
  /// @return Self-reference.
  template <typename U, typename Enable = T, typename = std::enable_if_t<!std::is_reference<Enable>::value>>
  constexpr auto operator=(const Optional<U>& rhs) noexcept(std::is_nothrow_constructible<T, const U&>::value &&
                                                            std::is_nothrow_assignable<T, const U&>::value)
      -> Optional&;
  /// @brief Move-convert another optional to this optional.
  /// @tparam U The wrapped type to convert from.
  /// @param rhs The wrapped value to convert from.
  /// @return Self-reference.
  template <typename U, typename Enable = T, typename = std::enable_if_t<!std::is_reference<Enable>::value>>
  constexpr auto operator=(Optional<U>&& rhs) noexcept(std::is_nothrow_constructible<T, U>::value &&
                                                       std::is_nothrow_assignable<T, U>::value) -> Optional&;

  /// @name Unckecked Access
  /// @brief Access the underlying value without engagement checking.
  /// @return Reference or member access to the underlying value.
  ///  @{
  ETCPAL_NODISCARD constexpr auto operator->() noexcept -> std::remove_reference_t<T>*
  {
    return std::addressof(**this);
  }
  ETCPAL_NODISCARD constexpr auto           operator->() const noexcept -> const std::remove_reference_t<T>*;
  ETCPAL_NODISCARD constexpr decltype(auto) operator*() & noexcept { return *storage_; }
  ETCPAL_NODISCARD constexpr decltype(auto) operator*() && noexcept { return *std::move(storage_); }
  ETCPAL_NODISCARD constexpr decltype(auto) operator*() const& noexcept { return *storage_; }
  ETCPAL_NODISCARD constexpr decltype(auto) operator*() const&& noexcept { return *std::move(storage_); }
  /// @}

  /// @name Checked Access
  /// @brief Access the underlying value if it exists.
  /// @return Reference to the underlying value.
  /// @throws BadOptionalAccess if the optional is empty.
  /// @{
  ETCPAL_NODISCARD constexpr decltype(auto) value() &
  {
    return ETCPAL_TERNARY_THROW(*this, *storage_, BadOptionalAccess{});
  }
  ETCPAL_NODISCARD constexpr decltype(auto) value() &&;
  ETCPAL_NODISCARD constexpr decltype(auto) value() const&
  {
    return ETCPAL_TERNARY_THROW(*this, *storage_, BadOptionalAccess{});
  }
  ETCPAL_NODISCARD constexpr decltype(auto) value() const&&;
  /// @}

  /// @name Monadic Operations
  ///
  /// @brief Operate on this optional as a mondad.
  ///
  /// All of these operations allow conditional access to the underlying value, without having to use conditional
  /// syntax:
  /// - `value_or` returns the wrapped value if it exists, or the provided default otherwise
  /// - `and_then` invokes `f` with the contained value as an argument, returning the result if the value exists, or an
  ///     empty optional otherwise; `f` must return a specialization of `Optional`
  /// - `transform` invokes `f` with the contaiined value as an argument, returning the result wrapped in an `Optional`
  ///     if the value exists, or an empty `Optional` otherwise
  /// - `or_else` returns the contained value wrapped in an `Optional` if it exists, or the result of invoking `f` with
  ///     no arguments otherwise
  /// - `visit` invokes `f` with the contained value as an argument if it exists, or with `nullopt` as an argument
  ///     otherwise, returning the invocation result
  ///
  /// @tparam U The type of default value to use.
  /// @tparam F The type of monadic visitor to apply to this optional.
  ///
  /// @param default_value The default value to return if this optional is empty.
  /// @param f             The monadic visitor to apply to this optional.
  ///
  /// @return The visitation result.
  ///
  /// @{
  template <typename U>
  ETCPAL_NODISCARD constexpr auto value_or(U&& default_value) const& -> T;
  template <typename U>
  ETCPAL_NODISCARD constexpr auto value_or(U&& default_value) && -> T;
  template <typename F>
  constexpr auto and_then(F&& f) &;
  template <typename F>
  constexpr auto and_then(F&& f) &&;
  template <typename F>
  constexpr auto and_then(F&& f) const&;
  template <typename F>
  constexpr auto and_then(F&& f) const&&;
  template <typename F>
  ETCPAL_NODISCARD constexpr auto transform(F&& f) &;
  template <typename F>
  ETCPAL_NODISCARD constexpr auto transform(F&& f) &&;
  template <typename F>
  ETCPAL_NODISCARD constexpr auto transform(F&& f) const&;
  template <typename F>
  ETCPAL_NODISCARD constexpr auto transform(F&& f) const&&;
  template <typename F>
  ETCPAL_NODISCARD constexpr auto or_else(F&& f) const& -> Optional;
  template <typename F>
  ETCPAL_NODISCARD constexpr auto or_else(F&& f) && -> Optional;
  template <typename F>
  constexpr decltype(auto) visit(F&& f) &;
  template <typename F>
  constexpr decltype(auto) visit(F&& f) &&;
  template <typename F>
  constexpr decltype(auto) visit(F&& f) const&;
  template <typename F>
  constexpr decltype(auto) visit(F&& f) const&&;
  /// @}

  /// @name Engagement
  /// @brief Check if this optional is engaged or empty.
  /// @return `true` if this optional is engaged, or `false` if it is empty.
  /// @{
  ETCPAL_NODISCARD explicit constexpr operator bool() const noexcept { return has_value(); }
  ETCPAL_NODISCARD constexpr bool     has_value() const noexcept { return bool{storage_}; }
  /// @}

  /// @brief Reset this optional, destroying any existing contained value.
  constexpr void reset() noexcept { storage_.reset(); }

  /// @name Emplacement
  /// @brief Construct a contained value in-place.
  /// @tparam U    The type of elements in the initializer list to initialize the underlying value with.
  /// @tparam Args The types of arguments to initialize the underlying value with.
  /// @param ilist The initializer list to initialize the underlying value with.
  /// @param args  The arguments to initialize the underlying value with.
  /// @return Reference to the underlying value.
  /// @{
  template <typename Enable = T, typename = std::enable_if_t<!std::is_reference<Enable>::value>, typename... Args>
  constexpr auto emplace(Args&&... args) noexcept(std::is_nothrow_constructible<T, Args...>::value) -> T&;
  template <typename Enable = T,
            typename        = std::enable_if_t<!std::is_reference<Enable>::value>,
            typename U,
            typename... Args>
  constexpr auto emplace(std::initializer_list<U> ilist, Args&&... args) noexcept(
      std::is_nothrow_constructible<T, std::initializer_list<U>, Args...>::value) -> T&;
  /// @}

  /// @name Relational Operations
  ///
  /// @brief Relational operators that use the underlying value's relational operators to compare optionals.
  ///
  /// If two optionals are both engaged, the operation uses the underlying values' relational operators. Otherwise,
  /// empty optionals compare equal to each other, and less-than engaged optionals.
  ///
  /// @tparam U The type of underlying value to compare against.
  ///
  /// @param lhs The left-hand relational operand.
  /// @param rhs The right-hand relational operatnd.
  ///
  /// @return The relational comparison result.
  /// @{
  template <typename U>
  friend constexpr bool operator==(const Optional& lhs, const Optional<U>& rhs) noexcept
  {
    return (!lhs && !rhs) || ((lhs && rhs) && (*lhs == *rhs));
  }
  template <typename U>
  friend constexpr bool operator!=(const Optional& lhs, const Optional<U>& rhs) noexcept
  {
    return (lhs.has_value() != rhs.has_value()) || (lhs && rhs && (*lhs != *rhs));
  }
  template <typename U>
  friend constexpr bool operator<(const Optional& lhs, const Optional<U>& rhs) noexcept
  {
    return (!lhs && rhs) || (lhs && rhs && (*lhs < *rhs));
  }
  template <typename U>
  friend constexpr bool operator>(const Optional& lhs, const Optional<U>& rhs) noexcept
  {
    return (lhs && !rhs) || (lhs && rhs && (*lhs > *rhs));
  }
  template <typename U>
  friend constexpr bool operator<=(const Optional& lhs, const Optional<U>& rhs) noexcept
  {
    return (!lhs && rhs) || (lhs && rhs && (*lhs <= *rhs));
  }
  template <typename U>
  friend constexpr bool operator>=(const Optional& lhs, const Optional<U>& rhs) noexcept
  {
    return (lhs && !rhs) || (lhs && rhs && (*lhs >= *rhs));
  }
  /// @}

private:
  template <typename U>
  friend class Optional;

  detail::StorageFor<T> storage_ = {};
};

/// @brief Construct an optional containing the given value.
/// @note This function never creates a wrapped reference, always a wrapped value.
/// @tparam T The type of value to create an optional from.
/// @param value The value to initialize the optional from.
/// @return The newly-created wrrapped object.
template <typename T>
ETCPAL_NODISCARD constexpr auto make_optional(T&& value) noexcept(
    std::is_nothrow_constructible<Optional<RemoveCVRef_t<T>>, T>::value)
{
  return Optional<RemoveCVRef_t<T>>{std::forward<T>(value)};
}

}  // namespace etcpal

template <typename T, typename>
struct etcpal::detail::StorageFor
{
  union Storage
  {
    constexpr Storage() noexcept : null{} {}

    ~Storage() noexcept {}

    Nullopt_t null;
    T         value;
  } storage{};
  bool engaged{false};

  constexpr StorageFor() noexcept = default;
  explicit constexpr StorageFor(T&& init) noexcept { new (std::addressof(storage.value)) T(std::move(init)); }
  explicit constexpr StorageFor(const T& init) noexcept(std::is_nothrow_copy_constructible<T>::value)
      : StorageFor{T{init}}
  {
  }

  template <typename U>
  constexpr StorageFor(const StorageFor<U>& rhs) noexcept(std::is_nothrow_constructible<T, U>::value)
      : engaged{rhs.engaged}
  {
    if (engaged)
    {
      emplace(rhs.storage.value);
    }
  }
  template <typename U>
  constexpr StorageFor(StorageFor<U>&& rhs) noexcept(std::is_nothrow_constructible<T, U>::value) : engaged{rhs.engaged}
  {
    if (engaged)
    {
      emplace(std::move(rhs.storage.value));
      rhs.reset();
    }
  }

  constexpr StorageFor(const StorageFor& rhs) noexcept(std::is_nothrow_copy_constructible<T>::value)
      : engaged{rhs.engaged}
  {
    if (*this)
    {
      new (std::addressof(storage.value)) T(rhs.storage.value);
    }
  }
  constexpr StorageFor(StorageFor&& rhs) noexcept : engaged{std::exchange(rhs.engaged, false)}
  {
    if (*this)
    {
      new (std::addressof(storage.value)) T(std::move(rhs.storage.value));
      rhs.storage.value.~T();
    }
  }

  ~StorageFor() noexcept
  {
    if (*this)
    {
      storage.value.~T();
    }
  }

  constexpr auto operator=(const StorageFor& rhs) noexcept(std::is_nothrow_copy_assignable<T>::value) -> StorageFor&
  {
    if (*this)
    {
      if (rhs)
      {
        storage.value = rhs.storage.value;
        return *this;
      }

      storage.value.~T();
      engaged = false;
      return *this;
    }

    engaged = rhs.engaged;
    if (*this)
    {
      new (std::addressof(storage.value)) T(rhs.storage.value);
    }

    return *this;
  }

  constexpr auto operator=(StorageFor&& rhs) noexcept -> StorageFor&
  {
    if (*this)
    {
      if (rhs)
      {
        storage.value = std::move(rhs.storage.value);
        engaged       = std::exchange(rhs.engaged, false);
        rhs.storage.value.~T();

        return *this;
      }

      storage.value.~T();
      engaged = false;
      return *this;
    }

    engaged = std::exchange(rhs.engaged, false);
    if (*this)
    {
      new (std::addressof(storage.value)) T(std::move(rhs.storage.value));
      rhs.storage.value.~T();
    }

    return *this;
  }

  ETCPAL_NODISCARD constexpr auto     operator*() const&& noexcept -> const T&& { return std::move(storage.value); }
  ETCPAL_NODISCARD constexpr auto     operator*() const& noexcept -> const T& { return storage.value; }
  ETCPAL_NODISCARD constexpr auto     operator*() && noexcept -> T&& { return std::move(storage.value); }
  ETCPAL_NODISCARD constexpr auto     operator*() & noexcept -> T& { return storage.value; }
  ETCPAL_NODISCARD explicit constexpr operator bool() const noexcept { return engaged; }

  constexpr void reset() noexcept
  {
    if (*this)
    {
      storage.value.~T();
      engaged = false;
    }
  }

  template <typename... Args>
  constexpr void emplace(Args&&... args) noexcept(std::is_nothrow_constructible<T, Args...>::value)
  {
    reset();
    new (std::addressof(storage.value)) T(std::forward<Args>(args)...);
    engaged = true;
  }
};

template <typename T>
struct etcpal::detail::StorageFor<T, std::enable_if_t<std::is_reference<T>::value>>
{
  std::remove_reference_t<T>* value = nullptr;

  constexpr StorageFor() noexcept = default;
  explicit constexpr StorageFor(T init) noexcept : value{std::addressof(init)} {}

  constexpr StorageFor(const StorageFor& rhs) noexcept = default;
  constexpr StorageFor(StorageFor&& rhs) noexcept : value{std::exchange(rhs.value, nullptr)} {}

  ~StorageFor() noexcept = default;

  constexpr auto operator=(const StorageFor& rhs) noexcept -> StorageFor& = default;
  constexpr auto operator=(StorageFor&& rhs) noexcept -> StorageFor&
  {
    value = std::exchange(rhs.value, nullptr);
    return *this;
  }

  ETCPAL_NODISCARD constexpr auto     operator*() const noexcept -> T { return static_cast<T>(*value); }
  ETCPAL_NODISCARD explicit constexpr operator bool() const noexcept { return value; }

  constexpr void emplace(T arg) noexcept { value = std::addressof(arg); }
};

template <typename T>
template <typename U, std::enable_if_t<std::is_convertible<U, T>::value>*, typename, typename>
constexpr etcpal::Optional<T>::Optional(const Optional<U>& rhs)
{
  if (rhs)
  {
    emplace(*rhs);
  }
}

template <typename T>
template <typename U, std::enable_if_t<!std::is_convertible<U, T>::value>*, typename, typename>
constexpr etcpal::Optional<T>::Optional(const Optional<U>& rhs) : storage_{rhs.storage_}
{
}

template <typename T>
template <typename U, std::enable_if_t<std::is_convertible<U, T>::value>*, typename, typename>
constexpr etcpal::Optional<T>::Optional(Optional<U>&& rhs) noexcept : storage_{std::move(rhs.storage_)}
{
}

template <typename T>
template <typename U, std::enable_if_t<!std::is_convertible<U, T>::value>*, typename, typename>
constexpr etcpal::Optional<T>::Optional(Optional<U>&& rhs) noexcept : storage_{std::move(rhs.storage_)}
{
}

template <typename T>
template <typename... Args, typename, typename>
constexpr etcpal::Optional<T>::Optional(InPlace_t tag,
                                        Args&&... args) noexcept(std::is_nothrow_constructible<T, Args...>::value)
{
  emplace(std::forward<Args>(args)...);
}

template <typename T>
template <typename U, typename... Args, typename, typename>
constexpr etcpal::Optional<T>::Optional(InPlace_t tag, std::initializer_list<U> ilist, Args&&... args) noexcept(
    std::is_nothrow_constructible<T, std::initializer_list<U>, Args...>::value)
{
  emplace(ilist, std::forward<Args>(args)...);
}

template <typename T>
template <typename U, std::enable_if_t<std::is_convertible<U&&, T>::value>*, typename, typename>
constexpr etcpal::Optional<T>::Optional(U&& rhs) noexcept(std::is_nothrow_constructible<T, U>::value)
{
  emplace(std::forward<U>(rhs));
}

template <typename T>
template <typename U, typename>
constexpr etcpal::Optional<T>::Optional(T rhs) noexcept : storage_{static_cast<T>(rhs)}
{
}

template <typename T>
template <typename U, std::enable_if_t<!std::is_convertible<U&&, T>::value>*, typename, typename>
constexpr etcpal::Optional<T>::Optional(U&& rhs) noexcept(std::is_nothrow_constructible<T, U>::value)
{
  emplace(std::forward<U>(rhs));
}

template <typename T>
constexpr auto etcpal::Optional<T>::operator=(Nullopt_t tag) noexcept -> Optional&
{
  reset();
  return *this;
}

template <typename T>
template <typename U, typename, typename>
constexpr auto etcpal::Optional<T>::operator=(U&& rhs) noexcept(std::is_nothrow_constructible<T, U>::value &&
                                                                std::is_nothrow_assignable<T, U>::value) -> Optional&
{
  if (*this)
  {
    **this = std::forward<U>(rhs);
  }
  else
  {
    emplace(std::forward<U>(rhs));
  }

  return *this;
}

template <typename T>
template <typename U, typename, typename>
constexpr auto etcpal::Optional<T>::operator=(const Optional<U>& rhs) noexcept(
    std::is_nothrow_constructible<T, const U&>::value && std::is_nothrow_assignable<T, const U&>::value) -> Optional&
{
  if (!rhs)
  {
    reset();
    return *this;
  }

  if (*this)
  {
    **this = *rhs;
  }
  else
  {
    emplace(*rhs);
  }

  return *this;
}

template <typename T>
template <typename U, typename, typename>
constexpr auto etcpal::Optional<T>::operator=(Optional<U>&& rhs) noexcept(std::is_nothrow_constructible<T, U>::value &&
                                                                          std::is_nothrow_assignable<T, U>::value)
    -> Optional&
{
  if (!rhs)
  {
    reset();
    return *this;
  }

  if (*this)
  {
    **this = *rhs;
  }
  else
  {
    emplace(*rhs);
  }

  return *this;
}

template <typename T>
ETCPAL_NODISCARD constexpr auto etcpal::Optional<T>::operator->() const noexcept -> const std::remove_reference_t<T>*
{
  return std::addressof(**this);
}

template <typename T>
ETCPAL_NODISCARD constexpr decltype(auto) etcpal::Optional<T>::value() &&
{
  return ETCPAL_TERNARY_THROW(*this, *std::move(storage_), BadOptionalAccess{});
}

template <typename T>
ETCPAL_NODISCARD constexpr decltype(auto) etcpal::Optional<T>::value() const&&
{
  return ETCPAL_TERNARY_THROW(*this, *std::move(storage_), BadOptionalAccess{});
}

template <typename T>
template <typename U>
ETCPAL_NODISCARD constexpr auto etcpal::Optional<T>::value_or(U&& default_value) const& -> T
{
  return *this ? **this : std::forward<U>(default_value);
}

template <typename T>
template <typename U>
ETCPAL_NODISCARD constexpr auto etcpal::Optional<T>::value_or(U&& default_value) && -> T
{
  return *this ? std::move(**this) : std::forward<U>(default_value);
}

template <typename T>
template <typename F>
constexpr auto etcpal::Optional<T>::and_then(F&& f) &
{
  return *this ? invoke(std::forward<F>(f), **this) : invoke_result_t<F, T&>{};
}

template <typename T>
template <typename F>
constexpr auto etcpal::Optional<T>::and_then(F&& f) &&
{
  return *this ? invoke(std::forward<F>(f), *std::move(*this)) : invoke_result_t<F, T>{};
}

template <typename T>
template <typename F>
constexpr auto etcpal::Optional<T>::and_then(F&& f) const&
{
  return *this ? invoke(std::forward<F>(f), **this) : invoke_result_t<F, const T&>{};
}

template <typename T>
template <typename F>
constexpr auto etcpal::Optional<T>::and_then(F&& f) const&&
{
  return *this ? invoke(std::forward<F>(f), *std::move(*this)) : invoke_result_t<F, const T>{};
}

template <typename T>
template <typename F>
ETCPAL_NODISCARD constexpr auto etcpal::Optional<T>::transform(F&& f) &
{
  using result_type = Optional<invoke_result_t<F, T&>>;
  return *this ? invoke(std::forward<F>(f), **this) : result_type{};
}

template <typename T>
template <typename F>
ETCPAL_NODISCARD constexpr auto etcpal::Optional<T>::transform(F&& f) &&
{
  using result_type = Optional<invoke_result_t<F, T>>;
  return *this ? invoke(std::forward<F>(f), std::move(**this)) : result_type{};
}

template <typename T>
template <typename F>
ETCPAL_NODISCARD constexpr auto etcpal::Optional<T>::transform(F&& f) const&
{
  using result_type = Optional<invoke_result_t<F, const T&>>;
  return *this ? invoke(std::forward<F>(f), **this) : result_type{};
}

template <typename T>
template <typename F>
ETCPAL_NODISCARD constexpr auto etcpal::Optional<T>::transform(F&& f) const&&
{
  using result_type = Optional<invoke_result_t<F, const T>>;
  return *this ? invoke(std::forward<F>(f), std::move(**this)) : result_type{};
}

template <typename T>
template <typename F>
ETCPAL_NODISCARD constexpr auto etcpal::Optional<T>::or_else(F&& f) const& -> Optional
{
  return *this ? **this : invoke(std::forward<F>(f));
}

template <typename T>
template <typename F>
ETCPAL_NODISCARD constexpr auto etcpal::Optional<T>::or_else(F&& f) && -> Optional
{
  return *this ? *std::move(*this) : invoke(std::forward<F>(f));
}

template <typename T>
template <typename F>
constexpr decltype(auto) etcpal::Optional<T>::visit(F&& f) &
{
  return *this ? invoke(std::forward<F>(f), **this) : invoke(std::forward<F>(f), nullopt);
}

template <typename T>
template <typename F>
constexpr decltype(auto) etcpal::Optional<T>::visit(F&& f) &&
{
  return *this ? invoke(std::forward<F>(f), *std::move(*this)) : invoke(std::forward<F>(f), nullopt);
}

template <typename T>
template <typename F>
constexpr decltype(auto) etcpal::Optional<T>::visit(F&& f) const&
{
  return *this ? invoke(std::forward<F>(f), **this) : invoke(std::forward<F>(f), nullopt);
}

template <typename T>
template <typename F>
constexpr decltype(auto) etcpal::Optional<T>::visit(F&& f) const&&
{
  return *this ? invoke(std::forward<F>(f), *std::move(*this)) : invoke(std::forward<F>(f), nullopt);
}

template <typename T>
template <typename Enable, typename, typename... Args>
constexpr auto etcpal::Optional<T>::emplace(Args&&... args) noexcept(std::is_nothrow_constructible<T, Args...>::value)
    -> T&
{
  storage_.emplace(std::forward<Args>(args)...);
  return **this;
}

template <typename T>
template <typename Enable, typename, typename U, typename... Args>
constexpr auto etcpal::Optional<T>::emplace(std::initializer_list<U> ilist, Args&&... args) noexcept(
    std::is_nothrow_constructible<T, std::initializer_list<U>, Args...>::value) -> T&
{
  storage_.emplace(ilist, std::forward<Args>(args)...);
  return **this;
}
