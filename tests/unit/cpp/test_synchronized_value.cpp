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

#include "etcpal/cpp/synchronized_value.h"

#include <cstddef>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include "etcpal/common.h"
#include "etcpal/cpp/mutex.h"
#include "etcpal/cpp/recursive_mutex.h"
#include "etcpal/cpp/signal.h"
#include "etcpal/cpp/thread.h"
#include "etcpal/cpp/timer.h"
#include "unity_fixture.h"

// SupportsTimedLock is a compile-time trait, so its contract is checked here with static_assert rather than in a
// runtime test. The EtcPal lock types must track their platform capability macros, and an unspecialized type
// must default to no timed-lock support. (The trait is also exercised at runtime by every TryToSynchronize call,
// which routes through it.)
static_assert(etcpal::SupportsTimedLock<etcpal::Mutex>::value == (ETCPAL_MUTEX_HAS_TIMED_LOCK != 0),
              "Mutex timed-lock trait must track ETCPAL_MUTEX_HAS_TIMED_LOCK");
#if !DISABLE_RECURSIVE_MUTEX_TESTS
static_assert(etcpal::SupportsTimedLock<etcpal::RecursiveMutex>::value == (ETCPAL_RECURSIVE_MUTEX_HAS_TIMED_LOCK != 0),
              "RecursiveMutex timed-lock trait must track ETCPAL_RECURSIVE_MUTEX_HAS_TIMED_LOCK");
#endif
static_assert(!etcpal::SupportsTimedLock<int>::value, "an unspecialized lock type defaults to no timed support");

// The in-place constructor is excluded from overload resolution for a single SynchronizedValue argument, so
// copy/move attempts resolve to the implicitly-deleted special members (deleted via the lock member) and
// these traits report correctly instead of the forwarding template hijacking them.
static_assert(!std::is_copy_constructible<etcpal::SynchronizedValue<int>>::value,
              "SynchronizedValue must not be copyable");
static_assert(!std::is_move_constructible<etcpal::SynchronizedValue<int>>::value,
              "SynchronizedValue must not be movable");
static_assert(!std::is_copy_assignable<etcpal::SynchronizedValue<int>>::value,
              "SynchronizedValue must not be copy-assignable");
static_assert(!std::is_move_assignable<etcpal::SynchronizedValue<int>>::value,
              "SynchronizedValue must not be move-assignable");
// The constraint must not break in-place construction (zero, one, or many forwarded arguments).
static_assert(std::is_constructible<etcpal::SynchronizedValue<std::string>>::value, "in-place default construction");
static_assert(std::is_constructible<etcpal::SynchronizedValue<std::string>, const char*>::value,
              "in-place single-argument construction");
static_assert(std::is_constructible<etcpal::SynchronizedValue<std::string>, std::size_t, char>::value,
              "in-place multi-argument construction");
// The ctor is also constrained on T's constructibility, so the trait cannot report constructible for
// argument lists that would hard-error in the member initializer (relevant to metaprogramming consumers).
static_assert(!std::is_constructible<etcpal::SynchronizedValue<int>, const char*>::value,
              "constructibility must mirror T's constructibility");

// The lock handles are move-only: factory returns rely on the implicitly-generated move constructors under
// C++14 (no guaranteed elision), so pin their existence (and noexcept-ness) here.
template <typename Handle>
constexpr bool IsMoveOnlyHandle()
{
  return std::is_nothrow_move_constructible<Handle>::value && !std::is_copy_constructible<Handle>::value &&
         !std::is_copy_assignable<Handle>::value && !std::is_move_assignable<Handle>::value;
}
static_assert(IsMoveOnlyHandle<etcpal::ConstStrictLockPtr<int>>(), "ConstStrictLockPtr must be move-only");
static_assert(IsMoveOnlyHandle<etcpal::StrictLockPtr<int>>(), "StrictLockPtr must be move-only");
static_assert(IsMoveOnlyHandle<etcpal::ConstUniqueLockPtr<int>>(), "ConstUniqueLockPtr must be move-only");
static_assert(IsMoveOnlyHandle<etcpal::UniqueLockPtr<int>>(), "UniqueLockPtr must be move-only");

