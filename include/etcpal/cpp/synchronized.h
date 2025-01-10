#pragma once

#include <memory>
#include <utility>

#include <etcpal/cpp/common.h>

namespace etcpal
{

template <typename T, typename Lock>
class Synchronized;

namespace detail
{

template <typename T, typename = void>
struct IsReaderWriterLock : public std::false_type
{
};

struct Unlock;
struct WriteUnlock;

}  // namespace detail

template <typename T, typename Lock, typename = void>
class SynchronizedRef
{
public:
  explicit SynchronizedRef(Synchronized<T, Lock>& value) noexcept;

  [[nodiscard]] auto operator*() const noexcept -> T& { return value_->value_; }
  [[nodiscard]] auto operator->() const noexcept -> T* { return std::addressof(value_->value_); }

private:
  std::unique_ptr<Synchronized<T, Lock>, detail::Unlock> value_;
};

template <typename T, typename Lock>
class SynchronizedRef<const T, Lock, std::enable_if_t<!detail::IsReaderWriterLock<Lock>::value>>
{
public:
  explicit SynchronizedRef(const Synchronized<T, Lock>& value) noexcept;

  [[nodiscard]] auto operator*() const noexcept -> const T& { return value_->value_; }
  [[nodiscard]] auto operator->() const noexcept -> const T* { return std::addressof(value_->value_); }

private:
  std::unique_ptr<const Synchronized<T, Lock>, detail::Unlock> value_;
};

template <typename T, typename Lock>
class SynchronizedRef<T, Lock, std::enable_if_t<detail::IsReaderWriterLock<Lock>::value>>
{
public:
  explicit SynchronizedRef(Synchronized<T, Lock>& value) noexcept;

  [[nodiscard]] auto operator*() const noexcept -> T& { return value_->value_; }
  [[nodiscard]] auto operator->() const noexcept -> T* { return std::addressof(value_->value_); }

private:
  std::unique_ptr<Synchronized<T, Lock>, detail::WriteUnlock> value_;
};

template <typename T, typename Lock>
class SynchronizedRef<const T, Lock, std::enable_if_t<detail::IsReaderWriterLock<Lock>::value>>
{
public:
  explicit SynchronizedRef(const Synchronized<T, Lock>& value) noexcept;

  SynchronizedRef(const SynchronizedRef& rhs) noexcept;
  SynchronizedRef(SynchronizedRef&& rhs) noexcept : value_{std::exchange(rhs.value_, nullptr)} {}

  ~SynchronizedRef() noexcept;

  auto operator=(const SynchronizedRef& rhs) noexcept -> SynchronizedRef&;
  auto operator=(SynchronizedRef&& rhs) noexcept -> SynchronizedRef&;

  [[nodiscard]] auto operator*() const noexcept -> const T& { return value_->value_; }
  [[nodiscard]] auto operator->() const noexcept -> const T* { return std::addressof(value_->value_); }

private:
  const Synchronized<T, Lock>* value_;
};

template <typename T, typename Lock>
class Synchronized
{
public:
  template <typename... Args>
  explicit Synchronized(Args&&... args) noexcept(std::is_nothrow_constructible<T, Args...>::value);

  template <typename Action>
  decltype(auto) capply(Action&& fun) const;
  template <typename Action>
  decltype(auto) apply(Action&& fun) const;
  template <typename Action>
  decltype(auto) apply(Action&& fun);

  [[nodiscard]] auto clock() const noexcept { return lock(); }
  [[nodiscard]] auto lock() const noexcept { return SynchronizedRef<const T, Lock>{*this}; }
  [[nodiscard]] auto lock() noexcept { return SynchronizedRef<T, Lock>{*this}; }

private:
  friend struct detail::Unlock;
  friend struct detail::WriteUnlock;
  friend class SynchronizedRef<T, Lock>;
  friend class SynchronizedRef<const T, Lock>;

  mutable Lock mutex_ = {};
  T            value_ = {};
};

}  // namespace etcpal

template <typename T>
struct etcpal::detail::IsReaderWriterLock<T,
                                          etcpal::void_t<decltype(std::declval<T&>().ReadLock()),
                                                         decltype(std::declval<T&>().ReadUnlock()),
                                                         decltype(std::declval<T&>().WriteLock()),
                                                         decltype(std::declval<T&>().WriteUnlock())>>
    : public std::true_type
{
};

