#include <etcpal/cpp/variant.h>

#include <unity_fixture.h>

extern "C" {

TEST_GROUP(etcpal_cpp_variant);

TEST_SETUP(etcpal_cpp_variant)
{
}

TEST_TEAR_DOWN(etcpal_cpp_variant)
{
}

TEST(etcpal_cpp_variant, variant)
{
  // initialization
  TEST_ASSERT_TRUE((etcpal::Variant<int, double>{}.index()) == 0);
  TEST_ASSERT_TRUE((etcpal::Variant<int, double>{etcpal::InPlaceIndex_t<0>{}}).index() == 0);
  TEST_ASSERT_TRUE((etcpal::Variant<int, double>{etcpal::InPlaceIndex_t<1>{}}).index() == 1);
  TEST_ASSERT_TRUE((etcpal::Variant<int, double>{etcpal::InPlaceType_t<int>{}}).index() == 0);
  TEST_ASSERT_TRUE((etcpal::Variant<int, double>{etcpal::InPlaceType_t<double>{}}).index() == 1);

  // comparision
  const auto test_string_0 = std::string{"test string 0"};
  const auto test_string_1 = std::string{"test string 1"};
  TEST_ASSERT_TRUE((etcpal::Variant<int, std::string>{test_string_0} == etcpal::Variant<int, std::string>{test_string_0}));
  TEST_ASSERT_FALSE((etcpal::Variant<int, std::string>{test_string_0} == etcpal::Variant<int, std::string>{test_string_1}));
  TEST_ASSERT_TRUE((etcpal::Variant<int, std::string>{test_string_0} < etcpal::Variant<int, std::string>{test_string_1}));
  TEST_ASSERT_FALSE((etcpal::Variant<int, std::string>{test_string_1} < etcpal::Variant<int, std::string>{test_string_0}));
  TEST_ASSERT_TRUE((etcpal::Variant<int, std::string>{test_string_1} > etcpal::Variant<int, std::string>{test_string_0}));
  TEST_ASSERT_FALSE((etcpal::Variant<int, std::string>{test_string_0} > etcpal::Variant<int, std::string>{test_string_1}));
  TEST_ASSERT_TRUE((etcpal::Variant<int, std::string>{test_string_0} <= etcpal::Variant<int, std::string>{test_string_0}));
  TEST_ASSERT_TRUE((etcpal::Variant<int, std::string>{test_string_0} <= etcpal::Variant<int, std::string>{test_string_1}));
  TEST_ASSERT_FALSE((etcpal::Variant<int, std::string>{test_string_1} <= etcpal::Variant<int, std::string>{test_string_0}));
  TEST_ASSERT_TRUE((etcpal::Variant<int, std::string>{test_string_0} >= etcpal::Variant<int, std::string>{test_string_0}));
  TEST_ASSERT_TRUE((etcpal::Variant<int, std::string>{test_string_1} >= etcpal::Variant<int, std::string>{test_string_0}));
  TEST_ASSERT_FALSE((etcpal::Variant<int, std::string>{test_string_0} >= etcpal::Variant<int, std::string>{test_string_1}));

  // value access
  TEST_ASSERT_TRUE((etcpal::Variant<int, std::string>{test_string_0}).get<std::string>() == test_string_0);
  TEST_ASSERT_TRUE((etcpal::Variant<int, std::string>{test_string_0}).get_if<std::string>());
  TEST_ASSERT_FALSE((etcpal::Variant<int, std::string>{0}).get_if<std::string>());

  // destruction
  const auto test_value = std::make_shared<int>();
  // initialization
  auto test_variant = etcpal::Variant<std::shared_ptr<int>, std::string>{test_value};
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
