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

/// @file etcpal/cpp/sem.h
/// @brief C++ wrapper and utilities for etcpal/sem.h

#ifndef ETCPAL_CPP_SEM_H_
#define ETCPAL_CPP_SEM_H_

#include "etcpal/cpp/common.h"
#include "etcpal/sem.h"

namespace etcpal
{
/// @defgroup etcpal_cpp_sem sem (Counting Semaphores)
/// @ingroup etcpal_cpp
/// @brief C++ utilities for the @ref etcpal_sem module.
///
/// Provides C++ wrappers for the EtcPal counting semaphore type.

/// @ingroup etcpal_cpp_sem
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
  Semaphore(Semaphore&& other)                 = delete;
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

};  // namespace etcpal

#endif  // ETCPAL_CPP_SEM_H_