// Templated test bodies live outside the extern "C" block below: templates cannot have C language linkage.
// They run on the main test thread (called from the TEST cases), so using TEST_ASSERT_* in them is fine.
// Worker threads, by contrast, never call TEST_ASSERT_* (Unity longjmps on failure, which is UB off the main
// thread); they only touch the SynchronizedValue under test (through its lock) and report via Join().
namespace
{
// A same-thread try-lock cannot verify that a lock was released: with a recursive mutex it succeeds even
// while the lock is still held. A try-lock from another thread fails iff the lock is held, for both types.
template <typename T, typename LockType>
bool LockIsFree(const etcpal::SynchronizedValue<T, LockType>& v)
{
  bool           acquired = false;
  etcpal::Thread checker([&]() { acquired = v.TryToSynchronize().OwnsLock(); });
  return checker.Join().IsOk() && acquired;
}

// ---- single-threaded API coverage (run under both Mutex and RecursiveMutex) -------------------------------

template <typename LockType>
void ConstructInPlaceImpl()
{
  etcpal::SynchronizedValue<std::string, LockType> s("hi");
  TEST_ASSERT_EQUAL_STRING("hi", s.GetValueUnsafe().c_str());

  etcpal::SynchronizedValue<std::vector<int>, LockType> v(3, 7);
  TEST_ASSERT_EQUAL_INT(3, static_cast<int>(v.GetValueUnsafe().size()));
  for (int elem : v.GetValueUnsafe())
    TEST_ASSERT_EQUAL_INT(7, elem);
}

template <typename LockType>
void DefaultConstructImpl()
{
  etcpal::SynchronizedValue<int, LockType> v;
  TEST_ASSERT_EQUAL_INT(0, v.GetValueUnsafe());
}

template <typename LockType>
void GetValueUnsafeImpl()
{
  etcpal::SynchronizedValue<int, LockType> v(42);
  static_assert(std::is_same<decltype(v.GetValueUnsafe()), int&>::value, "GetValueUnsafe yields T&");
  TEST_ASSERT_EQUAL_INT(42, v.GetValueUnsafe());
}

template <typename LockType>
void GetLockUnsafeImpl()
{
  etcpal::SynchronizedValue<int, LockType> v(0);
  static_assert(std::is_same<decltype(v.GetLockUnsafe()), LockType&>::value, "GetLockUnsafe yields Lock&");
  // Primarily compile coverage: the returned reference must be usable as a lock.
  TEST_ASSERT_TRUE(v.GetLockUnsafe().Lock());
  v.GetLockUnsafe().Unlock();
}

template <typename LockType>
void ArrowOperatorMutableImpl()
{
  etcpal::SynchronizedValue<std::vector<int>, LockType> v;
  // The lock is taken and released for each statement; the second push_back would deadlock a plain Mutex if the
  // first did not release, so its success proves transient release.
  v->push_back(5);
  v->push_back(6);
  TEST_ASSERT_EQUAL_INT(2, static_cast<int>(v.GetValueUnsafe().size()));
  TEST_ASSERT_EQUAL_INT(5, v.GetValueUnsafe()[0]);
  TEST_ASSERT_EQUAL_INT(6, v.GetValueUnsafe()[1]);
}

template <typename LockType>
void ArrowOperatorConstImpl()
{
  const etcpal::SynchronizedValue<std::string, LockType> v("abc");
  TEST_ASSERT_EQUAL_INT(3, static_cast<int>(v->size()));
}

template <typename LockType>
void SynchronizeScopeMutableImpl()
{
  etcpal::SynchronizedValue<std::string, LockType> v("a");
  {
    auto handle = v.Synchronize();
    *handle     = "hello";
    TEST_ASSERT_EQUAL_INT(5, static_cast<int>(handle->size()));
  }
  TEST_ASSERT_EQUAL_STRING("hello", v.GetValueUnsafe().c_str());
}

template <typename LockType>
void SynchronizeConstYieldsConstRefImpl()
{
  const etcpal::SynchronizedValue<int, LockType> v(9);
  auto                                           handle = v.Synchronize();
  static_assert(std::is_same<decltype(*handle), const int&>::value, "const Synchronize yields const T&");
  TEST_ASSERT_EQUAL_INT(9, *handle);
}

template <typename LockType>
void StrictLockReleasesOnDestructionImpl()
{
  etcpal::SynchronizedValue<int, LockType> v(0);
  {
    auto handle = v.Synchronize();
    *handle     = 1;
  }
  // The strict handle released the lock on scope exit.
  TEST_ASSERT_TRUE(LockIsFree(v));
  TEST_ASSERT_EQUAL_INT(1, v.GetValueUnsafe());
}

template <typename LockType>
void StrictLockMoveTransfersOwnershipImpl()
{
  etcpal::SynchronizedValue<int, LockType> v(0);
  {
    auto a = v.Synchronize();
    auto b = std::move(a);  // moved-from a must not unlock; b owns the lock
    *b     = 7;
  }
  // The lock ended up free: released by b on scope exit, and moved-from a must not unlock.
  TEST_ASSERT_TRUE(LockIsFree(v));
  TEST_ASSERT_EQUAL_INT(7, v.GetValueUnsafe());
}

template <typename LockType>
void TryToSynchronizeSucceedsWhenFreeImpl()
{
  etcpal::SynchronizedValue<int, LockType> v(1);
  {
    auto handle = v.TryToSynchronize();
    TEST_ASSERT_TRUE(static_cast<bool>(handle));
    TEST_ASSERT_TRUE(handle.OwnsLock());
    *handle = 2;
  }
  TEST_ASSERT_EQUAL_INT(2, v.GetValueUnsafe());
}

template <typename LockType>
void TryToSynchronizeConstImpl()
{
  const etcpal::SynchronizedValue<int, LockType> v(5);
  auto                                           handle = v.TryToSynchronize();
  static_assert(std::is_same<decltype(*handle), const int&>::value, "const TryToSynchronize yields const T&");
  TEST_ASSERT_TRUE(handle.OwnsLock());
  TEST_ASSERT_EQUAL_INT(5, *handle);
}

template <typename LockType>
void UniqueLockUnlockIdempotentImpl()
{
  etcpal::SynchronizedValue<int, LockType> v(0);
  auto                                     handle = v.TryToSynchronize();
  TEST_ASSERT_TRUE(handle.OwnsLock());
  handle.Unlock();
  TEST_ASSERT_FALSE(handle.OwnsLock());
  handle.Unlock();  // second call is a no-op, not a double-unlock
  TEST_ASSERT_FALSE(handle.OwnsLock());
  // Lock is free again.
  TEST_ASSERT_TRUE(LockIsFree(v));
}

template <typename LockType>
void UniqueLockMoveLeavesSourceNonOwningImpl()
{
  etcpal::SynchronizedValue<int, LockType> v(0);
  {
    auto a = v.TryToSynchronize();
    TEST_ASSERT_TRUE(a.OwnsLock());
    auto b = std::move(a);
    // NOLINTNEXTLINE(clang-analyzer-cplusplus.Move) - moved-from inspection is the behavior under test
    TEST_ASSERT_FALSE(a.OwnsLock());
    TEST_ASSERT_TRUE(b.OwnsLock());
  }
  // The lock ended up free, released by b only.
  TEST_ASSERT_TRUE(LockIsFree(v));
}

template <typename LockType>
void StrictAdoptAfterReleaseImpl()
{
  etcpal::SynchronizedValue<int, LockType> v(0);
  {
    auto unique = v.TryToSynchronize();
    TEST_ASSERT_TRUE(unique.OwnsLock());
    unique.Release();  // the mutex stays locked; the handle just no longer manages it
    TEST_ASSERT_FALSE(unique.OwnsLock());

    etcpal::StrictLockPtr<int, LockType> strict(v, etcpal::AdoptLockTag{});
    *strict = 5;
  }  // strict releases the adopted lock here
  TEST_ASSERT_TRUE(LockIsFree(v));
  TEST_ASSERT_EQUAL_INT(5, v.GetValueUnsafe());
}

template <typename LockType>
void StrictFromUniqueConversionImpl()
{
  etcpal::SynchronizedValue<int, LockType> v(1);
  {
    auto unique = v.TryToSynchronize();
    TEST_ASSERT_TRUE(unique.OwnsLock());

    etcpal::StrictLockPtr<int, LockType> strict(std::move(unique));
    TEST_ASSERT_FALSE(unique.OwnsLock());  // consumed; the strict handle owns the lock now
    *strict = 2;
  }
  // The lock ended up free, released by the strict handle only (the consumed unique handle must not unlock).
  TEST_ASSERT_TRUE(LockIsFree(v));
  TEST_ASSERT_EQUAL_INT(2, v.GetValueUnsafe());
}

template <typename LockType>
void DeferSynchronizeThenLockImpl()
{
  etcpal::SynchronizedValue<int, LockType> v(3);
  {
    auto handle = v.DeferSynchronize();
    TEST_ASSERT_FALSE(handle.OwnsLock());
    handle.Lock();
    TEST_ASSERT_TRUE(handle.OwnsLock());
    *handle = 4;
  }
  TEST_ASSERT_TRUE(LockIsFree(v));
  TEST_ASSERT_EQUAL_INT(4, v.GetValueUnsafe());
}

template <typename LockType>
void UniqueLockRelockCycleImpl()
{
  etcpal::SynchronizedValue<int, LockType> v(0);

  auto handle = v.DeferSynchronize();
  TEST_ASSERT_TRUE(handle.TryLock());
  *handle = 1;
  handle.Unlock();
  handle.Lock();
  *handle += 1;
  handle.Unlock();
  TEST_ASSERT_TRUE(handle.TryLock(10));  // timed try on a free lock succeeds on all platforms
  *handle += 1;
  handle.Unlock();
  TEST_ASSERT_EQUAL_INT(3, v.GetValueUnsafe());
}

template <typename LockType>
void UniqueSynchronizeImpl()
{
  etcpal::SynchronizedValue<int, LockType> v(7);
  {
    auto handle = v.UniqueSynchronize();
    TEST_ASSERT_TRUE(handle.OwnsLock());
    *handle = 8;
  }

  const auto& cv      = v;
  auto        chandle = cv.UniqueSynchronize();
  static_assert(std::is_same<decltype(*chandle), const int&>::value, "const UniqueSynchronize yields const T&");
  TEST_ASSERT_TRUE(chandle.OwnsLock());
  TEST_ASSERT_EQUAL_INT(8, *chandle);
}

template <typename LockType>
void AdoptSynchronizeImpl()
{
  etcpal::SynchronizedValue<int, LockType> v(0);
  {
    v.UniqueSynchronize().Release();  // leave the mutex locked with no managing handle
    auto adopted = v.AdoptSynchronize();
    TEST_ASSERT_TRUE(adopted.OwnsLock());
    *adopted = 9;
  }
  TEST_ASSERT_TRUE(LockIsFree(v));
  TEST_ASSERT_EQUAL_INT(9, v.GetValueUnsafe());
}

template <typename LockType>
void TryToLockTagCtorImpl()
{
  etcpal::SynchronizedValue<int, LockType> v(6);

  etcpal::UniqueLockPtr<int, LockType> handle(v, etcpal::TryToLockTag{});
  TEST_ASSERT_TRUE(handle.OwnsLock());
  TEST_ASSERT_EQUAL_INT(6, *handle);
}

template <typename LockType>
void UniqueLockArrowImpl()
{
  etcpal::SynchronizedValue<std::string, LockType> v("ab");
  {
    auto handle = v.TryToSynchronize();
    TEST_ASSERT_TRUE(handle.OwnsLock());
    handle->push_back('c');  // mutable operator->
    TEST_ASSERT_EQUAL_STRING("abc", handle->c_str());
  }

  const etcpal::SynchronizedValue<std::string, LockType> cv("xy");
  auto                                                   chandle = cv.TryToSynchronize();
  TEST_ASSERT_TRUE(chandle.OwnsLock());
  TEST_ASSERT_EQUAL_INT(2, static_cast<int>(chandle->size()));  // const operator->
}

template <typename LockType>
void ConstUniqueHandlesImpl()
{
  const etcpal::SynchronizedValue<int, LockType> v(11);

  {
    auto deferred = v.DeferSynchronize();
    TEST_ASSERT_FALSE(deferred.OwnsLock());
    deferred.Lock();
    TEST_ASSERT_TRUE(deferred.OwnsLock());
    TEST_ASSERT_EQUAL_INT(11, *deferred);
  }

  etcpal::ConstUniqueLockPtr<int, LockType> tried(v, etcpal::TryToLockTag{});
  TEST_ASSERT_TRUE(tried.OwnsLock());
  tried.Unlock();

  v.UniqueSynchronize().Release();  // leave the mutex locked with no managing handle
  {
    auto adopted = v.AdoptSynchronize();
    TEST_ASSERT_TRUE(adopted.OwnsLock());
    TEST_ASSERT_EQUAL_INT(11, *adopted);
  }
  TEST_ASSERT_TRUE(LockIsFree(v));
}

template <typename LockType>
void ConstStrictFromConstUniqueConversionImpl()
{
  const etcpal::SynchronizedValue<int, LockType> v(12);
  {
    auto unique = v.TryToSynchronize();
    TEST_ASSERT_TRUE(unique.OwnsLock());

    etcpal::ConstStrictLockPtr<int, LockType> strict(std::move(unique));
    TEST_ASSERT_FALSE(unique.OwnsLock());  // consumed; the strict handle owns the lock now
    static_assert(std::is_same<decltype(*strict), const int&>::value, "ConstStrictLockPtr yields const T&");
    TEST_ASSERT_EQUAL_INT(12, *strict);
  }
  // The lock ended up free, released by the strict handle only (the consumed unique handle must not unlock).
  TEST_ASSERT_TRUE(LockIsFree(v));
}

template <typename LockType>
void TimedTrySucceedsWhenFreeImpl()
{
  etcpal::SynchronizedValue<int, LockType> v(13);
  // A free lock must be acquired immediately on both timed and polling platforms.
  auto t = v.TryToSynchronize(50);
  TEST_ASSERT_TRUE(t.OwnsLock());
  TEST_ASSERT_EQUAL_INT(13, *t);
}

// Mutex-only: a same-thread try-lock fails iff the mutex is still locked, which would not hold for a
// recursive mutex (the same thread could just re-enter it).
void ReleaseLeavesMutexLockedImpl()
{
  etcpal::SynchronizedValue<int, etcpal::Mutex> v(0);

  auto handle = v.UniqueSynchronize();
  handle.Release();
  TEST_ASSERT_FALSE(v.TryToSynchronize().OwnsLock());  // still locked after Release()
  v.AdoptSynchronize();                                // adopt and immediately unlock via the temporary
  TEST_ASSERT_TRUE(v.TryToSynchronize().OwnsLock());
}

#if ETCPAL_BUILDING_WITH_EXCEPTIONS
template <typename LockType>
void LockMisuseThrowsImpl()
{
  etcpal::SynchronizedValue<int, LockType> v(0);

  auto handle = v.UniqueSynchronize();
  try
  {
    handle.Lock();  // already owns the lock
    TEST_FAIL_MESSAGE("This block should not be entered");
  }
  catch (const std::runtime_error&)
  {
  }

  handle.Unlock();
  handle.Release();
  try
  {
    handle.TryLock();  // released; no lock referenced anymore
    TEST_FAIL_MESSAGE("This block should not be entered");
  }
  catch (const std::runtime_error&)
  {
  }
}

template <typename LockType>
void MovedFromUniqueLockThrowsImpl()
{
  etcpal::SynchronizedValue<int, LockType> v(0);

  auto source = v.UniqueSynchronize();
  auto dest   = std::move(source);
  try
  {
    // NOLINTNEXTLINE(clang-analyzer-cplusplus.Move) - moved-from misuse is the behavior under test
    source.Lock();
    TEST_FAIL_MESSAGE("This block should not be entered");
  }
  catch (const std::runtime_error&)
  {
  }
  try
  {
    source.TryLock();
    TEST_FAIL_MESSAGE("This block should not be entered");
  }
  catch (const std::runtime_error&)
  {
  }
  try
  {
    *source = 1;  // moved-from source no longer references a value; must throw, not dereference null
    TEST_FAIL_MESSAGE("This block should not be entered");
  }
  catch (const std::runtime_error&)
  {
  }
  TEST_ASSERT_TRUE(dest.OwnsLock());  // the failed calls must not have touched the destination's lock
}

template <typename LockType>
void LockAfterReleaseThrowsImpl()
{
  etcpal::SynchronizedValue<int, LockType> v(0);

  auto handle = v.UniqueSynchronize();
  handle.Unlock();
  handle.Release();
  try
  {
    handle.Lock();
    TEST_FAIL_MESSAGE("This block should not be entered");
  }
  catch (const std::runtime_error&)
  {
  }
}

template <typename LockType>
void NonOwningDereferenceThrowsImpl()
{
  etcpal::SynchronizedValue<std::string, LockType> v("abc");

  auto handle = v.DeferSynchronize();  // mutable handle that does not own the lock
  try
  {
    *handle = "x";
    TEST_FAIL_MESSAGE("This block should not be entered");
  }
  catch (const std::runtime_error&)
  {
  }
  try
  {
    handle->clear();
    TEST_FAIL_MESSAGE("This block should not be entered");
  }
  catch (const std::runtime_error&)
  {
  }

  const auto& cv      = v;
  auto        chandle = cv.DeferSynchronize();  // const handle that does not own the lock
  try
  {
    (void)*chandle;
    TEST_FAIL_MESSAGE("This block should not be entered");
  }
  catch (const std::runtime_error&)
  {
  }
  try
  {
    (void)chandle->size();
    TEST_FAIL_MESSAGE("This block should not be entered");
  }
  catch (const std::runtime_error&)
  {
  }

  // The failed dereferences must not have locked anything or touched the value.
  TEST_ASSERT_TRUE(LockIsFree(v));
  TEST_ASSERT_EQUAL_STRING("abc", v.GetValueUnsafe().c_str());
}

template <typename LockType>
void ConstStrictFromNonOwningUniqueThrowsImpl()
{
  const etcpal::SynchronizedValue<int, LockType> v(0);
  try
  {
    auto deferred = v.DeferSynchronize();

    etcpal::ConstStrictLockPtr<int, LockType> strict(std::move(deferred));
    TEST_FAIL_MESSAGE("This block should not be entered");
  }
  catch (const std::runtime_error&)
  {
  }
  // The failed conversion must not have locked or unlocked anything.
  TEST_ASSERT_TRUE(LockIsFree(v));
}

template <typename LockType>
void StrictFromNonOwningUniqueThrowsImpl()
{
  etcpal::SynchronizedValue<int, LockType> v(0);
  try
  {
    auto deferred = v.DeferSynchronize();

    etcpal::StrictLockPtr<int, LockType> strict(std::move(deferred));
    TEST_FAIL_MESSAGE("This block should not be entered");
  }
  catch (const std::runtime_error&)
  {
  }
  // The failed conversion must not have locked or unlocked anything.
  TEST_ASSERT_TRUE(LockIsFree(v));
}
#endif

template <typename LockType>
void ApplyReturnsResultImpl()
{
  etcpal::SynchronizedValue<std::string, LockType> v("ab");
  auto                                             size = v.Apply([](std::string& s) {
    s += "c";
    return s.size();
                                              });
  TEST_ASSERT_EQUAL_INT(3, static_cast<int>(size));
  TEST_ASSERT_EQUAL_STRING("abc", v.GetValueUnsafe().c_str());
}

template <typename LockType>
void ApplyConstImpl()
{
  const etcpal::SynchronizedValue<std::string, LockType> v("hello");
  auto                                                   size = v.Apply([](const std::string& s) { return s.size(); });
  TEST_ASSERT_EQUAL_INT(5, static_cast<int>(size));
}

template <typename LockType>
void ApplyVoidReturnImpl()
{
  etcpal::SynchronizedValue<int, LockType> v(1);
  v.Apply([](int& i) { ++i; });  // void-returning callable must compile (guards decltype(auto))
  TEST_ASSERT_EQUAL_INT(2, v.GetValueUnsafe());
}

// ---- multithreaded coverage (run under both Mutex and RecursiveMutex) -------------------------------------

constexpr int kStressThreads       = 8;
constexpr int kIncrementsPerThread = 10000;
constexpr int kIterationsPerThread = 2000;

template <typename LockType>
void ConcurrentIncrementsSerializeImpl()
{
  etcpal::SynchronizedValue<int, LockType> counter(0);

  std::vector<etcpal::Thread> threads;
  threads.reserve(kStressThreads);
  for (int t = 0; t < kStressThreads; ++t)
  {
    const bool use_apply = (t % 2 == 0);
    threads.emplace_back([&counter, use_apply]() {
      for (int i = 0; i < kIncrementsPerThread; ++i)
      {
        if (use_apply)
          counter.Apply([](int& c) { ++c; });
        else
          *counter.Synchronize() += 1;
      }
    });
  }
  for (auto& thread : threads)
  {
    TEST_ASSERT_TRUE(thread.Join().IsOk());
  }

  // If the lock failed to serialize the read-modify-writes, increments would be lost and the total would be low.
  TEST_ASSERT_EQUAL_INT(kStressThreads * kIncrementsPerThread, counter.GetValueUnsafe());
}

struct Occupancy
{
  int active   = 0;
  int max_seen = 0;
};

template <typename LockType>
void SingleOccupantInCriticalSectionImpl()
{
  etcpal::SynchronizedValue<Occupancy, LockType> occupancy;

  std::vector<etcpal::Thread> threads;
  threads.reserve(kStressThreads);
  for (int t = 0; t < kStressThreads; ++t)
  {
    threads.emplace_back([&occupancy]() {
      for (int i = 0; i < kIterationsPerThread; ++i)
      {
        occupancy.Apply([](Occupancy& o) {
          ++o.active;
          if (o.active > o.max_seen)
            o.max_seen = o.active;
          // Widen the race window without sleeping while holding the lock (keeps sanitizer runs fast).
          for (volatile int spin = 0; spin < 50; ++spin)
          {
          }
          --o.active;
        });
      }
    });
  }
  for (auto& thread : threads)
  {
    TEST_ASSERT_TRUE(thread.Join().IsOk());
  }

  // If two threads were ever inside the critical section at once, max_seen would exceed 1.
  TEST_ASSERT_EQUAL_INT(1, occupancy.GetValueUnsafe().max_seen);
  TEST_ASSERT_EQUAL_INT(0, occupancy.GetValueUnsafe().active);
}

template <typename LockType>
void TryToSynchronizeContentionImpl()
{
  etcpal::SynchronizedValue<int, LockType> v(0);
  etcpal::Signal                           acquired;
  etcpal::Signal                           release;

  etcpal::Thread holder([&]() {
    auto handle = v.Synchronize();  // hold the lock until told to release
    *handle     = 42;
    acquired.Notify();
    release.Wait();
    // handle releases the lock here on scope exit
  });

  const bool holder_acquired = acquired.TryWait(5000);

  // While another thread holds the lock, a non-blocking try must fail.
  bool acquired_while_held = false;
  {
    auto t              = v.TryToSynchronize();
    acquired_while_held = t.OwnsLock();
  }

  // Where the platform honors timeouts, a timed try must wait (roughly) the timeout and then fail; elsewhere it
  // polls and returns immediately. Only assert the wall-clock behavior on timed-capable platforms.
  bool     timed_checked = false;
  bool     timed_blocked = true;
  uint32_t timed_elapsed = 0;
  if (etcpal::SupportsTimedLock<LockType>::value)
  {
    timed_checked = true;
    etcpal::Timer timer(50);
    auto          t = v.TryToSynchronize(50);
    timed_elapsed   = timer.GetElapsed();
    timed_blocked   = t.OwnsLock();
  }

  // Complete the handshake and join BEFORE asserting, so a failing assertion can never strand the worker.
  release.Notify();
  const bool holder_joined = holder.Join().IsOk();

  // Now that the lock is free, a try must succeed.
  bool free_acquired = false;
  {
    auto t        = v.TryToSynchronize();
    free_acquired = t.OwnsLock();
  }

  TEST_ASSERT_TRUE(holder_acquired);
  TEST_ASSERT_TRUE(holder_joined);
  TEST_ASSERT_FALSE(acquired_while_held);
  if (timed_checked)
  {
    TEST_ASSERT_FALSE(timed_blocked);
    // Heuristic borrowed from test_mutex.c: assert at least half the timeout elapsed, allowing for OS slop.
    TEST_ASSERT_GREATER_OR_EQUAL_UINT32(25, timed_elapsed);
  }
  TEST_ASSERT_TRUE(free_acquired);
  TEST_ASSERT_EQUAL_INT(42, v.GetValueUnsafe());
}

#if !DISABLE_RECURSIVE_MUTEX_TESTS
void RecursiveNestedSynchronizeImpl()
{
  // A recursive mutex lets the same thread re-enter the lock through any access path. The equivalent nesting with
  // a plain Mutex would deadlock, so that negative case is intentionally not tested here.
  etcpal::SynchronizedValue<int, etcpal::RecursiveMutex> v(0);

  auto outer = v.Synchronize();
  *outer     = 1;
  {
    auto inner = v.Synchronize();  // re-enter while outer still holds the lock
    *inner += 10;
  }
  *v.Synchronize() += 100;             // transient re-entry
  v.Apply([](int& i) { i += 1000; });  // re-entry through Apply

  TEST_ASSERT_EQUAL_INT(1111, *outer);
}
#endif

}  // namespace

