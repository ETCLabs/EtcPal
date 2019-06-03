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
#include "lwpatest.h"
#include <stdlib.h>
#include <math.h>
#include "lwpa_rbtree.h"

#define INT_ARRAY_SIZE 100
int incrementing_int_array[INT_ARRAY_SIZE];

static int g_alloc_call_count;
static LwpaRbNode* node_alloc()
{
  ++g_alloc_call_count;
  return (LwpaRbNode*)_mem_alloc(sizeof(LwpaRbNode));
}

static int g_dealloc_call_count;
static void node_dealloc(LwpaRbNode* node)
{
  ++g_dealloc_call_count;
  _mem_free(node);
}

static int int_cmp(const LwpaRbTree* self, const LwpaRbNode* node_a, const LwpaRbNode* node_b)
{
  int a = *(int*)node_a->value;
  int b = *(int*)node_b->value;
  return a - b;
}

static int intval_pool[INT_ARRAY_SIZE];
static LwpaRbNode node_pool[INT_ARRAY_SIZE];

static int g_clearfunc_call_count;
bool remove_one_flag;
#define MAGIC_REMOVE_VALUE (INT_ARRAY_SIZE / 2)
static void clear_func(const LwpaRbTree* self, LwpaRbNode* node)
{
  if (remove_one_flag && *(int*)node->value == MAGIC_REMOVE_VALUE)
    remove_one_flag = false;
  ++g_clearfunc_call_count;
}

bool test_insert_static()
{
  int i;
  LwpaRbTree tree;
  bool ok;

  ok = (NULL != rb_tree_init(&tree, int_cmp, NULL, NULL));
  if (ok)
  {
    /* Copy and shuffle the integer array */
    memcpy(intval_pool, incrementing_int_array, sizeof intval_pool);
    for (i = 0; i < INT_ARRAY_SIZE; ++i)
    {
      /* Swap the number at this index with the number at a random index later in the array */
      int index = i + rand() / (RAND_MAX / (INT_ARRAY_SIZE - i) + 1);
      int t = intval_pool[index];
      intval_pool[index] = intval_pool[i];
      intval_pool[i] = t;
    }
    /* Now, point each node at its respective value and insert it into the tree */
    for (i = 0; i < INT_ARRAY_SIZE; ++i)
    {
      LwpaRbNode* node = &node_pool[i];
      ok = (NULL != rb_node_init(node, &intval_pool[i]));
      if (!ok)
        break;
      ok = (0 != rb_tree_insert_node(&tree, node));
      if (!ok)
        break;
    }
  }
  if (ok)
    ok = (rb_tree_size(&tree) == INT_ARRAY_SIZE);
  if (ok)
    ok = (0 != rb_tree_test(&tree, tree.root));
  if (ok)
  {
    int to_find = rand() / (RAND_MAX / INT_ARRAY_SIZE + 1);
    int* found = (int*)rb_tree_find(&tree, &to_find);
    ok = (found != NULL && *found == to_find);
  }
  if (ok)
  {
    g_clearfunc_call_count = 0;
    remove_one_flag = true;
    int to_remove = MAGIC_REMOVE_VALUE;
    ok = (0 != rb_tree_remove_with_cb(&tree, &to_remove, clear_func)) && !remove_one_flag;
  }
  if (ok)
    ok = (0 != rb_tree_clear_with_cb(&tree, clear_func));
  if (ok)
    ok = (rb_tree_size(&tree) == 0);
  if (ok)
    ok = (g_clearfunc_call_count == INT_ARRAY_SIZE);
  return ok;
}

bool test_insert_dynamic()
{
  int i;
  LwpaRbTree tree;
  bool ok;

  g_alloc_call_count = g_dealloc_call_count = 0;
  ok = (NULL != rb_tree_init(&tree, int_cmp, node_alloc, node_dealloc));
  if (ok)
  {
    /* Copy and shuffle the integer array */
    memcpy(intval_pool, incrementing_int_array, sizeof intval_pool);
    for (i = 0; i < INT_ARRAY_SIZE; ++i)
    {
      /* Swap the number at this index with the number at a random index later in the array */
      int index = i + rand() / (RAND_MAX / (INT_ARRAY_SIZE - i) + 1);
      int t = intval_pool[index];
      intval_pool[index] = intval_pool[i];
      intval_pool[i] = t;
    }
    /* Now, insert each value into the tree; dynamic alloc functions should be called. */
    for (i = 0; i < INT_ARRAY_SIZE; ++i)
    {
      ok = (0 != rb_tree_insert(&tree, &intval_pool[i]));
      if (!ok)
        break;
    }
  }
  if (ok)
    ok = (rb_tree_size(&tree) == INT_ARRAY_SIZE);
  if (ok)
    ok = (g_alloc_call_count == INT_ARRAY_SIZE);
  if (ok)
    ok = (0 != rb_tree_test(&tree, tree.root));
  if (ok)
  {
    int to_find = rand() / (RAND_MAX / INT_ARRAY_SIZE + 1);
    int* found = (int*)rb_tree_find(&tree, &to_find);
    ok = (found != NULL && *found == to_find);
  }
  if (ok)
  {
    int not_in_tree = INT_ARRAY_SIZE + 1;
    ok = (0 == rb_tree_remove(&tree, &not_in_tree));
  }
  if (ok)
    ok = (0 != rb_tree_clear(&tree));
  if (ok)
    ok = (rb_tree_size(&tree) == 0);
  if (ok)
    ok = (g_dealloc_call_count == INT_ARRAY_SIZE);
  return ok;
}

