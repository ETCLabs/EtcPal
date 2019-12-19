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
#include "etcpal/cpp/inet.h"
#include "etcpal/cpp/thread.h"

namespace etcpal
{
/// \defgroup etcpal_cpp_log log (Logging)
/// \ingroup etcpal_cpp
/// \brief C++ utilities for the \ref etcpal_log module.
///
/// **WARNING:** This module must be explicitly initialized before use. Initialize the module by
/// calling etcpal_init() with the relevant feature mask:
/// \code
/// etcpal_init(ETCPAL_FEATURE_LOGGING);
/// \endcode
///
/// This module contains a class Logger which is used to gather log messages dispatched from
/// different portions of an application into a single stream with a unified format. It wraps the C
/// logging module, which provides the core of this functionality, in a convenience layer which can
/// also spawn a background thread to handle large log volumes. See the
/// \ref etcpal_log "documentation for the C module" for more detailed information on the log
/// format.
///
/// Generally a single instance per application is sufficient:
///
/// \code
/// // This class handles the log messages gathered by the Logger class.
/// class MyLogHandler : public etcpal::LogMessageHandler
/// {
///   EtcPalLogTimeParams GetLogTimestamp() override
///   {
///     // Grab a timestamp from the local system and return it
///   }
///
///   void HandleLogMessage(const EtcPalLogStrings& strings) override
///   {
///     // "strings" will contain strings pertaining to what action was requested by
///     // Logger::SetLogAction().
///
///     // Dispatch log messages in an application-defined way.
///
///     // If human-readable logging is enabled, maybe...
///     printf("%s\n", strings.human_readable);
///     // If syslog is enabled, maybe...
///     etcpal_sendto(my_syslog_socket, strings.syslog, strlen(strings.syslog), 0, &my_syslog_server);
///   }
/// };
///
/// MyLogHandler log_handler;
/// etcpal::Logger logger;
///
/// // Set parameters and start logging using the method-chaining setters:
/// logger.SetLogMask(ETCPAL_LOG_UPTO(ETCPAL_LOG_INFO))
///       .SetLogAction(kEtcPalLogCreateBoth)
///       .SetSyslogFacility(ETCPAL_LOG_LOCAL1)
///       .SetSyslogHostname(my_hostname)
///       .SetSyslogAppName("My App")
///       .SetSyslogProcId(0)
///       .Startup(log_handler);
///
/// // By default, a background thread is started to dispatch the log messages.
///
/// logger.Log(ETCPAL_LOG_INFO, "Starting up!");
///
/// // Use the shortcuts to log at a specific priority level:
///
/// logger.Warning("Uh oh...");
/// logger.Debug("We've reached line 250");
///
/// // All log functions accept printf-style format specifiers and arguments
/// logger.Info("This module has gone %d days without an accident", -1);
///
/// // Use the log_params() getter to add logging output from other libraries with C APIs that
/// // support it.
/// somelib_init(&logger.log_params());
/// \endcode

/// \ingroup etcpal_cpp_log
/// \brief An interface which handles log messages.
///
/// Users of the Logger class must provide a class that derives from LogMessageHandler to handle
/// the Logger's output. LogMessageHandler has two jobs: getting a current timestamp to apply to
/// log messages, and handling the final dispatched log message.
class LogMessageHandler
{
public:
  /// Return an EtcPalLogTimeParams representing the current local time.
  virtual EtcPalLogTimeParams GetLogTimestamp() = 0;

  /// \brief Define this function to handle log messages and determine what to do with them.
  ///
  /// If the corresponding Logger has a dispatch policy of LogDispatchPolicy::Direct, this function
  /// is invoked directly from the context of the corresponding call to Log() or similar. In this
  /// case, be mindful of whether this function implementation has potential to block. If
  /// significant blocking is a possibility while handling log messages, consider using
  /// LogDispatchPolicy::Queued.
  ///
  /// \param strings Strings associated with the log message. Will contain valid strings
  ///                corresponding to the log actions requested using Logger::SetLogAction().
  virtual void HandleLogMessage(const EtcPalLogStrings& strings) = 0;
};

/// \ingroup etcpal_cpp_log
/// \brief Options for the method by which the Logger dispatches log messages.
enum class LogDispatchPolicy
{
  Direct,  ///< Log messages propagate directly from Log() calls to output streams (normally only used for testing)
  Queued   ///< Log messages are queued and dispatched from another thread (recommended)
};

/// \ingroup etcpal_cpp_log
/// \brief A class for dispatching log messages.
///
/// See the long description for the \ref etcpal_cpp_log module for more detailed usage
/// information.
class Logger
{
public:
  bool Startup(LogMessageHandler& message_handler);
  void Shutdown();

