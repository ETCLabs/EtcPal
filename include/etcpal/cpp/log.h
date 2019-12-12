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

#include <memory>
#include <queue>
#include <string>
#include "etcpal/log.h"
#include "etcpal/cpp/lock.h"
#include "etcpal/cpp/thread.h"

namespace etcpal
{
/// \defgroup etcpal_cpp_log log (Logging)
/// \ingroup etcpal_cpp
/// \brief C++ utilities for the \ref etcpal_log module.

class LogMessageHandler
{
public:
  virtual void GetLogTimestamp(EtcPalLogTimeParams& time) = 0;
  virtual void HandleLogMessage(const EtcPalLogStrings& strings) = 0;
};

/// Options for the method by which the Logger dispatches log messages.
enum class LogDispatchPolicy
{
  Direct,  ///< Log messages propagate directly from Log() calls to output streams (normally only used for testing)
  Queued   ///< Log messages are queued and dispatched from another thread (recommended)
};

/// \ingroup etcpal_cpp_log
/// \brief A class for dispatching log messages.
class Logger
{
public:
  explicit Logger(LogDispatchPolicy dispatch_policy = LogDispatchPolicy::Queued);

  bool Startup(LogMessageHandler& message_handler);
  void Shutdown();

  bool CanLog(int pri) const noexcept;
  void Log(int pri, const char* format, ...);

  void Debug(const char* format, ...);
  void Info(const char* format, ...);
  void Notice(const char* format, ...);
  void Warning(const char* format, ...);
  void Error(const char* format, ...);
  void Critical(const char* format, ...);
  void Alert(const char* format, ...);
  void Emergency(const char* format, ...);

  Logger& SetLogMask(int log_mask) noexcept;
  Logger& SetLogAction(etcpal_log_action_t log_action) noexcept;
  Logger& SetSyslogFacility(int facility) noexcept;
  Logger& SetSyslogHostname(const char* hostname) noexcept;
  Logger& SetSyslogAppName(const char* app_name) noexcept;
  Logger& SetSyslogProcId(const char* proc_id) noexcept;

  int log_mask() const noexcept;
  etcpal_log_action_t log_action() const noexcept;
  int syslog_facility() const noexcept;
  const char* syslog_hostname() const noexcept;
  const char* syslog_app_name() const noexcept;
  const char* syslog_procid() const noexcept;
  const EtcPalLogParams& log_params() const noexcept;

private:
  LogDispatchPolicy dispatch_policy_;

  LogMessageHandler* handler_{nullptr};

  EtcPalLogParams log_params_{};

  struct LogStrings
  {
    char human_readable[ETCPAL_LOG_STR_MAX_LEN];
    char syslog[ETCPAL_SYSLOG_STR_MAX_LEN];
    char raw[ETCPAL_RAW_LOG_MSG_MAX_LEN];
  };

  // Used when dispatch_policy_ == Queued
  std::queue<LogStrings> msg_q_;
  std::unique_ptr<etcpal::Signal> signal_;
  std::unique_ptr<etcpal::Mutex> mutex_;
  etcpal::Thread thread_;
  bool keep_running_{false};
};
};  // namespace etcpal

#endif  // ETCPAL_CPP_LOG_H_
