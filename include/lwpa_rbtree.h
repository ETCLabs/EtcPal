/* lwpa_rbtree.h: C-language red-black tree implementation. This module is
 * based heavily on a public-domain red-black tree implementation. The license
 * of that implementation follows as it was when retrieved from Github on
 * 2018-02-28.
 *
 ******************************************************************************
 * Based on Julienne Walker's <http://eternallyconfuzzled.com/> rb_tree
 * implementation.
 *
 * Modified by Mirek Rusin <http://github.com/mirek/rb_tree>.
 *
 * This is free and unencumbered software released into the public domain.
 *
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 *
 * In jurisdictions that recognize copyright laws, the author or authors
 * of this software dedicate any and all copyright interest in the
 * software to the public domain. We make this dedication for the benefit
 * of the public at large and to the detriment of our heirs and
 * successors. We intend this dedication to be an overt act of
 * relinquishment in perpetuity of all present and future rights to this
 * software under copyright law.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * For more information, please refer to <http://unlicense.org/>
 */
#ifndef _LWPA_RBTREE_H_
#define _LWPA_RBTREE_H_

#include <stddef.h>

/*! \defgroup lwpa_rbtree lwpa_rbtree
 *  \ingroup lwpa
 *  \brief A red-black tree implementation.
 *
 *  \#include "lwpa_rbtree.h"
 *
 *  A red-black tree is a popular design for a self-balancing binary search
 *  tree. Based on a public-domain red-black tree implementation; see the
 *  header file for details.
 *
 *  @{
 */

/*! The tallest allowable tree that can be iterated over. */
#ifndef RB_ITER_MAX_HEIGHT
#define RB_ITER_MAX_HEIGHT 64
#endif

typedef struct LwpaRbNode LwpaRbNode;
typedef struct LwpaRbTree LwpaRbTree;

/*! \name Callback Functions
 *  @{
 */

/*! \brief A function type that compares two nodes in a tree.
 *
 *  A default, rb_tree_node_cmp_ptr_cb(), is provided which simply compares the
 *  pointer (address) of the value member of each node.
 *
 *  \param[in] self The tree in which two nodes are being compared.
 *  \param[in] node_a The first node being compared.
 *  \param[in] node_b The second node being compared.
 *  \return < 0 (node_a's value is less than node_b's value)\n
 *            0 (node_a's value is equal to node_b's value)\n
 *          > 0 (node_a's value is greater than node_b's value)
 */
typedef int (*rb_tree_node_cmp_f)(const LwpaRbTree *self, const LwpaRbNode *node_a, const LwpaRbNode *node_b);

/*! \brief A function type to be called for each node in a tree.
 *
 *  Usually provided on a tree-wide clear or destroy operation; in this case,
 *  it should provide any deallocation necessary for the node structure and its
 *  value. A default, rb_tree_node_dealloc_cb(), is provided which simply calls
 *  the tree's #rb_node_dealloc_f on the node.
 *
 *  \param[in] self The tree in which the node resides.
 *  \param[in] node The node for which an action should be performed.
 */
typedef void (*rb_tree_node_f)(const LwpaRbTree *self, LwpaRbNode *node);

/*! \brief A function type to allocate a new node.
 *
 *  The user provides the allocation method for new nodes, whether this be
 *  malloc() or some more static method. A function of this type is saved by
 *  the tree struct and called on a call to rb_tree_insert().
 *
 *  \return Pointer to the newly allocated node.
 */
typedef LwpaRbNode *(*rb_node_alloc_f)();

/*! \brief A function type to deallocate a node.
 *
 *  The user provides the deallocation method for nodes, whether this be free()
 *  or some more static method. A function of this type is saved by the tree
 *  struct and called on calls to rb_tree_remove() and rb_tree_clear().
 *
 *  \param[in] node Pointer to node to deallocate.
 */
typedef void (*rb_node_dealloc_f)(LwpaRbNode *node);

/*!@}*/

/*! \brief A red-black tree node. */
struct LwpaRbNode
{
  int red;             /*!< The node color: red (1), black (0) */
  LwpaRbNode *link[2]; /*!< Child node links: left [0], right [1] */
  void *value;         /*!< The value object represented by this node. */
};

/*! \brief A red-black tree.
 *
 *  Initialize using rb_tree_init() before carrying out any other operation on
 *  the tree.
 */
struct LwpaRbTree
{
  LwpaRbNode *root;       /*!< The root node of the tree. */
  rb_tree_node_cmp_f cmp; /*!< A function to use for comparing two nodes. */
  size_t size;            /*!< The current count of nodes in the tree. */
  /*! A function to use for allocating a new node.*/
  rb_node_alloc_f alloc_f;
  /*! A function to use for deallocating a node. */
  rb_node_dealloc_f dealloc_f;
  void *info; /*!< User provided, not used by rb_tree. */
};

/*! \brief A red-black tree iterator.
 *
 *  Initialize using rb_iter_init() before carrying out any other operation on
 *  the iterator.
 */
typedef struct LwpaRbIter
{
  LwpaRbTree *tree; /*!< The tree being iterated over. */
  LwpaRbNode *node; /*!< The current node. */
  /*! The traversal path to the current node. */
  LwpaRbNode *path[RB_ITER_MAX_HEIGHT];
  size_t top; /*!< Top of the traversal stack. */
  void *info; /*!< User provided, not used by rb_iter. */
} LwpaRbIter;

#ifdef __cplusplus
extern "C" {
#endif

int rb_tree_node_cmp_ptr_cb(const LwpaRbTree *self, const LwpaRbNode *a, const LwpaRbNode *b);
void rb_tree_node_dealloc_cb(const LwpaRbTree *self, LwpaRbNode *node);

LwpaRbNode *rb_node_init(LwpaRbNode *self, void *value);

LwpaRbTree *rb_tree_init(LwpaRbTree *self, rb_tree_node_cmp_f cmp, rb_node_alloc_f alloc_f,
                         rb_node_dealloc_f dealloc_f);
void *rb_tree_find(LwpaRbTree *self, void *value);
int rb_tree_insert(LwpaRbTree *self, void *value);
int rb_tree_remove(LwpaRbTree *self, void *value);
int rb_tree_clear(LwpaRbTree *self);
size_t rb_tree_size(LwpaRbTree *self);

int rb_tree_insert_node(LwpaRbTree *self, LwpaRbNode *node);
int rb_tree_remove_with_cb(LwpaRbTree *self, void *value, rb_tree_node_f node_cb);
int rb_tree_clear_with_cb(LwpaRbTree *self, rb_tree_node_f node_cb);

int rb_tree_test(LwpaRbTree *self, LwpaRbNode *root);

LwpaRbIter *rb_iter_init(LwpaRbIter *self);
void *rb_iter_first(LwpaRbIter *self, LwpaRbTree *tree);
void *rb_iter_last(LwpaRbIter *self, LwpaRbTree *tree);
void *rb_iter_next(LwpaRbIter *self);
void *rb_iter_prev(LwpaRbIter *self);

#ifdef __cplusplus
}
#endif

/*!@}*/

#endif /* _LWPA_RBTREE_H_ */
