/* lwpa_rbtree.c: C-language red-black tree implementation. This module is
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

#include "etcpal/rbtree.h"

#include "etcpal/bool.h"

/* lwpa_rbnode */

static LwpaRbNode* lwpa_rbnode_alloc(LwpaRbTree* tree)
{
  if (tree && tree->alloc_f)
    return tree->alloc_f();
  return NULL;
}

/*! \brief Initialize a red-black tree node.
 *
 *  This function must be called on a new node before inserting it manually using
 *  lwpa_rbtree_insert_node(). When using lwpa_rbtree_insert(), this function is called on the new
 *  node automatically.
 *
 *  \param[in] self The node to be initialized.
 *  \param[in] value Pointer to the value to assign to the node.
 *  \return Pointer to the node that was initialized.
 */
LwpaRbNode* lwpa_rbnode_init(LwpaRbNode* self, void* value)
{
  if (self)
  {
    self->red = 1;
    self->link[0] = self->link[1] = NULL;
    self->value = value;
  }
  return self;
}

static LwpaRbNode* rb_node_create(LwpaRbTree* tree, void* value)
{
  return lwpa_rbnode_init(lwpa_rbnode_alloc(tree), value);
}

static void rb_node_dealloc(LwpaRbNode* self, const LwpaRbTree* tree)
{
  if (self && tree && tree->dealloc_f)
  {
    tree->dealloc_f(self);
  }
}

static int rb_node_is_red(const LwpaRbNode* self)
{
  return self ? self->red : 0;
}

static LwpaRbNode* rb_node_rotate(LwpaRbNode* self, int dir)
{
  LwpaRbNode* result = NULL;
  if (self)
  {
    result = self->link[!dir];
    self->link[!dir] = result->link[dir];
    result->link[dir] = self;
    self->red = 1;
    result->red = 0;
  }
  return result;
}

static LwpaRbNode* rb_node_rotate2(LwpaRbNode* self, int dir)
{
  LwpaRbNode* result = NULL;
  if (self)
  {
    self->link[!dir] = rb_node_rotate(self->link[!dir], !dir);
    result = rb_node_rotate(self, dir);
  }
  return result;
}

/* lwpa_rbtree - default callbacks */

/*! \brief The default node comparison callback.
 *
 *  This function can be supplied as an argument to any function that takes a
 *  #lwpa_rbtree_node_cmp_f. Simply compares the pointer addresses of the two node values.
 */
int lwpa_rbtree_node_cmp_ptr_cb(const LwpaRbTree* self, const LwpaRbNode* a, const LwpaRbNode* b)
{
  (void)self;
  return (a->value > b->value) - (a->value < b->value);
}

/*! \brief The default node deallocation callback.
 *
 *  This function can be supplied as an argument to any function that takes a #lwpa_rbtree_node_f.
 *  Simply deallocates the node using the tree's dealloc_f.
 */
void lwpa_rbtree_node_dealloc_cb(const LwpaRbTree* self, LwpaRbNode* node)
{
  rb_node_dealloc(node, self);
}

/* lwpa_rbtree */

/*! \brief Initialize a red-black tree node.
 *
 *  This function must be called on a new red-black tree before performing any other operations on
 *  it.
 *
 *  \param[in] self The tree to be initialized.
 *  \param[in] node_cmp_cb A function to use for comparing values in the tree.
 *  \param[in] alloc_f A function to use for allocating new node structures.
 *  \param[in] dealloc_f A function to use for deallocating node structures.
 *  \return Pointer to the tree that was initialized.
 */
LwpaRbTree* lwpa_rbtree_init(LwpaRbTree* self, lwpa_rbtree_node_cmp_f node_cmp_cb, lwpa_rbnode_alloc_f alloc_f,
                             lwpa_rbnode_dealloc_f dealloc_f)
{
  if (self)
  {
    self->root = NULL;
    self->size = 0;
    self->cmp = node_cmp_cb ? node_cmp_cb : lwpa_rbtree_node_cmp_ptr_cb;
    self->alloc_f = alloc_f;
    self->dealloc_f = dealloc_f;
  }
  return self;
}

/*! \brief Find a value in a red-black tree.
 *
 *  Uses the #lwpa_rbtree_node_cmp_f provided in lwpa_rbtree_init() to compare values. Lookup
 *  guaranteed in log(n) time.
 *
 *  \param[in] self Tree in which to find the value.
 *  \param[in] value Value to find.
 *  \return Pointer to the value (value found) or NULL (value not found).
 */
