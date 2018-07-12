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
#include "lwpa_mempool.h"
#include "gtest/gtest.h"
#include <cstddef>
#include <cstdlib>
#include <vector>
#include <Windows.h>

class MempoolTest : public ::testing::Test
{
protected:
  MempoolTest()
  {
    timeBeginPeriod(1);
    srand(timeGetTime());
  }

  virtual ~MempoolTest()
  {
    timeEndPeriod(1);
  }

  static const size_t TEST_ALLOC_MEMP_SIZE = 500;

  struct test_elem
  {
    int val1;
    char val2;
  };
  LWPA_MEMPOOL_DEFINE(alloc_test, test_elem, TEST_ALLOC_MEMP_SIZE);

  struct alloctest
  {
    alloctest()
      : elem(NULL)
      , freed(false)
    {
    }

    struct test_elem *elem;
    bool freed;
  };
  std::vector<alloctest> test_vec;
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
    alloctest test_val;
    test_val.elem = static_cast<struct test_elem *>(lwpa_mempool_alloc(alloc_test));
    ASSERT_TRUE(test_val.elem != NULL);
    test_vec.push_back(test_val);
  }
  ASSERT_EQ(TEST_ALLOC_MEMP_SIZE, lwpa_mempool_used(alloc_test));

  // Free the elements back in random order.
  size_t num_freed = 0;
  while (num_freed < TEST_ALLOC_MEMP_SIZE)
  {
    size_t i = (rand() % TEST_ALLOC_MEMP_SIZE);
    if (!test_vec[i].freed)
    {
      lwpa_mempool_free(alloc_test, test_vec[i].elem);
      test_vec[i].freed = true;
      ++num_freed;
    }
  }
  ASSERT_EQ(0u, lwpa_mempool_used(alloc_test));

  // Make sure we can allocate the entire pool again.
  for (size_t i = 0; i < TEST_ALLOC_MEMP_SIZE; ++i)
  {
    test_vec[i].elem = static_cast<struct test_elem *>(lwpa_mempool_alloc(alloc_test));
    ASSERT_TRUE(test_vec[i].elem != NULL);
  }
}
