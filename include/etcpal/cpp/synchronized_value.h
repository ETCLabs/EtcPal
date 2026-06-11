/******************************************************************************
 * Copyright 2026 ETC Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *******************************************************************************
 * This file is a part of EtcPal. For more information, go to:
 * https://github.com/ETCLabs/EtcPal
 ******************************************************************************/

/// Partially based on / inspired by Boost's synchronized_value type:
/// https://github.com/boostorg/thread/blob/ac15f907be14ce070a99d0c719aa8bcdb14de6b0/include/boost/thread/synchronized_value.hpp
/// (C) Copyright 2010 Just Software Solutions Ltd http://www.justsoftwaresolutions.co.uk
/// (C) Copyright 2012 Vicente J. Botet Escriba
/// Distributed under the Boost Software License, Version 1.0. (See
/// accompanying file LICENSE_1_0.txt or copy at
/// http://www.boost.org/LICENSE_1_0.txt)

/// @file etcpal/cpp/synchronized_value.h
/// @brief C++ utility for bundling a value with the lock that guards it. Like a Rust `Mutex<T>`.

#ifndef ETCPAL_CPP_SYNCHRONIZED_VALUE_H_
#define ETCPAL_CPP_SYNCHRONIZED_VALUE_H_

#include <memory>
#include <stdexcept>
#include <type_traits>
#include <utility>

#include "etcpal/cpp/common.h"
#include "etcpal/cpp/mutex.h"
#include "etcpal/cpp/recursive_mutex.h"

