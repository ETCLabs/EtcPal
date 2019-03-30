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
#include "lwpatest.h"
#include <string.h>
#include <stdarg.h>
#include "lwpa_log.h"

static void *expect_context;
static bool expect_syslog_str_present, expect_human_str_present;
static char expect_syslog_str[LWPA_SYSLOG_STR_MAX_LEN];
static char expect_human_str[LWPA_HUMAN_LOG_STR_MAX_LEN];
static char expect_raw_str[LWPA_LOG_MSG_MAX_LEN];
static bool log_passed_flag;

static void log_cb(void *context, const char *syslog_str, const char *human_str, const char *raw_str)
{
  if (context != expect_context || !raw_str || 0 != strcmp(raw_str, expect_raw_str) ||
      (expect_syslog_str_present && (!syslog_str || 0 != strcmp(syslog_str, expect_syslog_str))) ||
      (!expect_syslog_str_present && syslog_str) ||
      (expect_human_str_present && (!human_str || 0 != strcmp(human_str, expect_human_str))) ||
      (!expect_human_str_present && human_str))
  {
    log_passed_flag = false;
  }
  else
  {
    log_passed_flag = true;
  }
}

LwpaLogTimeParams tparams;
bool time_fn_called;

static void time_cb(void *context, LwpaLogTimeParams *time_params)
{
  (void)context;
  *time_params = tparams;
}

static void fill_default_time()
{
  tparams.year = 1970;     // absolute year
  tparams.month = 1;       // month of the year - [1, 12]
  tparams.day = 1;         // day of the month - [1, 31]
  tparams.hour = 0;        // hours since midnight - [0, 23]
  tparams.minute = 0;      // minutes after the hour - [0, 59]
  tparams.second = 0;      // seconds after the minute - [0, 60] including leap second
  tparams.msec = 0;        // milliseconds after the second - [0, 999]
  tparams.utc_offset = 0;  // Local offset from UTC in minutes
}

static void init_flags()
{
  log_passed_flag = false;
  expect_syslog_str_present = false;
  expect_human_str_present = false;
  time_fn_called = false;
  fill_default_time();
}

bool test_log_sanitize()
{
  LwpaSyslogParams syslog_params;
  const unsigned char special_char_array[] = {
      0x01, 0x10, 0x7f,  // Some non-printing chars
      0x41, 0x42, 0x43,  // "ABC"
      0x80, 0x81, 0xff,  // Some characters out of the ASCII range
      0x00               // Null terminator
  };
  memcpy(syslog_params.app_name, special_char_array, sizeof special_char_array);
  memcpy(syslog_params.hostname, special_char_array, sizeof special_char_array);
  memcpy(syslog_params.procid, special_char_array, sizeof special_char_array);
  syslog_params.facility = -1;

  lwpa_sanitize_syslog_params(&syslog_params);
  if ((0 != strcmp(syslog_params.app_name, "___ABC???")) || (0 != strcmp(syslog_params.hostname, "___ABC???")) ||
      (LWPA_LOG_FAC(syslog_params.facility) >= LWPA_LOG_NFACILITIES))
  {
    return false;
  }
  return true;
}

bool test_log_validate()
{
  LwpaLogParams lparams;
  const unsigned char special_char_array[] = {
      0x01, 0x10, 0x7f,  // Some non-printing chars
      0x41, 0x42, 0x43,  // "ABC"
      0x80, 0x81, 0xff,  // Some characters out of the ASCII range
      0x00               // Null terminator
  };

  // Test some normal params
  lparams.action = kLwpaLogCreateSyslog;
  lparams.log_fn = log_cb;
  lparams.syslog_params.hostname[0] = '\0';
  lparams.syslog_params.procid[0] = '\0';
  lparams.syslog_params.facility = LWPA_LOG_KERN;
  memcpy(lparams.syslog_params.app_name, special_char_array, sizeof special_char_array);
  lparams.log_mask = 0;
  lparams.time_fn = time_cb;
  lparams.context = NULL;

  // Make sure the validation returned true and the syslog field got sanitized
  if (!lwpa_validate_log_params(&lparams))
    return false;

  if (0 != strcmp(lparams.syslog_params.app_name, "___ABC???"))
    return false;

  // Invalid log callback
  lparams.log_fn = NULL;
  if (lwpa_validate_log_params(&lparams))
    return false;

  // It's valid for time_fn to be NULL
  lparams.log_fn = log_cb;
  lparams.time_fn = NULL;
  if (!lwpa_validate_log_params(&lparams))
    return false;

  return true;
}

