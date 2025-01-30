#include <etcpal/cpp/async.h>

#include <random>

#include <etcpal/cpp/overload.h>

#include <unity_fixture.h>

#if (__cplusplus >= 201703L)

namespace
{

auto* default_resource = std::pmr::null_memory_resource();

}

#endif  // #if (__cplusplus >= 201703L)

extern "C" {

TEST_GROUP(etcpal_cpp_async);

TEST_SETUP(etcpal_cpp_async)
{
#if (__cplusplus >= 201703L)
  default_resource = std::pmr::get_default_resource();
  std::pmr::set_default_resource(std::pmr::null_memory_resource());
#endif  // #if (__cplusplus >= 201703L)
}

TEST_TEAR_DOWN(etcpal_cpp_async)
{
#if (__cplusplus >= 201703L)
  std::pmr::set_default_resource(default_resource);
#endif  // #if (__cplusplus >= 201703L)
}

TEST(etcpal_cpp_async, promise_future)
{
  using namespace std::chrono_literals;

  etcpal::SyncBlockMemory<1 << 9> buffer{};
  const auto                      alloc = etcpal::DefaultAllocator{std::addressof(buffer)};

  auto promise = etcpal::Promise<int>{alloc};
  auto future  = promise.get_future();

  TEST_ASSERT_TRUE(future.valid());
  TEST_ASSERT_TRUE(future.wait_for(50ms) == etcpal::FutureStatus::timeout);

  constexpr auto test_val = 2039;
  promise.set_value(test_val);
  TEST_ASSERT_TRUE(future.valid());
  TEST_ASSERT_TRUE(future.wait_for(50ms) == etcpal::FutureStatus::ready);
  TEST_ASSERT_TRUE(future.get() == test_val);
}

TEST(etcpal_cpp_async, thread_pool)
{
  etcpal::SyncDualLevelBlockPool<1 << 22, sizeof(std::max_align_t) << 10> buffer{};
  const auto alloc = etcpal::DefaultAllocator{std::addressof(buffer)};

  constexpr auto num_items = 1024;

  etcpal::ThreadPool<32> pool{{ETCPAL_THREAD_DEFAULT_PRIORITY, ETCPAL_THREAD_DEFAULT_STACK, "test pool"}, alloc};
  auto                   future_futures                = pool.post(etcpal::use_future, [&] {
    auto futures = std::vector<etcpal::Future<int>, etcpal::DefaultAllocator>{alloc};
    for (auto i = 0; i < num_items; ++i)
    {
      auto promise = etcpal::Promise<int>{alloc};
      futures.push_back(promise.get_future());
      pool.post([prom = std::move(promise), i]() mutable { prom.set_value(i); });
    }

    return futures;
  });
  auto                   future_futures_for_get_if     = pool.post(etcpal::use_future, [&] {
    auto futures = std::vector<etcpal::Future<int>, etcpal::DefaultAllocator>{alloc};
    for (auto i = 0; i < num_items; ++i)
    {
      auto promise = etcpal::Promise<int>{alloc};
      futures.push_back(promise.get_future());
      pool.post([prom = std::move(promise), i]() mutable { prom.set_value(i); });
    }

    return futures;
  });
  auto                   future_continued_futures      = pool.post(etcpal::use_future, [&] {
    auto futures = std::vector<etcpal::Future<int>, etcpal::DefaultAllocator>{alloc};
    for (auto i = 0; i < num_items; ++i)
    {
      auto promise = etcpal::Promise<int>{alloc};
      futures.push_back(
          promise.get_future().and_then([](auto status, auto& value, auto exception) { return value.value(); }));
      pool.post([prom = std::move(promise), i]() mutable { prom.set_value(i); });
    }

    return futures;
  });
  auto                   future_continued_exec_futures = pool.post(etcpal::use_future, [&] {
    auto futures = std::vector<etcpal::Future<int>, etcpal::DefaultAllocator>{alloc};
    for (auto i = 0; i < num_items; ++i)
    {
      futures.push_back(
          pool.post(etcpal::use_future, [i]() { return i; }).and_then([](auto status, auto& value, auto exception) {
            return value.value();
          }));
    }

    return futures;
  });
  auto                   future_abandoned_futures      = pool.post(etcpal::use_future, [&] {
    auto futures = std::vector<etcpal::Future<int>, etcpal::DefaultAllocator>{alloc};
    for (auto i = 0; i < num_items; ++i)
    {
      futures.push_back(etcpal::Promise<int>{alloc}.get_future());
    }

    return futures;
  });

  using namespace std::chrono_literals;

  TEST_ASSERT_TRUE(future_futures.wait_for(1s) == etcpal::FutureStatus::ready);
  TEST_ASSERT_TRUE(future_futures_for_get_if.wait_for(1s) == etcpal::FutureStatus::ready);
  TEST_ASSERT_TRUE(future_continued_futures.wait_for(1s) == etcpal::FutureStatus::ready);
  TEST_ASSERT_TRUE(future_continued_exec_futures.wait_for(1s) == etcpal::FutureStatus::ready);
  TEST_ASSERT_TRUE(future_abandoned_futures.wait_for(1s) == etcpal::FutureStatus::ready);
  auto futures                = future_futures.get();
  auto futures_for_get_if     = future_futures_for_get_if.get();
  auto continued_futures      = future_continued_futures.get();
  auto continued_exec_futures = future_continued_exec_futures.get();
  auto abandoned_futures      = future_abandoned_futures.get();
  for (auto i = 0; i < num_items; ++i)
  {
    TEST_ASSERT_TRUE(futures[i].wait_for(50ms) == etcpal::FutureStatus::ready);
    TEST_ASSERT_TRUE(futures[i].get() == i);

    TEST_ASSERT_TRUE(futures_for_get_if[i].get_if().get_if<int>().value_or(-1) == i);

    TEST_ASSERT_TRUE(continued_futures[i].wait_for(50ms) == etcpal::FutureStatus::ready);
    TEST_ASSERT_TRUE(continued_futures[i].get() == i);

    TEST_ASSERT_TRUE(continued_exec_futures[i].wait_for(50ms) == etcpal::FutureStatus::ready);
    auto twice_contnued =
        continued_exec_futures[i].and_then([](auto status, auto& value, auto exception) { return value.value(); });
    TEST_ASSERT_TRUE(twice_contnued.wait_for(50ms) == etcpal::FutureStatus::ready);
    TEST_ASSERT_TRUE(twice_contnued.get() == i);

    TEST_ASSERT_TRUE(abandoned_futures[i].wait_for(50ms) == etcpal::FutureStatus::abandoned);
  }
}

TEST(etcpal_cpp_async, promise_chain)
{
  using namespace std::chrono_literals;
  using NumVector = std::vector<int, etcpal::DefaultAllocator>;

  etcpal::SyncDualLevelBlockPool<1 << 17> buffer{};
  const auto                              alloc = etcpal::DefaultAllocator{std::addressof(buffer)};

  constexpr auto num_elements = std::size_t{1024};

  etcpal::ThreadPool<8> pool{{ETCPAL_THREAD_DEFAULT_PRIORITY, ETCPAL_THREAD_DEFAULT_STACK, "test pool"}, alloc};
  auto                  numbers = NumVector(num_elements, alloc);
  auto                  rd      = std::mt19937{std::random_device{}()};
  auto                  dist    = std::uniform_int_distribution<int>{-1000, 1000};
  std::generate(std::begin(numbers), std::end(numbers), [&] { return dist(rd); });

  auto max_val_promise = etcpal::Promise<int>{alloc};
  auto min_val_promise = etcpal::Promise<int>{alloc};
  auto max_val         = max_val_promise.get_future();
  auto min_val         = min_val_promise.get_future();
  auto task_chain_done =
      pool.post(etcpal::use_future,
                [&]() -> NumVector& {
                  std::make_heap(std::begin(numbers), std::end(numbers));
                  max_val_promise.set_value(numbers.front());
                  return numbers;
                })
          .and_then(
              pool.get_executor(),
              [&](NumVector& nums) -> NumVector& {
                std::make_heap(std::begin(nums), std::end(nums), std::greater<>{});
                min_val_promise.set_value(nums.front());
                return nums;
              },
              [](std::exception_ptr ptr) -> NumVector& { std::rethrow_exception(ptr); },
              [](etcpal::FutureStatus err) -> NumVector& { throw std::logic_error{"promise chain broken"}; })
          .and_then(
              pool.get_executor(), [](NumVector& nums) { std::sort(std::begin(nums), std::end(nums)); },
              [](std::exception_ptr ptr) { std::rethrow_exception(ptr); },
              [](auto&&) { throw std::logic_error{"promise chain broken"}; });
  TEST_ASSERT_TRUE(max_val.wait_for(50ms) == etcpal::FutureStatus::ready);
  TEST_ASSERT_TRUE(min_val.wait_for(50ms) == etcpal::FutureStatus::ready);
  TEST_ASSERT_TRUE(task_chain_done.wait_for(1s) == etcpal::FutureStatus::ready);
  TEST_ASSERT_TRUE(std::is_sorted(std::cbegin(numbers), std::cend(numbers)));
  TEST_ASSERT_TRUE(max_val.get() == numbers.back());
  TEST_ASSERT_TRUE(min_val.get() == numbers.front());
}

TEST_GROUP_RUNNER(etcpal_cpp_async)
{
  RUN_TEST_CASE(etcpal_cpp_async, promise_future);
  RUN_TEST_CASE(etcpal_cpp_async, thread_pool);
  RUN_TEST_CASE(etcpal_cpp_async, promise_chain);
}
}
