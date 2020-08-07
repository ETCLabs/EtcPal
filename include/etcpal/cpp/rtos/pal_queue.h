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

/// \file etcpal/cpp/rtos/pal_queue.h
/// \brief C++ wrapper and utilities for etcpal/rtos/pal_queue.h

#ifndef ETCPAL_CPP_RTOS_ERROR_H
#define ETCPAL_CPP_RTOS_ERROR_H

#include "etcpal/rtos/pal_queue.h"
#include "etcpal/cpp/common.h"
#include <chrono>

namespace etcpal
{

template <class T>
class Queue
{
  etcpal_queue_t m_queueHandle;

public:
  explicit Queue(unsigned size) { etcpal_queue_create(&m_queueHandle, size, sizeof(T)); }

  bool AddToQueue(const T& data, bool block=false, unsigned timeout_ms=0)
  {
      if (block)
      {
        return etcpal_queue_add_blocking(&m_queueHandle, &data);
      }
      else
      {
        return etcpal_queue_add(&m_queueHandle, &data, timeout_ms);
      }
      
  }

  bool AddToQueue_from_isr(const T& data) { return etcpal_queue_add_from_isr(&m_queueHandle, &data); }

  bool GetFromQueue(T& data, unsigned timeout_ms = 0) { return etcpal_queue_get(&m_queueHandle, &data, timeout_ms); }
  bool GetFromQueue(T& data, std::chrono::milliseconds timeout_ms = std::chrono::milliseconds(0)) { return etcpal_queue_get(&m_queueHandle, &data, timeout_ms.count()); }
  bool GetFromQueue_blocking(T& data) { return etcpal_queue_add_blocking(&m_queueHandle, &data); }
  bool IsEmpty() const { return etcpal_queue_is_empty(&m_queueHandle); };
  bool IsEmpty_from_isr() const { return etcpal_queue_is_empty_from_isr(&m_queueHandle); };
};

};     // namespace etcpal

#endif // ETCPAL_CPP_RTOS_ERROR_H
