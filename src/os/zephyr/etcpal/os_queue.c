/******************************************************************************
 * Copyright 2022 ETC Inc.
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

#include "etcpal/queue.h"

/*************************** Function definitions ****************************/

bool etcpal_queue_create(etcpal_queue_t *id, size_t size, size_t item_size) {
  return false;
}

bool etcpal_queue_create_static(etcpal_queue_t *id, size_t size,
                                size_t item_size, uint8_t *buffer) {
  k_msgq_init(id, buffer, item_size, size);
  return true;
}

void etcpal_queue_destroy(etcpal_queue_t *id) {
  k_msgq_cleanup(id);
  id = 0;
}

bool etcpal_queue_send(etcpal_queue_t *id, const void *data) {
  bool true_if_success = false;

  if (k_msgq_put(id, data, K_FOREVER) == 0) {
    true_if_success = true;
  }

  return true_if_success;
}

bool etcpal_queue_timed_send(etcpal_queue_t *id, const void *data,
                             int timeout_ms) {
  bool true_if_success = false;

  k_timeout_t timeout;
  if (timeout_ms == ETCPAL_WAIT_FOREVER) {
    timeout = K_FOREVER;
  } else {
    timeout = K_TIMEOUT_ABS_MS(timeout_ms);
  }

  if (k_msgq_put(id, data, timeout) == 0) {
    true_if_success = true;
  }

  return true_if_success;
}

bool etcpal_queue_send_from_isr(etcpal_queue_t *id, const void *data) {
  // The normal k_msgq_put is isr safe
  return etcpal_queue_timed_send(id, data, ETCPAL_WAIT_FOREVER);
}

bool etcpal_queue_receive(etcpal_queue_t *id, void *data) {
  bool true_if_success = false;

  if (k_msgq_get(id, data, K_FOREVER) == 0) {
    true_if_success = true;
  }

  return true_if_success;
}

bool etcpal_queue_timed_receive(etcpal_queue_t *id, void *data,
                                int timeout_ms) {
  bool true_if_success = false;

  k_timeout_t timeout;
  
  if (timeout_ms == ETCPAL_WAIT_FOREVER) {
    timeout = K_FOREVER;
  } else {
    timeout = K_TIMEOUT_ABS_MS(timeout_ms);
  }

  if (k_msgq_get(id, data, timeout) == 0) {
    true_if_success = true;
  }

  return true_if_success;
}

bool etcpal_queue_receive_from_isr(etcpal_queue_t *id, void *data) {
  // The normal k_msgq_get is isr safe
  return etcpal_queue_timed_receive(id, data, ETCPAL_NO_WAIT);
}

bool etcpal_queue_reset(etcpal_queue_t *id) {
  k_msgq_purge(id);
  return true;
}

bool etcpal_queue_is_empty(const etcpal_queue_t *id) {

  bool true_if_empty = false;

  struct k_msgq_attrs attrs;

  k_msgq_get_attrs(id, &attrs);

  if (attrs.used_msgs == 0) {
    true_if_empty = true;
  }

  return true_if_empty;
}

bool etcpal_queue_is_empty_from_isr(const etcpal_queue_t *id) {
  // I think the native call is ISR safe
  return etcpal_queue_is_empty(id);
}

bool etcpal_queue_is_full(const etcpal_queue_t *id) {
  bool true_if_full = false;

  struct k_msgq_attrs attrs;

  k_msgq_get_attrs(id, &attrs);

  if (attrs.used_msgs == attrs.max_msgs) {
    true_if_full = true;
  }

  return true_if_full;
}

bool etcpal_queue_is_full_from_isr(const etcpal_queue_t *id) {
  return etcpal_queue_is_full(id);
}

size_t etcpal_queue_slots_used(const etcpal_queue_t *id) {
  struct k_msgq_attrs attrs;

  k_msgq_get_attrs(id, &attrs);

  return attrs.used_msgs;
}

size_t etcpal_queue_slots_used_from_isr(const etcpal_queue_t *id) {
  return etcpal_queue_slots_used(id);
}

size_t etcpal_queue_slots_available(const etcpal_queue_t *id) {
  struct k_msgq_attrs attrs;

  k_msgq_get_attrs(id, &attrs);

  return (attrs.max_msgs - attrs.used_msgs);
}
