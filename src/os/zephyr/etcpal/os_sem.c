#include "etcpal/sem.h"
#include <zephyr/kernel.h>

bool etcpal_sem_create(etcpal_sem_t* id, unsigned int initial_count, unsigned int max_count)
{
  int err = k_sem_init(&id->sem, initial_count, max_count);
  if (err)
  {
    return false;
  }
  id->count = initial_count;
  id->limit = max_count;
  return true;
}

bool etcpal_sem_wait(etcpal_sem_t* id)
{
  int err = k_sem_take(&id->sem, K_FOREVER);
  if (err)
  {
    return false;
  }
  id->count--;
  return true;
}

bool etcpal_sem_try_wait(etcpal_sem_t* id)
{
  int err = k_sem_take(&id->sem, K_NO_WAIT);
  if (err)
  {
    return false;
  }
  id->count--;
  return true;
}

bool etcpal_sem_timed_wait(etcpal_sem_t* id, int timeout_ms)
{
  int err = k_sem_take(&id->sem, K_MSEC(timeout_ms));
  if (err)
  {
    return false;
  }
  id->count--;
  return true;
}

bool etcpal_sem_post(etcpal_sem_t* id)
{
  if (id->count >= id->limit)
  {
    return false;
  }
  k_sem_give(&id->sem);
  id->count++;
  return true;
}

void etcpal_sem_destroy(etcpal_sem_t* id)
{
  // Not implemented
}