// Test logging of:
//    - int values
//    - time from callback
//    - weird and missing value in syslog header
bool test_log_intval()
{
  LwpaLogParams lparams;
  // A string with a non-ASCII character: "host\xC8name"
  // Should be sanitized to "host?name"
  const unsigned char weird_hostname[] = {0x68, 0x6f, 0x73, 0x74, 0xc8, 0x6e, 0x61, 0x6d, 0x65, 0x00};
  char syslog_buf[LWPA_SYSLOG_STR_MAX_LEN];
  char human_buf[LWPA_HUMAN_LOG_STR_MAX_LEN];

  init_flags();

  lparams.action = kLwpaLogCreateSyslog;
  lparams.log_fn = log_cb;
  memcpy(lparams.syslog_params.hostname, weird_hostname, sizeof weird_hostname);
  strcpy(lparams.syslog_params.app_name, "My_App");
  lparams.syslog_params.procid[0] = '\0';
  lparams.syslog_params.facility = LWPA_LOG_KERN;
  lparams.log_mask = 0;
  lparams.time_fn = time_cb;
  lparams.context = NULL;

  strcpy(expect_syslog_str,
         "<0>1 1970-01-01T00:00:00.000Z host?name My_App - - - Here are some int values: 1 42 4294967295");
  strcpy(expect_human_str, "1970-01-01 00:00:00.000Z Here are some int values: 1 42 4294967295");
  strcpy(expect_raw_str, "Here are some int values: 1 42 4294967295");

  if (!lwpa_validate_log_params(&lparams))
    return false;

#define INTVAL_FORMAT_STR_AND_ARGS "Here are some int values: %d %d %u", 1, 42, 0xffffffffu

  // Try the functions that simply build the log strings
  if (!lwpa_create_syslog_str(syslog_buf, LWPA_SYSLOG_STR_MAX_LEN, &tparams, &lparams.syslog_params, LWPA_LOG_EMERG,
                              INTVAL_FORMAT_STR_AND_ARGS))
  {
    return false;
  }

  if (0 != strcmp(syslog_buf, expect_syslog_str))
    return false;

  if (!lwpa_create_human_log_str(human_buf, LWPA_HUMAN_LOG_STR_MAX_LEN, &tparams, INTVAL_FORMAT_STR_AND_ARGS))
  {
    return false;
  }

  if (0 != strcmp(human_buf, expect_human_str))
    return false;

  // Try logging with the log mask set to 0, should not work.
  if (lwpa_canlog(&lparams, LWPA_LOG_EMERG))
    return false;
  lwpa_log(&lparams, LWPA_LOG_EMERG, INTVAL_FORMAT_STR_AND_ARGS);
  if (log_passed_flag)
    return false;

  // Try logging only syslog
  expect_syslog_str_present = true;
  expect_human_str_present = false;
  lparams.log_mask = LWPA_LOG_UPTO(LWPA_LOG_EMERG);
  lwpa_log(&lparams, LWPA_LOG_EMERG, INTVAL_FORMAT_STR_AND_ARGS);
  if (!log_passed_flag)
    return false;

  // Try logging both
  log_passed_flag = false;
  lparams.action = kLwpaLogCreateBoth;
  expect_human_str_present = true;
  lwpa_log(&lparams, LWPA_LOG_EMERG, INTVAL_FORMAT_STR_AND_ARGS);
  if (!log_passed_flag)
    return false;

  // Try logging only human-readable
  log_passed_flag = false;
  lparams.action = kLwpaLogCreateHumanReadableLog;
  expect_syslog_str_present = false;
  lwpa_log(&lparams, LWPA_LOG_EMERG, INTVAL_FORMAT_STR_AND_ARGS);
  if (!log_passed_flag)
    return false;

  return true;
}

// Used by the following tests. Try using lwpa_vlog() to log various combinations of syslog and
// human-readable logging.
bool vlog_helper(LwpaLogParams *lparams, int pri, const char *format, ...)
{
  va_list args;
  va_start(args, format);

  // Try logging only syslog
  log_passed_flag = false;
  lparams->action = kLwpaLogCreateSyslog;
  expect_syslog_str_present = true;
  expect_human_str_present = false;
  lwpa_vlog(lparams, pri, format, args);
  if (!log_passed_flag)
    return false;

  // Try logging both
  log_passed_flag = false;
  lparams->action = kLwpaLogCreateBoth;
  expect_human_str_present = true;
  lwpa_vlog(lparams, pri, format, args);
  if (!log_passed_flag)
    return false;

  // Try logging only human-readable
  log_passed_flag = false;
  lparams->action = kLwpaLogCreateHumanReadableLog;
  expect_syslog_str_present = false;
  lwpa_vlog(lparams, pri, format, args);
  if (!log_passed_flag)
    return false;

  va_end(args);
  return true;
}

