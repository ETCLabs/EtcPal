#pragma once

#include <initializer_list>
#include <memory>
#include <type_traits>

#include <etcpal/cpp/common.h>

namespace etcpal
{

struct Nullopt_t
{
};

class BadOptionalAccess : public std::exception
{
};

ETCPAL_INLINE_VARIABLE constexpr auto nullopt = Nullopt_t{};

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

}  // namespace detail

template <typename T>
class Optional
{
public:
  constexpr Optional() noexcept = default;
  constexpr Optional(Nullopt_t tag) noexcept : Optional{} {}
  constexpr Optional(const Optional& rhs);
  constexpr Optional(Optional&& rhs) noexcept;
  template <typename U,
            std::enable_if_t<std::is_convertible<U, T>::value>* = nullptr,
            typename = std::enable_if_t<std::is_constructible<T, const U&>::value>>
  constexpr Optional(const Optional<U>& rhs);
  template <typename U,
            std::enable_if_t<!std::is_convertible<U, T>::value>* = nullptr,
            typename = std::enable_if_t<std::is_constructible<T, const U&>::value>>
  explicit constexpr Optional(const Optional<U>& rhs);
  template <typename U,
            std::enable_if_t<std::is_convertible<U, T>::value>* = nullptr,
            typename                                            = std::enable_if_t<std::is_constructible<T, U>::value>>
  constexpr Optional(Optional<U>&& rhs) noexcept;
  template <typename U,
            std::enable_if_t<!std::is_convertible<U, T>::value>* = nullptr,
            typename                                             = std::enable_if_t<std::is_constructible<T, U>::value>>
  explicit constexpr Optional(Optional<U>&& rhs) noexcept;
  template <typename... Args, typename = std::enable_if_t<std::is_constructible<T, Args...>::value>>
  explicit constexpr Optional(InPlace_t tag, Args&&... args);
  template <typename U,
            typename... Args,
            typename = std::enable_if_t<std::is_constructible<T, std::initializer_list<U>, Args...>::value>>
  explicit constexpr Optional(InPlace_t tag, std::initializer_list<U> ilist, Args&&... args);
  template <typename U                                            = T,
            std::enable_if_t<std::is_convertible<U&&, T>::value>* = nullptr,
            typename = std::enable_if_t<std::is_constructible<T, U&&>::value &&
                                        !std::is_same<std::remove_cv_t<std::remove_reference_t<U>>, InPlace_t>::value &&
                                        !detail::IsOptional<std::remove_cv_t<std::remove_reference_t<U>>>::value>>
  constexpr Optional(U&& rhs) noexcept;
  template <typename U                                             = T,
            std::enable_if_t<!std::is_convertible<U&&, T>::value>* = nullptr,
            typename = std::enable_if_t<std::is_constructible<T, U&&>::value &&
                                        !std::is_same<std::remove_cv_t<std::remove_reference_t<U>>, InPlace_t>::value &&
                                        !detail::IsOptional<std::remove_cv_t<std::remove_reference_t<U>>>::value>>
  explicit constexpr Optional(U&& rhs) noexcept;

  ~Optional() noexcept;

  constexpr auto operator=(Nullopt_t tag) noexcept -> Optional&;
  constexpr auto operator=(const Optional& rhs) -> Optional&;
  constexpr auto operator=(Optional&& rhs) noexcept -> Optional&;
  template <typename U = T,
            typename   = std::enable_if_t<!detail::IsOptional<U>::value && std::is_constructible<T, U>::value &&
                                          !std::is_same<std::decay_t<U>, T>::value>>
  constexpr auto operator=(U&& rhs) -> Optional&;
  template <typename U>
  constexpr auto operator=(const Optional<U>& rhs) -> Optional&;
  template <typename U>
  constexpr auto operator=(Optional<U>&& rhs) noexcept -> Optional&;

  [[nodiscard]] constexpr auto operator->() noexcept -> T* { return std::addressof(storage_.value); }
  [[nodiscard]] constexpr auto operator->() const noexcept -> const T* { return std::addressof(storage_.value); }
  [[nodiscard]] constexpr auto operator*() & noexcept -> T& { return storage_.value; }
  [[nodiscard]] constexpr auto operator*() && noexcept -> T&& { return std::move(storage_.value); }
  [[nodiscard]] constexpr auto operator*() const& noexcept -> const T& { return storage_.value; }
  [[nodiscard]] constexpr auto operator*() const&& noexcept -> const T&& { return std::move(storage_.value); }