namespace etcpal
{
/// @defgroup etcpal_cpp_synchronized_value synchronized_value (Synchronized Values)
/// @ingroup etcpal_cpp
/// @brief A value bundled with a lock, where access requires holding the lock.
///
/// SynchronizedValue wraps a value of type T together with a lock and only hands out access to the
/// value through RAII handles that hold that lock. This makes it impossible to touch the value
/// without synchronization, in the style of Rust's `Mutex<T>`.
///
/// The value is reached either transiently with `operator->` (locked for one call), for a scope via
/// a handle returned from Synchronize(), through a callable with Apply(), or without blocking via
/// TryToSynchronize(). By default the lock is an etcpal::Mutex.
///
/// Example usage:
/// @code
/// etcpal::SynchronizedValue<std::vector<int>> list;
///
/// list->push_back(42);                      // locked for the duration of this single call
///
/// {
///   auto locked = list.Synchronize();       // locked for the scope of `locked`
///   locked->push_back(1);
///   locked->push_back(2);
/// }
///
/// auto count = list.Apply([](std::vector<int>& v) { return v.size(); });  // locked during the call
///
/// if (auto locked = list.TryToSynchronize())  // populated only if the lock was acquired
///   locked->push_back(3);
/// @endcode
///
/// An API that requires an already-locked value can take a StrictLockPtr&, which owns its lock unless
/// it has been moved from. A UniqueLockPtr that owns its lock can be converted into one, either directly
/// (`StrictLockPtr<T>{std::move(unique)}`) or Boost-style via Release() and an AdoptLockTag constructor.
///
/// @note With a non-recursive lock, two handles alive on the same thread deadlock — including two created
///       in the same full expression, e.g. `func(*v.Synchronize(), *v.Synchronize())` or
///       `v->Append(v->GetPrefix())`, since each handle lives until the end of the full expression.

template <typename T, typename LockType = Mutex>
class SynchronizedValue;
template <typename T, typename LockType = Mutex>
class ConstUniqueLockPtr;
template <typename T, typename LockType = Mutex>
class UniqueLockPtr;

namespace detail
{
/// @brief Meta struct to extract the first type from a parameter pack.
template <typename...>
struct FirstType
{
  using type = void;  // NOLINT(readability-identifier-naming)
};
template <typename First, typename... Rest>
struct FirstType<First, Rest...>
{
  using type = First;  // NOLINT(readability-identifier-naming)
};
}  // namespace detail

/// @ingroup etcpal_cpp_synchronized_value
/// @brief Tag requesting that a lock already held by the calling thread be adopted rather than acquired.
struct AdoptLockTag
{
};
/// @ingroup etcpal_cpp_synchronized_value
/// @brief Tag requesting that the lock be left unacquired on construction, to be acquired later with
///        ConstUniqueLockPtr::Lock() or ConstUniqueLockPtr::TryLock().
struct DeferLockTag
{
};
/// @ingroup etcpal_cpp_synchronized_value
/// @brief Tag requesting that the lock be acquired with a poll or optional timed wait, never failing with
///        an exception; check OwnsLock() for the result.
struct TryToLockTag
{
};

/// @ingroup etcpal_cpp_synchronized_value
/// @brief Trait reporting whether SynchronizedValue::TryToSynchronize() honors its timeout for @p LockType
///        on the target platform.
///
/// Defaults to false. EtcPal's lock types are specialized to reflect their platform capability macros (e.g.
/// ETCPAL_MUTEX_HAS_TIMED_LOCK). Specialize this for a custom lock type to advertise timed-lock support.
template <typename LockType>
struct SupportsTimedLock : std::false_type
{
};
template <>
struct SupportsTimedLock<Mutex> : std::integral_constant<bool, ETCPAL_MUTEX_HAS_TIMED_LOCK>
{
};
template <>
struct SupportsTimedLock<RecursiveMutex> : std::integral_constant<bool, ETCPAL_RECURSIVE_MUTEX_HAS_TIMED_LOCK>
{
};

/// @ingroup etcpal_cpp_synchronized_value
/// @brief An RAII handle holding a SynchronizedValue's lock and granting read-only access to the value.
///
/// The lock is acquired when the handle is constructed and released when it is destroyed. Handles are
/// normally obtained from SynchronizedValue rather than constructed directly. Move-only.
///
/// @note ConstStrictLockPtr implies the lock is held, but this fails to be true if moved.
///
/// @tparam T        The type of the guarded value.
/// @tparam LockType The type of lock guarding the value.
template <typename T, typename LockType = Mutex>
class ConstStrictLockPtr
{
public:
  /// @brief Lock the given SynchronizedValue for read-only access.
  /// @throw std::runtime_error if lock could not be acquired.
  explicit ConstStrictLockPtr(const SynchronizedValue<T, LockType>& sync) : sync_(&sync)
  {
    if (!sync_->lock_.Lock())
      ETCPAL_THROW(std::runtime_error("etcpal SynchronizedValue lock failed."));
  }

  /// @brief Adopt the given SynchronizedValue's lock, which the calling thread must already hold.
  ConstStrictLockPtr(const SynchronizedValue<T, LockType>& sync, AdoptLockTag /*unused*/) noexcept : sync_(&sync) {}

  /// @brief Take over the lock held by @p other, which is left empty.
  /// @throw std::runtime_error if @p other does not own the lock.
  explicit ConstStrictLockPtr(ConstUniqueLockPtr<T, LockType>&& other) : sync_(other.sync_)
  {
    if (!other.owns_)
      ETCPAL_THROW(std::runtime_error("etcpal ConstStrictLockPtr constructed from a non-owning handle."));
    other.sync_ = nullptr;
    other.owns_ = false;
  }

  ConstStrictLockPtr(const ConstStrictLockPtr&) = delete;
  ConstStrictLockPtr& operator=(const ConstStrictLockPtr&) = delete;
  ConstStrictLockPtr& operator=(ConstStrictLockPtr&&) = delete;

  /// @brief Move ownership of the held lock from another handle, which is left empty.
  ConstStrictLockPtr(ConstStrictLockPtr&& other) noexcept : sync_(other.sync_) { other.sync_ = nullptr; }

  /// @brief Release the lock.
  ~ConstStrictLockPtr() noexcept
  {
    if (sync_)
      sync_->lock_.Unlock();
  }