void* lwpa_rbtree_find(LwpaRbTree* self, void* value)
{
  void* result = NULL;
  if (self)
  {
    LwpaRbNode node;
    LwpaRbNode* it = self->root;
    int cmp = 0;

    node.value = value;
    while (it)
    {
      if ((cmp = self->cmp(self, it, &node)) != 0)
      {
        /* If the tree supports duplicates, they should be chained to the right subtree for this to
         * work */
        it = it->link[cmp < 0];
      }
      else
      {
        break;
      }
    }
    result = it ? it->value : NULL;
  }
  return result;
}

/*! \brief Insert a new value into a red-black tree.
 *
 *  If the value did not already exist in the tree, a new node is allocated using the
 *  #lwpa_rbnode_alloc_f provided in lwpa_rbtree_init(). Uses the #lwpa_rbtree_node_cmp_f provided
 *  in lwpa_rbtree_init() to compare values. Insertion guaranteed in log(n) time.
 *
 *  \param[in] self Tree in which to insert the value.
 *  \param[in] value Value to insert.
 *  \return #kLwpaErrOk: the value was inserted.
 *  \return #kLwpaErrExists: the value already existed in the tree.
 *  \return #kLwpaErrNoMem: Couldn't allocate new node.
 *  \return #kLwpaErrInvalid: Invalid argument provided.
 */
lwpa_error_t lwpa_rbtree_insert(LwpaRbTree* self, void* value)
{
  LwpaRbNode* new_node = rb_node_create(self, value);
  if (new_node)
  {
    lwpa_error_t insert_res = lwpa_rbtree_insert_node(self, new_node);
    if (insert_res == kLwpaErrOk)
    {
      return kLwpaErrOk;
    }
    else
    {
      rb_node_dealloc(new_node, self);
      return insert_res;
    }
  }
  else
  {
    return kLwpaErrNoMem;
  }
}

/*! \brief Insert a node containing a new value into a red-black tree.
 *
 *  The node is supplied by the caller and its memory must remain valid as long as it remains in the
 *  tree. Uses the #lwpa_rbtree_node_cmp_f provided in lwpa_rbtree_init() to compare values.
 *  Insertion guaranteed in log(n) time.
 *
 *  \param[in] self Tree in which to insert the value.
 *  \param[in] node Node containing value to insert. Must have been previously initialized using
 *                  lwpa_rbnode_init().
 *  \return #kLwpaErrOk: The value was inserted.
 *  \return #kLwpaErrExists: The value already existed in the tree.
 *  \return #kLwpaErrInvalid: Invalid argument provided.
 */
lwpa_error_t lwpa_rbtree_insert_node(LwpaRbTree* self, LwpaRbNode* node)
{
  lwpa_error_t result = kLwpaErrInvalid;
  if (self && node)
  {
    if (self->root == NULL)
    {
      self->root = node;
      result = kLwpaErrOk;
    }
    else
    {
      LwpaRbNode head = {0}; /* False tree root */
      LwpaRbNode *g, *t;     /* Grandparent & parent */
      LwpaRbNode *p, *q;     /* Iterator & parent */
      int dir = 0, last = 0;

      /* Set up our helpers */
      t = &head;
      g = p = NULL;
      q = t->link[1] = self->root;

      /* Search down the tree for a place to insert */
      while (1)
      {
        bool inserted = false;

        if (q == NULL)
        {
          /* Insert node at the first null link. */
          p->link[dir] = q = node;
          inserted = true;
        }
        else if (rb_node_is_red(q->link[0]) && rb_node_is_red(q->link[1]))
        {
          /* Simple red violation: color flip */
          q->red = 1;
          q->link[0]->red = 0;
          q->link[1]->red = 0;
        }

        if (rb_node_is_red(q) && rb_node_is_red(p))
        {
          /* Hard red violation: rotations necessary */
          int dir2 = t->link[1] == g;
          if (q == p->link[last])
            t->link[dir2] = rb_node_rotate(g, !last);
          else
            t->link[dir2] = rb_node_rotate2(g, !last);
        }

        /* Stop working if we inserted a node. This check also disallows duplicates in the tree */
        if (self->cmp(self, q, node) == 0)
        {
          result = inserted ? kLwpaErrOk : kLwpaErrExists;
          break;
        }

        last = dir;
        dir = self->cmp(self, q, node) < 0;

        /* Move the helpers down */
        if (g != NULL)
          t = g;

        g = p, p = q;
        q = q->link[dir];
      }

      /* Update the root (it may be different) */
      self->root = head.link[1];
    }

    /* Make the root black for simplified logic */
    self->root->red = 0;
    ++self->size;
  }

  return result;
}