  [[nodiscard]] constexpr auto value() & -> T&;
  [[nodiscard]] constexpr auto value() && -> T&&;
  [[nodiscard]] constexpr auto value() const& -> const T&;
  [[nodiscard]] constexpr auto value() const&& -> const T&&;

  template <typename U>
  [[nodiscard]] constexpr auto value_or(U&& default_value) const& -> T;
  template <typename U>
  [[nodiscard]] constexpr auto value_or(U&& default_value) && -> T;
  template <typename F>
  constexpr auto and_then(F&& f) &;
  template <typename F>
  constexpr auto and_then(F&& f) &&;
  template <typename F>
  constexpr auto and_then(F&& f) const&;
  template <typename F>
  constexpr auto and_then(F&& f) const&&;
  template <typename F>
  [[nodiscard]] constexpr auto transform(F&& f) &;
  template <typename F>
  [[nodiscard]] constexpr auto transform(F&& f) &&;
  template <typename F>
  [[nodiscard]] constexpr auto transform(F&& f) const&;
  template <typename F>
  [[nodiscard]] constexpr auto transform(F&& f) const&&;
  template <typename F>
  [[nodiscard]] constexpr auto or_else(F&& f) const& -> Optional;
  template <typename F>
  [[nodiscard]] constexpr auto or_else(F&& f) && -> Optional;
  template <typename F>
  constexpr decltype(auto) visit(F&& f) &;
  template <typename F>
  constexpr decltype(auto) visit(F&& f) &&;
  template <typename F>
  constexpr decltype(auto) visit(F&& f) const&;
  template <typename F>
  constexpr decltype(auto) visit(F&& f) const&&;

  [[nodiscard]] explicit constexpr operator bool() const noexcept { return has_value(); }
  [[nodiscard]] constexpr bool     has_value() const noexcept { return engaged_; }

  constexpr void reset() noexcept;

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

private:
  union Storage
  {
    constexpr Storage() noexcept : null{} {}

    ~Storage() noexcept {}

    Nullopt_t null;
    T         value;
  } storage_;
  bool engaged_ = false;
};

}  // namespace etcpal

template <typename T>
constexpr etcpal::Optional<T>::Optional(const Optional& rhs) : engaged_{rhs.engaged_}
{
  if (rhs)
  {
    new (std::addressof(storage_.value)) T{*rhs};
  }
}

template <typename T>
constexpr etcpal::Optional<T>::Optional(Optional&& rhs) noexcept : engaged_{rhs.engaged_}
{
  if (rhs)
  {
    new (std::addressof(storage_.value)) T{std::move(*rhs)};
    rhs.reset();
  }
}

template <typename T>
template <typename U, std::enable_if_t<std::is_convertible<U, T>::value>*, typename>
constexpr etcpal::Optional<T>::Optional(const Optional<U>& rhs) : engaged_{rhs.has_value()}
{
  if (rhs)
  {
    new (std::addressof(storage_.value)) T(*rhs);
  }
}

template <typename T>
template <typename U, std::enable_if_t<!std::is_convertible<U, T>::value>*, typename>
constexpr etcpal::Optional<T>::Optional(const Optional<U>& rhs) : engaged_{rhs.has_value()}
{
  if (rhs)
  {
    new (std::addressof(storage_.value)) T(*rhs);
  }
}

template <typename T>
template <typename U, std::enable_if_t<std::is_convertible<U, T>::value>*, typename>
constexpr etcpal::Optional<T>::Optional(Optional<U>&& rhs) noexcept : engaged_{rhs.has_value()}
{
  if (rhs)
  {
    new (std::addressof(storage_.value)) T(std::move(*rhs));
    rhs.reset();
  }
}

template <typename T>
template <typename U, std::enable_if_t<!std::is_convertible<U, T>::value>*, typename>
constexpr etcpal::Optional<T>::Optional(Optional<U>&& rhs) noexcept : engaged_{rhs.has_value()}
{
  if (rhs)
  {
    new (std::addressof(storage_.value)) T(std::move(*rhs));
    rhs.reset();
  }
}