  /// @brief Get a reference to the guarded value. Undefined behavior if this handle has been moved from.
  const T& operator*() const noexcept { return sync_->value_; }
  /// @brief Access a member of the guarded value. Undefined behavior if this handle has been moved from.
  const T* operator->() const noexcept { return std::addressof(sync_->value_); }

protected:
  // NOLINTNEXTLINE(cppcoreguidelines-non-private-member-variables-in-classes) - derived handles need direct access
  const SynchronizedValue<T, LockType>* sync_;
};

/// @ingroup etcpal_cpp_synchronized_value
/// @brief An RAII handle holding a SynchronizedValue's lock and granting mutable access to the value.
///
/// Like ConstStrictLockPtr, but hands out mutable access. Normally obtained from SynchronizedValue. Move-only.
///
/// @note StrictLockPtr implies the lock is held, but this fails to be true if moved.
///
/// @tparam T        The type of the guarded value.
/// @tparam LockType The type of lock guarding the value.
template <typename T, typename LockType = Mutex>
class StrictLockPtr : public ConstStrictLockPtr<T, LockType>
{
public:
  /// @brief Lock the given SynchronizedValue for mutable access.
  /// @throw std::runtime_error if lock could not be acquired.
  explicit StrictLockPtr(SynchronizedValue<T, LockType>& sync) : ConstStrictLockPtr<T, LockType>(sync) {}

  /// @brief Adopt the given SynchronizedValue's lock, which the calling thread must already hold.
  StrictLockPtr(SynchronizedValue<T, LockType>& sync, AdoptLockTag /*unused*/) noexcept
      : ConstStrictLockPtr<T, LockType>(sync, AdoptLockTag{})
  {
  }

  /// @brief Take over the lock held by @p other, which is left empty.
  /// @throw std::runtime_error if @p other does not own the lock.
  explicit StrictLockPtr(UniqueLockPtr<T, LockType>&& other) : ConstStrictLockPtr<T, LockType>(std::move(other)) {}

  using ConstStrictLockPtr<T, LockType>::operator*;
  using ConstStrictLockPtr<T, LockType>::operator->;

  // The casts below are sound: mutable handles can only be constructed from a non-const SynchronizedValue.
  /// @brief Get a mutable reference to the guarded value. Undefined behavior if this handle has been moved from.
  T& operator*() noexcept
  {
    return const_cast<T&>(this->sync_->value_);  // NOLINT(cppcoreguidelines-pro-type-const-cast)
  }
  /// @brief Access a member of the guarded value. Undefined behavior if this handle has been moved from.
  T* operator->() noexcept
  {
    return const_cast<T*>(std::addressof(this->sync_->value_));  // NOLINT(cppcoreguidelines-pro-type-const-cast)
  }
};

/// @ingroup etcpal_cpp_synchronized_value
/// @brief An RAII handle for read-only access whose lock may or may not be held.
///
/// Returned by SynchronizedValue::TryToSynchronize() and the *Synchronize() factories, or constructed
/// directly in blocking, adopting, deferred, or try-to-lock mode. Acquisition can fail (or be deferred), so
/// always check OwnsLock() (or `operator bool`) before dereferencing; accessing the value while the handle
/// does not own the lock is undefined behavior. The lock, if owned, is released on destruction or by
/// Unlock(). Move-only.
/// @tparam T        The type of the guarded value.
/// @tparam LockType The type of lock guarding the value.
template <typename T, typename LockType>
class ConstUniqueLockPtr
{
public:
  /// @brief Lock the given SynchronizedValue for read-only access, blocking until the lock is acquired.
  /// @throw std::runtime_error if lock could not be acquired.
  explicit ConstUniqueLockPtr(const SynchronizedValue<T, LockType>& sync) : sync_(&sync), owns_(false) { Lock(); }

  /// @brief Adopt the given SynchronizedValue's lock, which the calling thread must already hold.
  ConstUniqueLockPtr(const SynchronizedValue<T, LockType>& sync, AdoptLockTag /*unused*/) noexcept
      : sync_(&sync), owns_(true)
  {
  }

