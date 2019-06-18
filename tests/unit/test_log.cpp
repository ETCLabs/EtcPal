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
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <cstddef>
#include <cstring>
#include <cstdarg>
#include <string>

// Disable strcpy() warning on Windows/MSVC
#ifdef _MSC_VER
#pragma warning(disable : 4996)
#endif

using testing::Mock;

class LogTest : public ::testing::Test
{
protected:
  LogTest()
  {
    lwpa_init(LWPA_FEATURE_LOGGING);
    FillDefaultTime(cur_time);
  }
  ~LogTest() { lwpa_deinit(LWPA_FEATURE_LOGGING); }

  void TestLwpaVlogHelper(std::string expect_syslog_str, std::string expect_human_str, std::string expect_raw_str,
                          LwpaLogParams* lparams, int pri, const char* format, ...);

public:
  static void FillDefaultTime(LwpaLogTimeParams& tparams);

  MOCK_METHOD3(VerifyLogCallback, void(std::string syslog_str, std::string human_str, std::string raw_str));

  LwpaLogTimeParams cur_time;
  bool time_fn_called;
};

static void log_cb(void* context, const char* syslog_str, const char* human_str, const char* raw_str)
{
  LogTest* lt = static_cast<LogTest*>(context);
  if (lt)
  {
    lt->VerifyLogCallback(syslog_str ? syslog_str : "", human_str ? human_str : "", raw_str ? raw_str : "");
  }
}

static void time_cb(void* context, LwpaLogTimeParams* time_params)
{
  LogTest* lt = static_cast<LogTest*>(context);
  if (lt && time_params)
  {
    *time_params = lt->cur_time;
    lt->time_fn_called = true;
  }
}

void LogTest::FillDefaultTime(LwpaLogTimeParams& time_params)
{
  time_params.year = 1970;     // absolute year
  time_params.month = 1;       // month of the year - [1, 12]
  time_params.day = 1;         // day of the month - [1, 31]
  time_params.hour = 0;        // hours since midnight - [0, 23]
  time_params.minute = 0;      // minutes after the hour - [0, 59]
  time_params.second = 0;      // seconds after the minute - [0, 60] including leap second
  time_params.msec = 0;        // milliseconds after the second - [0, 999]
  time_params.utc_offset = 0;  // Local offset from UTC in minutes
}

// Test the LWPA_SET_LOG_MASK() and LWPA_CAN_LOG() macros
TEST_F(LogTest, log_mask)
{
  // LWPA_CAN_LOG() should always return false (and not crash) on a null pointer
  LwpaLogParams* params_ptr = NULL;
  EXPECT_FALSE(LWPA_CAN_LOG(params_ptr, LWPA_LOG_EMERG));

  LwpaLogParams params;

  // Test a zero mask
  LWPA_SET_LOG_MASK(&params, 0);
  EXPECT_FALSE(LWPA_CAN_LOG(&params, LWPA_LOG_EMERG));
  EXPECT_FALSE(LWPA_CAN_LOG(&params, LWPA_LOG_DEBUG));

  // Test some LOG_UPTO() values
  LWPA_SET_LOG_MASK(&params, LWPA_LOG_UPTO(LWPA_LOG_EMERG));
  EXPECT_TRUE(LWPA_CAN_LOG(&params, LWPA_LOG_EMERG));
  EXPECT_FALSE(LWPA_CAN_LOG(&params, LWPA_LOG_ALERT));
  EXPECT_FALSE(LWPA_CAN_LOG(&params, LWPA_LOG_DEBUG));

  LWPA_SET_LOG_MASK(&params, LWPA_LOG_UPTO(LWPA_LOG_DEBUG));
  EXPECT_TRUE(LWPA_CAN_LOG(&params, LWPA_LOG_EMERG));
  EXPECT_TRUE(LWPA_CAN_LOG(&params, LWPA_LOG_DEBUG));

  // Test a weird mask with only one middle value
  LWPA_SET_LOG_MASK(&params, LWPA_LOG_MASK(LWPA_LOG_ERR));
  EXPECT_FALSE(LWPA_CAN_LOG(&params, LWPA_LOG_EMERG));
  EXPECT_FALSE(LWPA_CAN_LOG(&params, LWPA_LOG_DEBUG));
  EXPECT_TRUE(LWPA_CAN_LOG(&params, LWPA_LOG_ERR));
}

