#include <etcpal/cpp/stop_token.h>

#include <algorithm>
#include <array>

#include <unity_fixture.h>

extern "C" {

TEST_GROUP(etcpal_cpp_stop_token);

TEST_SETUP(etcpal_cpp_stop_token)
{
}

TEST_TEAR_DOWN(etcpal_cpp_stop_token)
{
}

TEST(etcpal_cpp_stop_token, stop_source)
{
#if (__cplusplus >= 201703L)
  auto       buffer          = std::array<std::byte, 1 << 8>{};
  auto       memory_resource = std::pmr::monotonic_buffer_resource{std::data(buffer), std::size(buffer)};
  const auto alloc           = std::pmr::polymorphic_allocator<std::byte>{std::addressof(memory_resource)};
#endif  // #if (__cplusplus >= 201703L)
  const auto alloc = etcpal::DefaultAllocator{};

  // initialize source with state
  auto ssource = etcpal::StopSource<>{alloc};
  TEST_ASSERT_FALSE(ssource.stop_requested());
  TEST_ASSERT_TRUE(ssource.stop_possible());

  // grab token from source and register a callback
  constexpr auto init_val = 5;
  constexpr auto end_val  = 6;
  auto           test_val = init_val;
  const auto     token    = ssource.get_token();
#if (__cplusplus >= 201703L)
  const auto callback = etcpal::StopCallback{token, [&] { test_val = end_val; }};
#else   // #if (__cplusplus >= 201703L)
  const auto set_to_end  = [&] { test_val = end_val; };
  const auto set_to_init = [&] { test_val = init_val; };
  const etcpal::StopCallback<std::function<void()>, std::remove_cv_t<decltype(alloc)> > callback{token, set_to_end};
#endif  // #if (__cplusplus >= 201703L)
  TEST_ASSERT_FALSE(token.stop_requested());
  TEST_ASSERT_TRUE(token.stop_possible());
  TEST_ASSERT_TRUE(test_val == init_val);

  // request stop
  TEST_ASSERT_TRUE(ssource.request_stop());
  TEST_ASSERT_TRUE(ssource.stop_requested());
  TEST_ASSERT_TRUE(token.stop_requested());
  TEST_ASSERT_TRUE(ssource.stop_possible());
  TEST_ASSERT_TRUE(token.stop_possible());
  TEST_ASSERT_TRUE(test_val == end_val);
  TEST_ASSERT_FALSE(ssource.request_stop());

  // registering callback on stopped token should invoke the callback
#if (__cplusplus >= 201703L)
  etcpal::StopCallback{token, [&] { test_val = init_val; }};
#else   // #if (__cplusplus >= 201703L)
  etcpal::StopCallback<std::function<void()>, std::remove_cv_t<decltype(alloc)> >{token, set_to_init};
#endif  // #if (__cplusplus >= 201703L)
  TEST_ASSERT_TRUE(test_val == init_val);

  // replacing valid source with empty source should not affect already-triggered token
  ssource = etcpal::StopSource<>{etcpal::NoStopState};
  TEST_ASSERT_FALSE(ssource.stop_requested());
  TEST_ASSERT_FALSE(ssource.stop_possible());
  TEST_ASSERT_FALSE(ssource.request_stop());
  TEST_ASSERT_TRUE(token.stop_requested());
  TEST_ASSERT_TRUE(token.stop_possible());

  // empty tokens
  TEST_ASSERT_FALSE(ssource.get_token().stop_possible());
  TEST_ASSERT_FALSE(ssource.get_token().stop_requested());
  TEST_ASSERT_FALSE(etcpal::StopToken<>{}.stop_possible());
  TEST_ASSERT_FALSE(etcpal::StopToken<>{}.stop_requested());

  // registering callback on empty token should not invoke the callback
#if (__cplusplus >= 201703L)
  etcpal::StopCallback{ssource.get_token(), [&] { test_val = end_val; }};
#else   // #if (__cplusplus >= 201703L)
  etcpal::StopCallback<std::function<void()>, std::remove_cv_t<decltype(alloc)> >{ssource.get_token(), set_to_end};
#endif  // #if (__cplusplus >= 201703L)
  TEST_ASSERT_TRUE(test_val == init_val);
}

TEST_GROUP_RUNNER(etcpal_cpp_stop_token)
{
  RUN_TEST_CASE(etcpal_cpp_stop_token, stop_source);
}
}
