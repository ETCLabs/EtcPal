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
 * This file is a part of EtcPal. For more information, go to:
 * https://github.com/ETCLabs/EtcPal
 ******************************************************************************/
#include "etcpal/rbtree.h"
#include "unity_fixture.h"
#include "fff.h"

#include <math.h>
#include <stddef.h>
#include <stdio.h>

// Disable sprintf() warning on Windows/MSVC
#ifdef _MSC_VER
#pragma warning(disable : 4996)
#endif

#define INT_ARRAY_SIZE 100
#define RANDOM_INT_IN_ARRAY() (int)(rand() / ((RAND_MAX + 1.0) / INT_ARRAY_SIZE))

static int incrementing_int_array[INT_ARRAY_SIZE];
static int random_int_array[INT_ARRAY_SIZE];

LwpaRbNode node_pool[INT_ARRAY_SIZE];
size_t next_node_index;

// Private function prototypes

static LwpaRbNode* get_node();
static int int_compare(const LwpaRbTree* self, const LwpaRbNode* node_a, const LwpaRbNode* node_b);
static void populate_int_arrays();

FAKE_VALUE_FUNC(LwpaRbNode*, node_alloc);
FAKE_VOID_FUNC(node_dealloc, LwpaRbNode*);
FAKE_VOID_FUNC(clear_func, const LwpaRbTree*, LwpaRbNode*);

// Very very dumb "allocation" of nodes but all we need for these tests - each call returns the
// next one from the array, freeing does nothing.
LwpaRbNode* get_node(void)
{
  return &node_pool[next_node_index++];
}

int int_compare(const LwpaRbTree* self, const LwpaRbNode* node_a, const LwpaRbNode* node_b)
{
  (void)self;
  int a = *(int*)node_a->value;
  int b = *(int*)node_b->value;
  return a - b;
}

void populate_int_arrays()
{
  // Initialize two arrays of monotonically increasing ints
  for (int i = 0; i < INT_ARRAY_SIZE; ++i)
  {
    incrementing_int_array[i] = i;
    random_int_array[i] = i;
  }

  // Shuffle the random array
  for (int i = 0; i < INT_ARRAY_SIZE - 1; ++i)
  {
    // srand() is taken care of at the entry point.
    int j = i + rand() / (RAND_MAX / (INT_ARRAY_SIZE - i) + 1);
    int swap_val = random_int_array[j];
    random_int_array[j] = random_int_array[i];
    random_int_array[i] = swap_val;
  }
}

// The tests themselves

TEST_GROUP(lwpa_rbtree);

TEST_SETUP(lwpa_rbtree)
{
  RESET_FAKE(node_alloc);
  RESET_FAKE(node_dealloc);
  RESET_FAKE(clear_func);

  node_alloc_fake.custom_fake = get_node;

  populate_int_arrays();
  next_node_index = 0;
}

TEST_TEAR_DOWN(lwpa_rbtree)
{
}

TEST(lwpa_rbtree, insert_node_functions_work)
{
  LwpaRbTree tree;
  TEST_ASSERT_NOT_NULL(lwpa_rbtree_init(&tree, int_compare, NULL, NULL));

  // Point each node at its respective value and insert it into the tree
  for (int i = 0; i < INT_ARRAY_SIZE; ++i)
  {
    LwpaRbNode* node = &node_pool[i];
    TEST_ASSERT_NOT_NULL(lwpa_rbnode_init(node, &random_int_array[i]));
    TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_rbtree_insert_node(&tree, node));
  }
  TEST_ASSERT_EQUAL_UINT(INT_ARRAY_SIZE, lwpa_rbtree_size(&tree));
  TEST_ASSERT_GREATER_THAN(0, lwpa_rbtree_test(&tree, tree.root));

  // Find a random number
  int to_find = RANDOM_INT_IN_ARRAY();

  char test_error_msg[100];
  sprintf(test_error_msg, "Couldn't find value %d in the tree.", to_find);

  int* found = (int*)lwpa_rbtree_find(&tree, &to_find);
  TEST_ASSERT_NOT_NULL_MESSAGE(found, test_error_msg);
  TEST_ASSERT_EQUAL(*found, to_find);

  // Try removing one item
  int to_remove = RANDOM_INT_IN_ARRAY();
  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_rbtree_remove_with_cb(&tree, &to_remove, clear_func));
  TEST_ASSERT_EQUAL(to_remove, *((int*)(clear_func_fake.arg1_val->value)));

  // Clear the tree
  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_rbtree_clear_with_cb(&tree, clear_func));
  TEST_ASSERT_EQUAL_UINT(0u, lwpa_rbtree_size(&tree));
  TEST_ASSERT_EQUAL_UINT(clear_func_fake.call_count, INT_ARRAY_SIZE);
}

