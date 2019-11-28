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

/// \file etcpal/cpp/lock.h
/// \brief C++ wrapper and utilities for etcpal/lock.h

#ifndef ETCPAL_CPP_LOCK_H_
#define ETCPAL_CPP_LOCK_H_

#include <stdexcept>
#include "etcpal/lock.h"

namespace etcpal
{
/// \defgroup etcpal_cpp_lock Locks and Synchronization (lock)
/// \ingroup etcpal_cpp
/// \brief C++ utilities for the \ref etcpal_lock module.

/// \ingroup etcpal_cpp_lock
/// \brief A wrapper class for the EtcPal mutex type.
///
/// Note: The etcpal::Mutex functions are not normally used directly - prefer usage of the RAII
/// type etcpal::MutexGuard to manage locking and unlocking of an etcpal::Mutex.
///
/// See \ref etcpal_mutex for more information.
class Mutex
{
public:
  Mutex();
  ~Mutex();

  Mutex(const Mutex& other) = delete;
  Mutex& operator=(const Mutex& other) = delete;
  Mutex(Mutex&& other) = delete;
  Mutex& operator=(Mutex&& other) = delete;

  bool Lock();
  bool TryLock();
  void Unlock();

  etcpal_mutex_t& get();

private:
  etcpal_mutex_t mutex_{};
};

/// \brief Create a new mutex.
inline Mutex::Mutex()
{
  etcpal_mutex_create(&mutex_);
}

/// \brief Destroy the mutex.
inline Mutex::~Mutex()
{
  etcpal_mutex_destroy(&mutex_);
}

/// \brief Lock the mutex.
/// \return The result of etcpal_mutex_take() on the underlying mutex.
inline bool Mutex::Lock()
{
  return etcpal_mutex_take(&mutex_);
}

/// \brief Attempt to lock the mutex.
/// \return The result of etcpal_mutex_try_take() on the underlying mutex.
inline bool Mutex::TryLock()
{
  return etcpal_mutex_try_take(&mutex_);
}

/// \brief Unlock the mutex.
///
/// See etcpal_mutex_give().
inline void Mutex::Unlock()
{
  etcpal_mutex_give(&mutex_);
}

/// \brief Get a reference to the underlying etcpal_mutex_t type.
inline etcpal_mutex_t& Mutex::get()
{
  return mutex_;
}

/// \ingroup etcpal_cpp_lock
/// \brief A wrapper class for the EtcPal signal type.
///
/// See \ref etcpal_signal for more information.
class Signal
{
public:
  Signal();
  ~Signal();

  Signal(const Signal& other) = delete;
  Signal& operator=(const Signal& other) = delete;
  Signal(Signal&& other) = delete;
  Signal& operator=(Signal&& other) = delete;

  bool Wait();
  bool Poll();
  void Notify();

  etcpal_signal_t& get();

private:
  etcpal_signal_t signal_{};
};

/// \brief Create a new signal.
inline Signal::Signal()
{
  etcpal_signal_create(&signal_);
}

/// \brief Destroy the signal.
inline Signal::~Signal()
{
  etcpal_signal_destroy(&signal_);
}

/// \brief Wait for the signal.
/// \return The result of etcpal_signal_wait() on the underlying signal.
inline bool Signal::Wait()
{
  return etcpal_signal_wait(&signal_);
}

/// \brief Poll the state of the signal.
/// \return The result of etcpal_signal_poll() on the underlying signal.
inline bool Signal::Poll()
{
  return etcpal_signal_poll(&signal_);
}

/// \brief Notify those waiting on the signal.
///
/// See etcpal_signal_post().
inline void Signal::Notify()
{
  etcpal_signal_post(&signal_);
}

/// \brief Get a reference to the underlying etcpal_signal_t type.
inline etcpal_signal_t& Signal::get()
{
  return signal_;
}

/// \ingroup etcpal_cpp_lock
/// \brief A wrapper class for the EtcPal read-write lock type.
///
/// Note: The etcpal::RwLock functions are not normally used directly - prefer usage of the RAII
/// types etcpal::ReadGuard and etcpal::WriteGuard to manage locking and unlocking of an
/// etcpal::RwLock.
///
/// See \ref etcpal_rwlock for more information.
class RwLock
{
public:
  RwLock();
  ~RwLock();

