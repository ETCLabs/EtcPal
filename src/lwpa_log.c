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

#include "lwpa_log.h"

#include <stdio.h>
#include <stddef.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "lwpa_bool.h"

/* Suppress strncpy() and gmtime() warnings on Windows/MSVC. */
#ifdef _MSC_VER
#pragma warning(disable : 4996)
#endif

#define SYSLOG_PROT_VERSION 1 /*RFC 5424, sec. 6.2.2 */
#define NILVALUE_STR "-"
#define MSGID_STR NILVALUE_STR
#define STRUCTURED_DATA_STR NILVALUE_STR
/* LOG_LOCAL1 seems to be one that's lightly used */
#define DEFAULT_FACILITY LWPA_LOG_LOCAL1

static void sanitize_str(char *str)
{
  /* Replace non-printing characters and spaces with '_'. Replace characters
   * above 127 with '?'. */
  unsigned char *cp;
  for (cp = (unsigned char *)str; *cp != '\0'; ++cp)
  {
    if (*cp < 33 || *cp == 127)
      *cp = '_';
    else if (*cp > 127)
      *cp = '?';
  }
}

/*! \brief Ensure that the given syslog parameters are compliant with the
 *         syslog RFC (modifying them if necessary).
 *
 *  Sanitizes the three string fields (hostname, app_name and procid) by
 *  replacing characters that are not allowed by RFC 5424 with filler
 *  characters. Also ensures that the facility value is within the correct
 *  range (#LWPA_LOG_NFACILITIES).
 *
 *  \param[in,out] params Syslog params to sanitize.
 */
void lwpa_sanitize_syslog_params(LwpaSyslogParams *params)
{
  if (LWPA_LOG_FAC(params->facility) >= LWPA_LOG_NFACILITIES)
    params->facility = DEFAULT_FACILITY;

  sanitize_str(params->hostname);
  sanitize_str(params->app_name);
  sanitize_str(params->procid);
}

/*! \brief Ensure that the given lwpa_log_params are valid.
 *
 *  This also sanitizes the syslog params using lwpa_sanitize_syslog_params()
 *  if action is set to kLwpaLogCreateSyslog or kLwpaLogCreateBoth.
 *
 *  Some examples of invalid log parameters are:
 *    * Log callback function is NULL
 *    * time_method is set to kLwpaLogUseTimeFn but time_fn is NULL
 *
 *  \param[in,out] params lwpa_log_params to validate.
 *  \return true (params are valid) or false (params are invalid).
 */
bool lwpa_validate_log_params(LwpaLogParams *params)
{
  if (!params || !params->log_fn || (params->time_method == kLwpaLogUseTimeFn && !params->time_fn))
  {
    return false;
  }

  if (params->action == kLwpaLogCreateSyslog || params->action == kLwpaLogCreateBoth)
  {
    lwpa_sanitize_syslog_params(&params->syslog_params);
  }
  return true;
}

/* Build the current timestamp. Buffer must be of length
 * LWPA_LOG_TIMESTAMP_LEN. */
static void make_timestamp(const LwpaLogTimeParams *time, char *buf, bool human_readable)
{
  if (time)
  {
    /* Print the basic timestamp */
    size_t pos = strftime(buf, LWPA_LOG_TIMESTAMP_LEN, human_readable ? "%Y-%m-%d %H:%M:%S" : "%Y-%m-%dT%H:%M:%S",
                          &time->cur_time);
    /* Add the millisecond value */
    snprintf(&buf[pos], LWPA_LOG_TIMESTAMP_LEN - pos, ".%03d", time->msec);
    pos += 4;
    /* Add the UTC offset */
    if (time->utc_offset == 0)
    {
      buf[pos] = 'Z';
      buf[pos + 1] = '\0';
    }
    else
    {
      snprintf(&buf[pos], LWPA_LOG_TIMESTAMP_LEN - pos, "%s%02d:%02d", time->utc_offset > 0 ? "+" : "-",
               abs(time->utc_offset) / 60, abs(time->utc_offset) % 60);
    }
  }
  else if (!human_readable)
    strcpy(buf, NILVALUE_STR);
  else
    buf[0] = '\0';
}