template <typename T>
template <typename... Args, typename>
constexpr etcpal::Optional<T>::Optional(InPlace_t tag, Args&&... args) : engaged_{true}
{
  new (std::addressof(storage_.value)) T{std::forward<Args>(args)...};
}

template <typename T>
template <typename U, typename... Args, typename>
constexpr etcpal::Optional<T>::Optional(InPlace_t tag, std::initializer_list<U> ilist, Args&&... args) : engaged_{true}
{
  new (std::addressof(storage_.value)) T{ilist, std::forward<Args>(args)...};
}

template <typename T>
template <typename U, std::enable_if_t<std::is_convertible<U&&, T>::value>*, typename>
constexpr etcpal::Optional<T>::Optional(U&& rhs) noexcept : engaged_{true}
{
  new (std::addressof(storage_.value)) T(std::forward<U>(rhs));
}

template <typename T>
template <typename U, std::enable_if_t<!std::is_convertible<U&&, T>::value>*, typename>
constexpr etcpal::Optional<T>::Optional(U&& rhs) noexcept : engaged_{true}
{
  new (std::addressof(storage_.value)) T(std::forward<U>(rhs));
}

template <typename T>
etcpal::Optional<T>::~Optional() noexcept
{
  if (*this)
  {
    storage_.value.~T();
  }
}

template <typename T>
constexpr auto etcpal::Optional<T>::operator=(Nullopt_t tag) noexcept -> Optional&
{
  reset();
  return *this;
}

template <typename T>
constexpr auto etcpal::Optional<T>::operator=(const Optional& rhs) -> Optional&
{
  if (!rhs)
  {
    reset();
    return *this;
  }

  if (*this)
  {
    storage_.value = *rhs;
  }
  else
  {
    engaged_ = true;
    new (std::addressof(storage_.value)) T{*rhs};
  }

  return *this;
}

template <typename T>
constexpr auto etcpal::Optional<T>::operator=(Optional&& rhs) noexcept -> Optional&
{
  if (!rhs)
  {
    reset();
    return *this;
  }

  if (*this)
  {
    storage_.value = std::move(*rhs);
  }
  else
  {
    engaged_ = true;
    new (std::addressof(storage_.value)) T{std::move(*rhs)};
  }

  rhs.reset();

  return *this;
}

template <typename T>
template <typename U, typename>
constexpr auto etcpal::Optional<T>::operator=(U&& rhs) -> Optional&
{
  if (*this)
  {
    storage_.value = std::forward<U>(rhs);
  }
  else
  {
    engaged_ = true;
    new (std::addressof(storage_.value)) T(std::forward<U>(rhs));
  }

  return *this;
}

template <typename T>
template <typename U>
constexpr auto etcpal::Optional<T>::operator=(const Optional<U>& rhs) -> Optional&
{
  if (!rhs)
  {
    reset();
    return *this;
  }

  if (*this)
  {
    storage_.value = *rhs;
  }
  else
  {
    engaged_ = true;
    new (std::addressof(storage_.value)) T(*rhs);
  }

  return *this;
}

template <typename T>
template <typename U>
constexpr auto etcpal::Optional<T>::operator=(Optional<U>&& rhs) noexcept -> Optional&
{
  if (!rhs)
  {
    reset();
    return *this;
  }

  if (*this)
  {
    storage_.value = std::move(*rhs);
  }
  else
  {
    engaged_ = true;
    new (std::addressof(storage_.value)) T(std::move(*rhs));
  }

  rhs.reset();

  return *this;
}

template <typename T>
[[nodiscard]] constexpr auto etcpal::Optional<T>::value() & -> T&
{
  return *this ? *this : throw BadOptionalAccess{};
}

template <typename T>
[[nodiscard]] constexpr auto etcpal::Optional<T>::value() && -> T&&
{
  return *this ? std::move(**this) : throw BadOptionalAccess{};
}

template <typename T>
[[nodiscard]] constexpr auto etcpal::Optional<T>::value() const& -> const T&
{
  return *this ? *this : throw BadOptionalAccess{};
}

template <typename T>
[[nodiscard]] constexpr auto etcpal::Optional<T>::value() const&& -> const T&&
{
  return *this ? std::move(**this) : throw BadOptionalAccess{};
}

