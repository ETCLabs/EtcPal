/******************************************************************************
 * Copyright 2018 ETC Inc.
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
 * This file is a part of lwpa. For more information, go to:
 * https://github.com/ETCLabs/lwpa
 ******************************************************************************/

/* lwpa_log.h: Common logging functions. */
#ifndef _LWPA_LOG_H_
#define _LWPA_LOG_H_

#include <time.h>
#include <stdarg.h>
#include "lwpa_bool.h"

/*! \defgroup lwpa_log lwpa_log
 *  \ingroup lwpa
 *  \brief A platform-neutral module enabling applications and libraries to log
 *         messages in either or both of syslog-compliant and human-readable
 *         format.
 *
 *  \#include "lwpa_log.h"
 *
 *  This module can be used in two ways. Applications can use the lightweight
 *  lwpa_create_syslog_str() and lwpa_create_human_log_str() to create log
 *  messages with a header defined by the Syslog protocol or with a
 *  human-readable header defined by ETC.
 *
 *  This module can also be used to enable other libraries to log messages via
 *  a callback function. Library functions can take a set of parameters
 *  (lwpa_log_params) on initialization. They use these parameters and the
 *  lwpa_log() or lwpa_vlog() functions to call back to the application to log
 *  messages. The application can then decide what to do with these log
 *  messages (print to console, syslog, etc.)
 *
 *  @{
 */

/* clang-format off */

/*! \name Log Facility
 *  @{
 */
#define LWPA_LOG_KERN (0 << 3)      /*!< Kernel messages. */
#define LWPA_LOG_USER (1 << 3)      /*!< Random user-level messages. */
#define LWPA_LOG_MAIL (2 << 3)      /*!< Mail system. */
#define LWPA_LOG_DAEMON (3 << 3)    /*!< System daemons. */
#define LWPA_LOG_AUTH (4 << 3)      /*!< Security/authorization messages. */
#define LWPA_LOG_SYSLOG (5 << 3)    /*!< Messages generated internally by
                                         syslogd. */
#define LWPA_LOG_LPR (6 << 3)       /*!< Line printer subsystem. */
#define LWPA_LOG_NEWS (7 << 3)      /*!< Network news subsystem. */
#define LWPA_LOG_UUCP (8 << 3)      /*!< UUCP subsystem. */
#define LWPA_LOG_CRON (9 << 3)      /*!< Clock daemon. */
#define LWPA_LOG_AUTHPRIV (10 << 3) /*!< Security/authorization messages. */
#define LWPA_LOG_FTP (11 << 3)      /*!< FTP daemon. */

#define LWPA_LOG_LOCAL0 (16 << 3) /*!< Reserved for local use. */
#define LWPA_LOG_LOCAL1 (17 << 3) /*!< Reserved for local use. */
#define LWPA_LOG_LOCAL2 (18 << 3) /*!< Reserved for local use. */
#define LWPA_LOG_LOCAL3 (19 << 3) /*!< Reserved for local use. */
#define LWPA_LOG_LOCAL4 (20 << 3) /*!< Reserved for local use. */
#define LWPA_LOG_LOCAL5 (21 << 3) /*!< Reserved for local use. */
#define LWPA_LOG_LOCAL6 (22 << 3) /*!< Reserved for local use. */
#define LWPA_LOG_LOCAL7 (23 << 3) /*!< Reserved for local use. */
/*!@}*/

#define LWPA_LOG_NFACILITIES 24 /*!< Current number of facilities. */
#define LWPA_LOG_FACMASK 0x03f8 /* Mask to extract facility part of a
                                   prival. */
#define LWPA_LOG_FAC(p) (((p)&LWPA_LOG_FACMASK) >> 3)

/*! \name Log Priority
 *  @{
 */
#define LWPA_LOG_EMERG 0   /*!< System is unusable. */
#define LWPA_LOG_ALERT 1   /*!< Action must be taken immediately. */
#define LWPA_LOG_CRIT 2    /*!< Critical conditions. */
#define LWPA_LOG_ERR 3     /*!< Error conditions. */
#define LWPA_LOG_WARNING 4 /*!< Warning conditions. */
#define LWPA_LOG_NOTICE 5  /*!< Normal but significant condition. */
#define LWPA_LOG_INFO 6    /*!< Informational. */
#define LWPA_LOG_DEBUG 7   /*!< Debug-level messages. */
/*!@}*/