  /// @brief Reference the given SynchronizedValue without acquiring the lock.
  ConstUniqueLockPtr(const SynchronizedValue<T, LockType>& sync, DeferLockTag /*unused*/) noexcept
      : sync_(&sync), owns_(false)
  {
  }

  /// @brief Try to lock the given SynchronizedValue, waiting up to @p timeout_ms; check OwnsLock() for the
  ///        result. @p timeout_ms has the same semantics as TryLock().
  ConstUniqueLockPtr(const SynchronizedValue<T, LockType>& sync, TryToLockTag /*unused*/, int timeout_ms = 0) noexcept
      : sync_(&sync), owns_(sync.TryLockFor(timeout_ms))
  {
  }

  ConstUniqueLockPtr(const ConstUniqueLockPtr&) = delete;
  ConstUniqueLockPtr& operator=(const ConstUniqueLockPtr&) = delete;
  ConstUniqueLockPtr& operator=(ConstUniqueLockPtr&&) = delete;

  /// @brief Move ownership from another handle, which is left empty.
  ConstUniqueLockPtr(ConstUniqueLockPtr&& other) noexcept : sync_(other.sync_), owns_(other.owns_)
  {
    other.sync_ = nullptr;
    other.owns_ = false;
  }

  /// @brief Release the lock if owned.
  ~ConstUniqueLockPtr() noexcept { Unlock(); }

  /// @brief Whether this handle currently owns the lock.
  explicit operator bool() const noexcept { return owns_; }
  /// @brief Whether this handle currently owns the lock.
  bool OwnsLock() const noexcept { return owns_; }

  /// @brief Release the lock early, if owned. Does nothing otherwise.
  void Unlock() noexcept
  {
    if (owns_)
    {
      sync_->lock_.Unlock();
      owns_ = false;
    }
  }

  /// @brief Acquire the lock, blocking until it is available.
  /// @throw std::runtime_error if this handle already owns the lock, no longer references one, or locking failed.
  void Lock()
  {
    if (!sync_)
      ETCPAL_THROW(std::runtime_error("etcpal lock handle does not reference a lock."));
    if (owns_)
      ETCPAL_THROW(std::runtime_error("etcpal lock handle already owns the lock."));
    if (!sync_->lock_.Lock())
      ETCPAL_THROW(std::runtime_error("etcpal SynchronizedValue lock failed."));
    owns_ = true;
  }

  /// @brief Try to acquire the lock, waiting up to @p timeout_ms.
  ///
  /// @p timeout_ms follows EtcPal timeout semantics, with the same platform behavior as
  /// SynchronizedValue::TryToSynchronize(): where the lock supports timed locking (see SupportsTimedLock)
  /// it is passed straight to the underlying etcpal_*_timed_lock() call, so a negative value
  /// (ETCPAL_WAIT_FOREVER) blocks until the lock is acquired; elsewhere this always polls.
  /// @param timeout_ms How long to wait for the lock, in milliseconds. Defaults to 0 (poll, return immediately).
  /// @return Whether the lock was acquired.
  /// @throw std::runtime_error if this handle already owns the lock or no longer references one.
  bool TryLock(int timeout_ms = 0)
  {
    if (!sync_)
      ETCPAL_THROW(std::runtime_error("etcpal lock handle does not reference a lock."));
    if (owns_)
      ETCPAL_THROW(std::runtime_error("etcpal lock handle already owns the lock."));
    owns_ = sync_->TryLockFor(timeout_ms);
    return owns_;
  }

  /// @brief Disassociate from the lock without unlocking it; the caller becomes responsible for the lock.
  void Release() noexcept
  {
    sync_ = nullptr;
    owns_ = false;
  }

