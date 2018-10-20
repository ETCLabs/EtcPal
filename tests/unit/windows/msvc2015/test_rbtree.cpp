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
#include "lwpa_rbtree.h"
#include "gtest/gtest.h"
#include <cstddef>
#include <cstdlib>
#include <cmath>
#include <Windows.h>

class RbTreeTest : public ::testing::Test
{
protected:
  RbTreeTest()
    : clearfunc_call_count(0)
    , remove_one_flag(false)
  {
    // Initialize the static variables. They need to be static because they are
    // accessed from the non-member functions (which need to be non-member
    // functions because they are passed as function pointers to the C library)
    alloc_call_count = 0;
    dealloc_call_count = 0;

    // Initialize an array of monotonically increasing ints
    for (int i = 0; i < INT_ARRAY_SIZE; ++i)
      incrementing_int_array[i] = i;

    // Copy and shuffle the integer array
    memcpy(random_int_array, incrementing_int_array, sizeof random_int_array);
    for (int i = 0; i < INT_ARRAY_SIZE; ++i)
    {
      // Swap the number at this index with the number at a random index later
      // in the array
      int index = i + rand() / (RAND_MAX / (INT_ARRAY_SIZE - i) + 1);
      int t = random_int_array[index];
      random_int_array[index] = random_int_array[i];
      random_int_array[i] = t;
    }

    timeBeginPeriod(1);
    srand(timeGetTime());
  }

  virtual ~RbTreeTest()
  {
    timeEndPeriod(1);
  }

  static const size_t INT_ARRAY_SIZE = 100;
  int incrementing_int_array[INT_ARRAY_SIZE];
  int random_int_array[INT_ARRAY_SIZE];
  LwpaRbNode node_pool[INT_ARRAY_SIZE];

public:
  static unsigned int alloc_call_count;
  static unsigned int dealloc_call_count;

  unsigned int clearfunc_call_count;
  bool remove_one_flag;
  static const int MAGIC_REMOVE_VALUE = INT_ARRAY_SIZE / 2;
};

unsigned int RbTreeTest::alloc_call_count;
unsigned int RbTreeTest::dealloc_call_count;

LwpaRbNode *node_alloc()
{
  ++RbTreeTest::alloc_call_count;
  return new LwpaRbNode;
}

void node_dealloc(LwpaRbNode *node)
{
  ++RbTreeTest::dealloc_call_count;
  delete node;
}

static int
int_cmp(const LwpaRbTree *self, const LwpaRbNode *node_a, const LwpaRbNode *node_b)
{
  (void)self;
  int a = *(int *)node_a->value;
  int b = *(int *)node_b->value;
  return a - b;
}

static void
clear_func(const LwpaRbTree *self, LwpaRbNode *node)
{
  RbTreeTest *rbtt = static_cast<RbTreeTest *>(self->info);
  if (rbtt)
  {
    if (rbtt->remove_one_flag &&
      *(int *)node->value == RbTreeTest::MAGIC_REMOVE_VALUE)
    {
      rbtt->remove_one_flag = false;
    }
    ++rbtt->clearfunc_call_count;
  }
}

TEST_F(RbTreeTest, insert_static)
{
  LwpaRbTree tree;
  ASSERT_TRUE(NULL != rb_tree_init(&tree, int_cmp, NULL, NULL));
  tree.info = this;

  // Point each node at its respective value and insert it into the tree
  for (size_t i = 0; i < INT_ARRAY_SIZE; ++i)
  {
    LwpaRbNode *node = &node_pool[i];
    ASSERT_TRUE(NULL != rb_node_init(node, &random_int_array[i]));
    ASSERT_NE(0, rb_tree_insert_node(&tree, node));
  }
  ASSERT_EQ(INT_ARRAY_SIZE, rb_tree_size(&tree));
  ASSERT_NE(0, rb_tree_test(&tree, tree.root));

  // Find a random number
  int to_find = rand() / (RAND_MAX / INT_ARRAY_SIZE + 1);
  int *found = (int *)rb_tree_find(&tree, &to_find);
  ASSERT_TRUE(found != NULL);
  ASSERT_EQ(*found, to_find);

  // Try removing one item
  remove_one_flag = true;
  int to_remove = MAGIC_REMOVE_VALUE;
  ASSERT_NE(0, rb_tree_remove_with_cb(&tree, &to_remove, clear_func));
  ASSERT_FALSE(remove_one_flag);

  // Clear the tree
  ASSERT_NE(0, rb_tree_clear_with_cb(&tree, clear_func));
  ASSERT_EQ(0u, rb_tree_size(&tree));
  ASSERT_EQ(clearfunc_call_count, INT_ARRAY_SIZE);
}