/* Get the current time via either the standard C library or a time callback.
 */
static bool get_time(const LwpaLogParams *params, LwpaLogTimeParams *time_params)
{
  bool got_time = false;
  switch (params->time_method)
  {
    case kLwpaLogUseGmtime:
    {
      time_t ct = time(NULL);
      struct tm *ptm = gmtime(&ct);
      if (ptm)
      {
        time_params->cur_time = *ptm;
        time_params->msec = 0;
        time_params->utc_offset = 0;
        got_time = true;
      }
    }
    break;
    case kLwpaLogUseTimeFn:
      if (params->time_fn)
      {
        time_params->msec = 0;
        time_params->utc_offset = 0;

        /* Get the time from the application */
        params->time_fn(params->context, time_params);
        got_time = true;
      }
      break;
    case kLwpaLogNoTime:
    default:
      break;
  }
  return got_time;
}

static bool lwpa_vcreate_syslog_str(char *buf, size_t buflen, const LwpaLogTimeParams *time,
                                    const LwpaSyslogParams *syslog_params, int pri, const char *format, va_list args)
{
  char timestamp[LWPA_LOG_TIMESTAMP_LEN];
  int prival = LWPA_LOG_PRI(pri) | syslog_params->facility;
  size_t syslog_header_size;

  if (buflen < LWPA_SYSLOG_HEADER_MAX_LEN)
    return false;

  make_timestamp(time, timestamp, false);

  syslog_header_size =
      snprintf(buf, LWPA_SYSLOG_HEADER_MAX_LEN, "<%d>%d %s %s %s %s %s %s ", prival, SYSLOG_PROT_VERSION, timestamp,
               syslog_params->hostname[0] ? syslog_params->hostname : NILVALUE_STR,
               syslog_params->app_name[0] ? syslog_params->app_name : NILVALUE_STR,
               syslog_params->procid[0] ? syslog_params->procid : NILVALUE_STR, MSGID_STR, STRUCTURED_DATA_STR);
  /* Copy in the message */
  /* Making an exception to the strict C89 rule here for safety. We have not
   * found a toolchain that doesn't support vsnprintf. vsnprintf will write
   * up to count - 1 bytes and always null-terminates. This allows
   * LWPA_LOG_MSG_MAX_LEN valid bytes to be written. */
  vsnprintf(&buf[syslog_header_size], buflen - syslog_header_size, format, args);
  return true;
}

/*! \brief Create a log message with syslog header in the given buffer.
 *
 *  Buffer must be at least #LWPA_SYSLOG_STR_MIN_LEN in length.
 *
 *  \param[out] buf Buffer in which to build the syslog message.
 *  \param[in] buflen Length in bytes of buf.
 *  \param[in] time A set of time parameters representing the current time.
 *  \param[in] syslog_params A set of parameters for the syslog header.
 *  \param[in] pri Priority of this log message.
 *  \param[in] format Log message with printf-style format specifiers. Provide
 *                    additional arguments as appropriate for format
 *                    specifiers.
 */
bool lwpa_create_syslog_str(char *buf, size_t buflen, const LwpaLogTimeParams *time,
                            const LwpaSyslogParams *syslog_params, int pri, const char *format, ...)
{
  va_list args;
  bool res;
  va_start(args, format);
  res = lwpa_vcreate_syslog_str(buf, buflen, time, syslog_params, pri, format, args);
  va_end(args);
  return res;
}