// Test logging of:
//    - string values
//    - no time
//    - weird and missing value in syslog header
bool test_log_strval()
{
  LwpaLogParams lparams;
  // A string with some non-printing and non-ASCII characters: "\x012\x034\xff"
  const unsigned char weird_procid[] = {0x01, 0x32, 0x03, 0x34, 0xff, 0x00};
  char syslog_buf[LWPA_SYSLOG_STR_MAX_LEN];
  char human_buf[LWPA_HUMAN_LOG_STR_MAX_LEN];

  init_flags();

  lparams.action = kLwpaLogCreateSyslog;
  lparams.log_fn = log_cb;
  strcpy(lparams.syslog_params.hostname, "10.101.17.38");
  memcpy(lparams.syslog_params.procid, weird_procid, sizeof weird_procid);
  lparams.syslog_params.app_name[0] = '\0';
  lparams.syslog_params.facility = LWPA_LOG_LOCAL2;
  lparams.log_mask = 0;
  lparams.time_fn = NULL;
  lparams.context = NULL;

  strcpy(expect_syslog_str, "<149>1 - 10.101.17.38 - _2_4? - - Here are some string values: hey wassup hello");
  strcpy(expect_human_str, "Here are some string values: hey wassup hello");
  strcpy(expect_raw_str, "Here are some string values: hey wassup hello");

  if (!lwpa_validate_log_params(&lparams))
    return false;

#define STRVAL_FORMAT_STR_AND_ARGS "Here are some string values: %s %s %s", "hey", "wassup", "hello"

  // Try the functions that simply build the log strings
  if (!lwpa_create_syslog_str(syslog_buf, LWPA_SYSLOG_STR_MAX_LEN, NULL, &lparams.syslog_params, LWPA_LOG_NOTICE,
                              STRVAL_FORMAT_STR_AND_ARGS))
  {
    return false;
  }

  if (0 != strcmp(syslog_buf, expect_syslog_str))
    return false;

  if (!lwpa_create_human_log_str(human_buf, LWPA_HUMAN_LOG_STR_MAX_LEN, NULL, STRVAL_FORMAT_STR_AND_ARGS))
  {
    return false;
  }

  if (0 != strcmp(human_buf, expect_human_str))
    return false;

  // Try logging with the log mask set to 0, should not work.
  if (lwpa_canlog(&lparams, LWPA_LOG_NOTICE))
    return false;

  lwpa_log(&lparams, LWPA_LOG_NOTICE, STRVAL_FORMAT_STR_AND_ARGS);
  if (log_passed_flag)
    return false;

  // Now try the actual logging using lwpa_vlog().
  lparams.log_mask = LWPA_LOG_UPTO(LWPA_LOG_NOTICE);
  return vlog_helper(&lparams, LWPA_LOG_NOTICE, STRVAL_FORMAT_STR_AND_ARGS);
}

// Helper to get the proper sanitized character from a loop counter for the
// max length string test
static unsigned char get_sanitized_char(size_t i)
{
  unsigned char ret = (i % 128 == 0 ? 1 : i % 128);

  // We also need to avoid '%' because it is the printf format specifier.
  if (ret < 33 || ret == 127 || ret == '%')
    ret = '_';
  else if (ret > 127)
    ret = '?';
  return ret;
}