extern "C" {
TEST_GROUP(etcpal_cpp_synchronized_value);

TEST_SETUP(etcpal_cpp_synchronized_value)
{
  // Needed by etcpal::Timer, used in the timed-contention test on platforms that honor lock timeouts.
  TEST_ASSERT_EQUAL(kEtcPalErrOk, etcpal_init(ETCPAL_FEATURE_TIMERS));
}

TEST_TEAR_DOWN(etcpal_cpp_synchronized_value)
{
  etcpal_deinit(ETCPAL_FEATURE_TIMERS);
}

// clang-format off
TEST(etcpal_cpp_synchronized_value, construct_in_place_mutex)                    { ConstructInPlaceImpl<etcpal::Mutex>(); }
TEST(etcpal_cpp_synchronized_value, default_construct_mutex)                     { DefaultConstructImpl<etcpal::Mutex>(); }
TEST(etcpal_cpp_synchronized_value, get_value_unsafe_mutex)                      { GetValueUnsafeImpl<etcpal::Mutex>(); }
TEST(etcpal_cpp_synchronized_value, get_lock_unsafe_mutex)                       { GetLockUnsafeImpl<etcpal::Mutex>(); }
TEST(etcpal_cpp_synchronized_value, arrow_operator_mutable_mutex)                { ArrowOperatorMutableImpl<etcpal::Mutex>(); }
TEST(etcpal_cpp_synchronized_value, arrow_operator_const_mutex)                  { ArrowOperatorConstImpl<etcpal::Mutex>(); }
TEST(etcpal_cpp_synchronized_value, synchronize_scope_mutable_mutex)             { SynchronizeScopeMutableImpl<etcpal::Mutex>(); }
TEST(etcpal_cpp_synchronized_value, synchronize_const_yields_const_ref_mutex)    { SynchronizeConstYieldsConstRefImpl<etcpal::Mutex>(); }
TEST(etcpal_cpp_synchronized_value, strict_lock_releases_on_destruction_mutex)   { StrictLockReleasesOnDestructionImpl<etcpal::Mutex>(); }
TEST(etcpal_cpp_synchronized_value, strict_lock_move_transfers_ownership_mutex)  { StrictLockMoveTransfersOwnershipImpl<etcpal::Mutex>(); }
TEST(etcpal_cpp_synchronized_value, try_to_synchronize_succeeds_when_free_mutex)     { TryToSynchronizeSucceedsWhenFreeImpl<etcpal::Mutex>(); }
TEST(etcpal_cpp_synchronized_value, try_to_synchronize_const_mutex)                  { TryToSynchronizeConstImpl<etcpal::Mutex>(); }
TEST(etcpal_cpp_synchronized_value, unique_lock_unlock_idempotent_mutex)         { UniqueLockUnlockIdempotentImpl<etcpal::Mutex>(); }
TEST(etcpal_cpp_synchronized_value, unique_lock_move_non_owning_mutex)           { UniqueLockMoveLeavesSourceNonOwningImpl<etcpal::Mutex>(); }
TEST(etcpal_cpp_synchronized_value, strict_adopt_after_release_mutex)            { StrictAdoptAfterReleaseImpl<etcpal::Mutex>(); }
TEST(etcpal_cpp_synchronized_value, strict_from_unique_conversion_mutex)         { StrictFromUniqueConversionImpl<etcpal::Mutex>(); }
TEST(etcpal_cpp_synchronized_value, defer_synchronize_then_lock_mutex)           { DeferSynchronizeThenLockImpl<etcpal::Mutex>(); }
TEST(etcpal_cpp_synchronized_value, unique_lock_relock_cycle_mutex)              { UniqueLockRelockCycleImpl<etcpal::Mutex>(); }
TEST(etcpal_cpp_synchronized_value, unique_synchronize_mutex)                    { UniqueSynchronizeImpl<etcpal::Mutex>(); }
TEST(etcpal_cpp_synchronized_value, adopt_synchronize_mutex)                     { AdoptSynchronizeImpl<etcpal::Mutex>(); }
TEST(etcpal_cpp_synchronized_value, try_to_lock_tag_ctor_mutex)                  { TryToLockTagCtorImpl<etcpal::Mutex>(); }
TEST(etcpal_cpp_synchronized_value, unique_lock_arrow_mutex)                     { UniqueLockArrowImpl<etcpal::Mutex>(); }
TEST(etcpal_cpp_synchronized_value, const_unique_handles_mutex)                  { ConstUniqueHandlesImpl<etcpal::Mutex>(); }
TEST(etcpal_cpp_synchronized_value, const_strict_from_const_unique_mutex)        { ConstStrictFromConstUniqueConversionImpl<etcpal::Mutex>(); }
TEST(etcpal_cpp_synchronized_value, timed_try_succeeds_when_free_mutex)          { TimedTrySucceedsWhenFreeImpl<etcpal::Mutex>(); }
TEST(etcpal_cpp_synchronized_value, release_leaves_mutex_locked)                 { ReleaseLeavesMutexLockedImpl(); }
#if ETCPAL_BUILDING_WITH_EXCEPTIONS
TEST(etcpal_cpp_synchronized_value, lock_misuse_throws_mutex)                    { LockMisuseThrowsImpl<etcpal::Mutex>(); }
TEST(etcpal_cpp_synchronized_value, moved_from_unique_lock_throws_mutex)         { MovedFromUniqueLockThrowsImpl<etcpal::Mutex>(); }
TEST(etcpal_cpp_synchronized_value, lock_after_release_throws_mutex)             { LockAfterReleaseThrowsImpl<etcpal::Mutex>(); }
TEST(etcpal_cpp_synchronized_value, strict_from_non_owning_unique_throws_mutex)  { StrictFromNonOwningUniqueThrowsImpl<etcpal::Mutex>(); }
TEST(etcpal_cpp_synchronized_value, const_strict_from_non_owning_throws_mutex)   { ConstStrictFromNonOwningUniqueThrowsImpl<etcpal::Mutex>(); }
TEST(etcpal_cpp_synchronized_value, non_owning_dereference_throws_mutex)         { NonOwningDereferenceThrowsImpl<etcpal::Mutex>(); }
#endif
TEST(etcpal_cpp_synchronized_value, apply_returns_result_mutex)                  { ApplyReturnsResultImpl<etcpal::Mutex>(); }
TEST(etcpal_cpp_synchronized_value, apply_const_mutex)                           { ApplyConstImpl<etcpal::Mutex>(); }
TEST(etcpal_cpp_synchronized_value, apply_void_return_mutex)                     { ApplyVoidReturnImpl<etcpal::Mutex>(); }
TEST(etcpal_cpp_synchronized_value, concurrent_increments_serialize_mutex)       { ConcurrentIncrementsSerializeImpl<etcpal::Mutex>(); }
TEST(etcpal_cpp_synchronized_value, single_occupant_in_critical_section_mutex)   { SingleOccupantInCriticalSectionImpl<etcpal::Mutex>(); }
TEST(etcpal_cpp_synchronized_value, try_to_synchronize_contention_mutex)             { TryToSynchronizeContentionImpl<etcpal::Mutex>(); }

#if !DISABLE_RECURSIVE_MUTEX_TESTS
TEST(etcpal_cpp_synchronized_value, construct_in_place_recursive_mutex)                   { ConstructInPlaceImpl<etcpal::RecursiveMutex>(); }
TEST(etcpal_cpp_synchronized_value, default_construct_recursive_mutex)                    { DefaultConstructImpl<etcpal::RecursiveMutex>(); }
TEST(etcpal_cpp_synchronized_value, get_value_unsafe_recursive_mutex)                     { GetValueUnsafeImpl<etcpal::RecursiveMutex>(); }
TEST(etcpal_cpp_synchronized_value, get_lock_unsafe_recursive_mutex)                      { GetLockUnsafeImpl<etcpal::RecursiveMutex>(); }
TEST(etcpal_cpp_synchronized_value, arrow_operator_mutable_recursive_mutex)               { ArrowOperatorMutableImpl<etcpal::RecursiveMutex>(); }
TEST(etcpal_cpp_synchronized_value, arrow_operator_const_recursive_mutex)                 { ArrowOperatorConstImpl<etcpal::RecursiveMutex>(); }
TEST(etcpal_cpp_synchronized_value, synchronize_scope_mutable_recursive_mutex)            { SynchronizeScopeMutableImpl<etcpal::RecursiveMutex>(); }
TEST(etcpal_cpp_synchronized_value, synchronize_const_yields_const_ref_recursive_mutex)   { SynchronizeConstYieldsConstRefImpl<etcpal::RecursiveMutex>(); }
TEST(etcpal_cpp_synchronized_value, strict_lock_releases_on_destruction_recursive_mutex)  { StrictLockReleasesOnDestructionImpl<etcpal::RecursiveMutex>(); }
TEST(etcpal_cpp_synchronized_value, strict_lock_move_transfers_ownership_recursive_mutex) { StrictLockMoveTransfersOwnershipImpl<etcpal::RecursiveMutex>(); }
TEST(etcpal_cpp_synchronized_value, try_to_synchronize_succeeds_when_free_recursive_mutex)    { TryToSynchronizeSucceedsWhenFreeImpl<etcpal::RecursiveMutex>(); }
TEST(etcpal_cpp_synchronized_value, try_to_synchronize_const_recursive_mutex)                 { TryToSynchronizeConstImpl<etcpal::RecursiveMutex>(); }
TEST(etcpal_cpp_synchronized_value, unique_lock_unlock_idempotent_recursive_mutex)        { UniqueLockUnlockIdempotentImpl<etcpal::RecursiveMutex>(); }
TEST(etcpal_cpp_synchronized_value, unique_lock_move_non_owning_recursive_mutex)          { UniqueLockMoveLeavesSourceNonOwningImpl<etcpal::RecursiveMutex>(); }
TEST(etcpal_cpp_synchronized_value, strict_adopt_after_release_recursive_mutex)           { StrictAdoptAfterReleaseImpl<etcpal::RecursiveMutex>(); }
TEST(etcpal_cpp_synchronized_value, strict_from_unique_conversion_recursive_mutex)        { StrictFromUniqueConversionImpl<etcpal::RecursiveMutex>(); }
TEST(etcpal_cpp_synchronized_value, defer_synchronize_then_lock_recursive_mutex)          { DeferSynchronizeThenLockImpl<etcpal::RecursiveMutex>(); }
TEST(etcpal_cpp_synchronized_value, unique_lock_relock_cycle_recursive_mutex)             { UniqueLockRelockCycleImpl<etcpal::RecursiveMutex>(); }
TEST(etcpal_cpp_synchronized_value, unique_synchronize_recursive_mutex)                   { UniqueSynchronizeImpl<etcpal::RecursiveMutex>(); }
TEST(etcpal_cpp_synchronized_value, adopt_synchronize_recursive_mutex)                    { AdoptSynchronizeImpl<etcpal::RecursiveMutex>(); }
TEST(etcpal_cpp_synchronized_value, try_to_lock_tag_ctor_recursive_mutex)                 { TryToLockTagCtorImpl<etcpal::RecursiveMutex>(); }
TEST(etcpal_cpp_synchronized_value, unique_lock_arrow_recursive_mutex)                    { UniqueLockArrowImpl<etcpal::RecursiveMutex>(); }
TEST(etcpal_cpp_synchronized_value, const_unique_handles_recursive_mutex)                 { ConstUniqueHandlesImpl<etcpal::RecursiveMutex>(); }
TEST(etcpal_cpp_synchronized_value, const_strict_from_const_unique_recursive_mutex)       { ConstStrictFromConstUniqueConversionImpl<etcpal::RecursiveMutex>(); }
TEST(etcpal_cpp_synchronized_value, timed_try_succeeds_when_free_recursive_mutex)         { TimedTrySucceedsWhenFreeImpl<etcpal::RecursiveMutex>(); }
#if ETCPAL_BUILDING_WITH_EXCEPTIONS
TEST(etcpal_cpp_synchronized_value, lock_misuse_throws_recursive_mutex)                   { LockMisuseThrowsImpl<etcpal::RecursiveMutex>(); }
TEST(etcpal_cpp_synchronized_value, moved_from_unique_lock_throws_recursive_mutex)        { MovedFromUniqueLockThrowsImpl<etcpal::RecursiveMutex>(); }
TEST(etcpal_cpp_synchronized_value, lock_after_release_throws_recursive_mutex)            { LockAfterReleaseThrowsImpl<etcpal::RecursiveMutex>(); }
TEST(etcpal_cpp_synchronized_value, strict_from_non_owning_unique_throws_recursive_mutex) { StrictFromNonOwningUniqueThrowsImpl<etcpal::RecursiveMutex>(); }
TEST(etcpal_cpp_synchronized_value, const_strict_from_non_owning_throws_recursive_mutex)  { ConstStrictFromNonOwningUniqueThrowsImpl<etcpal::RecursiveMutex>(); }
TEST(etcpal_cpp_synchronized_value, non_owning_dereference_throws_recursive_mutex)        { NonOwningDereferenceThrowsImpl<etcpal::RecursiveMutex>(); }
#endif
TEST(etcpal_cpp_synchronized_value, apply_returns_result_recursive_mutex)                 { ApplyReturnsResultImpl<etcpal::RecursiveMutex>(); }
TEST(etcpal_cpp_synchronized_value, apply_const_recursive_mutex)                          { ApplyConstImpl<etcpal::RecursiveMutex>(); }
TEST(etcpal_cpp_synchronized_value, apply_void_return_recursive_mutex)                    { ApplyVoidReturnImpl<etcpal::RecursiveMutex>(); }
TEST(etcpal_cpp_synchronized_value, concurrent_increments_serialize_recursive_mutex)      { ConcurrentIncrementsSerializeImpl<etcpal::RecursiveMutex>(); }
TEST(etcpal_cpp_synchronized_value, single_occupant_in_critical_section_recursive_mutex)  { SingleOccupantInCriticalSectionImpl<etcpal::RecursiveMutex>(); }
TEST(etcpal_cpp_synchronized_value, try_to_synchronize_contention_recursive_mutex)            { TryToSynchronizeContentionImpl<etcpal::RecursiveMutex>(); }
TEST(etcpal_cpp_synchronized_value, recursive_nested_synchronize)                         { RecursiveNestedSynchronizeImpl(); }
#endif
// clang-format on

TEST_GROUP_RUNNER(etcpal_cpp_synchronized_value)
{
  RUN_TEST_CASE(etcpal_cpp_synchronized_value, construct_in_place_mutex);
  RUN_TEST_CASE(etcpal_cpp_synchronized_value, default_construct_mutex);
  RUN_TEST_CASE(etcpal_cpp_synchronized_value, get_value_unsafe_mutex);
  RUN_TEST_CASE(etcpal_cpp_synchronized_value, get_lock_unsafe_mutex);
  RUN_TEST_CASE(etcpal_cpp_synchronized_value, arrow_operator_mutable_mutex);
  RUN_TEST_CASE(etcpal_cpp_synchronized_value, arrow_operator_const_mutex);
  RUN_TEST_CASE(etcpal_cpp_synchronized_value, synchronize_scope_mutable_mutex);
  RUN_TEST_CASE(etcpal_cpp_synchronized_value, synchronize_const_yields_const_ref_mutex);
  RUN_TEST_CASE(etcpal_cpp_synchronized_value, strict_lock_releases_on_destruction_mutex);
  RUN_TEST_CASE(etcpal_cpp_synchronized_value, strict_lock_move_transfers_ownership_mutex);
  RUN_TEST_CASE(etcpal_cpp_synchronized_value, try_to_synchronize_succeeds_when_free_mutex);
  RUN_TEST_CASE(etcpal_cpp_synchronized_value, try_to_synchronize_const_mutex);
  RUN_TEST_CASE(etcpal_cpp_synchronized_value, unique_lock_unlock_idempotent_mutex);
  RUN_TEST_CASE(etcpal_cpp_synchronized_value, unique_lock_move_non_owning_mutex);
  RUN_TEST_CASE(etcpal_cpp_synchronized_value, strict_adopt_after_release_mutex);
  RUN_TEST_CASE(etcpal_cpp_synchronized_value, strict_from_unique_conversion_mutex);
  RUN_TEST_CASE(etcpal_cpp_synchronized_value, defer_synchronize_then_lock_mutex);
  RUN_TEST_CASE(etcpal_cpp_synchronized_value, unique_lock_relock_cycle_mutex);
  RUN_TEST_CASE(etcpal_cpp_synchronized_value, unique_synchronize_mutex);
  RUN_TEST_CASE(etcpal_cpp_synchronized_value, adopt_synchronize_mutex);
  RUN_TEST_CASE(etcpal_cpp_synchronized_value, try_to_lock_tag_ctor_mutex);
  RUN_TEST_CASE(etcpal_cpp_synchronized_value, unique_lock_arrow_mutex);
  RUN_TEST_CASE(etcpal_cpp_synchronized_value, const_unique_handles_mutex);
  RUN_TEST_CASE(etcpal_cpp_synchronized_value, const_strict_from_const_unique_mutex);
  RUN_TEST_CASE(etcpal_cpp_synchronized_value, timed_try_succeeds_when_free_mutex);
  RUN_TEST_CASE(etcpal_cpp_synchronized_value, release_leaves_mutex_locked);
#if ETCPAL_BUILDING_WITH_EXCEPTIONS
  RUN_TEST_CASE(etcpal_cpp_synchronized_value, lock_misuse_throws_mutex);
  RUN_TEST_CASE(etcpal_cpp_synchronized_value, moved_from_unique_lock_throws_mutex);
  RUN_TEST_CASE(etcpal_cpp_synchronized_value, lock_after_release_throws_mutex);
  RUN_TEST_CASE(etcpal_cpp_synchronized_value, strict_from_non_owning_unique_throws_mutex);
  RUN_TEST_CASE(etcpal_cpp_synchronized_value, const_strict_from_non_owning_throws_mutex);
  RUN_TEST_CASE(etcpal_cpp_synchronized_value, non_owning_dereference_throws_mutex);
#endif
  RUN_TEST_CASE(etcpal_cpp_synchronized_value, apply_returns_result_mutex);
  RUN_TEST_CASE(etcpal_cpp_synchronized_value, apply_const_mutex);
  RUN_TEST_CASE(etcpal_cpp_synchronized_value, apply_void_return_mutex);
  RUN_TEST_CASE(etcpal_cpp_synchronized_value, concurrent_increments_serialize_mutex);
  RUN_TEST_CASE(etcpal_cpp_synchronized_value, single_occupant_in_critical_section_mutex);
  RUN_TEST_CASE(etcpal_cpp_synchronized_value, try_to_synchronize_contention_mutex);

#if !DISABLE_RECURSIVE_MUTEX_TESTS
  RUN_TEST_CASE(etcpal_cpp_synchronized_value, construct_in_place_recursive_mutex);
  RUN_TEST_CASE(etcpal_cpp_synchronized_value, default_construct_recursive_mutex);
  RUN_TEST_CASE(etcpal_cpp_synchronized_value, get_value_unsafe_recursive_mutex);
  RUN_TEST_CASE(etcpal_cpp_synchronized_value, get_lock_unsafe_recursive_mutex);
  RUN_TEST_CASE(etcpal_cpp_synchronized_value, arrow_operator_mutable_recursive_mutex);
  RUN_TEST_CASE(etcpal_cpp_synchronized_value, arrow_operator_const_recursive_mutex);
  RUN_TEST_CASE(etcpal_cpp_synchronized_value, synchronize_scope_mutable_recursive_mutex);
  RUN_TEST_CASE(etcpal_cpp_synchronized_value, synchronize_const_yields_const_ref_recursive_mutex);
  RUN_TEST_CASE(etcpal_cpp_synchronized_value, strict_lock_releases_on_destruction_recursive_mutex);
  RUN_TEST_CASE(etcpal_cpp_synchronized_value, strict_lock_move_transfers_ownership_recursive_mutex);
  RUN_TEST_CASE(etcpal_cpp_synchronized_value, try_to_synchronize_succeeds_when_free_recursive_mutex);
  RUN_TEST_CASE(etcpal_cpp_synchronized_value, try_to_synchronize_const_recursive_mutex);
  RUN_TEST_CASE(etcpal_cpp_synchronized_value, unique_lock_unlock_idempotent_recursive_mutex);
  RUN_TEST_CASE(etcpal_cpp_synchronized_value, unique_lock_move_non_owning_recursive_mutex);
  RUN_TEST_CASE(etcpal_cpp_synchronized_value, strict_adopt_after_release_recursive_mutex);
  RUN_TEST_CASE(etcpal_cpp_synchronized_value, strict_from_unique_conversion_recursive_mutex);
  RUN_TEST_CASE(etcpal_cpp_synchronized_value, defer_synchronize_then_lock_recursive_mutex);
  RUN_TEST_CASE(etcpal_cpp_synchronized_value, unique_lock_relock_cycle_recursive_mutex);
  RUN_TEST_CASE(etcpal_cpp_synchronized_value, unique_synchronize_recursive_mutex);
  RUN_TEST_CASE(etcpal_cpp_synchronized_value, adopt_synchronize_recursive_mutex);
  RUN_TEST_CASE(etcpal_cpp_synchronized_value, try_to_lock_tag_ctor_recursive_mutex);
  RUN_TEST_CASE(etcpal_cpp_synchronized_value, unique_lock_arrow_recursive_mutex);
  RUN_TEST_CASE(etcpal_cpp_synchronized_value, const_unique_handles_recursive_mutex);
  RUN_TEST_CASE(etcpal_cpp_synchronized_value, const_strict_from_const_unique_recursive_mutex);
  RUN_TEST_CASE(etcpal_cpp_synchronized_value, timed_try_succeeds_when_free_recursive_mutex);
#if ETCPAL_BUILDING_WITH_EXCEPTIONS
  RUN_TEST_CASE(etcpal_cpp_synchronized_value, lock_misuse_throws_recursive_mutex);
  RUN_TEST_CASE(etcpal_cpp_synchronized_value, moved_from_unique_lock_throws_recursive_mutex);
  RUN_TEST_CASE(etcpal_cpp_synchronized_value, lock_after_release_throws_recursive_mutex);
  RUN_TEST_CASE(etcpal_cpp_synchronized_value, strict_from_non_owning_unique_throws_recursive_mutex);
  RUN_TEST_CASE(etcpal_cpp_synchronized_value, const_strict_from_non_owning_throws_recursive_mutex);
  RUN_TEST_CASE(etcpal_cpp_synchronized_value, non_owning_dereference_throws_recursive_mutex);
#endif
  RUN_TEST_CASE(etcpal_cpp_synchronized_value, apply_returns_result_recursive_mutex);
  RUN_TEST_CASE(etcpal_cpp_synchronized_value, apply_const_recursive_mutex);
  RUN_TEST_CASE(etcpal_cpp_synchronized_value, apply_void_return_recursive_mutex);
  RUN_TEST_CASE(etcpal_cpp_synchronized_value, concurrent_increments_serialize_recursive_mutex);
  RUN_TEST_CASE(etcpal_cpp_synchronized_value, single_occupant_in_critical_section_recursive_mutex);
  RUN_TEST_CASE(etcpal_cpp_synchronized_value, try_to_synchronize_contention_recursive_mutex);
  RUN_TEST_CASE(etcpal_cpp_synchronized_value, recursive_nested_synchronize);
#endif
}
}  // extern "C"
