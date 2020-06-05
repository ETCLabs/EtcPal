/******************************************************************************
 * Copyright 2020 ETC Inc.
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

/* etcpal/mempool.h: Fixed-size memory pools. */

#ifndef ETCPAL_MEMPOOL_H_
#define ETCPAL_MEMPOOL_H_

#include <stddef.h>
#include "etcpal/error.h"

/**
 * @defgroup etcpal_mempool mempool (Memory Pools)
 * @ingroup etcpal
 * @brief Memory pools with fixed-size elements.
 *
 * ```c
 * #include "etcpal/mempool.h"
 * ```
 *
 * This module can be used to declare memory pools containing some number of elements of an
 * arbitrary type. Only elements of that type can be allocated from the pool or freed back into it.
 * A pool element can be a singular type (e.g. `MyStruct`) or a fixed-size array (e.g.
 * `uint8_t[100]`). Allocations and deallocations are ensured to be thread_safe by an
 * #etcpal_mutex_t internally.
 *
 * For example:
 * @code
 * typedef struct
 * {
 *   int data1;
 *   int data2;
 * } MyStruct;
 *
 * // Create a memory pool of 20 MyStruct instances
 * // This line typically appears at file scope
 * ETCPAL_MEMPOOL_DEFINE(my_struct_pool, MyStruct, 20);
 *
 * // Then, at function scope...
 *
 * // Initialize the pool
 * etcpal_mempool_init(my_struct_pool);
 *
 * // Get an item from the pool
 * MyStruct* val = (MyStruct*)etcpal_mempool_alloc(my_struct_pool);
 *
 * if (val)
 * {
 *   // Do something with the item
 * }
 *
 * // Check the status of the pool
 * printf("my_struct_pool status: %zu used out of %zu total\n", etcpal_mempool_used(my_struct_pool),
 *        etcpal_mempool_size(my_struct_pool));
 *
 * // Free the item back to the pool
 * etcpal_mempool_free(my_struct_pool, val);
 *
 * // No deinitialization required
 * @endcode
 *
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/** @cond internal_mempool_structs */

typedef struct EtcPalMempool EtcPalMempool;

/**
 * (Not for direct usage) A tiny structure simply used to maintain the freelist for each pool.
 */
struct EtcPalMempool
{
  EtcPalMempool* next;
};

/**
 * (Not for direct usage) A memory pool description structure.
 *
 * Do not declare or use this structure directly; instead, use ETCPAL_MEMPOOL_DECLARE(),
 * ETCPAL_MEMPOOL_DEFINE(), ETCPAL_MEMPOOL_DEFINE_ARRAY(), and the etcpal_mempool_* macros to
 * interact with it.
 */
typedef struct EtcPalMempoolDesc
{
  const size_t         elem_size;    /**< The size of each element. */
  const size_t         pool_size;    /**< The number of elements in the pool. */
  EtcPalMempool*       freelist;     /**< The current freelist. */
  EtcPalMempool* const list;         /**< The array of mempool list structs. */
  size_t               current_used; /**< The number of pool elements that have currently been allocated. */
  void* const          pool;         /**< The actual pool memory. */
} EtcPalMempoolDesc;

/** @endcond */

/**
 * @brief Declare a pool as an external variable.
 *
 * This optional macro is useful for header files; when used, it must be paired with a call of
 * ETCPAL_MEMPOOL_DEFINE() or ETCPAL_MEMPOOL_DEFINE_ARRAY() using the same name.
 *
 * @param name The name of the memory pool.
 */
#define ETCPAL_MEMPOOL_DECLARE(name) extern EtcPalMempoolDesc name##_pool_desc;

/**
 * @brief Define a new memory pool.
 *
 * Expands to a number of variable definitions. Should not be used in a header file; if you want
 * your memory pool to be accessible from multiple source files, use ETCPAL_MEMPOOL_DECLARE() in the
 * header file in addition to this macro.
 *
 * @param name The name of the memory pool.
 * @param type The type of each element in the memory pool (e.g. int, struct foo)
 * @param size The number of elements in the memory pool.
 */