bool test_iter()
{
  int i;
  LwpaRbTree tree;
  LwpaRbIter iter;
  int* val = NULL;
  bool ok;

  ok = (NULL != rb_tree_init(&tree, int_cmp, node_alloc, node_dealloc));
  if (ok)
  {
    /* Copy and shuffle the integer array */
    memcpy(intval_pool, incrementing_int_array, sizeof intval_pool);
    for (i = 0; i < INT_ARRAY_SIZE; ++i)
    {
      /* Swap the number at this index with the number at a random index later in the array */
      int index = i + rand() / (RAND_MAX / (INT_ARRAY_SIZE - i) + 1);
      int t = intval_pool[index];
      intval_pool[index] = intval_pool[i];
      intval_pool[i] = t;
    }
    /* Now, insert each value into the tree; dynamic alloc functions should be called. */
    for (i = 0; i < INT_ARRAY_SIZE; ++i)
    {
      ok = (0 != rb_tree_insert(&tree, &intval_pool[i]));
      if (!ok)
        break;
    }
  }
  if (ok)
    ok = (rb_tree_size(&tree) == INT_ARRAY_SIZE);
  if (ok)
    ok = (NULL != rb_iter_init(&iter));
  if (ok)
  {
    val = (int*)rb_iter_first(&iter, &tree);
    /* Although the elements were inserted in random order, the tree should be sorted so 0 should be
     * the first value. */
    ok = (*val == 0);
  }
  if (ok)
  {
    size_t num_iterations = 1;
    int last_val = *val;
    while ((val = (int*)rb_iter_next(&iter)))
    {
      ++num_iterations;
      /* Each value given by the iterator should be numerically higher than the one that came before
       * it. */
      if (*val < last_val)
      {
        ok = false;
        break;
      }
      last_val = *val;
    }
    if (ok)
      ok = (num_iterations == INT_ARRAY_SIZE);
  }
  if (ok)
  {
    val = (int*)rb_iter_last(&iter, &tree);
    /* Although the elements were inserted in random order, the tree should be sorted so
     * INT_ARRAY_SIZE - 1 should be the last value. */
    ok = (*val == (INT_ARRAY_SIZE - 1));
  }
  if (ok)
  {
    size_t num_iterations = 1;
    int last_val = *val;
    while ((val = (int*)rb_iter_prev(&iter)))
    {
      ++num_iterations;
      /* Each value given by the reverse iterator should be numerically smaller than the one that
       * came before it. */
      if (*val > last_val)
      {
        ok = false;
        break;
      }
      last_val = *val;
    }
    if (ok)
      ok = (num_iterations == INT_ARRAY_SIZE);
  }
  if (ok)
  {
    g_dealloc_call_count = 0;
    ok = (0 != rb_tree_clear(&tree));
  }
  if (ok)
    ok = (rb_tree_size(&tree) == 0);
  if (ok)
    ok = (g_dealloc_call_count == INT_ARRAY_SIZE);
  return ok;
}

bool test_max_height()
{
  int i;
  LwpaRbTree tree;
  bool ok;

  ok = (NULL != rb_tree_init(&tree, int_cmp, node_alloc, node_dealloc));
  if (ok)
  {
    /* Insert monotonically incrementing values into the tree. In a traditional binary tree, this
     * would result in a worst-case unbalanced tree of height INT_ARRAY_SIZE. In the red-black tree,
     * the maximum height should be determined by the formula 2 * log2(INT_ARRAY_SIZE + 1). */
    for (i = 0; i < INT_ARRAY_SIZE; ++i)
    {
      ok = (0 != rb_tree_insert(&tree, &incrementing_int_array[i]));
      if (!ok)
        break;
    }
  }
  if (ok)
    ok = (rb_tree_size(&tree) == INT_ARRAY_SIZE);
  if (ok)
  {
    LwpaRbIter iter;
    size_t max_height = 0;
    size_t theoretical_max_height;
    ok = (NULL != rb_iter_init(&iter));
    if (ok)
      ok = (NULL != rb_iter_first(&iter, &tree));
    if (ok)
    {
      do
      {
        if (iter.top > max_height)
          max_height = iter.top;
      } while (NULL != rb_iter_next(&iter));
      max_height += 1;
      /* http://www.doctrina.org/maximum-height-of-red-black-tree.html */
      theoretical_max_height = 2 * (size_t)(log(INT_ARRAY_SIZE + 1) / log(2));
      ok = (max_height <= theoretical_max_height);
    }
  }
  if (ok)
  {
    g_dealloc_call_count = 0;
    ok = (0 != rb_tree_clear(&tree));
  }
  if (ok)
    ok = (rb_tree_size(&tree) == 0);
  if (ok)
    ok = (g_dealloc_call_count == INT_ARRAY_SIZE);
  return ok;
}

void test_rbtree()
{
  bool ok;
  int i;

  OUTPUT_TEST_MODULE_BEGIN("rb_tree");
  watchdog_kick();

  /* Initialize the int array */
  for (i = 0; i < INT_ARRAY_SIZE; ++i)
    incrementing_int_array[i] = i;

  OUTPUT_TEST_BEGIN("rb_tree_insert_static");
  OUTPUT_TEST_RESULT((ok = test_insert_static()));
  if (ok)
  {
    watchdog_kick();
    OUTPUT_TEST_BEGIN("rb_tree_insert_dynamic");
    OUTPUT_TEST_RESULT((ok = test_insert_dynamic()));
  }
  if (ok)
  {
    watchdog_kick();
    OUTPUT_TEST_BEGIN("rb_tree_iterate");
    OUTPUT_TEST_RESULT((ok = test_iter()));
  }
  if (ok)
  {
    watchdog_kick();
    OUTPUT_TEST_BEGIN("rb_tree_max_height");
    OUTPUT_TEST_RESULT((ok = test_max_height()));
  }
  OUTPUT_TEST_MODULE_END("rb_tree", ok);
}
