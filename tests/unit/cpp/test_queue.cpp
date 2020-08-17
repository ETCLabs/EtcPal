/******************************************************************************
 * Copyright 2020 ETC Inc.
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

#include "etcpal/cpp/queue.h"

#include "unity_fixture.h"

extern "C" {

TEST_GROUP(etcpal_cpp_queue);

TEST_SETUP(etcpal_cpp_queue)
{
}

TEST_TEAR_DOWN(etcpal_cpp_queue)
{
}

TEST(etcpal_cpp_queue, check_empty)
{
    etcpal::Queue<char> q(5);
    TEST_ASSERT_TRUE(q.IsEmpty());
}

TEST(etcpal_cpp_queue, can_send_and_receive)
{
  etcpal::Queue<char> q(3);
  char data = 0xDE;
  TEST_ASSERT_TRUE(q.SendToQueue(data));
  char receivedData;
  TEST_ASSERT_TRUE(q.ReceiveFromQueue(receivedData, 0));
  TEST_ASSERT_EQUAL(data, receivedData);
}

TEST(etcpal_cpp_queue, will_timeout_on_send)
{
  // Create queue for 3 chars
  etcpal::Queue<char> q(3);
  char data = 0xDE;
  TEST_ASSERT_TRUE(q.SendToQueue(data, false, 0));
  data = 0xAD;
  TEST_ASSERT_TRUE(q.SendToQueue(data, false, 0));
  data = 0xBE;
  TEST_ASSERT_TRUE(q.SendToQueue(data, false, 0));

  // This one should NOT work because we are over our size
  data = 0xEF;
  TEST_ASSERT_FALSE(q.SendToQueue(data, false, 0));
}

TEST(etcpal_cpp_queue, will_timeout_on_receive)
{
  etcpal_queue_t queue;

  // Create queue for 3 chars
  etcpal::Queue<char> q(3);
  char data = 0xDE;
  TEST_ASSERT_TRUE(q.SendToQueue(data, false, 0));
  data = 0xAD;
  char receivedData = 0x00;
  TEST_ASSERT_TRUE(q.ReceiveFromQueue(receivedData, 10));
  TEST_ASSERT_FALSE(q.ReceiveFromQueue(receivedData, 10));
}

TEST(etcpal_cpp_queue, can_detect_empty)
{
  // Create queue for 3 chars
  etcpal::Queue<char> q(3);
  TEST_ASSERT_TRUE(q.IsEmpty());

  char data = 0xDE;
  TEST_ASSERT_TRUE(q.SendToQueue(data, false, 0));
  TEST_ASSERT_FALSE(q.IsEmpty());

  char receivedData = 0x00;
  TEST_ASSERT_TRUE(q.ReceiveFromQueue(receivedData, 0));
  TEST_ASSERT_TRUE(q.IsEmpty());

  TEST_ASSERT_TRUE(q.SendToQueue(data, false, 0));
  TEST_ASSERT_FALSE(q.IsEmpty());
}

TEST_GROUP_RUNNER(etcpal_cpp_queue)
{
  RUN_TEST_CASE(etcpal_cpp_queue, can_send_and_receive);
  RUN_TEST_CASE(etcpal_cpp_queue, will_timeout_on_send);
  RUN_TEST_CASE(etcpal_cpp_queue, will_timeout_on_receive);
  RUN_TEST_CASE(etcpal_cpp_queue, can_detect_empty);
}

}  // extern "C"