template <typename T>
template <typename U>
[[nodiscard]] constexpr auto etcpal::Optional<T>::value_or(U&& default_value) const& -> T
{
  return *this ? *this : std::forward<U>(default_value);
}

template <typename T>
template <typename U>
[[nodiscard]] constexpr auto etcpal::Optional<T>::value_or(U&& default_value) && -> T
{
  return *this ? std::move(**this) : std::forward<U>(default_value);
}

template <typename T>
template <typename F>
constexpr auto etcpal::Optional<T>::and_then(F&& f) &
{
  return *this ? std::forward<F>(f)(**this) : decltype(std::forward<F>(f)(**this)){};
}

template <typename T>
template <typename F>
constexpr auto etcpal::Optional<T>::and_then(F&& f) &&
{
  return *this ? std::forward<F>(f)(std::move(**this)) : decltype(std::forward<F>(f)(std::move(**this))){};
}

template <typename T>
template <typename F>
constexpr auto etcpal::Optional<T>::and_then(F&& f) const&
{
  return *this ? std::forward<F>(f)(**this) : decltype(std::forward<F>(f)(**this)){};
}

template <typename T>
template <typename F>
constexpr auto etcpal::Optional<T>::and_then(F&& f) const&&
{
  return *this ? std::forward<F>(f)(std::move(**this)) : decltype(std::forward<F>(f)(std::move(**this))){};
}

template <typename T>
template <typename F>
[[nodiscard]] constexpr auto etcpal::Optional<T>::transform(F&& f) &
{
  using result_type = Optional<decltype(std::forward<F>(f)(**this))>;
  return *this ? result_type{std::forward<F>(f)(**this)} : result_type{};
}

template <typename T>
template <typename F>
[[nodiscard]] constexpr auto etcpal::Optional<T>::transform(F&& f) &&
{
  using result_type = Optional<decltype(std::forward<F>(f)(std::move(**this)))>;
  return *this ? result_type{std::forward<F>(f)(std::move(**this))} : result_type{};
}

template <typename T>
template <typename F>
[[nodiscard]] constexpr auto etcpal::Optional<T>::transform(F&& f) const&
{
  using result_type = Optional<decltype(std::forward<F>(f)(**this))>;
  return *this ? result_type{std::forward<F>(f)(**this)} : result_type{};
}

template <typename T>
template <typename F>
[[nodiscard]] constexpr auto etcpal::Optional<T>::transform(F&& f) const&&
{
  using result_type = Optional<decltype(std::forward<F>(f)(std::move(**this)))>;
  return *this ? result_type{std::forward<F>(f)(std::move(**this))} : result_type{};
}

template <typename T>
template <typename F>
[[nodiscard]] constexpr auto etcpal::Optional<T>::or_else(F&& f) const& -> Optional
{
  return *this ? **this : std::forward<F>(f)();
}

template <typename T>
template <typename F>
[[nodiscard]] constexpr auto etcpal::Optional<T>::or_else(F&& f) && -> Optional
{
  return *this ? std::move(**this) : std::forward<F>(f)();
}

template <typename T>
template <typename F>
constexpr decltype(auto) etcpal::Optional<T>::visit(F&& f) &
{
  return *this ? std::forward<F>(f)(**this) : std::forward<F>(f)(nullopt);
}

template <typename T>
template <typename F>
constexpr decltype(auto) etcpal::Optional<T>::visit(F&& f) &&
{
  return *this ? std::forward<F>(f)(std::move(**this)) : std::forward<F>(f)(nullopt);
}

template <typename T>
template <typename F>
constexpr decltype(auto) etcpal::Optional<T>::visit(F&& f) const&
{
  return *this ? std::forward<F>(f)(**this) : std::forward<F>(f)(nullopt);
}

template <typename T>
template <typename F>
constexpr decltype(auto) etcpal::Optional<T>::visit(F&& f) const&&
{
  return *this ? std::forward<F>(f)(std::move(**this)) : std::forward<F>(f)(nullopt);
}

template <typename T>
constexpr void etcpal::Optional<T>::reset() noexcept
{
  if (*this)
  {
    engaged_ = false;
    storage_.value.~T();
  }
}