  bool CanLog(int pri) const noexcept;
  void Log(int pri, const char* format, ...);

  /// \name Logging Shortcuts
  /// @{
  void Debug(const char* format, ...);
  void Info(const char* format, ...);
  void Notice(const char* format, ...);
  void Warning(const char* format, ...);
  void Error(const char* format, ...);
  void Critical(const char* format, ...);
  void Alert(const char* format, ...);
  void Emergency(const char* format, ...);
  /// @}

  /// \name Getters
  /// @{
  LogDispatchPolicy dispatch_policy() const noexcept;
  int log_mask() const noexcept;
  etcpal_log_action_t log_action() const noexcept;
  int syslog_facility() const noexcept;
  const char* syslog_hostname() const noexcept;
  const char* syslog_app_name() const noexcept;
  const char* syslog_procid() const noexcept;
  const EtcPalLogParams& log_params() const noexcept;
  /// @}

  /// \name Setters
  /// @{
  Logger& SetDispatchPolicy(LogDispatchPolicy new_policy) noexcept;
  Logger& SetLogMask(int log_mask) noexcept;
  Logger& SetLogAction(etcpal_log_action_t log_action) noexcept;
  Logger& SetSyslogFacility(int facility) noexcept;
  Logger& SetSyslogHostname(const char* hostname) noexcept;
  Logger& SetSyslogHostname(const std::string& hostname) noexcept;
  Logger& SetSyslogAppName(const char* app_name) noexcept;
  Logger& SetSyslogAppName(const std::string& app_name) noexcept;
  Logger& SetSyslogProcId(const char* proc_id) noexcept;
  Logger& SetSyslogProcId(const std::string& proc_id) noexcept;
  Logger& SetSyslogProcId(int proc_id) noexcept;
  /// @}

private:
  LogDispatchPolicy dispatch_policy_{LogDispatchPolicy::Queued};

  LogMessageHandler* handler_{nullptr};

  EtcPalLogParams log_params_{};