// Test the lwpa_sanitize_syslog_params() function.
TEST_F(LogTest, sanitize)
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
  ASSERT_STREQ(syslog_params.app_name, "___ABC???");
  ASSERT_STREQ(syslog_params.hostname, "___ABC???");
  ASSERT_STREQ(syslog_params.procid, "___ABC???");
  ASSERT_LT(LWPA_LOG_FAC(syslog_params.facility), LWPA_LOG_NFACILITIES);
}

// Test the lwpa_validate_log_params() function.
TEST_F(LogTest, validate)
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
  lparams.syslog_params = {0, {0}, {0}, {0}};
  memcpy(lparams.syslog_params.app_name, special_char_array, sizeof special_char_array);
  lparams.log_mask = 0;
  lparams.time_fn = time_cb;
  lparams.context = this;

  // Make sure the validation returned true and the syslog field got sanitized
  ASSERT_TRUE(lwpa_validate_log_params(&lparams));
  ASSERT_STREQ(lparams.syslog_params.app_name, "___ABC???");

  // Invalid log callback
  lparams.log_fn = nullptr;
  ASSERT_FALSE(lwpa_validate_log_params(&lparams));

  // It's valid for time_fn to be NULL
  lparams.log_fn = log_cb;
  lparams.time_fn = nullptr;
  ASSERT_TRUE(lwpa_validate_log_params(&lparams));
}

// Test logging of:
//    - int values
//    - time from callback
//    - weird and missing value in syslog header
TEST_F(LogTest, log_intval)
{
  LwpaLogParams lparams;
  // A string with a non-ASCII character: "host\xC8name"
  // Should be sanitized to "host?name"
  const unsigned char weird_hostname[] = {0x68, 0x6f, 0x73, 0x74, 0xc8, 0x6e, 0x61, 0x6d, 0x65, 0x00};
  char syslog_buf[LWPA_SYSLOG_STR_MAX_LEN];
  char human_buf[LWPA_HUMAN_LOG_STR_MAX_LEN];

  lparams.action = kLwpaLogCreateSyslog;
  lparams.log_fn = log_cb;
  memcpy(lparams.syslog_params.hostname, weird_hostname, sizeof weird_hostname);
  strcpy(lparams.syslog_params.app_name, "My_App");
  lparams.syslog_params.procid[0] = '\0';
  lparams.syslog_params.facility = LWPA_LOG_KERN;
  lparams.log_mask = 0;
  lparams.time_fn = time_cb;
  lparams.context = this;

  std::string expect_syslog_str =
      "<0>1 1970-01-01T00:00:00.000Z host?name My_App - - - Here are some int values: 1 42 4294967295";
  std::string expect_human_str = "1970-01-01 00:00:00.000Z Here are some int values: 1 42 4294967295";
  std::string expect_raw_str = "Here are some int values: 1 42 4294967295";

  ASSERT_TRUE(lwpa_validate_log_params(&lparams));

#define INTVAL_FORMAT_STR_AND_ARGS "Here are some int values: %d %d %u", 1, 42, 0xffffffffu

  // Try the functions that simply build the log strings
  ASSERT_TRUE(lwpa_create_syslog_str(syslog_buf, LWPA_SYSLOG_STR_MAX_LEN, &cur_time, &lparams.syslog_params,
                                     LWPA_LOG_EMERG, INTVAL_FORMAT_STR_AND_ARGS));
  ASSERT_STREQ(syslog_buf, expect_syslog_str.c_str());

  ASSERT_TRUE(lwpa_create_human_log_str(human_buf, LWPA_HUMAN_LOG_STR_MAX_LEN, &cur_time, INTVAL_FORMAT_STR_AND_ARGS));
  ASSERT_STREQ(human_buf, expect_human_str.c_str());

  // Try logging with the log mask set to 0, should not work.
  ASSERT_FALSE(LWPA_CAN_LOG(&lparams, LWPA_LOG_EMERG));
  lwpa_log(&lparams, LWPA_LOG_EMERG, INTVAL_FORMAT_STR_AND_ARGS);

  // Try logging only syslog
  EXPECT_CALL(*this, VerifyLogCallback(expect_syslog_str, std::string(), expect_raw_str));
  lparams.log_mask = LWPA_LOG_UPTO(LWPA_LOG_EMERG);
  lwpa_log(&lparams, LWPA_LOG_EMERG, INTVAL_FORMAT_STR_AND_ARGS);
  Mock::VerifyAndClearExpectations(this);

  // Try logging both
  lparams.action = kLwpaLogCreateBoth;
  EXPECT_CALL(*this, VerifyLogCallback(expect_syslog_str, expect_human_str, expect_raw_str));
  lwpa_log(&lparams, LWPA_LOG_EMERG, INTVAL_FORMAT_STR_AND_ARGS);
  Mock::VerifyAndClearExpectations(this);

  // Try logging only human-readable
  lparams.action = kLwpaLogCreateHumanReadableLog;
  EXPECT_CALL(*this, VerifyLogCallback(std::string(), expect_human_str, expect_raw_str));
  lwpa_log(&lparams, LWPA_LOG_EMERG, INTVAL_FORMAT_STR_AND_ARGS);
  Mock::VerifyAndClearExpectations(this);
}