TEST(lwpa_rbtree, insert_functions_work)
{
  LwpaRbTree tree;
  TEST_ASSERT_NOT_NULL(lwpa_rbtree_init(&tree, int_compare, node_alloc, node_dealloc));

  // Insert each value into the tree; dynamic alloc functions should be called.
  for (int i = 0; i < INT_ARRAY_SIZE; ++i)
    TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_rbtree_insert(&tree, &random_int_array[i]));
  TEST_ASSERT_EQUAL_UINT(INT_ARRAY_SIZE, lwpa_rbtree_size(&tree));
  TEST_ASSERT_EQUAL_UINT(INT_ARRAY_SIZE, node_alloc_fake.call_count);
  TEST_ASSERT_GREATER_THAN(0, lwpa_rbtree_test(&tree, tree.root));

  // Find a random number
  int to_find = RANDOM_INT_IN_ARRAY();

  char test_error_msg[100];
  sprintf(test_error_msg, "Couldn't find value %d in the tree.", to_find);

  int* found = (int*)lwpa_rbtree_find(&tree, &to_find);
  TEST_ASSERT_NOT_NULL_MESSAGE(found, test_error_msg);
  TEST_ASSERT_EQUAL(*found, to_find);

  // Make sure removing something that wasn't in the tree fails.
  int not_in_tree = INT_ARRAY_SIZE;
  TEST_ASSERT_EQUAL(kLwpaErrNotFound, lwpa_rbtree_remove(&tree, &not_in_tree));
  TEST_ASSERT_EQUAL_UINT(node_dealloc_fake.call_count, 0u);

  // Clear the tree
  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_rbtree_clear(&tree));
  TEST_ASSERT_EQUAL_UINT(0u, lwpa_rbtree_size(&tree));
  TEST_ASSERT_EQUAL_UINT(node_dealloc_fake.call_count, INT_ARRAY_SIZE);
}

TEST(lwpa_rbtree, insert_should_fail_if_element_already_exists)
{
  LwpaRbTree tree;
  TEST_ASSERT_NOT_NULL(lwpa_rbtree_init(&tree, int_compare, node_alloc, node_dealloc));

  // Insert a few values into the tree. Enough so we have some branches to traverse
  for (int i = 0; i < 10; ++i)
    TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_rbtree_insert(&tree, &incrementing_int_array[i]));
  TEST_ASSERT_EQUAL_UINT(10u, lwpa_rbtree_size(&tree));

  // Try inserting a duplicate value - should fail with error 'already exists'
  int duplicate = 5;
  TEST_ASSERT_EQUAL(kLwpaErrExists, lwpa_rbtree_insert(&tree, &duplicate));

  // Remove the offending value and try inserting it again, should succeed
  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_rbtree_remove(&tree, &duplicate));
  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_rbtree_insert(&tree, &duplicate));
}

TEST(lwpa_rbtree, insert_failure_should_not_leak_memory)
{
  LwpaRbTree tree;
  TEST_ASSERT_NOT_NULL(lwpa_rbtree_init(&tree, int_compare, node_alloc, node_dealloc));

  // Insert a value, then try to insert it again
  int val = 0;
  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_rbtree_insert(&tree, &val));
  TEST_ASSERT_EQUAL(kLwpaErrExists, lwpa_rbtree_insert(&tree, &val));

  // Now clear the tree
  TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_rbtree_clear(&tree));

  // The alloc and dealloc count should be equal - no memory should be leaked
  TEST_ASSERT_EQUAL(node_alloc_fake.call_count, node_dealloc_fake.call_count);
}

