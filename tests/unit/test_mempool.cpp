/******************************************************************************
 * Copyright 2018 ETC Inc.
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
 * This file is a part of lwpa. For more information, go to:
 * https://github.com/ETCLabs/lwpa
 ******************************************************************************/
#include "lwpa/mempool.h"
#include "gtest/gtest.h"
#include <cstddef>
#include <vector>
#include <array>
#include <random>
#include <algorithm>

class MempoolTest : public ::testing::Test
{
protected:
  static const size_t TEST_ALLOC_MEMP_SIZE = 500;
  static const size_t TEST_ALLOC_MEMP_ARRAY_SIZE = 30;

  struct TestElem
  {
    int val1;
    char val2;
  };
  LWPA_MEMPOOL_DEFINE(alloc_test, TestElem, TEST_ALLOC_MEMP_SIZE);
  LWPA_MEMPOOL_DEFINE_ARRAY(alloc_array_test, TestElem, TEST_ALLOC_MEMP_ARRAY_SIZE, TEST_ALLOC_MEMP_SIZE);

  std::vector<TestElem *> test_vec;
};

TEST_F(MempoolTest, alloc_free)
{
  // Initialize the pool.
  ASSERT_EQ(LWPA_OK, lwpa_mempool_init(alloc_test));
  ASSERT_EQ(TEST_ALLOC_MEMP_SIZE, lwpa_mempool_size(alloc_test));

  // Allocate the entire pool.
  test_vec.reserve(TEST_ALLOC_MEMP_SIZE);
  for (size_t i = 0; i < TEST_ALLOC_MEMP_SIZE; ++i)
  {
    auto elem = static_cast<TestElem *>(lwpa_mempool_alloc(alloc_test));
    ASSERT_TRUE(elem != NULL);
    test_vec.push_back(elem);
  }
  ASSERT_EQ(TEST_ALLOC_MEMP_SIZE, lwpa_mempool_used(alloc_test));

  // Free the elements back in random order.
  // Generate and shuffle the index array.
  std::array<size_t, TEST_ALLOC_MEMP_SIZE> free_indices;
  for (size_t i = 0; i < TEST_ALLOC_MEMP_SIZE; ++i)
    free_indices[i] = i;
  std::random_device seed;
  std::default_random_engine rand(seed());
  std::shuffle(free_indices.begin(), free_indices.end(), rand);

  for (size_t i = 0; i < TEST_ALLOC_MEMP_SIZE; ++i)
  {
    lwpa_mempool_free(alloc_test, test_vec[free_indices[i]]);
  }
  ASSERT_EQ(0u, lwpa_mempool_used(alloc_test));

  // Make sure we can allocate the entire pool again.
  for (size_t i = 0; i < TEST_ALLOC_MEMP_SIZE; ++i)
  {
    test_vec[i] = static_cast<TestElem *>(lwpa_mempool_alloc(alloc_test));
    ASSERT_TRUE(test_vec[i] != NULL);
  }
}

TEST_F(MempoolTest, alloc_free_array)
{
  // Initialize the pool.
  ASSERT_EQ(LWPA_OK, lwpa_mempool_init(alloc_array_test));
  ASSERT_EQ(TEST_ALLOC_MEMP_SIZE, lwpa_mempool_size(alloc_array_test));

  // Allocate the entire pool.
  test_vec.reserve(TEST_ALLOC_MEMP_SIZE);
  for (size_t i = 0; i < TEST_ALLOC_MEMP_SIZE; ++i)
  {
    auto elem_arr = static_cast<TestElem *>(lwpa_mempool_alloc(alloc_array_test));
    ASSERT_TRUE(elem_arr != NULL);

    // Write to each spot in the array - failure could be caught by a segfault or similar
    for (size_t j = 0; j < TEST_ALLOC_MEMP_ARRAY_SIZE; ++j)
    {
      elem_arr[j].val1 = 1;
      elem_arr[j].val2 = 2;
    }

    test_vec.push_back(elem_arr);
  }
  ASSERT_EQ(TEST_ALLOC_MEMP_SIZE, lwpa_mempool_used(alloc_array_test));

  // Free the elements back in random order.
  // Generate and shuffle the index array.
  std::array<size_t, TEST_ALLOC_MEMP_SIZE> free_indices;
  for (size_t i = 0; i < TEST_ALLOC_MEMP_SIZE; ++i)
    free_indices[i] = i;
  std::random_device seed;
  std::default_random_engine rand(seed());
  std::shuffle(free_indices.begin(), free_indices.end(), rand);

  for (size_t i = 0; i < TEST_ALLOC_MEMP_SIZE; ++i)
  {
    lwpa_mempool_free(alloc_array_test, test_vec[free_indices[i]]);
  }
  ASSERT_EQ(0u, lwpa_mempool_used(alloc_array_test));

  // Make sure we can allocate the entire pool again.
  for (size_t i = 0; i < TEST_ALLOC_MEMP_SIZE; ++i)
  {
    test_vec[i] = static_cast<TestElem *>(lwpa_mempool_alloc(alloc_array_test));

    // Make sure the sentinel values we set before are still there.
    for (size_t j = 0; j < TEST_ALLOC_MEMP_ARRAY_SIZE; ++j)
    {
      ASSERT_EQ(test_vec[i][j].val1, 1);
      ASSERT_EQ(test_vec[i][j].val2, 2);
    }

    ASSERT_TRUE(test_vec[i] != NULL);
  }
}