#pragma once

#include <initializer_list>
#include <memory>
#include <type_traits>
#include <utility>

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

template <typename T, typename = void>
struct StorageFor;

}  // namespace detail

template <typename T>
class Optional
{
public:
  constexpr Optional() noexcept = default;
  constexpr Optional(Nullopt_t tag) noexcept : Optional{} {}
  constexpr Optional(const Optional& rhs)     = default;
  constexpr Optional(Optional&& rhs) noexcept = default;
  template <typename U,
            std::enable_if_t<std::is_convertible<U, T>::value>* = nullptr,
            typename Enable                                     = T,
            typename = std::enable_if_t<std::is_constructible<T, const U&>::value && !std::is_reference<Enable>::value>>
  constexpr Optional(const Optional<U>& rhs);
  template <typename U,
            std::enable_if_t<!std::is_convertible<U, T>::value>* = nullptr,
            typename Enable                                      = T,
            typename = std::enable_if_t<std::is_constructible<T, const U&>::value && !std::is_reference<Enable>::value>>
  explicit constexpr Optional(const Optional<U>& rhs);
  template <typename U,
            std::enable_if_t<std::is_convertible<U, T>::value>* = nullptr,
            typename Enable                                     = T,
            typename = std::enable_if_t<std::is_constructible<T, U>::value && !std::is_reference<Enable>::value>>
  constexpr Optional(Optional<U>&& rhs) noexcept;
  template <typename U,
            std::enable_if_t<!std::is_convertible<U, T>::value>* = nullptr,
            typename Enable                                      = T,
            typename = std::enable_if_t<std::is_constructible<T, U>::value && !std::is_reference<Enable>::value>>
  explicit constexpr Optional(Optional<U>&& rhs) noexcept;
  template <typename... Args,
            typename Enable = T,
            typename = std::enable_if_t<std::is_constructible<T, Args...>::value && !std::is_reference<Enable>::value>>
  explicit constexpr Optional(InPlace_t tag, Args&&... args) noexcept(std::is_nothrow_constructible<T, Args...>::value);
  template <typename U,
            typename... Args,
            typename Enable = T,
            typename        = std::enable_if_t<std::is_constructible<T, std::initializer_list<U>, Args...>::value &&
                                               !std::is_reference<Enable>::value>>
  explicit constexpr Optional(InPlace_t tag, std::initializer_list<U> ilist, Args&&... args) noexcept(
      std::is_nothrow_constructible<T, std::initializer_list<U>, Args...>::value);
  template <typename U                                            = T,
            std::enable_if_t<std::is_convertible<U&&, T>::value>* = nullptr,
            typename Enable                                       = T,
            typename = std::enable_if_t<std::is_constructible<T, U&&>::value &&
                                        !std::is_same<std::remove_cv_t<std::remove_reference_t<U>>, InPlace_t>::value &&
                                        !detail::IsOptional<std::remove_cv_t<std::remove_reference_t<U>>>::value &&
                                        !std::is_reference<Enable>::value>>
  constexpr Optional(U&& rhs) noexcept(std::is_nothrow_constructible<T, U>::value);
  template <typename U = T, typename = std::enable_if_t<std::is_reference<U>::value>>
  constexpr Optional(T rhs) noexcept;
  template <typename U                                             = T,
            std::enable_if_t<!std::is_convertible<U&&, T>::value>* = nullptr,
            typename Enable                                        = T,
            typename = std::enable_if_t<std::is_constructible<T, U&&>::value &&
                                        !std::is_same<std::remove_cv_t<std::remove_reference_t<U>>, InPlace_t>::value &&
                                        !detail::IsOptional<std::remove_cv_t<std::remove_reference_t<U>>>::value &&
                                        !std::is_reference<Enable>::value>>
  explicit constexpr Optional(U&& rhs) noexcept(std::is_nothrow_constructible<T, U>::value);

  ~Optional() noexcept = default;

  constexpr auto operator=(Nullopt_t tag) noexcept -> Optional&;
  constexpr auto operator=(const Optional& rhs) -> Optional&     = default;
  constexpr auto operator=(Optional&& rhs) noexcept -> Optional& = default;
  template <typename U      = T,
            typename Enable = T,
            typename        = std::enable_if_t<!detail::IsOptional<U>::value && std::is_constructible<T, U>::value &&
                                               !std::is_same<std::decay_t<U>, T>::value && !std::is_reference<Enable>::value>>
  constexpr auto operator=(U&& rhs) noexcept(std::is_nothrow_constructible<T, U>::value &&
                                             std::is_nothrow_assignable<T, U>::value) -> Optional&;
  template <typename U, typename Enable = T, typename = std::enable_if_t<!std::is_reference<Enable>::value>>
  constexpr auto operator=(const Optional<U>& rhs) noexcept(std::is_nothrow_constructible<T, const U&>::value &&
                                                            std::is_nothrow_assignable<T, const U&>::value)
      -> Optional&;
  template <typename U, typename Enable = T, typename = std::enable_if_t<!std::is_reference<Enable>::value>>
  constexpr auto operator=(Optional<U>&& rhs) noexcept(std::is_nothrow_constructible<T, U>::value &&
                                                       std::is_nothrow_assignable<T, U>::value) -> Optional&;