#define LWPA_LOG_PRIMASK 0x07 /* mask to extract priority part (internal) */
#define LWPA_LOG_PRI(p) ((p)&LWPA_LOG_PRIMASK) /* extract priority */
#define LWPA_LOG_MAKEPRI(fac, pri) (((fac) << 3) | (pri))

#define LWPA_LOG_MASK(pri) (1 << (pri)) /* mask for one priority */
#define LWPA_LOG_UPTO(pri) \
  ((1 << ((pri) + 1)) - 1) /* all priorities through pri */

#define LWPA_LOG_HOSTNAME_MAX_LEN 256 /*!< Max length of the hostname param. */
#define LWPA_LOG_APP_NAME_MAX_LEN 49  /*!< Max length of the app_name param. */
#define LWPA_LOG_PROCID_MAX_LEN 129   /*!< Max length of the procid param. */

/*! Max length of a log message string passed to lwpa_log() or lwpa_vlog(). */
#define LWPA_LOG_MSG_MAX_LEN 480

/* clang-format on */

/*! The maximum length of the timestamp used in syslog and human-readable
 *  logging. */
#define LWPA_LOG_TIMESTAMP_LEN (10 /*Date*/ + 1 /*T*/ + 12 /*Time*/ + 6 /*Offset*/ + 1 /*Nullterm*/)

/*! The maximum length of the syslog header. */
#define LWPA_SYSLOG_HEADER_MAX_LEN                                                                   \
  (5 /*PRIVAL*/ + 3 /*Version*/ + 1 /*SP*/ + (LWPA_LOG_TIMESTAMP_LEN - 1) /*Timestamp*/ + 1 /*SP*/ + \
   (LWPA_LOG_HOSTNAME_MAX_LEN - 1) + 1 /*SP*/ + (LWPA_LOG_APP_NAME_MAX_LEN - 1) + 1 /*SP*/ +         \
   (LWPA_LOG_PROCID_MAX_LEN - 1) + 1 /*SP*/ + 1 /*MSGID*/ + 1 /*SP*/ + 1 /*STRUCTURED-DATA*/ + 1 /*SP*/)

/*! The minimum length of a buffer passed to lwpa_create_syslog_str(). */
#define LWPA_SYSLOG_STR_MIN_LEN LWPA_SYSLOG_HEADER_MAX_LEN
/*! The minimum length of a buffer passed to lwpa_create_human_log_str(). */
#define LWPA_HUMAN_LOG_STR_MIN_LEN LWPA_LOG_TIMESTAMP_LEN

/*! The maximum length of a syslog string that will be passed to an
 *  lwpa_log_callback function.  */
#define LWPA_SYSLOG_STR_MAX_LEN (LWPA_SYSLOG_HEADER_MAX_LEN + LWPA_LOG_MSG_MAX_LEN)

/*! The maximum length of a human-readable string that will be passed to an
 *  lwpa_log_callback function.  */
#define LWPA_HUMAN_LOG_STR_MAX_LEN ((LWPA_LOG_TIMESTAMP_LEN - 1) + 1 /*SP*/ + LWPA_LOG_MSG_MAX_LEN)

/*! A set of parameters which represent the current local time with millisecond
 *  resolution. */
typedef struct LwpaLogTimeParams
{
  /*! The current time as specified in the C standard. Note that only the
   *  tm_sec, tm_min, tm_hour, tm_mday, tm_mon, and tm_year members are used by
   *  this module. */
  struct tm cur_time;
  /*! The number of milliseconds past the current second. */
  unsigned int msec;
  /*! The UTC offset in minutes. */
  long utc_offset;
} LwpaLogTimeParams;

/*! \brief Log callback function.
 *
 *  The function that library modules use to log messages. The application
 *  developer defines the function and determines where the messages go.
 *
 *  \param[in] context Optional application-provided value that was previously
 *                     passed to the library module.
 *  \param[in] syslog_str Log string, formatted compliant to RFC 5424.
 *  \param[in] human_str Log string, formatted for readability per ETC
 *                       convention.
 */
typedef void (*lwpa_log_callback)(void *context, const char *syslog_str, const char *human_str);

/*! \brief Time callback function.
 *
 *  A function by which the logging module can get the current local time.
 *
 *  \param[in] context Optional application-provided value that was previously
 *                     passed to the library module.
 *  \param[out] time Fill this in with the current time.
 *  \param[out] msec Fill this in with the current time past the second in
 *                   milliseconds, if it is available. Otherwise 0 ms will be
 *                   assumed.
 *  \param[out] utc_offset Fill this in with the UTC offset in minutes, if it
 *                         is available. Otherwise UTC will be assumed.
 */