#define ETCPAL_MEMPOOL_DEFINE(name, type, size)                                     \
  type                     name##_pool[size];                                       \
  struct EtcPalMempool     name##_pool_list[size];                                  \
  struct EtcPalMempoolDesc name##_pool_desc = {sizeof(type),     /* elem_size */    \
                                               size,             /* pool_size */    \
                                               NULL,             /* freelist */     \
                                               name##_pool_list, /* list */         \
                                               0,                /* current_used */ \
                                               name##_pool /* pool */}

/**
 * @brief Define a new memory pool composed of arrays of elements.
 *
 * This is an alternative to ETCPAL_MEMPOOL_DEFINE() for creating memory pools containing fixed-size
 * arrays of elements.
 *
 * @param name The name of the memory pool.
 * @param type The type of a single array element in the memory pool.
 * @param array_size The number of elements in each array.
 * @param pool_size The number of arrays in the memory pool.
 */
#define ETCPAL_MEMPOOL_DEFINE_ARRAY(name, type, array_size, pool_size)                      \
  type                     name##_pool[array_size][pool_size];                              \
  struct EtcPalMempool     name##_pool_list[pool_size];                                     \
  struct EtcPalMempoolDesc name##_pool_desc = {sizeof(type[array_size]), /* elem_size */    \
                                               pool_size,                /* pool_size */    \
                                               NULL,                     /* freelist */     \
                                               name##_pool_list,         /* list */         \
                                               0,                        /* current_used */ \
                                               name##_pool /* pool */}

/**
 * @brief Initialize a memory pool.
 *
 * Must be called on a pool before using etcpal_mempool_alloc() or etcpal_mempool_free() on it. There
 * is no deinitialization function because there is no cleanup necessary; to reset a pool, simply
 * call this function again.
 *
 * @param name The name of the memory pool to initialize.
 * @return #kEtcPalErrOk: The memory pool was initialized successfully.
 * @return #kEtcPalErrSys: An internal system call error occurred.
 */
#define etcpal_mempool_init(name) etcpal_mempool_init_priv(&name##_pool_desc)

/**
 * @brief Allocate a new element from a memory pool.
 * @param name The name of the memory pool from which to allocate a new element.
 * @return Pointer to the newly-allocated element (success) or NULL (pool is out of memory).
 */
#define etcpal_mempool_alloc(name) etcpal_mempool_alloc_priv(&name##_pool_desc)

/**
 * @brief Free an element back to a memory pool.
 *
 * CAUTION: Just like with a normal free(), freeing a pointer to an element that has not previously
 * been allocated or a pointer to somewhere outside of the memory pool will cause undefined
 * behavior.
 *
 * @param name The name of the memory pool to which to free an element.
 * @param mem Pointer to the element to free.
 */
#define etcpal_mempool_free(name, mem) etcpal_mempool_free_priv(&name##_pool_desc, mem)

/**
 * @brief Get the total size of a memory pool.
 *
 * This is a constant value that was provided in the ETCPAL_MEMPOOL_DEFINE() call.
 *
 * @param name The name of the memory pool of which to get the size.
 * @return The size of the memory pool.
 */
#define etcpal_mempool_size(name) (name##_pool_desc.pool_size)

/**
 * @brief Get the number of elements currently allocated from a memory pool.
 * @param name The name of the memory pool for which to get the current usage.
 * @return The number of elements currently allocated.
 */
#define etcpal_mempool_used(name) etcpal_mempool_used_priv(&name##_pool_desc)

/** @cond internal_mempool_functions */

etcpal_error_t etcpal_mempool_init_priv(EtcPalMempoolDesc* desc);
void*          etcpal_mempool_alloc_priv(EtcPalMempoolDesc* desc);
void           etcpal_mempool_free_priv(EtcPalMempoolDesc* desc, void* elem);
size_t         etcpal_mempool_used_priv(EtcPalMempoolDesc* desc);

/** @endcond */

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif /* ETCPAL_MEMPOOL_H_ */