  RwLock(const RwLock& other) = delete;
  RwLock& operator=(const RwLock& other) = delete;
  RwLock(RwLock&& other) = delete;
  RwLock& operator=(RwLock&& other) = delete;

  bool ReadLock();
  bool TryReadLock();
  void ReadUnlock();

  bool WriteLock();
  bool TryWriteLock();
  void WriteUnlock();

  etcpal_rwlock_t& get();

private:
  etcpal_rwlock_t rwlock_{};
};

/// \brief Create a new read-write lock.
inline RwLock::RwLock()
{
  etcpal_rwlock_create(&rwlock_);
}

/// \brief Destroy the read-write lock.
inline RwLock::~RwLock()
{
  etcpal_rwlock_destroy(&rwlock_);
}

/// \brief Access the read-write lock for reading.
/// \return The result of etcpal_rwlock_readlock() on the underlying read-write lock.
inline bool RwLock::ReadLock()
{
  return etcpal_rwlock_readlock(&rwlock_);
}

/// \brief Try to access the read-write lock for reading.
/// \return The result of etcpal_rwlock_try_readlock() on the underlying read-write lock.
inline bool RwLock::TryReadLock()
{
  return etcpal_rwlock_try_readlock(&rwlock_);
}

/// \brief Release a read lock on the read-write lock.
///
/// See etcpal_rwlock_readunlock().
inline void RwLock::ReadUnlock()
{
  etcpal_rwlock_readunlock(&rwlock_);
}

/// \brief Access the read-write lock for writing.
/// \return The result of etcpal_rwlock_writelock() on the underlying read-write lock.
inline bool RwLock::WriteLock()
{
  return etcpal_rwlock_writelock(&rwlock_);
}

/// \brief Try to access the read-write lock for writing.
/// \return The result of etcpal_rwlock_try_writelock() on the underlying read-write lock.
inline bool RwLock::TryWriteLock()
{
  return etcpal_rwlock_try_writelock(&rwlock_);
}

/// \brief Release a write lock on the read-write lock.
///
/// See etcpal_rwlock_writeunlock().
inline void RwLock::WriteUnlock()
{
  etcpal_rwlock_writeunlock(&rwlock_);
}

/// \brief Get a reference to the underlying etcpal_rwlock_t type.
inline etcpal_rwlock_t& RwLock::get()
{
  return rwlock_;
}

/// \name Lock guard classes
/// RAII-style wrapper classes for \ref etcpal_mutex and \ref etcpal_rwlock.
/// @{

/// \ingroup etcpal_cpp_lock
/// \brief Lock guard around a mutex.
///
/// Lock is taken when this class is instantiated, and released when it goes out of scope.
/// Example usage:
///
/// \code
/// void Class::CriticalFunction()
/// {
///   etcpal::MutexGuard lock(mutex_);
///   // Do critical section operations here
///
///   // Lock is released when it goes out of scope
/// }
/// \endcode
class MutexGuard
{
public:
  explicit MutexGuard(Mutex& mutex);
  explicit MutexGuard(etcpal_mutex_t& mutex);
  ~MutexGuard();

  MutexGuard(const MutexGuard& other) = delete;
  MutexGuard& operator=(const MutexGuard& other) = delete;
  MutexGuard(MutexGuard&& other) = delete;
  MutexGuard& operator=(MutexGuard&& other) = delete;

private:
  etcpal_mutex_t& mutex_;