TEST_F(RbTreeTest, insert_dynamic)
{
  LwpaRbTree tree;
  ASSERT_TRUE(NULL != rb_tree_init(&tree, int_cmp, node_alloc, node_dealloc));

  // Insert each value into the tree; dynamic alloc functions should be called.
  for (size_t i = 0; i < INT_ARRAY_SIZE; ++i)
    ASSERT_NE(0, rb_tree_insert(&tree, &random_int_array[i]));
  ASSERT_EQ(INT_ARRAY_SIZE, rb_tree_size(&tree));
  ASSERT_EQ(INT_ARRAY_SIZE, alloc_call_count);
  ASSERT_NE(0, rb_tree_test(&tree, tree.root));

  // Find a random number
  int to_find = rand() / (RAND_MAX / INT_ARRAY_SIZE + 1);
  int *found = (int *)rb_tree_find(&tree, &to_find);
  ASSERT_TRUE(found != NULL);
  ASSERT_EQ(*found, to_find);

  // Make sure removing something that wasn't in the tree fails.
  int not_in_tree = INT_ARRAY_SIZE + 1;
  ASSERT_EQ(0, rb_tree_remove(&tree, &not_in_tree));

  // Clear the tree
  ASSERT_NE(0, rb_tree_clear(&tree));
  ASSERT_EQ(0u, rb_tree_size(&tree));
  ASSERT_EQ(dealloc_call_count, INT_ARRAY_SIZE);
}

TEST_F(RbTreeTest, iter)
{
  LwpaRbTree tree;
  ASSERT_TRUE(NULL != rb_tree_init(&tree, int_cmp, node_alloc, node_dealloc));

  // Insert each value into the tree; dynamic alloc functions should be called.
  for (size_t i = 0; i < INT_ARRAY_SIZE; ++i)
    ASSERT_NE(0, rb_tree_insert(&tree, &random_int_array[i]));
  ASSERT_EQ(INT_ARRAY_SIZE, rb_tree_size(&tree));

  // Initialize an iterator
  LwpaRbIter iter;
  ASSERT_TRUE(NULL != rb_iter_init(&iter));

  // Get the first value.
  int *val = (int *)rb_iter_first(&iter, &tree);
  // Although the elements were inserted in random order, the tree should be
  // sorted so 0 should be the first value.
  ASSERT_EQ(*val, 0);

  // Test iterating through the tree in forward order.
  size_t num_iterations = 1;
  int last_val = *val;
  while ((val = (int *)rb_iter_next(&iter)) != NULL)
  {
    ++num_iterations;
    // Each value given by the iterator should be numerically higher than the
    // one that came before it.
    ASSERT_GT(*val, last_val);
    last_val = *val;
  }
  ASSERT_EQ(num_iterations, INT_ARRAY_SIZE);

  // Get the last value.
  val = (int *)rb_iter_last(&iter, &tree);
  ASSERT_EQ(*val, static_cast<int>(INT_ARRAY_SIZE - 1));

  // Test iterating through the tree in reverse order.
  num_iterations = 1;
  last_val = *val;
  while ((val = (int *)rb_iter_prev(&iter)) != NULL)
  {
    ++num_iterations;
    // Each value given by the iterator should be numerically lower than the
    // one that came before it.
    ASSERT_LT(*val, last_val);
    last_val = *val;
  }
  ASSERT_EQ(num_iterations, INT_ARRAY_SIZE);

  // Clear the tree.
  ASSERT_NE(0, rb_tree_clear(&tree));
  ASSERT_EQ(0u, rb_tree_size(&tree));
  ASSERT_EQ(INT_ARRAY_SIZE, dealloc_call_count);
}

TEST_F(RbTreeTest, max_height)
{
  LwpaRbTree tree;
  ASSERT_TRUE(NULL != rb_tree_init(&tree, int_cmp, node_alloc, node_dealloc));
  // Insert monotonically incrementing values into the tree. In a traditional
  // binary tree, this would result in a worst-case unbalanced tree of height
  // INT_ARRAY_SIZE. In the red-black tree, the maximum height should be
  // determined by the formula 2 * log2(INT_ARRAY_SIZE + 1). */
  for (size_t i = 0; i < INT_ARRAY_SIZE; ++i)
    ASSERT_NE(0, rb_tree_insert(&tree, &incrementing_int_array[i]));
  ASSERT_EQ(INT_ARRAY_SIZE, rb_tree_size(&tree));

  // Get the height of the tree and compare it against the theoretical maximum.
  LwpaRbIter iter;
  size_t max_height = 0;
  size_t theoretical_max_height;
  ASSERT_TRUE(NULL != rb_iter_init(&iter));
  ASSERT_TRUE(NULL != rb_iter_first(&iter, &tree));
  do
  {
    if (iter.top > max_height)
      max_height = iter.top;
  } while (NULL != rb_iter_next(&iter));
  max_height += 1;
  // http://www.doctrina.org/maximum-height-of-red-black-tree.html
  theoretical_max_height = 2 * (size_t)(log(INT_ARRAY_SIZE + 1) / log(2));
  ASSERT_LE(max_height, theoretical_max_height);

  // Clear the tree
  ASSERT_NE(0, rb_tree_clear(&tree));
  ASSERT_EQ(0u, rb_tree_size(&tree));
  ASSERT_EQ(INT_ARRAY_SIZE, dealloc_call_count);
}