typedef void (*lwpa_log_time_fn)(void *context, LwpaLogTimeParams *time_params);

/*! The method the lwpa_log() and lwpa_vlog() functions use to create a
 *  timestamp. */
typedef enum
{
  /*! lwpa_log() and lwpa_vlog() use the C library gmtime() internally. Note:
   *  only UTC and whole-second precision is possible with gmtime(), although
   *  milliseconds will still be present in the timestamp as '.000'. */
  kLwpaLogUseGmtime,
  /*! lwpa_log() and lwpa_vlog() use a time callback function to get the
   *  current time from the application when creating a timestamp. */
  kLwpaLogUseTimeFn,
  /*! lwpa_log() and lwpa_vlog() omit the timestamp when building log
   *  messages. */
  kLwpaLogNoTime
} lwpa_log_time_method_t;

/*! Which types of log message(s) the lwpa_log() and lwpa_vlog() functions
 *  create. */
typedef enum
{
  /*! lwpa_log() and lwpa_vlog() create a syslog message and pass it back in
   *  the syslog_str parameter of the log callback. */
  kLwpaLogCreateSyslog,
  /*! lwpa_log() and lwpa_vlog() create a human-readable log message and pass
   *  it back in the human_str parameter of the log callback. */
  kLwpaLogCreateHumanReadableLog,
  /*! lwpa_log() and lwpa_vlog() create both a syslog message and a
   *  human-readable log message and pass them back in the syslog_str and
   *  human_str parameters of the log callback. */
  kLwpaLogCreateBoth
} lwpa_log_action_t;

/*! A set of parameters for the syslog header. */
typedef struct LwpaSyslogParams
{
  /*! Syslog Facility; see RFC 5424 &sect; 6.2.1. */
  int facility;
  /*! Syslog HOSTNAME; see RFC 5424 &sect; 6.2.4. */
  char hostname[LWPA_LOG_HOSTNAME_MAX_LEN];
  /*! Syslog APP-NAME; see RFC 5424 &sect; 6.2.5. */
  char app_name[LWPA_LOG_APP_NAME_MAX_LEN];
  /*! Syslog PROCID; see RFC 5424 &sect; 6.2.6. */
  char procid[LWPA_LOG_PROCID_MAX_LEN];
} LwpaSyslogParams;

/*! A set of parameters used for the lwpa_*log() functions. */
typedef struct LwpaLogParams
{
  /*! What should be done when lwpa_*log() is called. */
  lwpa_log_action_t action;
  /*! A callback function for the finished log string(s). */
  lwpa_log_callback log_fn;
  /*! The syslog header parameters. */
  LwpaSyslogParams syslog_params;
  /*! A mask value that determines which priority messages can be logged. */
  int log_mask;
  /*! How the lwpa_*log() functions should obtain the current time. */
  lwpa_log_time_method_t time_method;
  /*! A callback function for lwpa_*log() functions to obtain the time from the
   *  application. */
  lwpa_log_time_fn time_fn;
  /*! Application context that will be passed back with the log callback
   *  function. */
  void *context;
} LwpaLogParams;

#define lwpa_setlogmask(logparamsptr, newlogmask) ((logparamsptr)->log_mask = newlogmask)

#define lwpa_canlog(logparamsptr, pri) ((LWPA_LOG_MASK(pri) & (logparamsptr)->log_mask) != 0)

#ifdef __cplusplus
extern "C" {
#endif

bool lwpa_create_syslog_str(char *buf, size_t buflen, const LwpaLogTimeParams *time,
                            const LwpaSyslogParams *syslog_params, int pri, const char *format, ...)
#ifdef __GNUC__
    /* GCC sorcery to check the format string just like a 'printf' call */
    __attribute__((__format__(__printf__, 6, 7)))
#endif
    ;
bool lwpa_create_human_log_str(char *buf, size_t buflen, const LwpaLogTimeParams *time, const char *format, ...)
#ifdef __GNUC__
    __attribute__((__format__(__printf__, 4, 5)))
#endif
    ;

void lwpa_sanitize_syslog_params(LwpaSyslogParams *params);
bool lwpa_validate_log_params(LwpaLogParams *params);
void lwpa_log(const LwpaLogParams *params, int pri, const char *format, ...)
#ifdef __GNUC__
    __attribute__((__format__(__printf__, 3, 4)))
#endif
    ;
void lwpa_vlog(const LwpaLogParams *params, int pri, const char *format, va_list args);

#ifdef __cplusplus
}
#endif

/*!@}*/

#endif /* _LWPA_LOG_H_ */