  /// @brief Get a reference to the guarded value.
  /// @throw std::runtime_error if this handle does not own the lock.
  const T& operator*() const
  {
    if (!owns_)
      ETCPAL_THROW(std::runtime_error("etcpal lock handle does not own the lock."));
    return sync_->value_;
  }
  /// @brief Access a member of the guarded value.
  /// @throw std::runtime_error if this handle does not own the lock.
  const T* operator->() const
  {
    if (!owns_)
      ETCPAL_THROW(std::runtime_error("etcpal lock handle does not own the lock."));
    return std::addressof(sync_->value_);
  }

protected:
  // NOLINT below: the derived mutable handle and the strict-pointer conversion need direct access.
  const SynchronizedValue<T, LockType>* sync_;  // NOLINT(cppcoreguidelines-non-private-member-variables-in-classes)
  bool                                  owns_;  // NOLINT(cppcoreguidelines-non-private-member-variables-in-classes)

  friend class ConstStrictLockPtr<T, LockType>;
};

/// @ingroup etcpal_cpp_synchronized_value
/// @brief An RAII handle for mutable access whose lock may or may not be held.
///
/// Like ConstUniqueLockPtr, but hands out mutable access. Returned by SynchronizedValue::TryToSynchronize()
/// and the *Synchronize() factories, or constructed directly. Move-only.
/// @tparam T        The type of the guarded value.
/// @tparam LockType The type of lock guarding the value.
template <typename T, typename LockType>
class UniqueLockPtr : public ConstUniqueLockPtr<T, LockType>
{
public:
  /// @brief Lock the given SynchronizedValue for mutable access, blocking until the lock is acquired.
  /// @throw std::runtime_error if lock could not be acquired.
  explicit UniqueLockPtr(SynchronizedValue<T, LockType>& sync) : ConstUniqueLockPtr<T, LockType>(sync) {}

  /// @brief Adopt the given SynchronizedValue's lock, which the calling thread must already hold.
  UniqueLockPtr(SynchronizedValue<T, LockType>& sync, AdoptLockTag /*unused*/) noexcept
      : ConstUniqueLockPtr<T, LockType>(sync, AdoptLockTag{})
  {
  }

  /// @brief Reference the given SynchronizedValue without acquiring the lock.
  UniqueLockPtr(SynchronizedValue<T, LockType>& sync, DeferLockTag /*unused*/) noexcept
      : ConstUniqueLockPtr<T, LockType>(sync, DeferLockTag{})
  {
  }

  /// @brief Try to lock the given SynchronizedValue, waiting up to @p timeout_ms; check OwnsLock() for the
  ///        result. @p timeout_ms has the same semantics as TryLock().
  UniqueLockPtr(SynchronizedValue<T, LockType>& sync, TryToLockTag /*unused*/, int timeout_ms = 0) noexcept
      : ConstUniqueLockPtr<T, LockType>(sync, TryToLockTag{}, timeout_ms)
  {
  }

  using ConstUniqueLockPtr<T, LockType>::operator*;
  using ConstUniqueLockPtr<T, LockType>::operator->;

