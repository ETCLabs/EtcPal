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

#if (__cplusplus >= 201703L)
  auto buffer = std::array<std::byte, 1 << 8>{};
  auto memory_resource =
      std::pmr::monotonic_buffer_resource{std::data(buffer), std::size(buffer), std::pmr::null_memory_resource()};
  const auto alloc = std::pmr::polymorphic_allocator<std::byte>{std::addressof(memory_resource)};
#else   // #if (__cplusplus >= 201703L)
  const auto alloc = etcpal::DefaultAllocator{};
#endif  // #if (__cplusplus >= 201703L)

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
#if (__cplusplus >= 201703L)
  auto buffer = std::array<std::byte, 1 << 22>{};
  auto memory_resource =
      std::pmr::monotonic_buffer_resource{std::data(buffer), std::size(buffer), std::pmr::null_memory_resource()};
  const auto alloc = std::pmr::polymorphic_allocator<std::byte>{std::addressof(memory_resource)};
#else   // #if (__cplusplus >= 201703L)
  const auto alloc = etcpal::DefaultAllocator{};
#endif  // #if (__cplusplus >= 201703L)

  constexpr auto num_items = 1024;

  etcpal::ThreadPool<32> pool{{ETCPAL_THREAD_DEFAULT_PRIORITY, ETCPAL_THREAD_DEFAULT_STACK, "test pool"}, alloc};
  auto                   futures                = std::vector<etcpal::Future<int>, etcpal::DefaultAllocator>{alloc};
  auto                   futures_for_get_if     = std::vector<etcpal::Future<int>, etcpal::DefaultAllocator>{alloc};
  auto                   continued_futures      = std::vector<etcpal::Future<int>, etcpal::DefaultAllocator>{alloc};
  auto                   continued_exec_futures = std::vector<etcpal::Future<int>, etcpal::DefaultAllocator>{alloc};
  auto                   abandoned_futures      = std::vector<etcpal::Future<int>, etcpal::DefaultAllocator>{alloc};
  futures.reserve(num_items);
  continued_futures.reserve(num_items);
  futures_for_get_if.reserve(num_items);
  continued_exec_futures.reserve(num_items);
  abandoned_futures.reserve(num_items);
  for (auto i = 0; i < num_items; ++i)
  {
    auto promise = etcpal::Promise<int>{alloc};
    futures.push_back(promise.get_future());
    pool.post([prom = std::move(promise), i]() mutable { prom.set_value(i); });

    promise = etcpal::Promise<int>{alloc};
    futures_for_get_if.push_back(promise.get_future());
    pool.post([prom = std::move(promise), i]() mutable { prom.set_value(i); });

    promise = etcpal::Promise<int>{alloc};
    continued_futures.push_back(
        promise.get_future().and_then([](auto status, auto& value, auto exception) { return value.value(); }));
    pool.post([prom = std::move(promise), i]() mutable { prom.set_value(i); });

    continued_exec_futures.push_back(
        pool.post(etcpal::use_future, [i]() { return i; }).and_then([](auto status, auto& value, auto exception) {
          return value.value();
        }));

    abandoned_futures.push_back(etcpal::Promise<int>{alloc}.get_future());
  }

  for (auto i = 0; i < num_items; ++i)
  {
    using namespace std::chrono_literals;

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

#if (__cplusplus >= 201703L)
  auto buffer = std::array<std::byte, 1 << 13>{};
  auto memory_resource =
      std::pmr::monotonic_buffer_resource{std::data(buffer), std::size(buffer), std::pmr::null_memory_resource()};
  const auto alloc = std::pmr::polymorphic_allocator<std::byte>{std::addressof(memory_resource)};
#else   // #if (__cplusplus >= 201703L)
  const auto alloc = etcpal::DefaultAllocator{};
#endif  // #if (__cplusplus >= 201703L)

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
