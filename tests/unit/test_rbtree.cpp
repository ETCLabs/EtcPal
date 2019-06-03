/******************************************************************************
 * Copyright 2019 ETC Inc.
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
#include "lwpa/rbtree.h"
#include "gtest/gtest.h"
#include <cstddef>
#include <cmath>
#include <random>
#include <algorithm>
#include <array>

class RbTreeTest : public ::testing::Test
{
protected:
  RbTreeTest() : clearfunc_call_count(0), remove_one_flag(false)
  {
    // Initialize the static variables. They need to be static because they are accessed from the
    // non-member functions (which need to be non-member functions because they are passed as
    // function pointers to the C library)
    alloc_call_count = 0;
    dealloc_call_count = 0;

    // Initialize an array of monotonically increasing ints
    for (int i = 0; i < kIntArraySize; ++i)
      incrementing_int_array[i] = i;

    // Copy and shuffle the integer array
    random_int_array = incrementing_int_array;
    std::random_device seed;
    std::default_random_engine rand(seed());
    std::shuffle(random_int_array.begin(), random_int_array.end(), rand);
  }

  static constexpr size_t kIntArraySize = 100;
  std::array<int, kIntArraySize> incrementing_int_array;
  std::array<int, kIntArraySize> random_int_array;
  std::array<LwpaRbNode, kIntArraySize> node_pool;

public:
  static unsigned int alloc_call_count;
  static unsigned int dealloc_call_count;

  unsigned int clearfunc_call_count;
  bool remove_one_flag;
  static const int kMagicRemoveValue = kIntArraySize / 2;
};

unsigned int RbTreeTest::alloc_call_count;
unsigned int RbTreeTest::dealloc_call_count;
constexpr size_t RbTreeTest::kIntArraySize;

LwpaRbNode* node_alloc()
{
  ++RbTreeTest::alloc_call_count;
  return new LwpaRbNode;
}

void node_dealloc(LwpaRbNode* node)
{
  ++RbTreeTest::dealloc_call_count;
  delete node;
}

static int int_cmp(const LwpaRbTree* self, const LwpaRbNode* node_a, const LwpaRbNode* node_b)
{
  (void)self;
  int a = *(int*)node_a->value;
  int b = *(int*)node_b->value;
  return a - b;
}

static void clear_func(const LwpaRbTree* self, LwpaRbNode* node)
{
  RbTreeTest* rbtt = static_cast<RbTreeTest*>(self->info);
  if (rbtt)
  {
    if (rbtt->remove_one_flag && *(int*)node->value == RbTreeTest::kMagicRemoveValue)
    {
      rbtt->remove_one_flag = false;
    }
    ++rbtt->clearfunc_call_count;
  }
}

TEST_F(RbTreeTest, insert_static)
{
  LwpaRbTree tree;
  ASSERT_TRUE(NULL != lwpa_rbtree_init(&tree, int_cmp, NULL, NULL));
  tree.info = this;

  // Point each node at its respective value and insert it into the tree
  for (size_t i = 0; i < kIntArraySize; ++i)
  {
    LwpaRbNode* node = &node_pool[i];
    ASSERT_TRUE(NULL != lwpa_rbnode_init(node, &random_int_array[i]));
    ASSERT_NE(0, lwpa_rbtree_insert_node(&tree, node));
  }
  ASSERT_EQ(kIntArraySize, lwpa_rbtree_size(&tree));
  ASSERT_NE(0, lwpa_rbtree_test(&tree, tree.root));

  // Find a random number
  int to_find = rand() / (RAND_MAX / kIntArraySize + 1);
  int* found = (int*)lwpa_rbtree_find(&tree, &to_find);
  ASSERT_TRUE(found != NULL);
  ASSERT_EQ(*found, to_find);

  // Try removing one item
  remove_one_flag = true;
  int to_remove = kMagicRemoveValue;
  ASSERT_NE(0, lwpa_rbtree_remove_with_cb(&tree, &to_remove, clear_func));
  ASSERT_FALSE(remove_one_flag);

  // Clear the tre
  ASSERT_NE(0, lwpa_rbtree_clear_with_cb(&tree, clear_func));
  ASSERT_EQ(0u, lwpa_rbtree_size(&tree));
  ASSERT_EQ(clearfunc_call_count, kIntArraySize);
}

TEST_F(RbTreeTest, insert_dynamic)
{
  LwpaRbTree tree;
  ASSERT_TRUE(NULL != lwpa_rbtree_init(&tree, int_cmp, node_alloc, node_dealloc));

  // Insert each value into the tree; dynamic alloc functions should be called.
  for (size_t i = 0; i < kIntArraySize; ++i)
    ASSERT_NE(0, lwpa_rbtree_insert(&tree, &random_int_array[i]));
  ASSERT_EQ(kIntArraySize, lwpa_rbtree_size(&tree));
  ASSERT_EQ(kIntArraySize, alloc_call_count);
  ASSERT_NE(0, lwpa_rbtree_test(&tree, tree.root));

  // Find a random number
  int to_find = rand() / (RAND_MAX / kIntArraySize + 1);
  int* found = (int*)lwpa_rbtree_find(&tree, &to_find);
  ASSERT_TRUE(found != NULL);
  ASSERT_EQ(*found, to_find);

  // Make sure removing something that wasn't in the tree fails.
  int not_in_tree = kIntArraySize + 1;
  ASSERT_EQ(0, lwpa_rbtree_remove(&tree, &not_in_tree));

  // Clear the tree
  ASSERT_NE(0, lwpa_rbtree_clear(&tree));
  ASSERT_EQ(0u, lwpa_rbtree_size(&tree));
  ASSERT_EQ(dealloc_call_count, kIntArraySize);
}

TEST_F(RbTreeTest, iter)
{
  LwpaRbTree tree;
  ASSERT_TRUE(NULL != lwpa_rbtree_init(&tree, int_cmp, node_alloc, node_dealloc));

  // Insert each value into the tree; dynamic alloc functions should be called.
  for (size_t i = 0; i < kIntArraySize; ++i)
    ASSERT_NE(0, lwpa_rbtree_insert(&tree, &random_int_array[i]));
  ASSERT_EQ(kIntArraySize, lwpa_rbtree_size(&tree));

  // Initialize an iterator
  LwpaRbIter iter;
  ASSERT_TRUE(NULL != lwpa_rbiter_init(&iter));

  // Get the first value.
  int* val = (int*)lwpa_rbiter_first(&iter, &tree);
  // Although the elements were inserted in random order, the tree should be sorted so 0 should be
  // the first value.
  ASSERT_EQ(*val, 0);

  // Test iterating through the tree in forward order.
  size_t num_iterations = 1;
  int last_val = *val;
  while ((val = (int*)lwpa_rbiter_next(&iter)) != NULL)
  {
    ++num_iterations;
    // Each value given by the iterator should be numerically higher than the one that came before
    // it.
    ASSERT_GT(*val, last_val);
    last_val = *val;
  }
  ASSERT_EQ(num_iterations, kIntArraySize);

  // Get the last value.
  val = (int*)lwpa_rbiter_last(&iter, &tree);
  ASSERT_EQ(*val, static_cast<int>(kIntArraySize - 1));

  // Test iterating through the tree in reverse order.
  num_iterations = 1;
  last_val = *val;
  while ((val = (int*)lwpa_rbiter_prev(&iter)) != NULL)
  {
    ++num_iterations;
    // Each value given by the iterator should be numerically lower than the one that came before
    // it.
    ASSERT_LT(*val, last_val);
    last_val = *val;
  }
  ASSERT_EQ(num_iterations, kIntArraySize);

  // Clear the tree.
  ASSERT_NE(0, lwpa_rbtree_clear(&tree));
  ASSERT_EQ(0u, lwpa_rbtree_size(&tree));
  ASSERT_EQ(kIntArraySize, dealloc_call_count);
}

TEST_F(RbTreeTest, max_height)
{
  LwpaRbTree tree;
  ASSERT_TRUE(NULL != lwpa_rbtree_init(&tree, int_cmp, node_alloc, node_dealloc));
  // Insert monotonically incrementing values into the tree. In a traditional binary tree, this
  // would result in a worst-case unbalanced tree of height kIntArraySize. In the red-black tree,
  // the maximum height should be determined by the formula 2 * log2(kIntArraySize + 1).
  for (size_t i = 0; i < kIntArraySize; ++i)
    ASSERT_NE(0, lwpa_rbtree_insert(&tree, &incrementing_int_array[i]));
  ASSERT_EQ(kIntArraySize, lwpa_rbtree_size(&tree));

  // Get the height of the tree and compare it against the theoretical maximum.
  LwpaRbIter iter;
  size_t max_height = 0;
  size_t theoretical_max_height;
  ASSERT_TRUE(NULL != lwpa_rbiter_init(&iter));
  ASSERT_TRUE(NULL != lwpa_rbiter_first(&iter, &tree));
  do
  {
    if (iter.top > max_height)
      max_height = iter.top;
  } while (NULL != lwpa_rbiter_next(&iter));
  max_height += 1;
  // http://www.doctrina.org/maximum-height-of-red-black-tree.html
  theoretical_max_height = 2 * (size_t)(log(kIntArraySize + 1) / log(2));
  ASSERT_LE(max_height, theoretical_max_height);

  // Clear the tree
  ASSERT_NE(0, lwpa_rbtree_clear(&tree));
  ASSERT_EQ(0u, lwpa_rbtree_size(&tree));
  ASSERT_EQ(kIntArraySize, dealloc_call_count);
}
