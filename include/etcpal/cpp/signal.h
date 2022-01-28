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

/// @file etcpal/cpp/signal.h
/// @brief C++ wrapper and utilities for etcpal/signal.h

#ifndef ETCPAL_CPP_SIGNAL_H_
#define ETCPAL_CPP_SIGNAL_H_

#include "etcpal/cpp/common.h"
#include "etcpal/signal.h"

namespace etcpal
{
/// @defgroup etcpal_cpp_signal signal (Signal Objects)
/// @ingroup etcpal_cpp
/// @brief C++ utilities for the @ref etcpal_signal module.
///
/// Provides a C++ wrapper for the EtcPal signal type. This class is similar to some uses of
/// std::condition_variable, with the key advantage that it will work on any threaded platform that
/// EtcPal is ported for, including the embedded RTOS platforms. If your application or library
/// does not need to run on these platforms, consider using std::condition_variable instead.

/// @ingroup etcpal_cpp_signal
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
  Signal(Signal&& other)                 = delete;
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

};  // namespace etcpal

#endif  // ETCPAL_CPP_SIGNAL_H_
