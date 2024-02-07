#include "etcpal/sem.h"
#include <zephyr/kernel.h>

bool etcpal_sem_post(etcpal_sem_t* id)
{
  k_sem_give(id);
  return true;
}

bool etcpal_sem_post_from_isr(etcpal_sem_t* id)
{
  k_sem_give(id);
  return true;
}

void etcpal_sem_destroy(etcpal_sem_t* id)
{
  // Not implemented
}
