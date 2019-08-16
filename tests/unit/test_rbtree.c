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
#include "unity_fixture.h"
#include "fff.h"

#include <stddef.h>
#include <math.h>

#define INT_ARRAY_SIZE 100

static int incrementing_int_array[INT_ARRAY_SIZE];
static int random_int_array[INT_ARRAY_SIZE];
LwpaRbNode node_pool[INT_ARRAY_SIZE];

class RbTreeTest : public ::testing::Test
{
protected:
  RbTreeTest()
  {
    // Initialize the static variables. They need to be static because they are accessed from the
    // non-member functions (which need to be non-member functions because they are passed as
    // function pointers to the C library)
    alloc_call_count = 0;
    dealloc_call_count = 0;

    // Initialize an array of monotonically increasing ints
    for (int i = 0; i < kIntArraySize; ++i)
      incrementing_int_array_[i] = i;

    // Copy and shuffle the integer array
    random_int_array_ = incrementing_int_array_;
    std::shuffle(random_int_array_.begin(), random_int_array_.end(), rand_);
  }

  static constexpr int kIntArraySize = 100;
  std::array<int, kIntArraySize> incrementing_int_array_;
  std::array<int, kIntArraySize> random_int_array_;
  std::array<LwpaRbNode, kIntArraySize> node_pool_;

  std::random_device seed_;
  std::default_random_engine rand_{seed_()};

public:
  static int alloc_call_count;
  static int dealloc_call_count;

  int clearfunc_call_count{0};
  bool remove_one_flag{false};
  static const int kMagicRemoveValue = kIntArraySize / 2;
};

int RbTreeTest::alloc_call_count;
int RbTreeTest::dealloc_call_count;
constexpr int RbTreeTest::kIntArraySize;

extern "C" {
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
}

TEST_F(RbTreeTest, insert_static)
{
  LwpaRbTree tree;
  ASSERT_TRUE(nullptr != lwpa_rbtree_init(&tree, int_cmp, nullptr, nullptr));
  tree.info = this;

  // Point each node at its respective value and insert it into the tree
  for (int i = 0; i < kIntArraySize; ++i)
  {
    LwpaRbNode* node = &node_pool_[i];
    ASSERT_TRUE(nullptr != lwpa_rbnode_init(node, &random_int_array_[i]));
    ASSERT_EQ(kLwpaErrOk, lwpa_rbtree_insert_node(&tree, node));
  }
  ASSERT_EQ(static_cast<size_t>(kIntArraySize), lwpa_rbtree_size(&tree));
  EXPECT_GT(lwpa_rbtree_test(&tree, tree.root), 0);

  // Find a random number
  std::uniform_int_distribution<> dist(0, kIntArraySize - 1);
  int to_find = dist(rand_);
  int* found = (int*)lwpa_rbtree_find(&tree, &to_find);
  ASSERT_TRUE(found != nullptr);
  EXPECT_EQ(*found, to_find);

  // Try removing one item
  remove_one_flag = true;
  int to_remove = kMagicRemoveValue;
  ASSERT_EQ(kLwpaErrOk, lwpa_rbtree_remove_with_cb(&tree, &to_remove, clear_func));
  ASSERT_FALSE(remove_one_flag);

  // Clear the tree
  EXPECT_EQ(kLwpaErrOk, lwpa_rbtree_clear_with_cb(&tree, clear_func));
  EXPECT_EQ(0u, lwpa_rbtree_size(&tree));
  EXPECT_EQ(clearfunc_call_count, kIntArraySize);
}

TEST_F(RbTreeTest, insert_dynamic)
{
  LwpaRbTree tree;
  ASSERT_TRUE(nullptr != lwpa_rbtree_init(&tree, int_cmp, node_alloc, node_dealloc));

  // Insert each value into the tree; dynamic alloc functions should be called.
  for (int i = 0; i < kIntArraySize; ++i)
    ASSERT_EQ(kLwpaErrOk, lwpa_rbtree_insert(&tree, &random_int_array_[i]));
  ASSERT_EQ(static_cast<size_t>(kIntArraySize), lwpa_rbtree_size(&tree));
  ASSERT_EQ(kIntArraySize, alloc_call_count);
  ASSERT_GT(lwpa_rbtree_test(&tree, tree.root), 0);

  // Find a random number
  int to_find = rand() / (int)(RAND_MAX / kIntArraySize + 1);
  int* found = (int*)lwpa_rbtree_find(&tree, &to_find);
  ASSERT_TRUE(found != nullptr);
  ASSERT_EQ(*found, to_find);

  // Make sure removing something that wasn't in the tree fails.
  int not_in_tree = kIntArraySize + 1;
  ASSERT_EQ(kLwpaErrNotFound, lwpa_rbtree_remove(&tree, &not_in_tree));

  // Clear the tree
  EXPECT_EQ(kLwpaErrOk, lwpa_rbtree_clear(&tree));
  EXPECT_EQ(0u, lwpa_rbtree_size(&tree));
  EXPECT_EQ(dealloc_call_count, kIntArraySize);
}

