/******************************************************************************
 * Copyright 2020 ETC Inc.
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

/// @file etcpal/cpp/lock.h
/// @brief C++ wrapper and utilities for etcpal/lock.h

#ifndef ETCPAL_CPP_LOCK_H_
#define ETCPAL_CPP_LOCK_H_

#include <stdexcept>
#include "etcpal/cpp/common.h"
#include "etcpal/lock.h"

namespace etcpal
{
/// @defgroup etcpal_cpp_lock lock (Locks and Synchronization)
/// @ingroup etcpal_cpp
/// @brief C++ utilities for the @ref etcpal_lock module.
///
/// Provides C++ wrappers for mutexes (Mutex), signals (Signal) and read-write locks (RwLock). Also
/// provides RAII guard classes for these types (MutexGuard, ReadGuard, WriteGuard), which should
/// be the primary constructs used to interact with the mutual-exclusion classes.
///
/// These classes are similar to std::mutex, std::condition_variable, and std::shared_mutex, with
/// the key advantage that they will work on any threaded platform that EtcPal is ported for,
/// including the embedded RTOS platforms. If your application or library does not need to run on
/// these platforms, consider using the STL versions instead.

/// @ingroup etcpal_cpp_lock
/// @brief A wrapper class for the EtcPal mutex type.
///
/// Note: The etcpal::Mutex functions are not normally used directly - prefer usage of the RAII
/// type etcpal::MutexGuard to manage locking and unlocking of an etcpal::Mutex.
///
/// Example usage:
/// @code
/// static int shared_counter;
/// etcpal::Mutex mutex;
///
/// void CounterThread()
/// {
///   for (int i = 0; i < 10000; ++i)
///   {
///     etcpal::MutexGuard lock(mutex);
///     ++shared_counter;
///   }
/// }
///
/// int main()
/// {
///   std::vector<etcpal::Thread> counter_threads;
///   counter_threads.reserve(5);
///   for (int i = 0; i < 5; ++i)
///   {
///     counter_threads.emplace_back(CounterThread);
///   }
///
///   // Wait for the threads to finish
///   etcpal::Thread::Sleep(1000);
///   for (auto& thread : counter_threads)
///   {
///     thread.Join();
///   }
///
///   std::cout << "The counter is: " << shared_counter << '\n'; // Prints "the counter is: 50000"
///
///   return 0;
/// }
/// @endcode
///
/// See @ref etcpal_mutex for more information.
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
  bool TryLock(int timeout_ms = 0);
  void Unlock();

  etcpal_mutex_t& get();

private:
  etcpal_mutex_t mutex_{};
};

/// @brief Create a new mutex.
inline Mutex::Mutex()
{
  (void)etcpal_mutex_create(&mutex_);
}

/// @brief Destroy the mutex.
inline Mutex::~Mutex()
{
  etcpal_mutex_destroy(&mutex_);
}

/// @brief Lock the mutex.
/// @return The result of etcpal_mutex_lock() on the underlying mutex.
inline bool Mutex::Lock()
{
  return etcpal_mutex_lock(&mutex_);
}

/// @brief Attempt to lock the mutex.
///
/// Returns when either the mutex is acquired or the timeout expires. **NOTE**: Timeout values
/// other than 0 or ETCPAL_WAIT_FOREVER are typically only honored on real-time platforms. See the
/// table in @ref etcpal_mutex for more information. On platforms where timeouts are not honored,
/// passing 0 for timeout_ms executes a poll for the mutex returning immediately, while any other
/// value executes the equivalent of Lock().
///
/// @param timeout_ms How long to wait to acquire the mutex, in milliseconds. Default is to poll
///                   and return immediately.
/// @return The result of etcpal_mutex_timed_lock() on the underlying mutex.
inline bool Mutex::TryLock(int timeout_ms)
{
  return etcpal_mutex_timed_lock(&mutex_, timeout_ms);
}

/// @brief Unlock the mutex.
///
/// See etcpal_mutex_give().
inline void Mutex::Unlock()
{
  etcpal_mutex_unlock(&mutex_);
}

/// @brief Get a reference to the underlying etcpal_mutex_t type.
inline etcpal_mutex_t& Mutex::get()
{
  return mutex_;
}

/// @ingroup etcpal_cpp_lock
/// @brief A wrapper class for the EtcPal signal type.
///
/// Example usage:
/// @code
/// etcpal::Signal sig;
/// std::string data;
///
/// void worker_thread()
/// {
///   // Wait until main() sends data
///   sig.Wait();
///
///   std::cout << "Worker thread is processing data\n";
///   data += " after processing";
///
///   // Send data back to main()
///   std::cout << "Worker thread signals data processing completed\n";
///   sig.Notify();
/// }
///
/// int main()
/// {
///   etcpal::Thread worker(worker_thread);
///
///   data = "Example data";
///   // send data to the worker thread
///   std::cout << "main() signals data ready for processing\n";
///   sig.Notify();
///
///   // wait for the worker
///   sig.Wait();
///   std::cout << "Back in main(), data = " << data << '\n';
///
///   worker.Join();
///   return 0;
/// }
/// @endcode
///
/// The output of the above would be:
/// ```
/// main() signals data ready for processing
/// Worker thread is processing data
/// Worker thread signals data processing completed
/// Back in main(), data = Example data after processing
/// ```
///
/// See @ref etcpal_signal for more information.
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
  bool TryWait(int timeout_ms = 0);
  void Notify();
  void NotifyFromIsr();

  etcpal_signal_t& get();

private:
  etcpal_signal_t signal_{};
};

/// @brief Create a new signal.
inline Signal::Signal()
{
  (void)etcpal_signal_create(&signal_);
}

/// @brief Destroy the signal.
inline Signal::~Signal()
{
  etcpal_signal_destroy(&signal_);
}

/// @brief Wait for the signal.
/// @return The result of etcpal_signal_wait() on the underlying signal.
inline bool Signal::Wait()
{
  return etcpal_signal_wait(&signal_);
}

/// @brief Wait for the signal until either it is received or a timeout expires.
///
/// **NOTE**: Timeouts other than 0 or ETCPAL_WAIT_FOREVER are typically only honored on real-time
/// platforms. See the table in @ref etcpal_signal for more information. On platforms where
/// timeouts are not honored, passing 0 for timeout_ms returns immediately whether the signal is in
/// the signaled or unsignaled state, while any other value executes the equivalent of Wait().
///
/// @param timeout_ms How long to wait for the signal, in milliseconds.
/// @return The result of etcpal_signal_timed_wait() on the underlying signal.
inline bool Signal::TryWait(int timeout_ms)
{
  return etcpal_signal_timed_wait(&signal_, timeout_ms);
}

/// @brief Notify those waiting on the signal.
///
/// See etcpal_signal_post().
inline void Signal::Notify()
{
  etcpal_signal_post(&signal_);
}

/// @brief Notify those waiting on the signal from an interrupt context.
///
/// **NOTE**: Only meaningful on some platforms. See the table in @ref etcpal_signal and
/// etcpal_signal_post_from_isr() for more information. On platforms on which it is not meaningful,
/// executes the equivalent of Notify().
inline void Signal::NotifyFromIsr()
{
  etcpal_signal_post_from_isr(&signal_);
}

/// @brief Get a reference to the underlying etcpal_signal_t type.
inline etcpal_signal_t& Signal::get()
{
  return signal_;
}

/// @ingroup etcpal_cpp_lock
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
  RwLock(RwLock&& other) = delete;
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

/// @ingroup etcpal_cpp_lock
/// @brief A wrapper class for the EtcPal counting semaphore type.
///
/// Usage example:
/// @code
/// etcpal::Semaphore sem;
///
/// void main()
/// {
///   // Install interrupt/signal handler and/or enable interrupts...
///
///   while(true)
///   {
///     if (sem.Wait())
///     {
///       // Service the interrupt.
///       // Because this is a counting semaphore, we will enter this block once for every time the
///       // interrupt is received.
///     }
///   }
/// }
///
/// // An interrupt or signal handler installed to handle some event
/// void InterruptHandler()
/// {
///   // We've received an event. Notify the service thread that a new event has occurred.
///   sem.PostFromIsr();
/// }
/// @endcode
///
/// See @ref etcpal_sem for more information.
class Semaphore
{
public:
  /// The default value used for the semaphore's max_count. This is not honored on all platforms -
  /// see @ref etcpal_sem for more information.
  static constexpr unsigned int kDefaultMaxCount = 50;

  Semaphore(unsigned int initial_count = 0, unsigned int max_count = kDefaultMaxCount);
  ~Semaphore();

  Semaphore(const Semaphore& other) = delete;
  Semaphore& operator=(const Semaphore& other) = delete;
  Semaphore(Semaphore&& other) = delete;
  Semaphore& operator=(Semaphore&& other) = delete;

  bool Wait();
  bool TryWait(int timeout_ms = 0);
  bool Post();
  bool PostFromIsr();

  etcpal_sem_t& get();

private:
  etcpal_sem_t sem_{};
};

/// @brief Create a new semaphore.
inline Semaphore::Semaphore(unsigned int initial_count, unsigned int max_count)
{
  (void)etcpal_sem_create(&sem_, initial_count, max_count);
}

/// @brief Destroy the semaphore.
inline Semaphore::~Semaphore()
{
  etcpal_sem_destroy(&sem_);
}

/// @brief Wait for the semaphore.
/// @return The result of etcpal_sem_wait() on the underlying semaphore.
inline bool Semaphore::Wait()
{
  return etcpal_sem_wait(&sem_);
}

/// @brief Wait for the semaphore until either it is received or a timeout expires.
///
/// **NOTE**: Timeouts other than 0 or #ETCPAL_WAIT_FOREVER are typically only honored on real-time
/// platforms. See the table in @ref etcpal_sem for more information. On platforms where
/// timeouts are not honored, passing 0 for timeout_ms returns immediately whether the semaphore
/// was available and taken, while any other value executes the equivalent of Wait().
///
/// @param timeout_ms How long to wait for the semaphore, in milliseconds.
/// @return The result of etcpal_sem_timed_wait() on the underlying semaphore.
inline bool Semaphore::TryWait(int timeout_ms)
{
  return etcpal_sem_timed_wait(&sem_, timeout_ms);
}

/// @brief Post the semaphore.
///
/// See etcpal_sem_post().
inline bool Semaphore::Post()
{
  return etcpal_sem_post(&sem_);
}

/// @brief Post the semaphore from an interrupt context.
///
/// **NOTE**: Only meaningful on some platforms. See the table in @ref etcpal_sem and
/// etcpal_sem_post_from_isr() for more information. On platforms on which it is not meaningful,
/// executes the equivalent of Post().
inline bool Semaphore::PostFromIsr()
{
  return etcpal_sem_post_from_isr(&sem_);
}

/// @brief Get a reference to the underlying etcpal_sem_t type.
inline etcpal_sem_t& Semaphore::get()
{
  return sem_;
}

/// @name Lock guard classes
/// RAII-style wrapper classes for @ref etcpal_mutex and @ref etcpal_rwlock.
/// @{

/// @ingroup etcpal_cpp_lock
/// @brief Lock guard around a mutex.
///
/// Lock is taken when this class is instantiated, and released when it goes out of scope.
/// Example usage:
///
/// @code
/// void Class::CriticalFunction()
/// {
///   etcpal::MutexGuard lock(mutex_);
///   // Do critical section operations here
///
///   // Lock is released when it goes out of scope
/// }
/// @endcode
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

/// @brief Lock an etcpal::Mutex.
/// @throw std::runtime_error if locking the mutex failed.
inline MutexGuard::MutexGuard(Mutex& mutex) : mutex_(mutex.get())
{
  GetLock();
}

/// @brief Lock an @ref etcpal_mutex_t.
/// @throw std::runtime_error if locking the mutex failed.
inline MutexGuard::MutexGuard(etcpal_mutex_t& mutex) : mutex_(mutex)
{
  GetLock();
}

/// @brief Release the lock upon going out-of-scope.
inline MutexGuard::~MutexGuard()
{
  etcpal_mutex_unlock(&mutex_);
}

inline void MutexGuard::GetLock()
{
  if (!etcpal_mutex_lock(&mutex_))
    ETCPAL_THROW(std::runtime_error("etcpal_mutex_lock failed."));
}

/// @ingroup etcpal_cpp_lock
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
  ReadGuard(ReadGuard&& other) = delete;
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

/// @ingroup etcpal_cpp_lock
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
  WriteGuard(WriteGuard&& other) = delete;
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

/// @}

};  // namespace etcpal

#endif  // ETCPAL_CPP_LOCK_H_