  // The casts below are sound: mutable handles can only be constructed from a non-const SynchronizedValue.
  /// @brief Get a mutable reference to the guarded value.
  /// @throw std::runtime_error if this handle does not own the lock.
  T& operator*()
  {
    if (!this->owns_)
      ETCPAL_THROW(std::runtime_error("etcpal lock handle does not own the lock."));
    return const_cast<T&>(this->sync_->value_);  // NOLINT(cppcoreguidelines-pro-type-const-cast)
  }
  /// @brief Access a member of the guarded value.
  /// @throw std::runtime_error if this handle does not own the lock.
  T* operator->()
  {
    if (!this->owns_)
      ETCPAL_THROW(std::runtime_error("etcpal lock handle does not own the lock."));
    return const_cast<T*>(std::addressof(this->sync_->value_));  // NOLINT(cppcoreguidelines-pro-type-const-cast)
  }
};

/// @ingroup etcpal_cpp_synchronized_value
/// @brief A value bundled with a lock that guards all access to it.
///
/// The wrapped value can only be reached through an RAII handle that holds the lock, so it cannot be
/// accessed without synchronization. See the @ref etcpal_cpp_synchronized_value module overview for an
/// example.
/// @tparam T        The type of the guarded value.
/// @tparam LockType The type of lock guarding the value. Must provide `bool Lock()`,
///                  `bool TryLock(int timeout_ms)`, and `Unlock()` (e.g. etcpal::Mutex or
///                  etcpal::RecursiveMutex; defaults to etcpal::Mutex). `TryLock()` and `Unlock()` are
///                  called from noexcept contexts and must not throw; report failure by return value.
template <typename T, typename LockType>
class SynchronizedValue
{
public:
  /// @brief Construct the guarded value in place, forwarding the given arguments to its constructor.
  ///
  /// Excluded from overload resolution when T is not constructible from the arguments (so
  /// std::is_constructible reports accurately) and for a single SynchronizedValue argument, so that
  /// copy/move attempts select the implicitly-deleted copy constructor (and get a clear diagnostic)
  /// instead of trying to construct T from a SynchronizedValue.
  template <typename... Args,
            typename = typename std::enable_if<
                std::is_constructible<T, Args&&...>::value &&
                !std::is_same<typename std::decay<typename detail::FirstType<Args...>::type>::type,
                              SynchronizedValue>::value>::type>
  // The noexcept clause assumes LockType's default constructor cannot throw, which holds for the EtcPal
  // lock types but is not checked (they do not declare it noexcept, so a trait check would always be false).
  explicit SynchronizedValue(Args&&... args) noexcept(std::is_nothrow_constructible<T, Args...>::value)
      : value_(std::forward<Args>(args)...)
  {
  }

  /// @name Unsafe Access
  /// @brief Access the value or lock directly, without locking. Use with care.
  /// @{
  T&        GetValueUnsafe() noexcept { return value_; }
  LockType& GetLockUnsafe() noexcept { return lock_; }
  /// @}

  /// @name Locked Access
  /// @brief Obtain an RAII handle that holds the lock for as long as the handle exists.
  /// @{

  /// @brief Lock the value and access a member of it for the duration of the enclosing expression.
  /// @throw std::runtime_error if lock could not be acquired.
  StrictLockPtr<T, LockType> operator->() { return StrictLockPtr<T, LockType>(*this); }
  /// @brief Lock the value and access a member of it for the duration of the enclosing expression.
  /// @throw std::runtime_error if lock could not be acquired.
  ConstStrictLockPtr<T, LockType> operator->() const { return ConstStrictLockPtr<T, LockType>(*this); }

  /// @brief Lock the value, returning a handle that holds the lock until it is destroyed.
  /// @throw std::runtime_error if lock could not be acquired.
  StrictLockPtr<T, LockType> Synchronize() { return StrictLockPtr<T, LockType>(*this); }
  /// @brief Lock the value, returning a handle that holds the lock until it is destroyed.
  /// @throw std::runtime_error if lock could not be acquired.
  ConstStrictLockPtr<T, LockType> Synchronize() const { return ConstStrictLockPtr<T, LockType>(*this); }
  /// @}

  /// @name Non-blocking Locked Access
  /// @brief Try to acquire the lock, waiting up to @p timeout_ms. Check OwnsLock() (or `operator bool`)
  ///        on the result.
  ///
  /// @p timeout_ms follows EtcPal timeout semantics. Where the lock supports timed locking (see
  /// SupportsTimedLock) it is passed straight to the underlying etcpal_*_timed_lock() call: 0 polls, a
  /// positive value waits up to that long, and a negative value (ETCPAL_WAIT_FOREVER) blocks until the
  /// lock is acquired. On platforms without timed-lock support, etcpal_*_timed_lock() ignores the timeout
  /// and blocks indefinitely for any nonzero value, so this always polls there instead.
  /// @param timeout_ms How long to wait for the lock, in milliseconds. Defaults to 0 (poll, return immediately).
  /// @{
  UniqueLockPtr<T, LockType> TryToSynchronize(int timeout_ms = 0) noexcept
  {
    return UniqueLockPtr<T, LockType>(*this, TryToLockTag{}, timeout_ms);
  }
  ConstUniqueLockPtr<T, LockType> TryToSynchronize(int timeout_ms = 0) const noexcept
  {
    return ConstUniqueLockPtr<T, LockType>(*this, TryToLockTag{}, timeout_ms);
  }
  /// @}