  // TODO explore ways to optimize memory usage
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

/// \brief Start logging.
///
/// Spawns a thread to dispatch log messages unless SetDispatchPolicy(LogDispatchPolicy::Direct)
/// has been called.
///
/// \param message_handler The class instance that will handle log messages from this logger.
inline bool Logger::Startup(LogMessageHandler& message_handler)
{
  // TODO
  return false;
}

/// \brief Stop logging.
///
/// If the dispatch policy is LogDispatchPolicy::Queued (the default) this will dispatch the rest
/// of the log messages and wait for the dispatch thread to join.
inline void Logger::Shutdown()
{
  // TODO
}

/// \brief Determine whether a priority level can be logged using the mask given via SetLogMask().
///
/// See etcpal_can_log() for more information.
inline bool Logger::CanLog(int pri) const noexcept
{
  // TODO
  return false;
}

/// \brief Log a message.
///
/// Only dispatches the log message if pri is part of the mask previously passed to SetLogMask().
///
/// \param pri The priority of this log message.
/// \param format Log message with printf-style format specifiers. Provide additional arguments as
///               appropriate.
inline void Logger::Log(int pri, const char* format, ...)
{
  // TODO
}

/// \brief Log a message at debug priority.
inline void Logger::Debug(const char* format, ...)
{
  // TODO
}

/// \brief Log a message at informational priority.
inline void Logger::Info(const char* format, ...)
{
  // TODO
}

/// \brief Log a message at notice priority.
inline void Logger::Notice(const char* format, ...)
{
  // TODO
}

/// \brief Log a message at warning priority.
inline void Logger::Warning(const char* format, ...)
{
  // TODO
}

/// \brief Log a message at error priority.
inline void Logger::Error(const char* format, ...)
{
  // TODO
}

/// \brief Log a message at critical priority.
inline void Logger::Critical(const char* format, ...)
{
  // TODO
}

/// \brief Log a message at alert priority.
inline void Logger::Alert(const char* format, ...)
{
  // TODO
}

/// \brief Log a message at emergency priority.
inline void Logger::Emergency(const char* format, ...)
{
  // TODO
}

/// \brief Get the current log dispatch policy.
inline LogDispatchPolicy Logger::dispatch_policy() const noexcept
{
  return dispatch_policy_;
}

/// \brief Get the current log mask.
inline int Logger::log_mask() const noexcept
{
  // TODO
  return 0;
}

/// \brief Get the current log action.
inline etcpal_log_action_t Logger::log_action() const noexcept
{
  // TODO
  return kEtcPalLogCreateHumanReadable;
}

/// \brief Get the current Syslog facility value.
inline int Logger::syslog_facility() const noexcept
{
  // TODO
  return 0;
}

/// \brief Get the current Syslog HOSTNAME.
inline const char* Logger::syslog_hostname() const noexcept
{
  // TODO
  return "";
}

/// \brief Get the current Syslog APP-NAME.
inline const char* Logger::syslog_app_name() const noexcept
{
  // TODO
  return "";
}

/// \brief Get the current Syslog PROCID.
inline const char* Logger::syslog_procid() const noexcept
{
  // TODO
  return "";
}

/// \brief Get the log params used by this logger.
///
/// This is convenient when interacting with C APIs which take an EtcPalLogParams instance to log
/// their own messages. Passing these params to those APIs will gather those log messages into this
/// logger instance.
inline const EtcPalLogParams& Logger::log_params() const noexcept
{
  // TODO
  return log_params_;
}

/// \brief Change the dispatch policy of this logger.
///
/// Only has any effect if the logger has not been started yet.
///
/// \param new_policy The new dispatch policy.
inline Logger& Logger::SetDispatchPolicy(LogDispatchPolicy new_policy) noexcept
{
  // TODO
  return *this;
}

/// \brief Set a new log mask.
///
/// Use the ETCPAL_LOG_UPTO() macro to create a mask up to and including a priority level. For
/// example, ETCPAL_LOG_UPTO(ETCPAL_LOG_WARNING) will allow all messages with priorites from
/// emergency through warning, inclusive.
///
/// \param log_mask The new log mask.
inline Logger& Logger::SetLogMask(int log_mask) noexcept
{
  // TODO
  return *this;
}

/// \brief Set the types of log messages to create and dispatch to the LogMessageHandler.
inline Logger& Logger::SetLogAction(etcpal_log_action_t log_action) noexcept
{
  // TODO
  return *this;
}

/// \brief Set the Syslog facility value; see RFC 5424 &sect; 6.2.1.
inline Logger& Logger::SetSyslogFacility(int facility) noexcept
{
  // TODO
  return *this;
}

/// \brief Set the Syslog HOSTNAME; see RFC 5424 &sect; 6.2.4.
inline Logger& Logger::SetSyslogHostname(const char* hostname) noexcept
{
  // TODO
  return *this;
}

/// \brief Set the Syslog HOSTNAME; see RFC 5424 &sect; 6.2.4.
inline Logger& Logger::SetSyslogHostname(const std::string& hostname) noexcept
{
  // TODO
  return *this;
}

/// \brief Set the Syslog APP-NAME; see RFC 5424 &sect; 6.2.5.
inline Logger& Logger::SetSyslogAppName(const char* app_name) noexcept
{
  // TODO
  return *this;
}

/// \brief Set the Syslog APP-NAME; see RFC 5424 &sect; 6.2.5.
inline Logger& Logger::SetSyslogAppName(const std::string& app_name) noexcept
{
  // TODO
  return *this;
}

/// \brief Set the Syslog PROCID; see RFC 5424 &sect; 6.2.6.
inline Logger& Logger::SetSyslogProcId(const char* proc_id) noexcept
{
  // TODO
  return *this;
}

/// \brief Set the Syslog PROCID; see RFC 5424 &sect; 6.2.6.
inline Logger& Logger::SetSyslogProcId(const std::string& proc_id) noexcept
{
  // TODO
  return *this;
}

/// \brief Set the Syslog PROCID; see RFC 5424 &sect; 6.2.6.
inline Logger& Logger::SetSyslogProcId(int proc_id) noexcept
{
  // TODO
  return *this;
}

};  // namespace etcpal

#endif  // ETCPAL_CPP_LOG_H_
