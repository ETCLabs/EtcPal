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
 * This file is a part of lwpa. For more information, go to:
 * https://github.com/ETCLabs/lwpa
 ******************************************************************************/

#include "lwpa/log.h"

#include <stdio.h>
#include <stddef.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include "lwpa/bool.h"
#include "lwpa/lock.h"
#include "lwpa/mempool.h"
#include "lwpa/private/log.h"

#ifdef _MSC_VER
/* Suppress strncpy() warnings on Windows/MSVC. */
#pragma warning(disable : 4996)

/* Provide va_copy() in pre-VS2013 environments. */
#if _MSC_VER < 1800
#define va_copy(dest, src) (dest = src)
#endif
#endif

/* This module uses some C99 features:
 * - vsnprintf()
 * - va_copy()
 * These additional exceptions to the normal C89 rules are permissible here because there are
 * workarounds for VS.
 */

/*************************** Private constants *******************************/

#define SYSLOG_PROT_VERSION 1 /*RFC 5424, sec. 6.2.2 */
#define NILVALUE_STR "-"
#define MSGID_STR NILVALUE_STR
#define STRUCTURED_DATA_STR NILVALUE_STR
/* LOG_LOCAL1 seems to be one that's lightly used */
#define DEFAULT_FACILITY LWPA_LOG_LOCAL1

/**************************** Private variables ******************************/

static unsigned int init_count;
static lwpa_mutex_t buf_lock;

/*********************** Private function prototypes *************************/

static void sanitize_str(char* str);

/*************************** Function definitions ****************************/

/* Initialize the lwpa_log module. Creates the mutex which locks the static buffers that log
 * messages are written into. */
lwpa_error_t lwpa_log_init()
{
  if (init_count == 0)
  {
    if (!lwpa_mutex_create(&buf_lock))
    {
      return kLwpaErrSys;
    }
  }
  ++init_count;
  return kLwpaErrOk;
}

/* Deinitialize the lwpa_log module. */
void lwpa_log_deinit()
{
  if (--init_count == 0)
  {
    lwpa_mutex_destroy(&buf_lock);
  }
}

/* Replace non-printing characters and spaces with '_'. Replace characters above 127 with '?'. */
void sanitize_str(char* str)
{
  // C library functions like isprint()/isgraph() are not used here because their behavior is not
  // well-defined in the presence of non-ASCII characters.
  unsigned char* cp;
  for (cp = (unsigned char*)str; *cp != '\0'; ++cp)
  {
    if (*cp < 33 || *cp == 127)
      *cp = '_';
    else if (*cp > 127)
      *cp = '?';
  }
}

/*! \brief Ensure that the given syslog parameters are compliant with the syslog RFC (modifying
 *         them if necessary).
 *
 *  Sanitizes the three string fields (hostname, app_name and procid) by replacing characters that
 *  are not allowed by RFC 5424 with filler characters. Also ensures that the facility value is
 *  within the correct range (#LWPA_LOG_NFACILITIES).
 *
 *  \param[in,out] params Syslog params to sanitize.
 */
void lwpa_sanitize_syslog_params(LwpaSyslogParams* params)
{
  if (LWPA_LOG_FAC(params->facility) >= LWPA_LOG_NFACILITIES)
    params->facility = DEFAULT_FACILITY;

  sanitize_str(params->hostname);
  sanitize_str(params->app_name);
  sanitize_str(params->procid);
}

/*! \brief Ensure that the given lwpa_log_params are valid.
 *
 *  This also sanitizes the syslog params using lwpa_sanitize_syslog_params() if action is set to
 *  kLwpaLogCreateSyslog or kLwpaLogCreateBoth.
 *
 *  \param[in,out] params lwpa_log_params to validate.
 *  \return true (params are valid) or false (params are invalid).
 */
bool lwpa_validate_log_params(LwpaLogParams* params)
{
  if (!params || !params->log_fn)
  {
    return false;
  }

  if (params->action == kLwpaLogCreateSyslog || params->action == kLwpaLogCreateBoth)
  {
    lwpa_sanitize_syslog_params(&params->syslog_params);
  }
  return true;
}

/* Enforce the range rules defined in the LwpaLogTimeParams struct definition. */
static bool validate_time(const LwpaLogTimeParams* tparams)
{
  return (tparams->year >= 0 && tparams->year <= 9999 && tparams->month >= 1 && tparams->month <= 12 &&
          tparams->day >= 1 && tparams->day <= 31 && tparams->hour >= 0 && tparams->hour <= 23 &&
          tparams->minute >= 0 && tparams->minute <= 59 && tparams->second >= 0 && tparams->second <= 60 &&
          tparams->msec >= 0 && tparams->msec <= 999);
}