TEST_F(RbTreeTest, element_already_exists)
{
  LwpaRbTree tree;
  ASSERT_TRUE(nullptr != lwpa_rbtree_init(&tree, int_cmp, node_alloc, node_dealloc));

  // Insert a few values into the tree. Enough so we have some branches to traverse
  for (int i = 0; i < 10; ++i)
    ASSERT_EQ(kLwpaErrOk, lwpa_rbtree_insert(&tree, &incrementing_int_array_[i]));
  ASSERT_EQ(10u, lwpa_rbtree_size(&tree));

  // Try inserting a duplicate value - should fail with error 'already exists'
  int duplicate = 5;
  ASSERT_EQ(kLwpaErrExists, lwpa_rbtree_insert(&tree, &duplicate));

  // Remove the offending value and try inserting it again, should succeed
  ASSERT_EQ(kLwpaErrOk, lwpa_rbtree_remove(&tree, &duplicate));
  ASSERT_EQ(kLwpaErrOk, lwpa_rbtree_insert(&tree, &duplicate));

  // Clear the tree
  EXPECT_EQ(kLwpaErrOk, lwpa_rbtree_clear(&tree));
  EXPECT_EQ(0u, lwpa_rbtree_size(&tree));
}

TEST_F(RbTreeTest, iter)
{
  LwpaRbTree tree;
  ASSERT_TRUE(nullptr != lwpa_rbtree_init(&tree, int_cmp, node_alloc, node_dealloc));

  // Insert each value into the tree; dynamic alloc functions should be called.
  for (int i = 0; i < kIntArraySize; ++i)
    ASSERT_EQ(kLwpaErrOk, lwpa_rbtree_insert(&tree, &random_int_array_[i]));
  ASSERT_EQ(static_cast<size_t>(kIntArraySize), lwpa_rbtree_size(&tree));

  // Initialize an iterator
  LwpaRbIter iter;
  ASSERT_TRUE(nullptr != lwpa_rbiter_init(&iter));

  // Get the first value.
  int* val = (int*)lwpa_rbiter_first(&iter, &tree);
  // Although the elements were inserted in random order, the tree should be sorted so 0 should be
  // the first value.
  ASSERT_TRUE(val != nullptr);
  ASSERT_EQ(*val, 0);

  // Test iterating through the tree in forward order.
  int num_iterations = 1;
  int last_val = *val;
  while ((val = (int*)lwpa_rbiter_next(&iter)) != nullptr)
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
  ASSERT_TRUE(val != nullptr);
  ASSERT_EQ(*val, static_cast<int>(kIntArraySize - 1));

  // Test iterating through the tree in reverse order.
  num_iterations = 1;
  last_val = *val;
  while ((val = (int*)lwpa_rbiter_prev(&iter)) != nullptr)
  {
    ++num_iterations;
    // Each value given by the iterator should be numerically lower than the one that came before
    // it.
    ASSERT_LT(*val, last_val);
    last_val = *val;
  }
  ASSERT_EQ(num_iterations, kIntArraySize);

  // Clear the tree.
  EXPECT_EQ(kLwpaErrOk, lwpa_rbtree_clear(&tree));
  EXPECT_EQ(0u, lwpa_rbtree_size(&tree));
  EXPECT_EQ(kIntArraySize, dealloc_call_count);
}

TEST_F(RbTreeTest, max_height)
{
  LwpaRbTree tree;
  ASSERT_TRUE(nullptr != lwpa_rbtree_init(&tree, int_cmp, node_alloc, node_dealloc));
  // Insert monotonically incrementing values into the tree. In a traditional binary tree, this
  // would result in a worst-case unbalanced tree of height kIntArraySize. In the red-black tree,
  // the maximum height should be determined by the formula 2 * log2(kIntArraySize + 1).
  for (int i = 0; i < kIntArraySize; ++i)
    ASSERT_EQ(kLwpaErrOk, lwpa_rbtree_insert(&tree, &incrementing_int_array_[i]));
  ASSERT_EQ(static_cast<size_t>(kIntArraySize), lwpa_rbtree_size(&tree));

  // Get the height of the tree and compare it against the theoretical maximum.
  LwpaRbIter iter;
  size_t max_height = 0;
  size_t theoretical_max_height;
  ASSERT_TRUE(nullptr != lwpa_rbiter_init(&iter));
  ASSERT_TRUE(nullptr != lwpa_rbiter_first(&iter, &tree));
  do
  {
    if (iter.top > max_height)
      max_height = iter.top;
  } while (nullptr != lwpa_rbiter_next(&iter));
  max_height += 1;
  // http://www.doctrina.org/maximum-height-of-red-black-tree.html
  theoretical_max_height = 2 * (size_t)(log(kIntArraySize + 1) / log(2));
  ASSERT_LE(max_height, theoretical_max_height);

  // Clear the tree
  EXPECT_EQ(kLwpaErrOk, lwpa_rbtree_clear(&tree));
  EXPECT_EQ(0u, lwpa_rbtree_size(&tree));
  EXPECT_EQ(kIntArraySize, dealloc_call_count);
}
