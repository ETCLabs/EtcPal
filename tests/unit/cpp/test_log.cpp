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

#include "etcpal/cpp/log.h"
#include "unity_fixture.h"

#include <functional>

// It would be nice to have GoogleMock available, but alas...
class TestLogMessageHandler : public etcpal::LogMessageHandler
{
public:
  void WillReturnTimestamp(const EtcPalLogTimestamp& timestamp) { timestamp_to_return_ = timestamp; }
  void OnLogEvent(const std::function<void(const EtcPalLogStrings&)>& func) { log_event_ = func; }

  int LogEventCallCount() const { return log_event_call_count_; }
  void ResetLogEventCallCount() { log_event_call_count_ = 0; }

private:
  EtcPalLogTimestamp GetLogTimestamp() override { return timestamp_to_return_; }
  void HandleLogMessage(const EtcPalLogStrings& strings) override;

  EtcPalLogTimestamp timestamp_to_return_{};
  std::function<void(const EtcPalLogStrings&)> log_event_;
  int log_event_call_count_{0};
};

void TestLogMessageHandler::HandleLogMessage(const EtcPalLogStrings& strings)
{
  if (log_event_)
    log_event_(strings);
  ++log_event_call_count_;
}

static TestLogMessageHandler test_log_handler;
static etcpal::Logger logger;