  /// @name Unique Locked Access
  /// @brief Obtain a UniqueLockPtr handle in a specific acquisition mode, mirroring Boost's
  ///        unique_synchronize()/defer_synchronize()/adopt_synchronize().
  /// @{

  /// @brief Lock the value, returning a handle that can also Unlock(), re-Lock(), or Release() it.
  /// @throw std::runtime_error if lock could not be acquired.
  UniqueLockPtr<T, LockType>      UniqueSynchronize() { return UniqueLockPtr<T, LockType>(*this); }
  ConstUniqueLockPtr<T, LockType> UniqueSynchronize() const { return ConstUniqueLockPtr<T, LockType>(*this); }

  /// @brief Reference the value without locking; acquire later with Lock() or TryLock() on the handle.
  UniqueLockPtr<T, LockType> DeferSynchronize() noexcept { return UniqueLockPtr<T, LockType>(*this, DeferLockTag{}); }
  ConstUniqueLockPtr<T, LockType> DeferSynchronize() const noexcept
  {
    return ConstUniqueLockPtr<T, LockType>(*this, DeferLockTag{});
  }

  /// @brief Adopt the lock, which the calling thread must already hold, into a handle that will release it.
  UniqueLockPtr<T, LockType> AdoptSynchronize() noexcept { return UniqueLockPtr<T, LockType>(*this, AdoptLockTag{}); }
  ConstUniqueLockPtr<T, LockType> AdoptSynchronize() const noexcept
  {
    return ConstUniqueLockPtr<T, LockType>(*this, AdoptLockTag{});
  }
  /// @}

  /// @name Visitation
  /// @brief Invoke a callable with the guarded value held under lock for the call's duration.
  ///
  /// @p func must not return a reference (enforced at compile time): the lock is released when Apply
  /// returns, so a returned reference into the guarded value would escape it unguarded.
  /// @param func A callable taking a reference to the guarded value.
  /// @return Whatever @p func returns.
  /// @throw std::runtime_error if lock could not be acquired.
  /// @{
  template <typename F>
  decltype(auto) Apply(F&& func)
  {
    static_assert(!std::is_reference<decltype(std::forward<F>(func)(std::declval<T&>()))>::value,
                  "Apply must not return a reference; it would outlive the lock");
    auto lock = Synchronize();
    return std::forward<F>(func)(*lock);
  }
  template <typename F>
  decltype(auto) Apply(F&& func) const
  {
    static_assert(!std::is_reference<decltype(std::forward<F>(func)(std::declval<const T&>()))>::value,
                  "Apply must not return a reference; it would outlive the lock");
    auto lock = Synchronize();
    return std::forward<F>(func)(*lock);
  }
  /// @}

private:
  friend class ConstStrictLockPtr<T, LockType>;
  friend class StrictLockPtr<T, LockType>;
  friend class ConstUniqueLockPtr<T, LockType>;
  friend class UniqueLockPtr<T, LockType>;

  // Honor the timeout only where the platform supports timed locking; otherwise poll, so an unsupported
  // platform never silently blocks forever (etcpal_*_timed_lock() with a nonzero timeout is Lock() there).
  bool TryLockFor(int timeout_ms) const noexcept
  {
    return SupportsTimedLock<LockType>::value ? lock_.TryLock(timeout_ms) : lock_.TryLock(0);
  }

  T                value_;
  mutable LockType lock_;
};

}  // namespace etcpal

#endif  // ETCPAL_CPP_SYNCHRONIZED_VALUE_H_
