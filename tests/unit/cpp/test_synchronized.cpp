#include <etcpal/cpp/synchronized.h>

#include <etcpal/cpp/mutex.h>
#include <etcpal/cpp/rwlock.h>
#include <etcpal/cpp/thread.h>

#include <unity_fixture.h>

extern "C" {

TEST_GROUP(etcpal_cpp_synchronized);

TEST_SETUP(etcpal_cpp_synchronized)
{
}

TEST_TEAR_DOWN(etcpal_cpp_synchronized)
{
}

TEST(etcpal_cpp_synchronized, synchronized)
{
  etcpal::Synchronized<int, etcpal::RwLock> test_rw_val{};
  etcpal::Synchronized<int, etcpal::RwLock> test_rw_val_copy{};
  etcpal::Synchronized<int, etcpal::Mutex>  test_val{};
  etcpal::Synchronized<int, etcpal::Mutex>  test_val_copy{};

  TEST_ASSERT_TRUE(test_rw_val.apply([](auto val) { return val; }) == 0);
  TEST_ASSERT_TRUE(test_rw_val_copy.apply([](auto val) { return val; }) == 0);
  TEST_ASSERT_TRUE(test_val.apply([](auto val) { return val; }) == 0);
  TEST_ASSERT_TRUE(test_val_copy.apply([](auto val) { return val; }) == 0);

  constexpr auto increments_per_thread = 1000;
  constexpr auto num_threads           = 20;
  const auto     increment_and_copy    = [&] {
    for (auto i = 0; i < increments_per_thread; ++i)
    {
      test_rw_val.apply([](auto& val) { ++val; });
      test_val.apply([](auto& val) { ++val; });

      test_rw_val_copy.apply([&](auto& val) { val = *test_rw_val.clock(); });
      test_val_copy.apply([&](auto& val) { val = *test_val.clock(); });
    }
  };
  auto threads = std::vector<etcpal::JThread<>>{};
  for (auto i = 0; i < num_threads; ++i)
  {
    threads.push_back(etcpal::JThread<>{increment_and_copy});
  }
  threads.clear();
  TEST_ASSERT_TRUE(test_rw_val.capply([](auto val) { return val; }) == increments_per_thread * num_threads);
  TEST_ASSERT_TRUE(test_rw_val_copy.capply([](auto val) { return val; }) == increments_per_thread * num_threads);
  TEST_ASSERT_TRUE(test_val.capply([](auto val) { return val; }) == increments_per_thread * num_threads);
  TEST_ASSERT_TRUE(test_val_copy.capply([](auto val) { return val; }) == increments_per_thread * num_threads);
}

TEST_GROUP_RUNNER(etcpal_cpp_synchronized)
{
  RUN_TEST_CASE(etcpal_cpp_synchronized, synchronized);
}
}