extern "C" {
TEST_GROUP(etcpal_cpp_log);

TEST_SETUP(etcpal_cpp_log)
{
  etcpal_init(ETCPAL_FEATURE_LOGGING);
  logger = etcpal::Logger{};
  test_log_handler = TestLogMessageHandler{};
}

TEST_TEAR_DOWN(etcpal_cpp_log)
{
  etcpal_deinit(ETCPAL_FEATURE_LOGGING);
}

TEST(etcpal_cpp_log, startup_works)
{
  test_log_handler.OnLogEvent([](const EtcPalLogStrings& strings) {
    TEST_ASSERT_EQUAL_STRING(strings.human_readable, "1970-01-01 00:00:00.000Z [DBUG] Test Message");
  });

  // Logging should not work if the logger has not been started
  logger.Debug("Test Message");
  TEST_ASSERT_EQUAL_INT(test_log_handler.LogEventCallCount(), 0);

  // Startup should work - after starting, logging should work
  TEST_ASSERT_TRUE(logger.SetDispatchPolicy(etcpal::LogDispatchPolicy::Direct)
                       .SetLogAction(kEtcPalLogCreateHumanReadable)
                       .Startup(test_log_handler));

  logger.Debug("Test Message");
  TEST_ASSERT_EQUAL_INT(test_log_handler.LogEventCallCount(), 1);

  logger.Shutdown();
}

// Test the dispatch_policy and log_action setters/getters
TEST(etcpal_cpp_log, basic_getters_work)
{
  logger.SetDispatchPolicy(etcpal::LogDispatchPolicy::Direct).SetLogAction(kEtcPalLogCreateSyslog);

  TEST_ASSERT_EQUAL(logger.dispatch_policy(), etcpal::LogDispatchPolicy::Direct);
  TEST_ASSERT_EQUAL(logger.log_action(), kEtcPalLogCreateSyslog);
}

// Test the functionality of the log mask.
TEST(etcpal_cpp_log, log_mask_works)
{
  TEST_ASSERT_TRUE(logger.SetDispatchPolicy(etcpal::LogDispatchPolicy::Direct)
                       .SetLogAction(kEtcPalLogCreateHumanReadable)
                       .Startup(test_log_handler));

  logger.SetLogMask(ETCPAL_LOG_UPTO(ETCPAL_LOG_WARNING));
  TEST_ASSERT_EQUAL_INT(logger.log_mask(), ETCPAL_LOG_UPTO(ETCPAL_LOG_WARNING));

  // Mask should allow levels at or above ETCPAL_LOG_WARNING
  TEST_ASSERT_TRUE(logger.CanLog(ETCPAL_LOG_EMERG));
  TEST_ASSERT_TRUE(logger.CanLog(ETCPAL_LOG_ALERT));
  TEST_ASSERT_TRUE(logger.CanLog(ETCPAL_LOG_CRIT));
  TEST_ASSERT_TRUE(logger.CanLog(ETCPAL_LOG_ERR));
  TEST_ASSERT_TRUE(logger.CanLog(ETCPAL_LOG_WARNING));
  TEST_ASSERT_FALSE(logger.CanLog(ETCPAL_LOG_NOTICE));
  TEST_ASSERT_FALSE(logger.CanLog(ETCPAL_LOG_INFO));
  TEST_ASSERT_FALSE(logger.CanLog(ETCPAL_LOG_DEBUG));

  // Log commands should follow the same logic
  logger.Log(ETCPAL_LOG_DEBUG, "Test Message");
  logger.Debug("Test Message");
  logger.Log(ETCPAL_LOG_INFO, "Test Message");
  logger.Info("Test Message");
  logger.Log(ETCPAL_LOG_NOTICE, "Test Message");
  logger.Notice("Test Message");
  TEST_ASSERT_EQUAL_INT(test_log_handler.LogEventCallCount(), 0);

  // Now the unfiltered ones
  logger.Log(ETCPAL_LOG_WARNING, "Test Message");
  logger.Warning("Test Message");
  logger.Log(ETCPAL_LOG_ERR, "Test Message");
  logger.Error("Test Message");
  logger.Log(ETCPAL_LOG_CRIT, "Test Message");
  logger.Critical("Test Message");
  logger.Log(ETCPAL_LOG_ALERT, "Test Message");
  logger.Alert("Test Message");
  logger.Log(ETCPAL_LOG_EMERG, "Test Message");
  logger.Emergency("Test Message");
  TEST_ASSERT_EQUAL_INT(test_log_handler.LogEventCallCount(), 10);

  logger.Shutdown();
}

// Test the functionality of the Log() and log shortcut functions.
TEST(etcpal_cpp_log, log_functions_work)
{
  TEST_ASSERT_TRUE(logger.SetDispatchPolicy(etcpal::LogDispatchPolicy::Direct)
                       .SetLogAction(kEtcPalLogCreateHumanReadable)
                       .Startup(test_log_handler));

  // Test the Log() function and corresponding shortcut for each priority:

  // Debug
  test_log_handler.OnLogEvent([](const EtcPalLogStrings& strings) {
    TEST_ASSERT_EQUAL_STRING(strings.human_readable, "1970-01-01 00:00:00.000Z [DBUG] Test Message 1");
  });
  logger.Log(ETCPAL_LOG_DEBUG, "Test message %d", 1);
  logger.Debug("Test Message %d", 1);
  TEST_ASSERT_EQUAL_INT(test_log_handler.LogEventCallCount(), 2);
  test_log_handler.ResetLogEventCallCount();

  // Info
  test_log_handler.OnLogEvent([](const EtcPalLogStrings& strings) {
    TEST_ASSERT_EQUAL_STRING(strings.human_readable, "1970-01-01 00:00:00.000Z [INFO] Test Message 2");
  });
  logger.Log(ETCPAL_LOG_INFO, "Test message %d", 2);
  logger.Info("Test Message %d", 2);
  TEST_ASSERT_EQUAL_INT(test_log_handler.LogEventCallCount(), 2);
  test_log_handler.ResetLogEventCallCount();

  // Notice
  test_log_handler.OnLogEvent([](const EtcPalLogStrings& strings) {
    TEST_ASSERT_EQUAL_STRING(strings.human_readable, "1970-01-01 00:00:00.000Z [NOTI] Test Message 3");
  });
  logger.Log(ETCPAL_LOG_NOTICE, "Test message %d", 3);
  logger.Notice("Test Message %d", 3);
  TEST_ASSERT_EQUAL_INT(test_log_handler.LogEventCallCount(), 2);
  test_log_handler.ResetLogEventCallCount();

  // Warning
  test_log_handler.OnLogEvent([](const EtcPalLogStrings& strings) {
    TEST_ASSERT_EQUAL_STRING(strings.human_readable, "1970-01-01 00:00:00.000Z [WARN] Test Message 4");
  });
  logger.Log(ETCPAL_LOG_WARNING, "Test message %d", 4);
  logger.Warning("Test Message %d", 4);
  TEST_ASSERT_EQUAL_INT(test_log_handler.LogEventCallCount(), 2);
  test_log_handler.ResetLogEventCallCount();

  // Error
  test_log_handler.OnLogEvent([](const EtcPalLogStrings& strings) {
    TEST_ASSERT_EQUAL_STRING(strings.human_readable, "1970-01-01 00:00:00.000Z [ERR ] Test Message 5");
  });
  logger.Log(ETCPAL_LOG_ERR, "Test message %d", 5);
  logger.Error("Test Message %d", 5);
  TEST_ASSERT_EQUAL_INT(test_log_handler.LogEventCallCount(), 2);
  test_log_handler.ResetLogEventCallCount();

  // Critical
  test_log_handler.OnLogEvent([](const EtcPalLogStrings& strings) {
    TEST_ASSERT_EQUAL_STRING(strings.human_readable, "1970-01-01 00:00:00.000Z [CRIT] Test Message 6");
  });
  logger.Log(ETCPAL_LOG_CRIT, "Test message %d", 6);
  logger.Critical("Test Message %d", 6);
  TEST_ASSERT_EQUAL_INT(test_log_handler.LogEventCallCount(), 2);
  test_log_handler.ResetLogEventCallCount();

  // Alert
  test_log_handler.OnLogEvent([](const EtcPalLogStrings& strings) {
    TEST_ASSERT_EQUAL_STRING(strings.human_readable, "1970-01-01 00:00:00.000Z [ALRT] Test Message 7");
  });
  logger.Log(ETCPAL_LOG_ALERT, "Test message %d", 7);
  logger.Alert("Test Message %d", 7);
  TEST_ASSERT_EQUAL_INT(test_log_handler.LogEventCallCount(), 2);
  test_log_handler.ResetLogEventCallCount();

  // Emergency
  test_log_handler.OnLogEvent([](const EtcPalLogStrings& strings) {
    TEST_ASSERT_EQUAL_STRING(strings.human_readable, "1970-01-01 00:00:00.000Z [ALRT] Test Message 8");
  });
  logger.Log(ETCPAL_LOG_EMERG, "Test message %d", 8);
  logger.Emergency("Test Message %d", 8);
  TEST_ASSERT_EQUAL_INT(test_log_handler.LogEventCallCount(), 2);

  logger.Shutdown();
}

TEST(etcpal_cpp_log, timestamps_work)
{
  TEST_ASSERT_TRUE(logger.SetDispatchPolicy(etcpal::LogDispatchPolicy::Direct)
                       .SetLogAction(kEtcPalLogCreateHumanReadable)
                       .Startup(test_log_handler));

  // January 22, 2020 16:57:33.123 UTC-06:00
  EtcPalLogTimestamp timestamp = {2020, 1, 22, 16, 57, 33, 123, -360};
  test_log_handler.WillReturnTimestamp(timestamp);

  test_log_handler.OnLogEvent([](const EtcPalLogStrings& strings) {
    TEST_ASSERT_EQUAL_STRING(strings.human_readable, "2020-01-22 16:57:33.123-06:00 [INFO] Test Message");
  });
  logger.Info("Test Message");
  TEST_ASSERT_EQUAL_INT(test_log_handler.LogEventCallCount(), 1);

  logger.Shutdown();
}

TEST(etcpal_cpp_log, syslog_params_work)
{
  TEST_ASSERT_TRUE(logger.SetSyslogFacility(ETCPAL_LOG_LOCAL1)
                       .SetSyslogHostname("MyHost")
                       .SetSyslogAppName("TestApp")
                       .SetSyslogProcId(200)
                       .SetDispatchPolicy(etcpal::LogDispatchPolicy::Direct)
                       .SetLogAction(kEtcPalLogCreateSyslog)
                       .Startup(test_log_handler));

  TEST_ASSERT_EQUAL_INT(logger.syslog_facility(), ETCPAL_LOG_LOCAL1);
  TEST_ASSERT_EQUAL_STRING(logger.syslog_hostname(), "MyHost");
  TEST_ASSERT_EQUAL_STRING(logger.syslog_app_name(), "TestApp");
  TEST_ASSERT_EQUAL_STRING(logger.syslog_procid(), "200");

  test_log_handler.OnLogEvent([](const EtcPalLogStrings& strings) {
    TEST_ASSERT_EQUAL_STRING(strings.syslog, "<142>1 1970-01-01T00:00:00.000Z MyHost TestApp 200 - - Test Message");
  });
  logger.Log(ETCPAL_LOG_INFO, "Test Message");
}

TEST_GROUP_RUNNER(etcpal_cpp_log)
{
  RUN_TEST_CASE(etcpal_cpp_log, startup_works);
  RUN_TEST_CASE(etcpal_cpp_log, basic_getters_work);
  RUN_TEST_CASE(etcpal_cpp_log, log_mask_works);
  RUN_TEST_CASE(etcpal_cpp_log, log_functions_work);
  RUN_TEST_CASE(etcpal_cpp_log, timestamps_work);
  RUN_TEST_CASE(etcpal_cpp_log, syslog_params_work);
}
}