/* Build the current timestamp. Buffer must be of length LWPA_LOG_TIMESTAMP_LEN. */
static void make_timestamp(const LwpaLogTimeParams* tparams, char* buf, bool human_readable)
{
  bool timestamp_created = false;

  if (tparams && validate_time(tparams))
  {
    // Print the basic timestamp
    int print_res = snprintf(
        buf, LWPA_LOG_TIMESTAMP_LEN,
        human_readable ? "%04d-%02d-%02d %02d:%02d:%02d.%03d" : "%04d-%02d-%02dT%02d:%02d:%02d.%03d", tparams->year,
        tparams->month, tparams->day, tparams->hour, tparams->minute, tparams->second, tparams->msec);

    if (print_res > 0 && print_res < LWPA_LOG_TIMESTAMP_LEN - 1)
    {
      // Add the UTC offset
      if (tparams->utc_offset == 0)
      {
        buf[print_res] = 'Z';
        buf[print_res + 1] = '\0';
      }
      else
      {
        snprintf(&buf[print_res], LWPA_LOG_TIMESTAMP_LEN - (size_t)print_res, "%s%02d:%02d",
                 tparams->utc_offset > 0 ? "+" : "-", abs(tparams->utc_offset) / 60, abs(tparams->utc_offset) % 60);
      }

      timestamp_created = true;
    }
  }

  if (!timestamp_created)
  {
    if (!human_readable)
      strcpy(buf, NILVALUE_STR);
    else
      buf[0] = '\0';
  }
}

/* Get the current time via either the standard C library or a time callback. */
static bool get_time(const LwpaLogParams* params, LwpaLogTimeParams* time_params)
{
  if (params->time_fn)
  {
    memset(time_params, 0, sizeof(LwpaLogTimeParams));
    params->time_fn(params->context, time_params);
    return true;
  }
  else
  {
    return false;
  }
}

/* Create a log message with syslog header given the appropriate va_list. Returns a pointer to the
 * original message within the syslog message, or NULL on failure. */
static char* lwpa_vcreate_syslog_str(char* buf, size_t buflen, const LwpaLogTimeParams* tparams,
                                     const LwpaSyslogParams* syslog_params, int pri, const char* format, va_list args)
{
  if (!buf || buflen < LWPA_SYSLOG_HEADER_MAX_LEN || !syslog_params || !format)
    return NULL;

  char timestamp[LWPA_LOG_TIMESTAMP_LEN];
  make_timestamp(tparams, timestamp, false);

  int prival = LWPA_LOG_PRI(pri) | syslog_params->facility;
  int syslog_header_size =
      snprintf(buf, LWPA_SYSLOG_HEADER_MAX_LEN, "<%d>%d %s %s %s %s %s %s ", prival, SYSLOG_PROT_VERSION, timestamp,
               syslog_params->hostname[0] ? syslog_params->hostname : NILVALUE_STR,
               syslog_params->app_name[0] ? syslog_params->app_name : NILVALUE_STR,
               syslog_params->procid[0] ? syslog_params->procid : NILVALUE_STR, MSGID_STR, STRUCTURED_DATA_STR);

  if (syslog_header_size >= 0)
  {
    // Copy in the message. vsnprintf will write up to count - 1 bytes and always null-terminates.
    // This allows LWPA_LOG_MSG_MAX_LEN valid bytes to be written.
    vsnprintf(&buf[syslog_header_size], buflen - (size_t)syslog_header_size, format, args);
    return &buf[syslog_header_size];
  }
  else
  {
    return NULL;
  }
}

/*! \brief Create a log message with syslog header in the given buffer.
 *
 *  Buffer must be at least #LWPA_SYSLOG_STR_MIN_LEN in length.
 *
 *  \param[out] buf Buffer in which to build the syslog message.
 *  \param[in] buflen Length in bytes of buf.
 *  \param[in] time A set of time parameters representing the current time. If NULL, no timestamp
 *                  will be added to the log message.
 *  \param[in] syslog_params A set of parameters for the syslog header.
 *  \param[in] pri Priority of this log message.
 *  \param[in] format Log message with printf-style format specifiers. Provide additional arguments
 *                    as appropriate for format specifiers.
 */
bool lwpa_create_syslog_str(char* buf, size_t buflen, const LwpaLogTimeParams* time,
                            const LwpaSyslogParams* syslog_params, int pri, const char* format, ...)
{
  va_list args;
  bool res;
  va_start(args, format);
  res = (NULL != lwpa_vcreate_syslog_str(buf, buflen, time, syslog_params, pri, format, args));
  va_end(args);
  return res;
}

/* Create a log message with a human-readable header given the appropriate va_list. Returns a
 * pointer to the original message within the log message, or NULL on failure. */
static char* lwpa_vcreate_human_log_str(char* buf, size_t buflen, const LwpaLogTimeParams* time, const char* format,
                                        va_list args)
{
  if (!buf || buflen < LWPA_LOG_TIMESTAMP_LEN + 1 || !format)
    return NULL;

  char timestamp[LWPA_LOG_TIMESTAMP_LEN];
  make_timestamp(time, timestamp, true);

  int human_header_size;
  if (timestamp[0] == '\0')
    human_header_size = 0;
  else
    human_header_size = snprintf(buf, LWPA_LOG_TIMESTAMP_LEN + 1, "%s ", timestamp);

  if (human_header_size >= 0)
  {
    // Copy in the message. vsnprintf will write up to count - 1 bytes and always null-terminates.
    // This allows LWPA_LOG_MSG_MAX_LEN valid bytes to be written.
    vsnprintf(&buf[human_header_size], buflen - (size_t)human_header_size, format, args);
    return &buf[human_header_size];
  }
  else
  {
    return NULL;
  }
}

