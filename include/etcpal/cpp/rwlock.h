/******************************************************************************
 * Copyright 2022 ETC Inc.
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

/// @file etcpal/cpp/rwlock.h
/// @brief C++ wrapper and utilities for etcpal/rwlock.h

#ifndef ETCPAL_CPP_RWLOCK_H_
#define ETCPAL_CPP_RWLOCK_H_

#include <stdexcept>
#include "etcpal/cpp/common.h"
#include "etcpal/rwlock.h"

namespace etcpal
{
/// @defgroup etcpal_cpp_rwlock rwlock (Read-Write Locks)
/// @ingroup etcpal_cpp
/// @brief C++ utilities for the @ref etcpal_rwlock module.
///
/// Provides a C++ wrappers the for EtcPal read-write lock type. Also provides RAII guard classes
/// for this type (ReadGuard and WriteGuard), which should be the primary constructs used to
/// interact with the read-write lock class.
///
/// This class is similar to std::shared_mutex, with the key advantage that they will work on any
/// threaded platform that EtcPal is ported for, including the embedded RTOS platforms. If your
/// application or library does not need to run on these platforms, consider using
/// std::shared_mutex instead.

/// @ingroup etcpal_cpp_rwlock
/// @brief A wrapper class for the EtcPal read-write lock type.
///
/// Note: The etcpal::RwLock functions are not normally used directly - prefer usage of the RAII
/// types etcpal::ReadGuard and etcpal::WriteGuard to manage locking and unlocking of an
/// etcpal::RwLock.
///
/// Example usage:
/// @code
/// class ThreadSafeCounter
/// {
/// public:
///   ThreadSafeCounter() = default;
///
///   // Multiple threads/readers can read the counter's value at the same time.
///   unsigned int get() const
///   {
///     etcpal::ReadGuard read_lock(lock_);
///     return value_;
///   }
///
///   // Only one thread/writer can increment/write the counter's value.
///   void increment()
///   {
///     etcpal::WriteGuard write_lock(lock_);
///     ++value_;
///   }
///
///   // Only one thread/writer can reset/write the counter's value.
///   void reset()
///   {
///     etcpal::WriteGuard write_lock(lock_);
///     value_ = 0;
///   }
///
///  private:
///   mutable etcpal::RwLock lock_;
///   unsigned int value_ = 0;
/// };
///
/// int main()
/// {
///   ThreadSafeCounter counter;
///
///   auto increment_and_print = [&counter]()
///   {
///     for (int i = 0; i < 3; i++)
///     {
///       counter.increment();
///       std::cout << counter.get() << '\n';
///     }
///   };
///
///   etcpal::Thread thread1(increment_and_print);
///   etcpal::Thread thread2(increment_and_print);
///
///   thread1.Join();
///   thread2.Join();
///
///   std::cout << counter.get() << '\n'; // Outputs '6'
///
///   return 0;
/// }
/// @endcode
///
/// See @ref etcpal_rwlock for more information.
class RwLock
{
public:
  RwLock();
  ~RwLock();

  RwLock(const RwLock& other) = delete;
  RwLock& operator=(const RwLock& other) = delete;
  RwLock(RwLock&& other)                 = delete;
  RwLock& operator=(RwLock&& other) = delete;

  bool ReadLock();
  bool TryReadLock(int timeout_ms = 0);
  void ReadUnlock();

  bool WriteLock();
  bool TryWriteLock(int timeout_ms = 0);
  void WriteUnlock();

  etcpal_rwlock_t& get();

private:
  etcpal_rwlock_t rwlock_{};
};

/// @brief Create a new read-write lock.
inline RwLock::RwLock()
{
  (void)etcpal_rwlock_create(&rwlock_);
}

/// @brief Destroy the read-write lock.
inline RwLock::~RwLock()
{
  etcpal_rwlock_destroy(&rwlock_);
}

/// @brief Access the read-write lock for reading.
/// @return The result of etcpal_rwlock_readlock() on the underlying read-write lock.
inline bool RwLock::ReadLock()
{
  return etcpal_rwlock_readlock(&rwlock_);
}

/// @brief Try to access the read-write lock for reading.
///
/// Returns when either the read lock is acquired or the timeout expires. **NOTE**: Timeout values
/// other than 0 or ETCPAL_WAIT_FOREVER are typically only honored on real-time platforms. See the
/// table in @ref etcpal_rwlock for more information. On platforms where timeouts are not honored,
/// passing 0 for timeout_ms executes a poll for the lock returning immediately, while any other
/// value executes the equivalent of ReadLock().
///
/// @param timeout_ms How long to wait to acquire the read lock, in milliseconds. Default is to
///                   poll and return immediately.
/// @return The result of etcpal_rwlock_timed_readlock() on the underlying read-write lock.
inline bool RwLock::TryReadLock(int timeout_ms)
{
  return etcpal_rwlock_timed_readlock(&rwlock_, timeout_ms);
}

/// @brief Release a read lock on the read-write lock.
///
/// See etcpal_rwlock_readunlock().
inline void RwLock::ReadUnlock()
{
  etcpal_rwlock_readunlock(&rwlock_);
}

/// @brief Access the read-write lock for writing.
/// @return The result of etcpal_rwlock_writelock() on the underlying read-write lock.
inline bool RwLock::WriteLock()
{
  return etcpal_rwlock_writelock(&rwlock_);
}

/// @brief Try to access the read-write lock for writing.
///
/// Returns when either the read lock is acquired or the timeout expires. **NOTE**: Timeout values
/// other than 0 or ETCPAL_WAIT_FOREVER are typically only honored on real-time platforms. See the
/// table in @ref etcpal_rwlock for more information. On platforms where timeouts are not honored,
/// passing 0 for timeout_ms executes a poll for the lock returning immediately, while any other
/// value executes the equivalent of WriteLock().
///
/// @param timeout_ms How long to wait to acquire the write lock, in milliseconds. Default is to
///                   poll and return immediately.
/// @return The result of etcpal_rwlock_timed_writelock() on the underlying read-write lock.
inline bool RwLock::TryWriteLock(int timeout_ms)
{
  return etcpal_rwlock_timed_writelock(&rwlock_, timeout_ms);
}

/// @brief Release a write lock on the read-write lock.
///
/// See etcpal_rwlock_writeunlock().
inline void RwLock::WriteUnlock()
{
  etcpal_rwlock_writeunlock(&rwlock_);
}

/// @brief Get a reference to the underlying etcpal_rwlock_t type.
inline etcpal_rwlock_t& RwLock::get()
{
  return rwlock_;
}

/// @ingroup etcpal_cpp_rwlock
/// @brief Read lock guard around a read-write lock
///
/// Read lock is taken when this class is instantiated, and released when it goes out of scope.
/// Example usage:
///
/// @code
/// void Class::CriticalFunction()
/// {
///   etcpal::ReadGuard read_lock(rwlock_);
///   // Do critical read operations here
///
///   // Lock is released when it goes out of scope
/// }
/// @endcode
class ReadGuard
{
public:
  explicit ReadGuard(RwLock& rwlock);
  explicit ReadGuard(etcpal_rwlock_t& rwlock);
  ~ReadGuard();

  ReadGuard(const ReadGuard& other) = delete;
  ReadGuard& operator=(const ReadGuard& other) = delete;
  ReadGuard(ReadGuard&& other)                 = delete;
  ReadGuard& operator=(ReadGuard&& other) = delete;

private:
  etcpal_rwlock_t& rwlock_;

  void GetReadLock();
};

/// @brief Lock an etcpal::RwLock for reading.
/// @throw std::runtime_error if getting a read lock failed.
inline ReadGuard::ReadGuard(RwLock& rwlock) : rwlock_(rwlock.get())
{
  GetReadLock();
}

/// @brief Lock an @ref etcpal_rwlock_t for reading.
/// @throw std::runtime_error if getting a read lock failed.
inline ReadGuard::ReadGuard(etcpal_rwlock_t& rwlock) : rwlock_(rwlock)
{
  GetReadLock();
}

/// @brief Release the read lock upon going out-of-scope.
inline ReadGuard::~ReadGuard()
{
  etcpal_rwlock_readunlock(&rwlock_);
}

inline void ReadGuard::GetReadLock()
{
  if (!etcpal_rwlock_readlock(&rwlock_))
    ETCPAL_THROW(std::runtime_error("etcpal_rwlock_readlock failed."));
}

/// @ingroup etcpal_cpp_rwlock
/// @brief Write lock guard around a read-write lock.
///
/// Write lock is taken when this class is instantiated, and released when it goes out of scope.
/// Example usage:
///
/// @code
/// void Class::CriticalFunction()
/// {
///   etcpal::WriteGuard write_lock(rwlock_);
///   // Do critical write operations here
///
///   // Lock is released when it goes out of scope
/// }
/// @endcode
class WriteGuard
{
public:
  explicit WriteGuard(RwLock& rwlock);
  explicit WriteGuard(etcpal_rwlock_t& rwlock);
  ~WriteGuard();

  WriteGuard(const WriteGuard& other) = delete;
  WriteGuard& operator=(const WriteGuard& other) = delete;
  WriteGuard(WriteGuard&& other)                 = delete;
  WriteGuard& operator=(WriteGuard&& other) = delete;

private:
  etcpal_rwlock_t& rwlock_;

  void GetWriteLock();
};

/// @brief Lock an etcpal::RwLock for writing.
/// @throw std::runtime_error if getting a write lock failed.
inline WriteGuard::WriteGuard(RwLock& rwlock) : rwlock_(rwlock.get())
{
  GetWriteLock();
}

/// @brief Lock an @ref etcpal_rwlock_t for writing.
/// @throw std::runtime_error if getting a write lock failed.
inline WriteGuard::WriteGuard(etcpal_rwlock_t& rwlock) : rwlock_(rwlock)
{
  GetWriteLock();
}

/// @brief Release the write lock upon going out-of-scope.
inline WriteGuard::~WriteGuard()
{
  etcpal_rwlock_writeunlock(&rwlock_);
}

inline void WriteGuard::GetWriteLock()
{
  if (!etcpal_rwlock_writelock(&rwlock_))
    ETCPAL_THROW(std::runtime_error("etcpal_rwlock_writelock failed."));
}

};  // namespace etcpal

#endif  // ETCPAL_CPP_RWLOCK_H_