/*! \brief Remove a value from a red-black tree.
 *
 *  The node memory is deallocated using the #lwpa_rbnode_dealloc_f provided in lwpa_rbtree_init();
 *  the user is responsible for deallocating the value memory. Uses the #lwpa_rbtree_node_cmp_f
 *  provided in lwpa_rbtree_init() to compare values. Removal guaranteed in log(n) time.
 *
 *  \param[in] self Tree from which to remove the value.
 *  \param[in] value Value to remove.
 *  \return #kLwpaErrOk: The value was removed.
 *  \return #kLwpaErrInvalid: Invalid argument provided.
 *  \return #kLwpaErrNotFound: The value did not exist in the tree.
 */
lwpa_error_t lwpa_rbtree_remove(LwpaRbTree* self, void* value)
{
  lwpa_error_t result = kLwpaErrInvalid;
  if (self)
    result = lwpa_rbtree_remove_with_cb(self, value, lwpa_rbtree_node_dealloc_cb);
  return result;
}

/*! \brief Remove a value from a red-black tree, calling back into the application with the node and
 *         value being removed.
 *
 *  The user provides a #lwpa_rbtree_node_f callback function and is responsible for deallocating
 *  both the node and value memory. Uses the #lwpa_rbtree_node_cmp_f provided in lwpa_rbtree_init()
 *  to compare values. Removal guaranteed in log(n) time.
 *
 *  \param[in] self Tree from which to remove the value.
 *  \param[in] value Value to remove.
 *  \param[in] node_cb Callback function to call with the node and value being removed.
 *  \return #kLwpaErrOk: The value was removed.
 *  \return #kLwpaErrInvalid: Invalid argument provided.
 *  \return #kLwpaErrNotFound: The value did not exist in the tree.
 */
lwpa_error_t lwpa_rbtree_remove_with_cb(LwpaRbTree* self, void* value, lwpa_rbtree_node_f node_cb)
{
  LwpaRbNode head = {0}; /* False tree root */
  LwpaRbNode node;       /* Value wrapper node */
  LwpaRbNode *q, *p, *g; /* Helpers */
  LwpaRbNode* f = NULL;  /* Found item */
  int dir = 1;

  if (!self)
    return kLwpaErrInvalid;
  if (self->root == NULL)
    return kLwpaErrNotFound;

  /* SMK added this check, because the removal code seems to fail badly in the case where the node
   * being removed didn't previously exist in the tree. */
  if (NULL == lwpa_rbtree_find(self, value))
    return kLwpaErrNotFound;

  /* Set up our helpers */
  node.value = value;
  q = &head;
  g = p = NULL;
  q->link[1] = self->root;

  /* Search and push a red node down to fix red violations as we go */
  while (q->link[dir] != NULL)
  {
    int last = dir;

    /* Move the helpers down */
    g = p, p = q;
    q = q->link[dir];
    dir = self->cmp(self, q, &node) < 0;

    /* Save the node with matching value and keep going; we'll do removal tasks at the end */
    if (self->cmp(self, q, &node) == 0)
      f = q;

    /* Push the red node down with rotations and color flips */
    if (!rb_node_is_red(q) && !rb_node_is_red(q->link[dir]))
    {
      if (rb_node_is_red(q->link[!dir]))
      {
        p = p->link[last] = rb_node_rotate(q, dir);
      }
      else if (!rb_node_is_red(q->link[!dir]))
      {
        LwpaRbNode* s = p->link[!last];
        if (s)
        {
          if (!rb_node_is_red(s->link[!last]) && !rb_node_is_red(s->link[last]))
          {
            /* Color flip */
            p->red = 0;
            s->red = 1;
            q->red = 1;
          }
          else
          {
            int dir2 = g->link[1] == p;
            if (rb_node_is_red(s->link[last]))
              g->link[dir2] = rb_node_rotate2(p, last);
            else if (rb_node_is_red(s->link[!last]))
              g->link[dir2] = rb_node_rotate(p, last);

            /* Ensure correct coloring */
            q->red = g->link[dir2]->red = 1;
            g->link[dir2]->link[0]->red = 0;
            g->link[dir2]->link[1]->red = 0;
          }
        }
      }
    }
  }

  /* Replace and remove the saved node */
  if (f)
  {
    void* tmp = f->value;
    f->value = q->value;
    q->value = tmp;

    p->link[p->link[1] == q] = q->link[q->link[0] == NULL];

    if (node_cb)
      node_cb(self, q);
    q = NULL;
  }

  /* Update the root (it may be different) */
  self->root = head.link[1];

  /* Make the root black for simplified logic */
  if (self->root != NULL)
    self->root->red = 0;

  --self->size;
  return kLwpaErrOk;
}

