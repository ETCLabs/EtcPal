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

#include "lwpa/mempool.h"

#include "lwpa/common.h"
#include "lwpa/bool.h"
#include "lwpa/lock.h"

static bool mempool_lock_initted = false;
static lwpa_mutex_t mempool_lock;

lwpa_error_t lwpa_mempool_init_priv(LwpaMempoolDesc* desc)
{
  lwpa_error_t res = kLwpaErrSys;

  if (!mempool_lock_initted)
  {
    if (lwpa_mutex_create(&mempool_lock))
      mempool_lock_initted = true;
    else
      return res;
  }

  if (lwpa_mutex_take(&mempool_lock, LWPA_WAIT_FOREVER))
  {
    size_t i;
    for (i = 0; i < desc->pool_size - 1; ++i)
      desc->list[i].next = &desc->list[i + 1];
    desc->list[i].next = NULL;
    desc->freelist = desc->list;
    desc->current_used = 0;
    res = kLwpaErrOk;
    lwpa_mutex_give(&mempool_lock);
  }
  return res;
}

void* lwpa_mempool_alloc_priv(LwpaMempoolDesc* desc)
{
  void* elem = NULL;

  if (lwpa_mutex_take(&mempool_lock, LWPA_WAIT_FOREVER))
  {
    char* c_pool = (char*)desc->pool;
    LwpaMempool* elem_desc = desc->freelist;
    if (elem_desc)
    {
      ptrdiff_t index = elem_desc - desc->list;
      if (index >= 0)
      {
        desc->freelist = elem_desc->next;
        elem = (void*)(c_pool + ((size_t)index * desc->elem_size));
        ++desc->current_used;
      }
    }
    lwpa_mutex_give(&mempool_lock);
  }

  return elem;
}

void lwpa_mempool_free_priv(LwpaMempoolDesc* desc, void* elem)
{
  char* c_pool = (char*)desc->pool;

  ptrdiff_t offset = (char*)elem - c_pool;
  if (offset >= 0)
  {
    if (((size_t)offset % desc->elem_size == 0) && lwpa_mutex_take(&mempool_lock, LWPA_WAIT_FOREVER))
    {
      size_t index = (size_t)offset / desc->elem_size;
      LwpaMempool* elem_desc = &desc->list[index];
      elem_desc->next = desc->freelist;
      desc->freelist = elem_desc;
      --desc->current_used;
      lwpa_mutex_give(&mempool_lock);
    }
  }
}

size_t lwpa_mempool_used_priv(LwpaMempoolDesc* desc)
{
  size_t res = 0;
  if (lwpa_mutex_take(&mempool_lock, LWPA_WAIT_FOREVER))
  {
    res = desc->current_used;
    lwpa_mutex_give(&mempool_lock);
  }
  return res;
}