TEST(lwpa_rbtree, iterators_work_as_expected)
{
  LwpaRbTree tree;
  TEST_ASSERT_NOT_NULL(lwpa_rbtree_init(&tree, int_compare, node_alloc, node_dealloc));

  // Insert each value into the tree; dynamic alloc functions should be called.
  for (int i = 0; i < INT_ARRAY_SIZE; ++i)
    TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_rbtree_insert(&tree, &random_int_array[i]));
  TEST_ASSERT_EQUAL_UINT(INT_ARRAY_SIZE, lwpa_rbtree_size(&tree));

  // Initialize an iterator
  LwpaRbIter iter;
  TEST_ASSERT_EQUAL_PTR(&iter, lwpa_rbiter_init(&iter));

  // Get the first value.
  int* val = (int*)lwpa_rbiter_first(&iter, &tree);
  // Although the elements were inserted in random order, the tree should be sorted so 0 should be
  // the first value.
  TEST_ASSERT_NOT_NULL(val);
  TEST_ASSERT_EQUAL(*val, 0);

  // Test iterating through the tree in forward order.
  int num_iterations = 1;
  int last_val = *val;
  while ((val = (int*)lwpa_rbiter_next(&iter)) != NULL)
  {
    ++num_iterations;
    // Each value given by the iterator should be the next one in order in the tree.
    TEST_ASSERT_EQUAL(*val, last_val + 1);
    last_val = *val;
  }
  TEST_ASSERT_EQUAL(num_iterations, INT_ARRAY_SIZE);

  // Get the last value.
  val = (int*)lwpa_rbiter_last(&iter, &tree);
  TEST_ASSERT_NOT_NULL(val);
  TEST_ASSERT_EQUAL(*val, INT_ARRAY_SIZE - 1);

  // Test iterating through the tree in reverse order.
  num_iterations = 1;
  last_val = *val;
  while ((val = (int*)lwpa_rbiter_prev(&iter)) != NULL)
  {
    ++num_iterations;
    // Each value given by the iterator should be the previous one in order in the tree.
    TEST_ASSERT_EQUAL(*val, last_val - 1);
    last_val = *val;
  }
  TEST_ASSERT_EQUAL(num_iterations, INT_ARRAY_SIZE);
}

TEST(lwpa_rbtree, max_height_is_within_bounds)
{
  LwpaRbTree tree;
  TEST_ASSERT_NOT_NULL(lwpa_rbtree_init(&tree, int_compare, node_alloc, node_dealloc));

  // Insert monotonically incrementing values into the tree. In a traditional binary tree, this
  // would result in a worst-case unbalanced tree of height INT_ARRAY_SIZE. In the red-black tree,
  // the maximum height should be determined by the formula 2 * log2(INT_ARRAY_SIZE + 1).
  for (int i = 0; i < INT_ARRAY_SIZE; ++i)
    TEST_ASSERT_EQUAL(kLwpaErrOk, lwpa_rbtree_insert(&tree, &incrementing_int_array[i]));
  TEST_ASSERT_EQUAL_UINT(INT_ARRAY_SIZE, lwpa_rbtree_size(&tree));

  // Get the height of the tree and compare it against the theoretical maximum.
  LwpaRbIter iter;
  size_t max_height = 0;
  size_t theoretical_max_height;
  TEST_ASSERT_NOT_NULL(lwpa_rbiter_init(&iter));
  TEST_ASSERT_NOT_NULL(lwpa_rbiter_first(&iter, &tree));
  do
  {
    if (iter.top > max_height)
      max_height = iter.top;
  } while (NULL != lwpa_rbiter_next(&iter));
  max_height += 1;
  // http://www.doctrina.org/maximum-height-of-red-black-tree.html
  theoretical_max_height = 2 * (size_t)(log(INT_ARRAY_SIZE + 1) / log(2));
  TEST_ASSERT_LESS_OR_EQUAL_UINT(theoretical_max_height, max_height);
}

TEST_GROUP_RUNNER(lwpa_rbtree)
{
  RUN_TEST_CASE(lwpa_rbtree, insert_node_functions_work);
  RUN_TEST_CASE(lwpa_rbtree, insert_functions_work);
  RUN_TEST_CASE(lwpa_rbtree, insert_should_fail_if_element_already_exists);
  RUN_TEST_CASE(lwpa_rbtree, insert_failure_should_not_leak_memory);
  RUN_TEST_CASE(lwpa_rbtree, iterators_work_as_expected);
  RUN_TEST_CASE(lwpa_rbtree, max_height_is_within_bounds);
}
