#include <etcpal/cpp/variant.h>

#include <unity_fixture.h>

#if (__cplusplus >= 201703L)

namespace
{

auto* default_resource = std::pmr::null_memory_resource();

}

#endif  // #if (__cplusplus >= 201703L)

extern "C" {

TEST_GROUP(etcpal_cpp_variant);

TEST_SETUP(etcpal_cpp_variant)
{
#if (__cplusplus >= 201703L)
  default_resource = std::pmr::get_default_resource();
  std::pmr::set_default_resource(std::pmr::null_memory_resource());
#endif  // #if (__cplusplus >= 201703L)
}

TEST_TEAR_DOWN(etcpal_cpp_variant)
{
#if (__cplusplus >= 201703L)
  std::pmr::set_default_resource(default_resource);
#endif  // #if (__cplusplus >= 201703L)
}

namespace
{

enum class Type
{
  integer,
  unsigned_8,
  unsigned_16,
  unsigned_32,
  unsigned_64,
  signed_integer,
  signed_8,
  signed_16,
  signed_32,
  signed_64,
  floating_point,
  double_precision,
  string
};

}

TEST(etcpal_cpp_variant, variant)
{
  // initialization
  TEST_ASSERT_TRUE((etcpal::Variant<int, double>{}.index()) == 0);
  TEST_ASSERT_TRUE((etcpal::Variant<int, double>{etcpal::InPlaceIndex_t<0>{}}).index() == 0);
  TEST_ASSERT_TRUE((etcpal::Variant<int, double>{etcpal::InPlaceIndex_t<1>{}}).index() == 1);
  TEST_ASSERT_TRUE((etcpal::Variant<int, double>{etcpal::InPlaceType_t<int>{}}).index() == 0);
  TEST_ASSERT_TRUE((etcpal::Variant<int, double>{etcpal::InPlaceType_t<double>{}}).index() == 1);

  using String         = std::basic_string<char, std::char_traits<char>, etcpal::DefaultAllocator>;
  using StringOrNumber = etcpal::Variant<int, String>;
#if (__cplusplus >= 201703L)
  auto buffer = std::array<std::byte, 1 << 5>{};
  auto memory_resource =
      std::pmr::monotonic_buffer_resource{std::data(buffer), std::size(buffer), std::pmr::null_memory_resource()};
  const auto alloc = std::pmr::polymorphic_allocator<std::byte>{std::addressof(memory_resource)};
#else   // #if (__cplusplus >= 201703L)
  const auto alloc = etcpal::DefaultAllocator{};
#endif  // #if (__cplusplus >= 201703L)

  // comparision
  const auto test_string_0 = String{"test string 0", alloc};
  const auto test_string_1 = String{"test string 1", alloc};
  TEST_ASSERT_TRUE((StringOrNumber{test_string_0} == StringOrNumber{test_string_0}));
  TEST_ASSERT_FALSE((StringOrNumber{test_string_0} == StringOrNumber{test_string_1}));
  TEST_ASSERT_TRUE((StringOrNumber{test_string_0} < StringOrNumber{test_string_1}));
  TEST_ASSERT_FALSE((StringOrNumber{test_string_1} < StringOrNumber{test_string_0}));
  TEST_ASSERT_TRUE((StringOrNumber{test_string_1} > StringOrNumber{test_string_0}));
  TEST_ASSERT_FALSE((StringOrNumber{test_string_0} > StringOrNumber{test_string_1}));
  TEST_ASSERT_TRUE((StringOrNumber{test_string_0} <= StringOrNumber{test_string_0}));
  TEST_ASSERT_TRUE((StringOrNumber{test_string_0} <= StringOrNumber{test_string_1}));
  TEST_ASSERT_FALSE((StringOrNumber{test_string_1} <= StringOrNumber{test_string_0}));
  TEST_ASSERT_TRUE((StringOrNumber{test_string_0} >= StringOrNumber{test_string_0}));
  TEST_ASSERT_TRUE((StringOrNumber{test_string_1} >= StringOrNumber{test_string_0}));
  TEST_ASSERT_FALSE((StringOrNumber{test_string_0} >= StringOrNumber{test_string_1}));

  // value access
  TEST_ASSERT_TRUE((StringOrNumber{test_string_0}).get<String>() == test_string_0);

  // destruction
  const auto test_value = std::allocate_shared<int>(alloc);
  // initialization
  auto test_variant = etcpal::Variant<std::shared_ptr<int>, String>{test_value};
  TEST_ASSERT(test_value.use_count() == 2);
  // copy construct
  auto copy = test_variant;
  TEST_ASSERT(test_value.use_count() == 3);
  // move construct
  auto move = std::move(test_variant);
  TEST_ASSERT(test_value.use_count() == 3);
  // destroy empty object
  test_variant = test_string_0;
  TEST_ASSERT(test_value.use_count() == 3);
  // copy assign
  test_variant = move;
  TEST_ASSERT(test_value.use_count() == 4);
  // move assign
  copy = std::move(move);
  TEST_ASSERT(test_value.use_count() == 3);
  // destroy engaged object
  copy = test_string_0;
  TEST_ASSERT(test_value.use_count() == 2);
}

TEST_GROUP_RUNNER(etcpal_cpp_variant)
{
  RUN_TEST_CASE(etcpal_cpp_variant, variant);
}
}
