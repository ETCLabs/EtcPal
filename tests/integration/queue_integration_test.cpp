#include <iostream>
#include "queue_tests/queue_tests.h"

#include "etcpal/queue.h"
#include "unity_fixture.h"

extern "C" {

TEST_GROUP(etcpal_queue_integration);

TEST_SETUP(etcpal_queue_integration)
{
  init_concurrency_test();
}

TEST_TEAR_DOWN(etcpal_queue_integration)
{
}

TEST(etcpal_queue_integration, concurrent_small_queue_few_writers)
{
  TEST_ASSERT_TRUE(concurrent_small_queue_few_writers());
}

TEST(etcpal_queue_integration, concurrent_large_queue_few_writers_many_items)
{
  TEST_ASSERT_TRUE(concurrent_large_queue_few_writers_many_items());
}

TEST(etcpal_queue_integration, concurrent_large_queue_many_writers_few_items)
{
  TEST_ASSERT_TRUE(concurrent_large_queue_many_writers_few_items());
}

TEST(etcpal_queue_integration, concurrent_small_queue_one_reader_few_writers_few_items)
{
  TEST_ASSERT_TRUE(concurrent_small_queue_one_reader_few_writers_few_items());
}

TEST(etcpal_queue_integration, concurrent_small_queue_few_readers_few_writers_few_items)
{
  TEST_ASSERT_TRUE(concurrent_small_queue_few_readers_few_writers_few_items());
}

TEST(etcpal_queue_integration, concurrent_small_queue_few_readers_many_writers_many_items)
{
  TEST_ASSERT_TRUE(concurrent_small_queue_few_readers_many_writers_many_items());
}

TEST(etcpal_queue_integration, concurrent_small_queue_many_readers_few_writers_many_items)
{
  TEST_ASSERT_TRUE(concurrent_small_queue_many_readers_few_writers_many_items());
}

TEST_GROUP_RUNNER(etcpal_queue_integration)
{
  RUN_TEST_CASE(etcpal_queue_integration, concurrent_small_queue_few_writers);
  RUN_TEST_CASE(etcpal_queue_integration, concurrent_large_queue_few_writers_many_items);
  RUN_TEST_CASE(etcpal_queue_integration, concurrent_large_queue_many_writers_few_items);
  RUN_TEST_CASE(etcpal_queue_integration, concurrent_small_queue_one_reader_few_writers_few_items);
  RUN_TEST_CASE(etcpal_queue_integration, concurrent_small_queue_few_readers_few_writers_few_items);
  RUN_TEST_CASE(etcpal_queue_integration, concurrent_small_queue_few_readers_many_writers_many_items);
}
}  // extern "C"
