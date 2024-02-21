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

/// @file etcpal/cpp/recursive_mutex.h
/// @brief C++ wrapper and utilities for etcpal/recursive_mutex.h

#ifndef ETCPAL_CPP_RECURSIVE_MUTEX_H_
#define ETCPAL_CPP_RECURSIVE_MUTEX_H_

#include <stdexcept>
#include "etcpal/cpp/common.h"
#include "etcpal/recursive_mutex.h"

namespace etcpal
{
/// @defgroup etcpal_cpp_recursive_mutex recursive_mutex (Recursive Mutexes)
/// @ingroup etcpal_cpp
/// @brief C++ utilities for the @ref etcpal_recursive_mutex module.
///
/// **WARNING:** Recursive mutexes are rarely necessary for new code. They are often used to
/// retrofit thread-safety into existing code. They can hide nasty bugs. Use regular etcpal::Mutex
/// whenever possible.
///
/// Provides a C++ wrapper for the EtcPal recursive mutex type. Also provides an RAII guard classes
/// for this type (RecursiveMutexGuard), which should be the primary constructs used to interact
/// with the mutex class.
///
/// This class is similar to std::recursive_mutex, with the key advantage that it will work on any
/// threaded platform that EtcPal is ported for, including the embedded RTOS platforms. If your
/// application or library does not need to run on these platforms, consider using
/// std::recursive_mutex instead.

/// @ingroup etcpal_cpp_recursive_mutex
/// @brief A wrapper class for the EtcPal recursive mutex type.
///
/// **WARNING:** Recursive mutexes are rarely necessary for new code. They are often used to
/// retrofit thread-safety into existing code. They can hide nasty bugs. Use regular etcpal::Mutex
/// whenever possible.
///
/// Note: The etcpal::RecursiveMutex functions are not normally used directly - prefer usage of the
/// RAII type etcpal::RecursiveMutexGuard to manage locking and unlocking of an
/// etcpal::RecursiveMutex.
///
/// See etcpal::Mutex for example usage.
///
/// See @ref etcpal_recursive_mutex for more information on the recursive mutex time.
class RecursiveMutex
{
public:
  RecursiveMutex();
  ~RecursiveMutex();

  RecursiveMutex(const RecursiveMutex& other) = delete;
  RecursiveMutex& operator=(const RecursiveMutex& other) = delete;
  RecursiveMutex(RecursiveMutex&& other)                 = delete;
  RecursiveMutex& operator=(RecursiveMutex&& other) = delete;

  bool Lock();
  bool TryLock(int timeout_ms = 0);
  void Unlock();

  etcpal_recursive_mutex_t& get();

private:
  etcpal_recursive_mutex_t mutex_{};
};

/// @brief Create a new recursive mutex.
inline RecursiveMutex::RecursiveMutex()
{
  (void)etcpal_recursive_mutex_create(&mutex_);
}

/// @brief Destroy the recursive mutex.
inline RecursiveMutex::~RecursiveMutex()
{
  etcpal_recursive_mutex_destroy(&mutex_);
}

/// @brief Lock the mutex.
/// @return The result of etcpal_recursive_mutex_lock() on the underlying mutex.
inline bool RecursiveMutex::Lock()
{
  return etcpal_recursive_mutex_lock(&mutex_);
}

/// @brief Attempt to lock the recursive mutex.
///
/// Returns when either the mutex is acquired or the timeout expires. **NOTE**: Timeout values
/// other than 0 or ETCPAL_WAIT_FOREVER are typically only honored on real-time platforms. See the
/// table in @ref etcpal_recursive_mutex for more information. On platforms where timeouts are not
/// honored, passing 0 for timeout_ms executes a poll for the mutex returning immediately, while
/// any other value executes the equivalent of Lock().
///
/// @param timeout_ms How long to wait to acquire the mutex, in milliseconds. Default is to poll
///                   and return immediately.
/// @return The result of etcpal_recursive_mutex_timed_lock() on the underlying mutex.
inline bool RecursiveMutex::TryLock(int timeout_ms)
{
  return etcpal_recursive_mutex_timed_lock(&mutex_, timeout_ms);
}

/// @brief Unlock the recursive mutex.
///
/// See etcpal_recursive_mutex_unlock().
inline void RecursiveMutex::Unlock()
{
  etcpal_recursive_mutex_unlock(&mutex_);
}

/// @brief Get a reference to the underlying etcpal_recursive_mutex_t type.
inline etcpal_recursive_mutex_t& RecursiveMutex::get()
{
  return mutex_;
}

/// @ingroup etcpal_cpp_recursive_mutex
/// @brief Lock guard around a recursive mutex.
///
/// Lock is taken when this class is instantiated, and released when it goes out of scope.
/// Example usage:
///
/// @code
/// void Class::CriticalFunction()
/// {
///   etcpal::RecursiveMutexGuard lock(mutex_);
///   // Do critical section operations here
///
///   // Lock is released when it goes out of scope
/// }
/// @endcode
class RecursiveMutexGuard
{
public:
  explicit RecursiveMutexGuard(RecursiveMutex& mutex);
  explicit RecursiveMutexGuard(etcpal_recursive_mutex_t& mutex);
  ~RecursiveMutexGuard();

  RecursiveMutexGuard(const RecursiveMutexGuard& other) = delete;
  RecursiveMutexGuard& operator=(const RecursiveMutexGuard& other) = delete;
  RecursiveMutexGuard(RecursiveMutexGuard&& other)                 = delete;
  RecursiveMutexGuard& operator=(RecursiveMutexGuard&& other) = delete;

private:
  etcpal_recursive_mutex_t& mutex_;

  void GetLock();
};

/// @brief Lock an etcpal::RecursiveMutex.
/// @throw std::runtime_error if locking the mutex failed.
inline RecursiveMutexGuard::RecursiveMutexGuard(RecursiveMutex& mutex) : mutex_(mutex.get())
{
  GetLock();
}

/// @brief Lock an @ref etcpal_recursive_mutex_t.
/// @throw std::runtime_error if locking the mutex failed.
inline RecursiveMutexGuard::RecursiveMutexGuard(etcpal_recursive_mutex_t& mutex) : mutex_(mutex)
{
  GetLock();
}

/// @brief Release the lock upon going out-of-scope.
inline RecursiveMutexGuard::~RecursiveMutexGuard()
{
  etcpal_recursive_mutex_unlock(&mutex_);
}

inline void RecursiveMutexGuard::GetLock()
{
  if (!etcpal_recursive_mutex_lock(&mutex_))
    ETCPAL_THROW(std::runtime_error("etcpal_recursive_mutex_lock failed."));
}

};  // namespace etcpal

#endif  // ETCPAL_CPP_RECURSIVE_MUTEX_H_