// Test logging a maximum length string.
bool test_log_maxlength()
{
  LwpaLogParams lparams;
  char syslog_buf[LWPA_SYSLOG_STR_MAX_LEN];
  char human_buf[LWPA_HUMAN_LOG_STR_MAX_LEN];
  char *expect_syslog_str_ptr;
  char *expect_human_str_ptr;
  char *expect_raw_str_ptr;

  init_flags();

  lparams.action = kLwpaLogCreateSyslog;
  lparams.log_fn = log_cb;
  lparams.syslog_params.facility = LWPA_LOG_LOCAL7;
  lparams.log_mask = 0;
  lparams.time_fn = time_cb;
  lparams.context = NULL;

#define SYSLOG_TIME_HEADER "<191>1 1970-01-01T00:00:00.000-12:00 "
#define HUMAN_TIME_HEADER "1970-01-01 00:00:00.000-12:00 "

  strcpy(expect_syslog_str, SYSLOG_TIME_HEADER);
  expect_syslog_str_ptr = expect_syslog_str + sizeof(SYSLOG_TIME_HEADER) - 1;
  strcpy(expect_human_str, HUMAN_TIME_HEADER);
  expect_human_str_ptr = expect_human_str + sizeof(HUMAN_TIME_HEADER) - 1;
  expect_raw_str_ptr = expect_raw_str;

  // Create our very long syslog header components
  size_t i;
  for (i = 0; i < LWPA_LOG_HOSTNAME_MAX_LEN - 1; ++i)
  {
    char to_add = get_sanitized_char(i);
    lparams.syslog_params.hostname[i] = to_add;
    *expect_syslog_str_ptr++ = to_add;
  }
  lparams.syslog_params.hostname[i] = '\0';
  *expect_syslog_str_ptr++ = ' ';

  for (i = 0; i < LWPA_LOG_APP_NAME_MAX_LEN - 1; ++i)
  {
    char to_add = get_sanitized_char(i);
    lparams.syslog_params.app_name[i] = to_add;
    *expect_syslog_str_ptr++ = to_add;
  }

  lparams.syslog_params.app_name[i] = '\0';
  *expect_syslog_str_ptr++ = ' ';

  for (i = 0; i < LWPA_LOG_PROCID_MAX_LEN - 1; ++i)
  {
    char to_add = get_sanitized_char(i);
    lparams.syslog_params.procid[i] = to_add;
    *expect_syslog_str_ptr++ = to_add;
  }
  lparams.syslog_params.procid[i] = '\0';
  strcpy(expect_syslog_str_ptr, " - - ");
  expect_syslog_str_ptr += sizeof(" - - ") - 1;

  char to_log_str[LWPA_LOG_MSG_MAX_LEN];
  for (i = 0; i < LWPA_LOG_MSG_MAX_LEN - 1; ++i)
  {
    char to_add = get_sanitized_char(i);
    *expect_syslog_str_ptr++ = to_add;
    *expect_human_str_ptr++ = to_add;
    *expect_raw_str_ptr++ = to_add;
    to_log_str[i] = to_add;
  }
  to_log_str[i] = '\0';

  if (!lwpa_validate_log_params(&lparams))
    return false;

  // We want to have a non-zero, two-digit UTC offset for the maximum length possible.
  tparams.utc_offset = -720;

  // Try the functions that simply build the log strings
  if (!lwpa_create_syslog_str(syslog_buf, LWPA_SYSLOG_STR_MAX_LEN, &tparams, &lparams.syslog_params, LWPA_LOG_DEBUG,
                              to_log_str))
  {
    return false;
  }

  if (0 != strcmp(syslog_buf, expect_syslog_str))
    return false;

  if (!lwpa_create_human_log_str(human_buf, LWPA_HUMAN_LOG_STR_MAX_LEN, &tparams, to_log_str))
    return false;

  if (0 != strcmp(human_buf, expect_human_str))
    return false;

  // Try logging with the log mask set to 0, should not work.
  if (lwpa_canlog(&lparams, LWPA_LOG_DEBUG))
    return false;

  lwpa_log(&lparams, LWPA_LOG_DEBUG, STRVAL_FORMAT_STR_AND_ARGS);
  if (log_passed_flag)
    return false;

  // Now try the actual logging using lwpa_vlog().
  lparams.log_mask = LWPA_LOG_UPTO(LWPA_LOG_DEBUG);
  return vlog_helper(&lparams, LWPA_LOG_DEBUG, to_log_str);
}

void test_log()
{
  bool ok;
  OUTPUT_TEST_MODULE_BEGIN("log");
  watchdog_kick();
  OUTPUT_TEST_BEGIN("log_sanitize");
  OUTPUT_TEST_RESULT((ok = test_log_sanitize()));
  if (ok)
  {
    watchdog_kick();
    OUTPUT_TEST_BEGIN("log_validate");
    OUTPUT_TEST_RESULT((ok = test_log_validate()));
  }
  if (ok)
  {
    watchdog_kick();
    OUTPUT_TEST_BEGIN("log_int_value");
    OUTPUT_TEST_RESULT((ok = test_log_intval()));
  }
  if (ok)
  {
    watchdog_kick();
    OUTPUT_TEST_BEGIN("log_str_value");
    OUTPUT_TEST_RESULT((ok = test_log_strval()));
  }
  if (ok)
  {
    watchdog_kick();
    OUTPUT_TEST_BEGIN("log_max_length");
    OUTPUT_TEST_RESULT((ok = test_log_maxlength()));
  }
  OUTPUT_TEST_MODULE_END("log", ok);
}
