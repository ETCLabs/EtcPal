/* lwpa/rbtree.h: C-language red-black tree implementation. This module is
 * based heavily on a public-domain red-black tree implementation. The license
 * of that implementation follows as it was when retrieved from Github on
 * 2018-02-28.
 *
 ******************************************************************************
 * Based on Julienne Walker's <http://eternallyconfuzzled.com/> lwpa_rbtree
 * implementation.
 *
 * Modified by Mirek Rusin <http://github.com/mirek/lwpa_rbtree>.
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
 *  \#include "lwpa/rbtree.h"
 *
 *  A red-black tree is a popular design for a self-balancing binary search tree. Based on a
 *  public-domain red-black tree implementation; see the header file for details.
 *
 *  @{
 */

/*! The tallest allowable tree that can be iterated over. */
#ifndef LWPA_RB_ITER_MAX_HEIGHT
#define LWPA_RB_ITER_MAX_HEIGHT 64
#endif

typedef struct LwpaRbNode LwpaRbNode;
typedef struct LwpaRbTree LwpaRbTree;

/*! \name Callback Functions
 *  @{
 */

/*! \brief A function type that compares two nodes in a tree.
 *
 *  A default, lwpa_rbtree_node_cmp_ptr_cb(), is provided which simply compares the pointer (address) of
 *  the value member of each node.
 *
 *  \param[in] self The tree in which two nodes are being compared.
 *  \param[in] node_a The first node being compared.
 *  \param[in] node_b The second node being compared.
 *  \return < 0 (node_a's value is less than node_b's value)\n
 *            0 (node_a's value is equal to node_b's value)\n
 *          > 0 (node_a's value is greater than node_b's value)
 */
typedef int (*lwpa_rbtree_node_cmp_f)(const LwpaRbTree* self, const LwpaRbNode* node_a, const LwpaRbNode* node_b);

/*! \brief A function type to be called for each node in a tree.
 *
 *  Usually provided on a tree-wide clear or destroy operation; in this case, it should provide any
 *  deallocation necessary for the node structure and its value. A default,
 *  lwpa_rbtree_node_dealloc_cb(), is provided which simply calls the tree's #lwpa_rbnode_dealloc_f on the
 *  node.
 *
 *  \param[in] self The tree in which the node resides.
 *  \param[in] node The node for which an action should be performed.
 */
typedef void (*lwpa_rbtree_node_f)(const LwpaRbTree* self, LwpaRbNode* node);

/*! \brief A function type to allocate a new node.
 *
 *  The user provides the allocation method for new nodes, whether this be malloc() or some more
 *  static method. A function of this type is saved by the tree struct and called on a call to
 *  lwpa_rbtree_insert().
 *
 *  \return Pointer to the newly allocated node.
 */
typedef LwpaRbNode* (*lwpa_rbnode_alloc_f)();

/*! \brief A function type to deallocate a node.
 *
 *  The user provides the deallocation method for nodes, whether this be free() or some more static
 *  method. A function of this type is saved by the tree struct and called on calls to
 *  lwpa_rbtree_remove() and lwpa_rbtree_clear().
 *
 *  \param[in] node Pointer to node to deallocate.
 */
typedef void (*lwpa_rbnode_dealloc_f)(LwpaRbNode* node);

/*!@}*/

/*! \brief A red-black tree node. */
struct LwpaRbNode
{
  int red;             /*!< The node color: red (1), black (0) */
  LwpaRbNode* link[2]; /*!< Child node links: left [0], right [1] */
  void* value;         /*!< The value object represented by this node. */
};

/*! \brief A red-black tree.
 *
 *  Initialize using lwpa_rbtree_init() before carrying out any other operation on the tree.
 */
struct LwpaRbTree
{
  LwpaRbNode* root;                /*!< The root node of the tree. */
  lwpa_rbtree_node_cmp_f cmp;      /*!< A function to use for comparing two nodes. */
  size_t size;                     /*!< The current count of nodes in the tree. */
  lwpa_rbnode_alloc_f alloc_f;     /*!< A function to use for allocating a new node.*/
  lwpa_rbnode_dealloc_f dealloc_f; /*!< A function to use for deallocating a node. */
  void* info;                      /*!< User provided, not used by lwpa_rbtree. */
};

/*! \brief A red-black tree iterator.
 *
 *  Initialize using lwpa_rbiter_init() before carrying out any other operation on the iterator.
 */
typedef struct LwpaRbIter
{
  LwpaRbTree* tree;                          /*!< The tree being iterated over. */
  LwpaRbNode* node;                          /*!< The current node. */
  LwpaRbNode* path[LWPA_RB_ITER_MAX_HEIGHT]; /*!< The traversal path to the current node. */
  size_t top;                                /*!< Top of the traversal stack. */
  void* info;                                /*!< User provided, not used by lwpa_rbiter. */
} LwpaRbIter;

#ifdef __cplusplus
extern "C" {
#endif

int lwpa_rbtree_node_cmp_ptr_cb(const LwpaRbTree* self, const LwpaRbNode* a, const LwpaRbNode* b);
void lwpa_rbtree_node_dealloc_cb(const LwpaRbTree* self, LwpaRbNode* node);

LwpaRbNode* lwpa_rbnode_init(LwpaRbNode* self, void* value);

LwpaRbTree* lwpa_rbtree_init(LwpaRbTree* self, lwpa_rbtree_node_cmp_f cmp, lwpa_rbnode_alloc_f alloc_f,
                             lwpa_rbnode_dealloc_f dealloc_f);
void* lwpa_rbtree_find(LwpaRbTree* self, void* value);
int lwpa_rbtree_insert(LwpaRbTree* self, void* value);
int lwpa_rbtree_remove(LwpaRbTree* self, void* value);
int lwpa_rbtree_clear(LwpaRbTree* self);
size_t lwpa_rbtree_size(LwpaRbTree* self);

int lwpa_rbtree_insert_node(LwpaRbTree* self, LwpaRbNode* node);
int lwpa_rbtree_remove_with_cb(LwpaRbTree* self, void* value, lwpa_rbtree_node_f node_cb);
int lwpa_rbtree_clear_with_cb(LwpaRbTree* self, lwpa_rbtree_node_f node_cb);

int lwpa_rbtree_test(LwpaRbTree* self, LwpaRbNode* root);

LwpaRbIter* lwpa_rbiter_init(LwpaRbIter* self);
void* lwpa_rbiter_first(LwpaRbIter* self, LwpaRbTree* tree);
void* lwpa_rbiter_last(LwpaRbIter* self, LwpaRbTree* tree);
void* lwpa_rbiter_next(LwpaRbIter* self);
void* lwpa_rbiter_prev(LwpaRbIter* self);

#ifdef __cplusplus
}
#endif

/*!@}*/

#endif /* _LWPA_RBTREE_H_ */
