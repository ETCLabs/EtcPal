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

/* etcpal/log.h: Common logging functions. */

#ifndef ETCPAL_LOG_H_
#define ETCPAL_LOG_H_

#include <stdarg.h>
#include <stddef.h>
#include "etcpal/bool.h"
#include "etcpal/common.h"

/*!
 * \defgroup etcpal_log Logging (log)
 * \ingroup etcpal
 * \brief A platform-neutral module enabling applications and libraries to log messages in either
 *        or both of syslog-compliant and human-readable format.
 *
 * ```c
 * #include "etcpal/log.h"
 * ```
 *
 * This module can be used in two ways. Applications can use the lightweight
 * etcpal_create_syslog_str() and etcpal_create_human_log_str() to create log messages with a header
 * defined by the Syslog protocol or with a human-readable header defined by ETC.
 *
 * This module can also be used to enable other libraries to log messages via a callback function.
 * Library functions can take a set of parameters (etcpal_log_params) on initialization. They use
 * these parameters and the etcpal_log() or etcpal_vlog() functions to call back to the application to
 * log messages. The application can then decide what to do with these log messages (print to
 * console, syslog, etc.)
 *
 * @{
 */

/* clang-format off */

/*!
 * \name Log Facility
 * @{
 */
#define ETCPAL_LOG_KERN (0 << 3)      /*!< Kernel messages. */
#define ETCPAL_LOG_USER (1 << 3)      /*!< Random user-level messages. */
#define ETCPAL_LOG_MAIL (2 << 3)      /*!< Mail system. */
#define ETCPAL_LOG_DAEMON (3 << 3)    /*!< System daemons. */
#define ETCPAL_LOG_AUTH (4 << 3)      /*!< Security/authorization messages. */
#define ETCPAL_LOG_SYSLOG (5 << 3)    /*!< Messages generated internally by syslogd. */
#define ETCPAL_LOG_LPR (6 << 3)       /*!< Line printer subsystem. */
#define ETCPAL_LOG_NEWS (7 << 3)      /*!< Network news subsystem. */
#define ETCPAL_LOG_UUCP (8 << 3)      /*!< UUCP subsystem. */
#define ETCPAL_LOG_CRON (9 << 3)      /*!< Clock daemon. */
#define ETCPAL_LOG_AUTHPRIV (10 << 3) /*!< Security/authorization messages. */
#define ETCPAL_LOG_FTP (11 << 3)      /*!< FTP daemon. */

#define ETCPAL_LOG_LOCAL0 (16 << 3) /*!< Reserved for local use. */
#define ETCPAL_LOG_LOCAL1 (17 << 3) /*!< Reserved for local use. */
#define ETCPAL_LOG_LOCAL2 (18 << 3) /*!< Reserved for local use. */
#define ETCPAL_LOG_LOCAL3 (19 << 3) /*!< Reserved for local use. */
#define ETCPAL_LOG_LOCAL4 (20 << 3) /*!< Reserved for local use. */
#define ETCPAL_LOG_LOCAL5 (21 << 3) /*!< Reserved for local use. */
#define ETCPAL_LOG_LOCAL6 (22 << 3) /*!< Reserved for local use. */
#define ETCPAL_LOG_LOCAL7 (23 << 3) /*!< Reserved for local use. */
/*!
 * @}
 */

#define ETCPAL_LOG_NFACILITIES 24 /*!< Current number of facilities. */
/*! \cond facmask */
#define ETCPAL_LOG_FACMASK 0x03f8 /* Mask off the facility part of a prival */
/*! \endcond */
#define ETCPAL_LOG_FAC(p) (((p) & ETCPAL_LOG_FACMASK) >> 3) /*!< Extract the facility part of a prival. */

/*!
 * \name Log Priority
 * @{
 */
#define ETCPAL_LOG_EMERG 0   /*!< System is unusable. */
#define ETCPAL_LOG_ALERT 1   /*!< Action must be taken immediately. */
#define ETCPAL_LOG_CRIT 2    /*!< Critical conditions. */
#define ETCPAL_LOG_ERR 3     /*!< Error conditions. */
#define ETCPAL_LOG_WARNING 4 /*!< Warning conditions. */
#define ETCPAL_LOG_NOTICE 5  /*!< Normal but significant condition. */
#define ETCPAL_LOG_INFO 6    /*!< Informational. */
#define ETCPAL_LOG_DEBUG 7   /*!< Debug-level messages. */
/*!
 * @}
 */