  void GetLock();
};

/// \brief Lock an etcpal::Mutex.
/// \throw std::runtime_error if locking the mutex failed.
inline MutexGuard::MutexGuard(Mutex& mutex) : mutex_(mutex.get())
{
  GetLock();
}

/// \brief Lock an \ref etcpal_mutex_t.
/// \throw std::runtime_error if locking the mutex failed.
inline MutexGuard::MutexGuard(etcpal_mutex_t& mutex) : mutex_(mutex)
{
  GetLock();
}

/// \brief Release the lock upon going out-of-scope.
inline MutexGuard::~MutexGuard()
{
  etcpal_mutex_give(&mutex_);
}

inline void MutexGuard::GetLock()
{
  if (!etcpal_mutex_take(&mutex_))
    throw std::runtime_error("etcpal_mutex_take failed.");
}

/// \ingroup etcpal_cpp_lock
/// \brief Read lock guard around a read-write lock
///
/// Read lock is taken when this class is instantiated, and released when it goes out of scope.
/// Example usage:
///
/// \code
/// void Class::CriticalFunction()
/// {
///   etcpal::ReadGuard read_lock(rwlock_);
///   // Do critical read operations here
///
///   // Lock is released when it goes out of scope
/// }
/// \endcode
class ReadGuard
{
public:
  explicit ReadGuard(RwLock& rwlock);
  explicit ReadGuard(etcpal_rwlock_t& rwlock);
  ~ReadGuard();

  ReadGuard(const ReadGuard& other) = delete;
  ReadGuard& operator=(const ReadGuard& other) = delete;
  ReadGuard(ReadGuard&& other) = delete;
  ReadGuard& operator=(ReadGuard&& other) = delete;

private:
  etcpal_rwlock_t& rwlock_;

  void GetReadLock();
};

/// \brief Lock an etcpal::RwLock for reading.
/// \throw std::runtime_error if getting a read lock failed.
inline ReadGuard::ReadGuard(RwLock& rwlock) : rwlock_(rwlock.get())
{
  GetReadLock();
}

/// \brief Lock an \ref etcpal_rwlock_t for reading.
/// \throw std::runtime_error if getting a read lock failed.
inline ReadGuard::ReadGuard(etcpal_rwlock_t& rwlock) : rwlock_(rwlock)
{
  GetReadLock();
}

/// \brief Release the read lock upon going out-of-scope.
inline ReadGuard::~ReadGuard()
{
  etcpal_rwlock_readunlock(&rwlock_);
}

inline void ReadGuard::GetReadLock()
{
  if (!etcpal_rwlock_readlock(&rwlock_))
    throw std::runtime_error("etcpal_rwlock_readlock failed.");
}

/// \ingroup etcpal_cpp_lock
/// \brief Write lock guard around a read-write lock.
///
/// Write lock is taken when this class is instantiated, and released when it goes out of scope.
/// Example usage:
///
/// \code
/// void Class::CriticalFunction()
/// {
///   etcpal::WriteGuard write_lock(rwlock_);
///   // Do critical write operations here
///
///   // Lock is released when it goes out of scope
/// }
/// \endcode
class WriteGuard
{
public:
  explicit WriteGuard(RwLock& rwlock);
  explicit WriteGuard(etcpal_rwlock_t& rwlock);
  ~WriteGuard();

  WriteGuard(const WriteGuard& other) = delete;
  WriteGuard& operator=(const WriteGuard& other) = delete;
  WriteGuard(WriteGuard&& other) = delete;
  WriteGuard& operator=(WriteGuard&& other) = delete;

private:
  etcpal_rwlock_t& rwlock_;

  void GetWriteLock();
};

/// \brief Lock an etcpal::RwLock for writing.
/// \throw std::runtime_error if getting a write lock failed.
inline WriteGuard::WriteGuard(RwLock& rwlock) : rwlock_(rwlock.get())
{
  GetWriteLock();
}

/// \brief Lock an \ref etcpal_rwlock_t for writing.
/// \throw std::runtime_error if getting a write lock failed.
inline WriteGuard::WriteGuard(etcpal_rwlock_t& rwlock) : rwlock_(rwlock)
{
  GetWriteLock();
}

/// \brief Release the write lock upon going out-of-scope.
inline WriteGuard::~WriteGuard()
{
  etcpal_rwlock_writeunlock(&rwlock_);
}

inline void WriteGuard::GetWriteLock()
{
  if (!etcpal_rwlock_writelock(&rwlock_))
    throw std::runtime_error("etcpal_rwlock_writelock failed.");
}

/// @}

};  // namespace etcpal

#endif  // ETCPAL_CPP_LOCK_H_