  [[nodiscard]] constexpr auto operator->() noexcept -> std::remove_reference_t<T>* { return std::addressof(**this); }
  [[nodiscard]] constexpr auto operator->() const noexcept -> const std::remove_reference_t<T>*;
  [[nodiscard]] constexpr decltype(auto) operator*() & noexcept { return *storage_; }
  [[nodiscard]] constexpr decltype(auto) operator*() && noexcept { return *std::move(storage_); }
  [[nodiscard]] constexpr decltype(auto) operator*() const& noexcept { return *storage_; }
  [[nodiscard]] constexpr decltype(auto) operator*() const&& noexcept { return *std::move(storage_); }

  [[nodiscard]] constexpr decltype(auto) value() & { return *this ? *storage_ : throw BadOptionalAccess{}; }
  [[nodiscard]] constexpr decltype(auto) value() &&;
  [[nodiscard]] constexpr decltype(auto) value() const& { return *this ? *storage_ : throw BadOptionalAccess{}; }
  [[nodiscard]] constexpr decltype(auto) value() const&&;

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
  [[nodiscard]] constexpr bool     has_value() const noexcept { return bool{storage_}; }

  constexpr void reset() noexcept { storage_.reset(); }
  template <typename Enable = T, typename = std::enable_if_t<!std::is_reference<Enable>::value>, typename... Args>
  constexpr auto emplace(Args&&... args) noexcept(std::is_nothrow_constructible<T, Args...>::value) -> T&;
  template <typename Enable = T,
            typename        = std::enable_if_t<!std::is_reference<Enable>::value>,
            typename U,
            typename... Args>
  constexpr auto emplace(std::initializer_list<U> ilist, Args&&... args) noexcept(
      std::is_nothrow_constructible<T, std::initializer_list<U>, Args...>::value) -> T&;

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
  template <typename U>
  friend class Optional;

  detail::StorageFor<T> storage_ = {};
};

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

  [[nodiscard]] constexpr auto     operator*() const&& noexcept -> const T&& { return std::move(storage.value); }
  [[nodiscard]] constexpr auto     operator*() const& noexcept -> const T& { return storage.value; }
  [[nodiscard]] constexpr auto     operator*() && noexcept -> T&& { return std::move(storage.value); }
  [[nodiscard]] constexpr auto     operator*() & noexcept -> T& { return storage.value; }
  [[nodiscard]] explicit constexpr operator bool() const noexcept { return engaged; }

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

  [[nodiscard]] constexpr auto     operator*() const noexcept -> T { return static_cast<T>(*value); }
  [[nodiscard]] explicit constexpr operator bool() const noexcept { return value; }

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
[[nodiscard]] constexpr auto etcpal::Optional<T>::operator->() const noexcept -> const std::remove_reference_t<T>*
{
  return std::addressof(**this);
}

template <typename T>
[[nodiscard]] constexpr decltype(auto) etcpal::Optional<T>::value() &&
{
  return *this ? *std::move(*storage_) : throw BadOptionalAccess{};
}

template <typename T>
[[nodiscard]] constexpr decltype(auto) etcpal::Optional<T>::value() const&&
{
  return *this ? *std::move(*storage_) : throw BadOptionalAccess{};
}

template <typename T>
template <typename U>
[[nodiscard]] constexpr auto etcpal::Optional<T>::value_or(U&& default_value) const& -> T
{
  return *this ? **this : std::forward<U>(default_value);
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
  return *this ? std::forward<F>(f)(*std::move(*this)) : decltype(std::forward<F>(f)(*std::move(*this))){};
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
  return *this ? std::forward<F>(f)(*std::move(*this)) : decltype(std::forward<F>(f)(*std::move(*this))){};
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
  using result_type = Optional<decltype(std::forward<F>(f)(*std::move(*this)))>;
  return *this ? result_type{std::forward<F>(f)(*std::move(*this))} : result_type{};
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
  using result_type = Optional<decltype(std::forward<F>(f)(*std::move(*this)))>;
  return *this ? result_type{std::forward<F>(f)(*std::move(*this))} : result_type{};
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
  return *this ? *std::move(*this) : std::forward<F>(f)();
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
  return *this ? std::forward<F>(f)(*std::move(*this)) : std::forward<F>(f)(nullopt);
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
  return *this ? std::forward<F>(f)(*std::move(*this)) : std::forward<F>(f)(nullopt);
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