/*! \brief Clear all values from a red-black tree.
 *
 *  The node memory is deallocated using the #lwpa_rbnode_dealloc_f provided in lwpa_rbtree_init();
 *  the user is responsible for deallocating the value memory.
 *
 *  \param[in] self Tree to clear.
 *  \return #kLwpaErrOk: The tree was cleared.
 *  \return #kLwpaErrInvalid: Invalid argument provided.
 */
lwpa_error_t lwpa_rbtree_clear(LwpaRbTree* self)
{
  lwpa_error_t result = kLwpaErrInvalid;
  if (self)
    result = lwpa_rbtree_clear_with_cb(self, lwpa_rbtree_node_dealloc_cb);
  return result;
}

/*! \brief Clear all values from a red-black tree, calling back into the application for each node
 *         and value being removed.
 *
 *  The user provides a #lwpa_rbtree_node_f callback function which is called for each node in the
 *  tree. The user is responsible for deallocating both the node and value memory.
 *
 *  \param[in] self Tree to clear.
 *  \param[in] node_cb Callback function to call with each node and value being removed.
 *  \return #kLwpaErrOk: The tree was cleared.
 *  \return #kLwpaErrInvalid: Invalid argument provided.
 */
lwpa_error_t lwpa_rbtree_clear_with_cb(LwpaRbTree* self, lwpa_rbtree_node_f node_cb)
{
  lwpa_error_t result = kLwpaErrInvalid;
  if (self && node_cb)
  {
    LwpaRbNode* node = self->root;
    LwpaRbNode* save = NULL;

    /* Rotate away the left links so that we can treat this like the destruction of a linked list */
    while (node)
    {
      if (node->link[0] == NULL)
      {
        /* No left links, just kill the node and move on */
        save = node->link[1];
        node_cb(self, node);
        node = NULL;
      }
      else
      {
        /* Rotate away the left link and check again */
        save = node->link[0];
        node->link[0] = save->link[1];
        save->link[1] = node;
      }
      node = save;
    }
    self->root = NULL;
    self->size = 0;
    result = kLwpaErrOk;
  }
  return result;
}

/*! \brief Get the current number of values in a red-black tree.
 *  \param[in] self The tree of which to get the size.
 *  \return The number of values currently in the tree.
 */
size_t lwpa_rbtree_size(LwpaRbTree* self)
{
  size_t result = 0;
  if (self)
    result = self->size;
  return result;
}

/*! \brief Test the validity of a red-black tree.
 *
 *  A debugging function; tests that both basic binary tree rules and specific red-black rules are
 *  not violated.
 *
 *  \param[in] self Tree to test.
 *  \param[in] root Node at which to start the test. All nodes beneath this node will be tested.
 *  \return The "black height" of the tree; the number of black nodes present in the traversal
 *          from the root to each leaf. A valid red-black tree requires this number to be the same
 *          for every possible traversal.
 *  \return 0: Invalid tree.
 */
int lwpa_rbtree_test(LwpaRbTree* self, LwpaRbNode* root)
{
  int lh, rh;

  if (root == NULL)
  {
    return 1;
  }
  else
  {
    LwpaRbNode* ln = root->link[0];
    LwpaRbNode* rn = root->link[1];

    /* Consecutive red links */
    if (rb_node_is_red(root))
    {
      if (rb_node_is_red(ln) || rb_node_is_red(rn))
      {
        return 0;
      }
    }

    lh = lwpa_rbtree_test(self, ln);
    rh = lwpa_rbtree_test(self, rn);

    /* Invalid binary search tree */
    if ((ln != NULL && self->cmp(self, ln, root) >= 0) || (rn != NULL && self->cmp(self, rn, root) <= 0))
    {
      return 0;
    }

    /* Black height mismatch */
    if (lh != 0 && rh != 0 && lh != rh)
    {
      return 0;
    }

    /* Only count black links */
    if (lh != 0 && rh != 0)
      return rb_node_is_red(root) ? lh : lh + 1;
    else
      return 0;
  }
}