/*! \cond primask */
#define ETCPAL_LOG_PRIMASK 0x07 /* mask off the priority part of a prival */
/*! \endcond */
#define ETCPAL_LOG_PRI(p) ((p) & ETCPAL_LOG_PRIMASK) /*!< Extract the priority part of a prival. */
#define ETCPAL_LOG_MAKEPRI(fac, pri) (((fac) << 3) | (pri)) /*!< Make a prival from a facility and priority value. */

#define ETCPAL_LOG_MASK(pri) (1 << (pri)) /*!< Create a priority mask for one priority. */
#define ETCPAL_LOG_UPTO(pri) ((1 << ((pri) + 1)) - 1) /*!< Create a priority mask for all priorities through pri. */

#define ETCPAL_LOG_HOSTNAME_MAX_LEN 256u /*!< Max length of the hostname param. */
#define ETCPAL_LOG_APP_NAME_MAX_LEN 49u  /*!< Max length of the app_name param. */
#define ETCPAL_LOG_PROCID_MAX_LEN 129u   /*!< Max length of the procid param. */

/*! Max length of a log message string passed to etcpal_log() or etcpal_vlog(). */
#define ETCPAL_LOG_MSG_MAX_LEN 480u

/* clang-format on */

/*! The maximum length of the timestamp used in syslog and human-readable logging. */
#define ETCPAL_LOG_TIMESTAMP_LEN (10u /*Date*/ + 1u /*T*/ + 12u /*Time*/ + 6u /*Offset*/ + 1u /*Nullterm*/)

/*! The maximum length of the syslog header. */
#define ETCPAL_SYSLOG_HEADER_MAX_LEN                                                                        \
  (5u /*PRIVAL*/ + 3u /*Version*/ + 1u /*SP*/ + (ETCPAL_LOG_TIMESTAMP_LEN - 1u) /*Timestamp*/ + 1u /*SP*/ + \
   (ETCPAL_LOG_HOSTNAME_MAX_LEN - 1u) + 1u /*SP*/ + (ETCPAL_LOG_APP_NAME_MAX_LEN - 1u) + 1u /*SP*/ +        \
   (ETCPAL_LOG_PROCID_MAX_LEN - 1u) + 1u /*SP*/ + 1u /*MSGID*/ + 1u /*SP*/ + 1u /*STRUCTURED-DATA*/ + 1u /*SP*/)

/*! The minimum length of a buffer passed to etcpal_create_syslog_str(). */
#define ETCPAL_SYSLOG_STR_MIN_LEN ETCPAL_SYSLOG_HEADER_MAX_LEN
/*! The minimum length of a buffer passed to etcpal_create_human_log_str(). */
#define ETCPAL_HUMAN_LOG_STR_MIN_LEN ETCPAL_LOG_TIMESTAMP_LEN

/*! The maximum length of a syslog string that will be passed to an etcpal_log_callback function. */
#define ETCPAL_SYSLOG_STR_MAX_LEN (ETCPAL_SYSLOG_HEADER_MAX_LEN + ETCPAL_LOG_MSG_MAX_LEN)

/*! The maximum length of a human-readable string that will be passed to an etcpal_log_callback
 *  function.  */
#define ETCPAL_HUMAN_LOG_STR_MAX_LEN ((ETCPAL_LOG_TIMESTAMP_LEN - 1u) + 1u /*SP*/ + ETCPAL_LOG_MSG_MAX_LEN)

/*! A set of parameters which represent the current local time with millisecond resolution. */
typedef struct EtcPalLogTimeParams
{
  int year;       /*!< Absolute year. Valid range 0-9999. */
  int month;      /*!< Month of the year. Valid range 1-12 (starting with 1 for January). */
  int day;        /*!< Day of the month. Valid range 1-31.  */
  int hour;       /*!< Hours since midnight. Valid range 0-23. */
  int minute;     /*!< Minutes past the current hour. Valid range 0-59. */
  int second;     /*!< Seconds past the current minute. Valid range 0-60 (to handle leap seconds). */
  int msec;       /*!< Milliseconds past the current second. Valid range 0-999. */
  int utc_offset; /*!< The local offset from UTC in minutes. */
} EtcPalLogTimeParams;

/*! The set of log strings passed with a call to an etcpal_log_callback function. Any members not
 *  requested in the corresponding EtcPalLogParams struct will be NULL.
 */
typedef struct EtcPalLogStrings
{
  /*! Log string formatted compliant to RFC 5424. */
  const char* syslog;
  /*! Log string formatted for readability per ETC convention. */
  const char* human_readable;
  /*! The original log string that was passed to etcpal_log() or etcpal_vlog(). Will overlap with one of
   * syslog_str or human_str. */
  const char* raw;
} EtcPalLogStrings;

