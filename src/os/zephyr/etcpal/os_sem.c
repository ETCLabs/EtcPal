#include "etcpal/sem.h"
#include <zephyr/kernel.h>

bool etcpal_sem_create(etcpal_sem_t *id, unsigned int initial_count,
                       unsigned int max_count) {

  return k_sem_init(id, initial_count, max_count) == 0;
}

bool etcpal_sem_wait(etcpal_sem_t *id) {
  return k_sem_take(id, K_FOREVER) == 0;
}

bool etcpal_sem_try_wait(etcpal_sem_t *id) {
  return k_sem_take(id, K_NO_WAIT) == 0;
}

bool etcpal_sem_timed_wait(etcpal_sem_t *id, int timeout_ms) {
  return k_sem_take(id, K_TIMEOUT_ABS_MS(timeout_ms)) == 0;
}

bool etcpal_sem_post(etcpal_sem_t *id) {
  k_sem_give(id);
  return true;
}

bool etcpal_sem_post_from_isr(etcpal_sem_t *id) {
  k_sem_give(id);
  return true;
}

void etcpal_sem_destroy(etcpal_sem_t *id) {}