/* lwpa_rbiter */

/*! \brief Initialize a red-black tree iterator.
 *
 *  This function must be called on a new iterator before using any of the other lwpa_rbiter_*
 *  functions on it.
 *
 *  \param[in] self The iterator to be initialized.
 *  \return Pointer to the iterator that was initialized.
 */
LwpaRbIter* lwpa_rbiter_init(LwpaRbIter* self)
{
  if (self)
  {
    self->tree = NULL;
    self->node = NULL;
    self->top = 0;
  }
  return self;
}

/* Internal function, init traversal object, dir determines whether to begin traversal at the
 * smallest or largest valued node. */
static void* rb_iter_start(LwpaRbIter* self, LwpaRbTree* tree, int dir)
{
  void* result = NULL;
  if (self)
  {
    self->tree = tree;
    self->node = tree->root;
    self->top = 0;

    /* Save the path for later selfersal */
    if (self->node != NULL)
    {
      while (self->node->link[dir] != NULL)
      {
        self->path[self->top++] = self->node;
        self->node = self->node->link[dir];
      }
    }

    result = self->node == NULL ? NULL : self->node->value;
  }
  return result;
}

/* Traverse a red black tree in the user-specified direction (0 asc, 1 desc) */
static void* rb_iter_move(LwpaRbIter* self, int dir)
{
  if (self->node->link[dir] != NULL)
  {
    /* Continue down this branch */
    self->path[self->top++] = self->node;
    self->node = self->node->link[dir];
    while (self->node->link[!dir] != NULL)
    {
      self->path[self->top++] = self->node;
      self->node = self->node->link[!dir];
    }
  }
  else
  {
    /* Move to the next branch */
    LwpaRbNode* last = NULL;
    do
    {
      if (self->top == 0)
      {
        self->node = NULL;
        break;
      }
      last = self->node;
      self->node = self->path[--self->top];
    } while (last == self->node->link[dir]);
  }
  return self->node == NULL ? NULL : self->node->value;
}

/*! \brief Point a red-black tree iterator at the first value in the tree.
 *
 *  The first value is the lowest, as determined by the #lwpa_rbtree_node_cmp_f provided in
 *  lwpa_rbtree_init(). Use lwpa_rbiter_next() to get the next higher value.
 *
 *  \param[in] self Iterator to point at the first value.
 *  \param[in] tree Tree of which to get the first value.
 *  \return Pointer to the first value or NULL (the tree was empty or invalid).
 */
void* lwpa_rbiter_first(LwpaRbIter* self, LwpaRbTree* tree)
{
  return rb_iter_start(self, tree, 0);
}

/*! \brief Point a red-black tree iterator at the last value in the tree.
 *
 *  The last value is the highest, as determined by the #lwpa_rbtree_node_cmp_f provided in
 *  lwpa_rbtree_init(). Use lwpa_rbiter_prev() to get the next lower value.
 *
 *  \param[in] self Iterator to point at the last value.
 *  \param[in] tree Tree of which to get the last value.
 *  \return Pointer to the last value or NULL (the tree was empty or invalid).
 */
void* lwpa_rbiter_last(LwpaRbIter* self, LwpaRbTree* tree)
{
  return rb_iter_start(self, tree, 1);
}

/*! \brief Advance a red-black tree iterator.
 *
 *  Gets the next higher value in the tree as determined by the #lwpa_rbtree_node_cmp_f provided in
 *  lwpa_rbtree_init().
 *
 *  \param[in] self Iterator to advance.
 *  \return Pointer to next higher value, or NULL (the end of the tree has been reached).
 */
void* lwpa_rbiter_next(LwpaRbIter* self)
{
  return rb_iter_move(self, 1);
}

/*! \brief Reverse-advance a red-black tree iterator.
 *
 *  Gets the next lower value in the tree as determined by the #lwpa_rbtree_node_cmp_f provided in
 *  lwpa_rbtree_init().
 *
 *  \param[in] self Iterator to reverse-advance.
 *  \return Pointer to next lower value, or NULL (the beginning of the tree has been reached).
 */
void* lwpa_rbiter_prev(LwpaRbIter* self)
{
  return rb_iter_move(self, 0);
}