// Used by the following tests. Try using lwpa_vlog() to log various combinations of syslog and
// human-readable logging.
void LogTest::TestLwpaVlogHelper(std::string expect_syslog_str, std::string expect_human_str,
                                 std::string expect_raw_str, LwpaLogParams* lparams, int pri, const char* format, ...)
{
  va_list args;
  va_start(args, format);

  // Try logging only syslog
  lparams->action = kLwpaLogCreateSyslog;
  EXPECT_CALL(*this, VerifyLogCallback(expect_syslog_str, std::string(), expect_raw_str));
  lwpa_vlog(lparams, pri, format, args);
  Mock::VerifyAndClearExpectations(this);

  va_end(args);
  va_start(args, format);

  // Try logging both
  lparams->action = kLwpaLogCreateBoth;
  EXPECT_CALL(*this, VerifyLogCallback(expect_syslog_str, expect_human_str, expect_raw_str));
  lwpa_vlog(lparams, pri, format, args);
  Mock::VerifyAndClearExpectations(this);

  va_end(args);
  va_start(args, format);

  // Try logging only human-readable
  lparams->action = kLwpaLogCreateHumanReadableLog;
  EXPECT_CALL(*this, VerifyLogCallback(std::string(), expect_human_str, expect_raw_str));
  lwpa_vlog(lparams, pri, format, args);
  Mock::VerifyAndClearExpectations(this);
}

