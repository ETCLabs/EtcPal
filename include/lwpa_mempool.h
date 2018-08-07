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

/* lwpa_mempool.h: Fixed-size memory pools. */
#ifndef _LWPA_MEMPOOL_H_
#define _LWPA_MEMPOOL_H_

#include <stddef.h>
#include "lwpa_error.h"

/*! \defgroup lwpa_mempool lwpa_mempool
 *  \ingroup lwpa
 *  \brief Memory pools with fixed-size elements.
 *
 *  \#include "lwpa_mempool.h"
 *
 *  This module can be used to declare memory pools containing some number of
 *  objects of an arbitrary type. Only objects of that type can be allocated
 *  from the pool or freed back into it. Allocations and deallocations are
 *  ensured to be thread_safe by an \ref lwpa_mutex internally.
 *
 *  @{
 */

/*! (Not for direct usage) A tiny structure simply used to maintain the
 *  freelist for each pool. */
typedef struct LwpaMempool LwpaMempool;
struct LwpaMempool
{
  LwpaMempool *next;
};

/*! (Not for direct usage) A memory pool description structure. Do not declare
 *  or use this structure directly; instead, use LWPA_MEMPOOL_DECLARE(),
 *  LWPA_MEMPOOL_DEFINE() and the lwpa_mempool_* macros to interact with it. */
typedef struct LwpaMempoolDesc
{
  const size_t elem_size;  /*!< The size of each element. */
  const size_t pool_size;  /*!< The number of elements in the pool. */
  LwpaMempool *freelist;   /*!< The current freelist. */
  LwpaMempool *const list; /*!< The array of mempool list structs. */
  /*! The number of pool elements that have currently been allocated. */
  size_t current_used;
  void *const pool; /*!< The actual pool memory. */
} LwpaMempoolDesc;

/*! \brief Declare a pool as an external variable.
 *
 *  This optional macro is useful for header files; when used, it must be
 *  paired with a call of LWPA_MEMPOOL_DEFINE() using the same name.
 *
 *  \param name The name of the memory pool.
 */
#define LWPA_MEMPOOL_DECLARE(name) extern LwpaMempoolDesc name##_pool_desc;

/*! \brief Define a new memory pool.
 *
 *  Expands to a number of variable definitions. Should not be used in a header
 *  file; if you want your memory pool to be accessible from multiple source
 *  files, use LWPA_MEMPOOL_DECLARE() in the header file in addition to this
 *  macro.
 *
 *  \param name The name of the memory pool.
 *  \param type The type of each element in the memory pool
 *              (i.e. int, struct foo)
 *  \param size The number of elements in the memory pool.
 */
#define LWPA_MEMPOOL_DEFINE(name, type, size)                                     \
  type name##_pool[size];                                                         \
  struct LwpaMempool name##_pool_list[size];                                      \
  struct LwpaMempoolDesc name##_pool_desc = {sizeof(type),     /* elem_size */    \
                                             size,             /* pool_size */    \
                                             NULL,             /* freelist */     \
                                             name##_pool_list, /* list */         \
                                             0,                /* current_used */ \
                                             name##_pool /* pool */}

/*! \brief Initialize a memory pool.
 *
 *  This macro must be called on a pool before using lwpa_mempool_alloc() or
 *  lwpa_mempool_free() on it. There is no deinitialization function because
 *  there is no cleanup necessary; to reset a pool, simply call this function
 *  again.
 *
 *  \param name The name of the memory pool to initialize.
 *  \return #LWPA_OK: The memory pool was initialized successfully.\n
 *          #LWPA_SYSERR: An internal system call error occurred.
 */
#define lwpa_mempool_init(name) lwpa_mempool_init_priv(&name##_pool_desc)

/*! \brief Allocate a new element from a memory pool.
 *  \param name The name of the memory pool from which to allocate a new
 *              element.
 *  \return Pointer to the newly-allocated element (success) or NULL (pool is
 *          out of memory).
 */
#define lwpa_mempool_alloc(name) lwpa_mempool_alloc_priv(&name##_pool_desc)

/*! \brief Free an element back to a memory pool.
 *
 *  CAUTION: Freeing a pointer to an element that has not previously been
 *  allocated or a pointer to somewhere outside of the memory pool will cause
 *  undefined behavior.
 *
 *  \param name The name of the memory pool to which to free an element.
 *  \param mem Pointer to the element to free.
 */
#define lwpa_mempool_free(name, mem) lwpa_mempool_free_priv(&name##_pool_desc, mem)

/*! \brief Get the total size of a memory pool.
 *
 *  This is a constant value that was provided in the LWPA_MEMPOOL_DEFINE()
 *  call.
 *
 *  \param name The name of the memory pool of which to get the size.
 *  \return The size of the memory pool.
 */
#define lwpa_mempool_size(name) (name##_pool_desc.pool_size)

/*! \brief Get the number of elements currently allocated from a memory pool.
 *  \param name The name of the memory pool for which to get the current usage.
 *  \return The number of elements currently allocated.
 */
#define lwpa_mempool_used(name) lwpa_mempool_used_priv(&name##_pool_desc)

#ifdef __cplusplus
extern "C" {
#endif

lwpa_error_t lwpa_mempool_init_priv(LwpaMempoolDesc *desc);
void *lwpa_mempool_alloc_priv(LwpaMempoolDesc *desc);
void lwpa_mempool_free_priv(LwpaMempoolDesc *desc, void *elem);
size_t lwpa_mempool_used_priv(LwpaMempoolDesc *desc);

#ifdef __cplusplus
}
#endif

/*!@}*/

#endif /* _LWPA_MEMPOOL_H_ */