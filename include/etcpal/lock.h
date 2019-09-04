/******************************************************************************
 * Copyright 2019 ETC Inc.
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

#ifndef _LWPA_LOCK_H_
#define _LWPA_LOCK_H_

#include "etcpal/os_lock.h"

#if defined(__cplusplus) || defined(DOXYGEN)
// C++ extras defined if including lwpa in a C++ application.

#include <stdexcept>

/// The C++ namespace for lwpa.
namespace lwpa
{
/// \addtogroup etcpal_lock
/// @{

/// \name Lock guard classes
/// RAII-style wrapper classes for \ref etcpal_mutex and \ref etcpal_rwlock.
/// @{

/// \brief Lock guard around an \ref etcpal_mutex.
/// Lock is taken when this class is instantiated, and released when it goes out of scope.
/// Example usage:
///
/// \code
/// void Class::CriticalFunction()
/// {
///   lwpa::MutexGuard lock(mutex_);
///   // Do critical section operations here
///
///   // Lock is released when it goes out of scope
/// }
/// \endcode
class MutexGuard
{
public:
  explicit MutexGuard(etcpal_mutex_t& mutex) : mutex_(mutex)
  {
    if (!etcpal_mutex_take(&mutex_))
      throw std::runtime_error("etcpal_mutex_take failed.");
  }
  ~MutexGuard() { etcpal_mutex_give(&mutex_); }

private:
  etcpal_mutex_t& mutex_;
};

/// \brief Read lock guard around an \ref etcpal_rwlock.
/// Read lock is taken when this class is instantiated, and released when it goes out of scope.
/// Example usage:
///
/// \code
/// void Class::CriticalFunction()
/// {
///   lwpa::ReadGuard read_lock(rwlock_);
///   // Do critical read operations here
///
///   // Lock is released when it goes out of scope
/// }
/// \endcode
class ReadGuard
{
public:
  explicit ReadGuard(etcpal_rwlock_t& rwlock) : rwlock_(rwlock)
  {
    if (!etcpal_rwlock_readlock(&rwlock_))
      throw std::runtime_error("etcpal_rwlock_readlock failed.");
  }
  ~ReadGuard() { etcpal_rwlock_readunlock(&rwlock_); }

private:
  etcpal_rwlock_t& rwlock_;
};

/// \brief Write lock guard around an \ref etcpal_rwlock.
/// Write lock is taken when this class is instantiated, and released when it goes out of scope.
/// Example usage:
///
/// \code
/// void Class::CriticalFunction()
/// {
///   lwpa::WriteGuard write_lock(rwlock_);
///   // Do critical write operations here
///
///   // Lock is released when it goes out of scope
/// }
/// \endcode
class WriteGuard
{
public:
  explicit WriteGuard(etcpal_rwlock_t& rwlock) : rwlock_(rwlock)
  {
    if (!etcpal_rwlock_writelock(&rwlock_))
      throw std::runtime_error("etcpal_rwlock_writelock failed.");
  }
  ~WriteGuard() { etcpal_rwlock_writeunlock(&rwlock_); }

private:
  etcpal_rwlock_t& rwlock_;
};

/// @}

/// @}

};  // namespace lwpa

#endif /* __cplusplus || DOXYGEN */

#endif /* _LWPA_LOCK_H_ */
