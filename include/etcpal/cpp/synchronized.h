#pragma once

#include <memory>
#include <utility>

#include <etcpal/cpp/common.h>
#include <etcpal/cpp/optional.h>

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
struct ReadUnlock;

struct NoLock
{
};

}  // namespace detail

template <typename T, typename Lock, typename = void>
class SynchronizedRef
{
public:
  explicit SynchronizedRef(Synchronized<T, Lock>& value) noexcept;

  [[nodiscard]] auto operator*() const noexcept -> T& { return value_->value_; }
  [[nodiscard]] auto operator->() const noexcept -> T* { return std::addressof(value_->value_); }

  [[nodiscard]] static auto try_acquire(Synchronized<T, Lock>& value) noexcept -> Optional<SynchronizedRef>;

private:
  explicit SynchronizedRef(detail::NoLock tag, Synchronized<T, Lock>& value) noexcept : value_(std::addressof(value)) {}

  std::unique_ptr<Synchronized<T, Lock>, detail::Unlock> value_;
};

template <typename T, typename Lock>
class SynchronizedRef<const T, Lock, std::enable_if_t<!detail::IsReaderWriterLock<Lock>::value>>
{
public:
  explicit SynchronizedRef(const Synchronized<T, Lock>& value) noexcept;

  [[nodiscard]] auto operator*() const noexcept -> const T& { return value_->value_; }
  [[nodiscard]] auto operator->() const noexcept -> const T* { return std::addressof(value_->value_); }

  [[nodiscard]] static auto try_acquire(const Synchronized<T, Lock>& value) noexcept -> Optional<SynchronizedRef>;

private:
  explicit SynchronizedRef(detail::NoLock tag, Synchronized<T, Lock>& value) noexcept : value_(std::addressof(value)) {}

  std::unique_ptr<const Synchronized<T, Lock>, detail::Unlock> value_;
};

template <typename T, typename Lock>
class SynchronizedRef<T, Lock, std::enable_if_t<detail::IsReaderWriterLock<Lock>::value>>
{
public:
  explicit SynchronizedRef(Synchronized<T, Lock>& value) noexcept;

  [[nodiscard]] auto operator*() const noexcept -> T& { return value_->value_; }
  [[nodiscard]] auto operator->() const noexcept -> T* { return std::addressof(value_->value_); }

  [[nodiscard]] static auto try_acquire(Synchronized<T, Lock>& value) noexcept -> Optional<SynchronizedRef>;

private:
  explicit SynchronizedRef(detail::NoLock tag, Synchronized<T, Lock>& value) noexcept : value_(std::addressof(value)) {}

  std::unique_ptr<Synchronized<T, Lock>, detail::WriteUnlock> value_;
};

template <typename T, typename Lock>
class SynchronizedRef<const T, Lock, std::enable_if_t<detail::IsReaderWriterLock<Lock>::value>>
{
public:
  explicit SynchronizedRef(const Synchronized<T, Lock>& value) noexcept;

  [[nodiscard]] auto operator*() const noexcept -> const T& { return value_->value_; }
  [[nodiscard]] auto operator->() const noexcept -> const T* { return std::addressof(value_->value_); }

  [[nodiscard]] static auto try_acquire(const Synchronized<T, Lock>& value) noexcept -> Optional<SynchronizedRef>;

private:
  explicit SynchronizedRef(detail::NoLock tag, const Synchronized<T, Lock>& value) noexcept
      : value_(std::addressof(value))
  {
  }

  std::unique_ptr<const Synchronized<T, Lock>, detail::ReadUnlock> value_;
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
  [[nodiscard]] auto try_clock() const noexcept { return try_lock(); }
  [[nodiscard]] auto try_lock() const noexcept { return SynchronizedRef<const T, Lock>::try_acquire(*this); }
  [[nodiscard]] auto try_lock() noexcept { return SynchronizedRef<T, Lock>::try_acquire(*this); }

private:
  friend struct detail::Unlock;
  friend struct detail::WriteUnlock;
  friend struct detail::ReadUnlock;
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

struct etcpal::detail::ReadUnlock
{
  template <typename T, typename Lock>
  void operator()(const Synchronized<T, Lock>* value) const noexcept
  {
    value->mutex_.ReadUnlock();
  }
};

template <typename T, typename Lock, typename Enable>
etcpal::SynchronizedRef<T, Lock, Enable>::SynchronizedRef(Synchronized<T, Lock>& value) noexcept
    : value_{std::addressof(value)}
{
  value_->mutex_.lock();
}

template <typename T, typename Lock, typename Enable>
[[nodiscard]] auto etcpal::SynchronizedRef<T, Lock, Enable>::try_acquire(Synchronized<T, Lock>& value) noexcept
    -> Optional<SynchronizedRef>
{
  if (value.mutex_.try_lock())
  {
    SynchronizedRef{detail::NoLock{}, value};
  }

  return {};
}

template <typename T, typename Lock>
etcpal::SynchronizedRef<const T, Lock, std::enable_if_t<!etcpal::detail::IsReaderWriterLock<Lock>::value>>::
    SynchronizedRef(const Synchronized<T, Lock>& value) noexcept
    : value_{std::addressof(value)}
{
  value_->mutex_.lock();
}

template <typename T, typename Lock>
[[nodiscard]] auto
etcpal::SynchronizedRef<const T, Lock, std::enable_if_t<!etcpal::detail::IsReaderWriterLock<Lock>::value>>::try_acquire(
    const Synchronized<T, Lock>& value) noexcept -> Optional<SynchronizedRef>
{
  if (value.mutex_.try_lock())
  {
    SynchronizedRef{detail::NoLock{}, value};
  }

  return {};
}

template <typename T, typename Lock>
etcpal::SynchronizedRef<T, Lock, std::enable_if_t<etcpal::detail::IsReaderWriterLock<Lock>::value>>::SynchronizedRef(
    Synchronized<T, Lock>& value) noexcept
    : value_{std::addressof(value)}
{
  value_->mutex_.WriteLock();
}

template <typename T, typename Lock>
[[nodiscard]] auto
etcpal::SynchronizedRef<T, Lock, std::enable_if_t<etcpal::detail::IsReaderWriterLock<Lock>::value>>::try_acquire(
    Synchronized<T, Lock>& value) noexcept -> Optional<SynchronizedRef>
{
  if (value.mutex_.TryWriteLock())
  {
    return SynchronizedRef{detail::NoLock{}, value};
  }

  return {};
}

template <typename T, typename Lock>
etcpal::SynchronizedRef<const T, Lock, std::enable_if_t<etcpal::detail::IsReaderWriterLock<Lock>::value>>::
    SynchronizedRef(const Synchronized<T, Lock>& value) noexcept
    : value_{std::addressof(value)}
{
  value_->mutex_.ReadLock();
}

template <typename T, typename Lock>
[[nodiscard]] auto
etcpal::SynchronizedRef<const T, Lock, std::enable_if_t<etcpal::detail::IsReaderWriterLock<Lock>::value>>::try_acquire(
    const Synchronized<T, Lock>& value) noexcept -> Optional<SynchronizedRef>
{
  if (value.mutex_.TryReadLock())
  {
    return SynchronizedRef{detail::NoLock{}, value};
  }

  return {};
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