/*!
 * \brief Log callback function.
 *
 * The function that library modules use to log messages. The application developer defines the
 * function and determines where the messages go.
 *
 * <b>Do not call etcpal_log() or etcpal_vlog() from this function; a deadlock will result.</b>
 *
 * \param[in] context Optional application-provided value that was previously passed to the library
 *                    module.
 * \param[in] strings Strings associated with the log message. Will contain valid strings
 *                    corresponding to the log actions requested in the corresponding EtcPalLogParams
 *                    struct.
 */
typedef void (*etcpal_log_callback)(void* context, const EtcPalLogStrings* strings);

/*!
 * \brief Time callback function.
 *
 * A function by which the logging module can get the current local time.
 *
 * \param[in] context Optional application-provided value that was previously passed to the library
 *                    module.
 * \param[out] time_params Fill this in with the current local time.
 */
typedef void (*etcpal_log_time_fn)(void* context, EtcPalLogTimeParams* time_params);

/*! Which types of log message(s) the etcpal_log() and etcpal_vlog() functions create. */
typedef enum
{
  /*! etcpal_log() and etcpal_vlog() create a syslog message and pass it back in the syslog_str
   *  parameter of the log callback. */
  kEtcPalLogCreateSyslog,
  /*! etcpal_log() and etcpal_vlog() create a human-readable log message and pass it back in the
   *  human_str parameter of the log callback. */
  kEtcPalLogCreateHumanReadableLog,
  /*! etcpal_log() and etcpal_vlog() create both a syslog message and a human-readable log message and
   *  pass them back in the syslog_str and human_str parameters of the log callback. */
  kEtcPalLogCreateBoth
} etcpal_log_action_t;

/*! A set of parameters for the syslog header. */
typedef struct EtcPalSyslogParams
{
  /*! Syslog Facility; see RFC 5424 &sect; 6.2.1. */
  int facility;
  /*! Syslog HOSTNAME; see RFC 5424 &sect; 6.2.4. */
  char hostname[ETCPAL_LOG_HOSTNAME_MAX_LEN];
  /*! Syslog APP-NAME; see RFC 5424 &sect; 6.2.5. */
  char app_name[ETCPAL_LOG_APP_NAME_MAX_LEN];
  /*! Syslog PROCID; see RFC 5424 &sect; 6.2.6. */
  char procid[ETCPAL_LOG_PROCID_MAX_LEN];
} EtcPalSyslogParams;

/*! A set of parameters used for the etcpal_*log() functions. */
typedef struct EtcPalLogParams
{
  /*! What should be done when etcpal_log() or etcpal_vlog() is called. */
  etcpal_log_action_t action;
  /*! A callback function for the finished log string(s). */
  etcpal_log_callback log_fn;
  /*! The syslog header parameters. */
  EtcPalSyslogParams syslog_params;
  /*! A mask value that determines which priority messages can be logged. */
  int log_mask;
  /*! A callback function for the etcpal_log() and etcpal_vlog() functions to obtain the time from the
   *  application. If NULL, no timestamp will be added to log messages. */
  etcpal_log_time_fn time_fn;
  /*! Application context that will be passed back with the log callback function. */
  void* context;
} EtcPalLogParams;

#ifdef __cplusplus
extern "C" {
#endif

/* The various compiler ifdefs are to use each compiler's method of checking printf-style
 * arguments. */

#ifdef __ICCARM__
#pragma __printf_args
#endif
bool etcpal_create_syslog_str(char* buf, size_t buflen, const EtcPalLogTimeParams* time,
                              const EtcPalSyslogParams* syslog_params, int pri, const char* format, ...)
#ifdef __GNUC__
    __attribute__((__format__(__printf__, 6, 7)))
#endif
    ;

#ifdef __ICCARM__
#pragma __printf_args
#endif
bool etcpal_create_human_log_str(char* buf, size_t buflen, const EtcPalLogTimeParams* time, const char* format, ...)
#ifdef __GNUC__
    __attribute__((__format__(__printf__, 4, 5)))
#endif
    ;

void etcpal_sanitize_syslog_params(EtcPalSyslogParams* params);
bool etcpal_validate_log_params(EtcPalLogParams* params);

bool etcpal_can_log(const EtcPalLogParams* params, int pri);

#ifdef __ICCARM__
#pragma __printf_args
#endif
void etcpal_log(const EtcPalLogParams* params, int pri, const char* format, ...)
#ifdef __GNUC__
    __attribute__((__format__(__printf__, 3, 4)))
#endif
    ;

void etcpal_vlog(const EtcPalLogParams* params, int pri, const char* format, va_list args);

#ifdef __cplusplus
}
#endif

/*!
 * @}
 */

#endif /* ETCPAL_LOG_H_ */