static bool lwpa_vcreate_human_log_str(char *buf, size_t buflen, const LwpaLogTimeParams *time, const char *format,
                                       va_list args)
{
  char timestamp[LWPA_LOG_TIMESTAMP_LEN];
  size_t human_header_size;

  if (buflen < LWPA_LOG_TIMESTAMP_LEN + 1)
    return false;

  make_timestamp(time, timestamp, true);

  if (timestamp[0] == '\0')
  {
    human_header_size = 0;
  }
  else
  {
    human_header_size = snprintf(buf, LWPA_LOG_TIMESTAMP_LEN + 1, "%s ", timestamp);
  }

  vsnprintf(&buf[human_header_size], buflen - human_header_size, format, args);
  return true;
}

/*! \brief Create a log message with a human-readable header in the given
 *         buffer.
 *
 *  Buffer must be at least #LWPA_HUMAN_LOG_STR_MIN_LEN in length.
 *
 *  \param[out] buf Buffer in which to build the log message.
 *  \param[in] buflen Length in bytes of buf.
 *  \param[in] time A set of time parameters representing the current time.
 *  \param[in] format Log message with printf-style format specifiers. Provide
 *                    additional arguments as appropriate for format
 *                    specifiers.
 */
bool lwpa_create_human_log_str(char *buf, size_t buflen, const LwpaLogTimeParams *time, const char *format, ...)
{
  va_list args;
  bool res;
  va_start(args, format);
  res = lwpa_vcreate_human_log_str(buf, buflen, time, format, args);
  va_end(args);
  return res;
}

/*! \brief Log a message from a library module.
 *
 *  Takes a printf-style format string which is formatted and passed to the
 *  application callback.
 *
 *  \param[in] params The log parameters to be used for this message.
 *  \param[in] pri Priority of this log message.
 *  \param[in] format Log message with printf-style format specifiers. Provide
 *                    additional arguments as appropriate for format
 *                    specifiers.
 */
void lwpa_log(const LwpaLogParams *params, int pri, const char *format, ...)
{
  va_list args;
  va_start(args, format);
  lwpa_vlog(params, pri, format, args);
  va_end(args);
}

/*! \brief Log a message from a library module with the list of format
 *         arguments already generated.
 *
 *  For normal usage, just use lwpa_log(). However, this function is useful if
 *  you want to create a wrapper function around lwpa_log() which also takes
 *  variable format arguments.
 *
 *  \param[in] params The log parameters to be used for this message.
 *  \param[in] pri Priority of this log message.
 *  \param[in] format Log message with printf-style format specifiers.
 *  \param[in] args Argument list for the format specifiers in format.
 */
void lwpa_vlog(const LwpaLogParams *params, int pri, const char *format, va_list args)
{
  char syslogmsg[LWPA_SYSLOG_STR_MAX_LEN + 1];
  char humanlogmsg[LWPA_HUMAN_LOG_STR_MAX_LEN + 1];
  char *syslog_msg_ptr = NULL;
  char *humanlog_msg_ptr = NULL;
  LwpaLogTimeParams time_params;
  bool have_time;

  if (!params || !params->log_fn || !format || !(LWPA_LOG_MASK(pri) & params->log_mask))
  {
    return;
  }

  have_time = get_time(params, &time_params);

  if (params->action == kLwpaLogCreateBoth || params->action == kLwpaLogCreateSyslog)
  {
    if (lwpa_vcreate_syslog_str(syslogmsg, LWPA_SYSLOG_STR_MAX_LEN + 1, have_time ? &time_params : NULL,
                                &params->syslog_params, pri, format, args))
    {
      syslog_msg_ptr = syslogmsg;
    }
  }
  if (params->action == kLwpaLogCreateBoth || params->action == kLwpaLogCreateHumanReadableLog)
  {
    if (lwpa_vcreate_human_log_str(humanlogmsg, LWPA_HUMAN_LOG_STR_MAX_LEN + 1, have_time ? &time_params : NULL, format,
                                   args))
    {
      humanlog_msg_ptr = humanlogmsg;
    }
  }

  params->log_fn(params->context, syslog_msg_ptr, humanlog_msg_ptr);
}
