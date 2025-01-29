#include <etcpal/cpp/optional.h>

#include <array>

#include <etcpal/cpp/overload.h>
#include <etcpal/cpp/memory.h>

#include <unity_fixture.h>

#if (__cplusplus >= 201703L)

namespace
{

auto* default_resource = std::pmr::null_memory_resource();

}

#endif  // #if (__cplusplus >= 201703L)

extern "C" {

TEST_GROUP(etcpal_cpp_optional);

TEST_SETUP(etcpal_cpp_optional)
{
#if (__cplusplus >= 201703L)
  default_resource = std::pmr::get_default_resource();
  std::pmr::set_default_resource(std::pmr::null_memory_resource());
#endif  // #if (__cplusplus >= 201703L)
}

TEST_TEAR_DOWN(etcpal_cpp_optional)
{
#if (__cplusplus >= 201703L)
  std::pmr::set_default_resource(default_resource);
#endif  // #if (__cplusplus >= 201703L)
}

TEST(etcpal_cpp_optional, optional)
{
  const auto is_engaged =
#if (__cplusplus >= 201703L)
      etcpal::Select{[](etcpal::Nullopt_t) { return false; }, [](auto&&) { return true; }};
#else   // #if (__cplusplus < 201703L)
      etcpal::make_selection([](etcpal::Nullopt_t) { return false; }, [](auto&&) { return true; });
#endif  // #if (__cplusplus < 201703L)

  // initialization
  TEST_ASSERT_FALSE(etcpal::Optional<int>{});
  TEST_ASSERT_FALSE(etcpal::Optional<int>{etcpal::nullopt});
  TEST_ASSERT_FALSE(etcpal::Optional<int>{etcpal::Optional<double>{}});
  TEST_ASSERT_TRUE(etcpal::Optional<int>{0});

  // destruction
  etcpal::BlockMemory<1 << 5> buffer{};
  const auto                  alloc = etcpal::DefaultAllocator{std::addressof(buffer)};

  const auto test_value = std::allocate_shared<int>(alloc);
  // initialization
  auto test_optional = etcpal::Optional<std::shared_ptr<int>>{test_value};
  TEST_ASSERT(test_value.use_count() == 2);
  // copy construct
  auto copy = test_optional;
  TEST_ASSERT(test_value.use_count() == 3);
  // move construct
  auto move = std::move(test_optional);
  TEST_ASSERT(test_value.use_count() == 3);
  // destroy empty object
  test_optional.reset();
  TEST_ASSERT(test_value.use_count() == 3);
  // copy assign
  test_optional = move;
  TEST_ASSERT(test_value.use_count() == 4);
  // move assign
  copy = std::move(move);
  TEST_ASSERT(test_value.use_count() == 3);
  // destroy engaged object
  copy.reset();
  TEST_ASSERT(test_value.use_count() == 2);

  // visitation
  TEST_ASSERT_FALSE(etcpal::Optional<int>{}.visit(is_engaged));
  TEST_ASSERT_FALSE(etcpal::Optional<int>{etcpal::nullopt}.visit(is_engaged));
}

TEST_GROUP_RUNNER(etcpal_cpp_optional)
{
  RUN_TEST_CASE(etcpal_cpp_optional, optional);
}
}