struct etcpal::detail::Unlock
{
  template <typename T, typename Lock>
  void operator()(Synchronized<T, Lock>* value) const noexcept
  {
    value->mutex_.unlock();
  }

  template <typename T, typename Lock>
  void operator()(const Synchronized<T, Lock>* value) const noexcept
  {
    value->mutex_.unlock();
  }
};

struct etcpal::detail::WriteUnlock
{
  template <typename T, typename Lock>
  void operator()(Synchronized<T, Lock>* value) const noexcept
  {
    value->mutex_.WriteUnlock();
  }
};

template <typename T, typename Lock, typename Enable>
etcpal::SynchronizedRef<T, Lock, Enable>::SynchronizedRef(Synchronized<T, Lock>& value) noexcept
    : value_{std::addressof(value)}
{
  value_->mutex_.lock();
}

template <typename T, typename Lock>
etcpal::SynchronizedRef<const T, Lock, std::enable_if_t<!etcpal::detail::IsReaderWriterLock<Lock>::value>>::
    SynchronizedRef(const Synchronized<T, Lock>& value) noexcept
    : value_{std::addressof(value)}
{
  value_->mutex_.lock();
}

template <typename T, typename Lock>
etcpal::SynchronizedRef<T, Lock, std::enable_if_t<etcpal::detail::IsReaderWriterLock<Lock>::value>>::SynchronizedRef(
    Synchronized<T, Lock>& value) noexcept
    : value_{std::addressof(value)}
{
  value_->mutex_.WriteLock();
}

template <typename T, typename Lock>
etcpal::SynchronizedRef<const T, Lock, std::enable_if_t<etcpal::detail::IsReaderWriterLock<Lock>::value>>::
    SynchronizedRef(const Synchronized<T, Lock>& value) noexcept
    : value_{std::addressof(value)}
{
  value_->mutex_.ReadLock();
}

template <typename T, typename Lock>
etcpal::SynchronizedRef<const T, Lock, std::enable_if_t<etcpal::detail::IsReaderWriterLock<Lock>::value>>::
    SynchronizedRef(const SynchronizedRef& rhs) noexcept
    : value_{rhs.value_}
{
  value_->mutex_.ReadLock();
}

template <typename T, typename Lock>
etcpal::SynchronizedRef<const T, Lock, std::enable_if_t<etcpal::detail::IsReaderWriterLock<Lock>::value>>::
    ~SynchronizedRef() noexcept
{
  if (value_)
  {
    value_->mutex_.ReadUnlock();
  }
}

template <typename T, typename Lock>
auto etcpal::SynchronizedRef<const T, Lock, std::enable_if_t<etcpal::detail::IsReaderWriterLock<Lock>::value>>::
operator=(const SynchronizedRef& rhs) noexcept -> SynchronizedRef&
{
  if (value_)
  {
    value_->mutex_.ReadUnlock();
  }

  value_ = rhs.value_;
  if (value_)
  {
    value_->mutex_.ReadLock();
  }

  return *this;
}

template <typename T, typename Lock>
auto etcpal::SynchronizedRef<const T, Lock, std::enable_if_t<etcpal::detail::IsReaderWriterLock<Lock>::value>>::
operator=(SynchronizedRef&& rhs) noexcept -> SynchronizedRef&
{
  if (value_)
  {
    value_->mutex_.ReadUnlock();
  }

  value_ = std::exchange(rhs.value_, nullptr);

  return *this;
}

template <typename T, typename Lock>
template <typename... Args>
etcpal::Synchronized<T, Lock>::Synchronized(Args&&... args) noexcept(std::is_nothrow_constructible<T, Args...>::value)
    : value_{std::forward<Args>(args)...}
{
}

template <typename T, typename Lock>
template <typename Action>
decltype(auto) etcpal::Synchronized<T, Lock>::capply(Action&& fun) const
{
  return apply(std::forward<Action>(fun));
}

template <typename T, typename Lock>
template <typename Action>
decltype(auto) etcpal::Synchronized<T, Lock>::apply(Action&& fun) const
{
  return std::forward<Action>(fun)(*lock());
}

template <typename T, typename Lock>
template <typename Action>
decltype(auto) etcpal::Synchronized<T, Lock>::apply(Action&& fun)
{
  return std::forward<Action>(fun)(*lock());
}