// Test logging of:
//    - string values
//    - no time
//    - weird and missing value in syslog header
TEST_F(LogTest, log_strval)
{
  LwpaLogParams lparams;
  // A string with some non-printing and non-ASCII characters: "\x012\x034\xff"
  const unsigned char weird_procid[] = {0x01, 0x32, 0x03, 0x34, 0xff, 0x00};
  char syslog_buf[LWPA_SYSLOG_STR_MAX_LEN];
  char human_buf[LWPA_HUMAN_LOG_STR_MAX_LEN];

  lparams.action = kLwpaLogCreateSyslog;
  lparams.log_fn = log_cb;
  strcpy(lparams.syslog_params.hostname, "10.101.17.38");
  memcpy(lparams.syslog_params.procid, weird_procid, sizeof weird_procid);
  lparams.syslog_params.app_name[0] = '\0';
  lparams.syslog_params.facility = LWPA_LOG_LOCAL2;
  lparams.log_mask = 0;
  lparams.time_fn = nullptr;
  lparams.context = this;

  std::string expect_syslog_str = "<149>1 - 10.101.17.38 - _2_4? - - Here are some string values: hey wassup hello";
  std::string expect_human_str = "Here are some string values: hey wassup hello";
  std::string expect_raw_str = expect_human_str;

  ASSERT_TRUE(lwpa_validate_log_params(&lparams));

#define STRVAL_FORMAT_STR_AND_ARGS "Here are some string values: %s %s %s", "hey", "wassup", "hello"

  // Try the functions that simply build the log strings
  ASSERT_TRUE(lwpa_create_syslog_str(syslog_buf, LWPA_SYSLOG_STR_MAX_LEN, nullptr, &lparams.syslog_params,
                                     LWPA_LOG_NOTICE, STRVAL_FORMAT_STR_AND_ARGS));
  ASSERT_STREQ(syslog_buf, expect_syslog_str.c_str());

  ASSERT_TRUE(lwpa_create_human_log_str(human_buf, LWPA_HUMAN_LOG_STR_MAX_LEN, nullptr, STRVAL_FORMAT_STR_AND_ARGS));
  ASSERT_STREQ(human_buf, expect_human_str.c_str());

  // Try logging with the log mask set to 0, should not work.
  ASSERT_FALSE(LWPA_CAN_LOG(&lparams, LWPA_LOG_NOTICE));
  lwpa_log(&lparams, LWPA_LOG_NOTICE, STRVAL_FORMAT_STR_AND_ARGS);

  // Now try the actual logging using lwpa_vlog().
  lparams.log_mask = LWPA_LOG_UPTO(LWPA_LOG_NOTICE);
  TestLwpaVlogHelper(expect_syslog_str, expect_human_str, expect_raw_str, &lparams, LWPA_LOG_NOTICE,
                     STRVAL_FORMAT_STR_AND_ARGS);
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
TEST_F(LogTest, log_maxlength)
{
  LwpaLogParams lparams;
  char syslog_buf[LWPA_SYSLOG_STR_MAX_LEN];
  char human_buf[LWPA_HUMAN_LOG_STR_MAX_LEN];

  lparams.action = kLwpaLogCreateSyslog;
  lparams.log_fn = log_cb;
  lparams.syslog_params.facility = LWPA_LOG_LOCAL7;
  lparams.log_mask = 0;
  lparams.time_fn = time_cb;
  lparams.context = this;

  std::string expect_syslog_str = "<191>1 1970-01-01T00:00:00.000-12:00 ";
  std::string expect_human_str = "1970-01-01 00:00:00.000-12:00 ";
  std::string expect_raw_str;

  // Create our very long syslog header components
  size_t i;
  for (i = 0; i < LWPA_LOG_HOSTNAME_MAX_LEN - 1; ++i)
  {
    char to_add = get_sanitized_char(i);
    lparams.syslog_params.hostname[i] = to_add;
    expect_syslog_str.append(1, to_add);
  }
  lparams.syslog_params.hostname[i] = '\0';
  expect_syslog_str.append(" ");

  for (i = 0; i < LWPA_LOG_APP_NAME_MAX_LEN - 1; ++i)
  {
    char to_add = get_sanitized_char(i);
    lparams.syslog_params.app_name[i] = to_add;
    expect_syslog_str.append(1, to_add);
  }
  lparams.syslog_params.app_name[i] = '\0';
  expect_syslog_str.append(" ");

  for (i = 0; i < LWPA_LOG_PROCID_MAX_LEN - 1; ++i)
  {
    char to_add = get_sanitized_char(i);
    lparams.syslog_params.procid[i] = to_add;
    expect_syslog_str.append(1, to_add);
  }
  lparams.syslog_params.procid[i] = '\0';
  expect_syslog_str.append(" - - ");

  // Now build our actual log message
  char to_log_str[LWPA_LOG_MSG_MAX_LEN];
  for (i = 0; i < LWPA_LOG_MSG_MAX_LEN - 1; ++i)
  {
    char to_add = get_sanitized_char(i);
    expect_syslog_str.append(1, to_add);
    expect_human_str.append(1, to_add);
    expect_raw_str.append(1, to_add);
    to_log_str[i] = to_add;
  }
  to_log_str[i] = '\0';

  ASSERT_TRUE(lwpa_validate_log_params(&lparams));
  // We want to have a non-zero, two-digit UTC offset for the maximum length possible.
  cur_time.utc_offset = -720;

  // Try the functions that simply build the log strings
  ASSERT_TRUE(lwpa_create_syslog_str(syslog_buf, LWPA_SYSLOG_STR_MAX_LEN, &cur_time, &lparams.syslog_params,
                                     LWPA_LOG_DEBUG, to_log_str));
  ASSERT_STREQ(syslog_buf, expect_syslog_str.c_str());

  ASSERT_TRUE(lwpa_create_human_log_str(human_buf, LWPA_HUMAN_LOG_STR_MAX_LEN, &cur_time, to_log_str));
  ASSERT_STREQ(human_buf, expect_human_str.c_str());

  // Try logging with the log mask set to 0, should not work.
  ASSERT_FALSE(LWPA_CAN_LOG(&lparams, LWPA_LOG_DEBUG));
  lwpa_log(&lparams, LWPA_LOG_DEBUG, STRVAL_FORMAT_STR_AND_ARGS);

  // Now try the actual logging using lwpa_vlog().
  lparams.log_mask = LWPA_LOG_UPTO(LWPA_LOG_DEBUG);
  TestLwpaVlogHelper(expect_syslog_str, expect_human_str, expect_raw_str, &lparams, LWPA_LOG_DEBUG, to_log_str);
}
