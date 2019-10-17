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

/// \file etcpal/cpp/log.h
/// \brief C++ wrapper and utilities for etcpal/log.h

#ifndef ETCPAL_CPP_LOG_H_
#define ETCPAL_CPP_LOG_H_

#include "etcpal/log.h"
#include "etcpal/cpp/lock.h"

namespace etcpal
{
/// \defgroup etcpal_cpp_log etcpal_cpp_log
/// \ingroup etcpal_cpp
/// \brief C++ utilities for the \ref etcpal_log module.

/// \ingroup etcpal_cpp_log
/// \brief A class for dispatching log messages.
class Logger
{
public:
  /// Options for the method by which the Logger dispatches log messages.
  enum class DispatchPolicy
  {
    kDirect,  ///< Log messages propagate directly from Log() calls to output streams (normally only used for testing)
    kQueued   ///< Log messages are queued and dispatched from another thread (recommended)
  };

  explicit Logger(DispatchPolicy dispatch_policy = DispatchPolicy::kQueued);
  virtual ~Logger();

  bool Startup(int log_mask);
  void Shutdown();

  // Log a message
  void Log(int pri, const char* format, ...);

  // Shortcuts to log at a specific priority level
  void Debug(const char* format, ...);
  void Info(const char* format, ...);
  void Notice(const char* format, ...);
  void Warning(const char* format, ...);
  void Error(const char* format, ...);
  void Critical(const char* format, ...);
  void Alert(const char* format, ...);
  void Emergency(const char* format, ...);

  const EtcPalLogParams& GetLogParams() const;

  virtual void GetTimestamp(EtcPalLogTimeParams& time) = 0;
  virtual void OutputLogMessage(const std::string& str) = 0;

protected:
  EtcPalLogParams log_params_;

  const DispatchPolicy dispatch_policy_{DispatchPolicy::kQueued};
};
};  // namespace etcpal

#endif  // ETCPAL_CPP_LOG_H_