/*! \brief Create a log message with a human-readable header in the given
 *         buffer.
 *
 *  Buffer must be at least #LWPA_HUMAN_LOG_STR_MIN_LEN in length.
 *
 *  \param[out] buf Buffer in which to build the log message.
 *  \param[in] buflen Length in bytes of buf.
 *  \param[in] time A set of time parameters representing the current time. If NULL, no timestamp
 *                  will be added to the log message.
 *  \param[in] format Log message with printf-style format specifiers. Provide additional arguments
 *                    as appropriate for format specifiers.
 */
bool lwpa_create_human_log_str(char* buf, size_t buflen, const LwpaLogTimeParams* time, const char* format, ...)
{
  va_list args;
  bool res;
  va_start(args, format);
  res = (NULL != lwpa_vcreate_human_log_str(buf, buflen, time, format, args));
  va_end(args);
  return res;
}

/*! \brief Determine whether a priority level can be logged given the mask present in the log
 *         params.
 *
 *  This is useful to use as a guard around doing any conversions that are only for logging, e.g.
 *  converting addresses to strings -- if the priority of the message can't be logged, then those
 *  conversions are just wasted work.
 *
 *  \param[in] params The log parameters to be checked.
 *  \param[in] pri Priority to check.
 *  \return Whether this priority will be logged with the current mask setting.
 */
bool lwpa_can_log(const LwpaLogParams* params, int pri)
{
  if (params)
    return ((LWPA_LOG_MASK(pri) & params->log_mask) != 0);
  return false;
}

/*! \brief Log a message from a library module.
 *
 *  Takes a printf-style format string which is formatted and passed to the application callback.
 *
 *  \param[in] params The log parameters to be used for this message.
 *  \param[in] pri Priority of this log message.
 *  \param[in] format Log message with printf-style format specifiers. Provide additional arguments
 *                    as appropriate for format specifiers.
 */
void lwpa_log(const LwpaLogParams* params, int pri, const char* format, ...)
{
  va_list args;
  va_start(args, format);
  lwpa_vlog(params, pri, format, args);
  va_end(args);
}

/*! \brief Log a message from a library module with the list of format arguments already generated.
 *
 *  For normal usage, just use lwpa_log(). However, this function is useful if you want to create a
 *  wrapper function around lwpa_log() which also takes variable format arguments.
 *
 *  \param[in] params The log parameters to be used for this message.
 *  \param[in] pri Priority of this log message.
 *  \param[in] format Log message with printf-style format specifiers.
 *  \param[in] args Argument list for the format specifiers in format.
 */
void lwpa_vlog(const LwpaLogParams* params, int pri, const char* format, va_list args)
{
  if (!init_count || !params || !params->log_fn || !format || !(LWPA_LOG_MASK(pri) & params->log_mask))
    return;

  LwpaLogTimeParams time_params;
  bool have_time = get_time(params, &time_params);

  if (lwpa_mutex_take(&buf_lock))
  {
    static char syslogmsg[LWPA_SYSLOG_STR_MAX_LEN + 1];
    static char humanlogmsg[LWPA_HUMAN_LOG_STR_MAX_LEN + 1];
    LwpaLogStrings strings = {NULL, NULL, NULL};

    if (params->action == kLwpaLogCreateBoth || params->action == kLwpaLogCreateSyslog)
    {
      // If we are calling both vcreate functions, we will need to copy the va_list.
      // For more info on using a va_list multiple times, see:
      // https://wiki.sei.cmu.edu/confluence/display/c/MSC39-C.+Do+not+call+va_arg%28%29+on+a+va_list+that+has+an+indeterminate+value
      // https://stackoverflow.com/a/26919307
      if (params->action == kLwpaLogCreateBoth)
      {
        va_list args_copy;
        va_copy(args_copy, args);
        strings.raw = lwpa_vcreate_syslog_str(syslogmsg, LWPA_SYSLOG_STR_MAX_LEN + 1, have_time ? &time_params : NULL,
                                              &params->syslog_params, pri, format, args_copy);
        va_end(args_copy);
      }
      else
      {
        strings.raw = lwpa_vcreate_syslog_str(syslogmsg, LWPA_SYSLOG_STR_MAX_LEN + 1, have_time ? &time_params : NULL,
                                              &params->syslog_params, pri, format, args);
      }
      if (strings.raw)
      {
        strings.syslog = syslogmsg;
      }
    }

    if (params->action == kLwpaLogCreateBoth || params->action == kLwpaLogCreateHumanReadableLog)
    {
      strings.raw = lwpa_vcreate_human_log_str(humanlogmsg, LWPA_HUMAN_LOG_STR_MAX_LEN + 1,
                                               have_time ? &time_params : NULL, format, args);
      if (strings.raw)
      {
        strings.human_readable = humanlogmsg;
      }
    }

    params->log_fn(params->context, &strings);

    lwpa_mutex_give(&buf_lock);
  }
}
