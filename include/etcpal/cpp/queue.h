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

/// \file etcpal/cpp/queue.h
/// \brief C++ wrapper and utilities for etcpal/queue.h

#ifndef ETCPAL_CPP_QUEUE_H
#define ETCPAL_CPP_QUEUE_H

#include "etcpal/queue.h"
#include "etcpal/cpp/common.h"
#include <chrono>

namespace etcpal
{
template <class T>
class Queue
{
  etcpal_queue_t m_queueHandle;

public:
  explicit Queue(unsigned size);

  bool SendToQueue(const T& data, bool block = false, unsigned timeout_ms = 0);

  bool SendToQueueFromIsr(const T& data);

  bool ReceiveFromQueue(T& data, unsigned timeout_ms = 0);
  bool ReceiveFromQueue(T& data, std::chrono::milliseconds timeout_ms = std::chrono::milliseconds(0));
  bool ReceiveFromQueueBlocking(T& data);
  bool IsEmpty() const;
  bool IsEmptyFromIsr() const;
};

/// @brief Create a new queue
///
/// @param size The size of the queue
template <class T>
inline Queue<T>::Queue(unsigned size)
{
  etcpal_queue_create(&m_queueHandle, size, sizeof(T));
}

/// @brief Add some data to the queue
///
/// Returns when either the timeout expires or the add was attempted. It is still possible for the attempt to be made
/// and the add to not work. Check the return value for confirmation.
///
/// @param data A reference to the data
/// @param block If this should cause the thread to block
/// @param timeout_ms How long to wait to add to the queue
///
/// @return The result of the attempt to add to the queue
template <class T>
inline bool Queue<T>::SendToQueue(const T& data, bool block, unsigned timeout_ms)
{
  if (block)
  {
    return etcpal_queue_send(&m_queueHandle, &data);
  }
  else
  {
    return etcpal_queue_timed_send(&m_queueHandle, &data, timeout_ms);
  }
}

/// @brief Add to a queue from an interrupt service routine. 
///
/// @param data A reference to the data to be added to the queue
///
/// @return The result of the attempt to add to the queue
template <class T>
inline bool Queue<T>::SendToQueueFromIsr(const T& data)
{
  return etcpal_queue_send_from_isr(&m_queueHandle, &data);
}

/// @brief Get an item from the queue
///
/// @param data A reference to the data that will receive the item from the queue
/// @param timeout_ms Amount of time to wait for data
///
/// @return The result of the attempt to get an item from the queue
template <class T>
inline bool Queue<T>::ReceiveFromQueue(T& data, unsigned timeout_ms)
{
  return etcpal_queue_timed_receive(&m_queueHandle, &data, timeout_ms);
}

/// @brief Get an item from the queue
///
/// @param data A reference to the data that will receive the item from the queue
/// @param timeout_ms Amount of time to wait for data
///
/// @return The result of the attempt to get an item from the queue
template <class T>
inline bool Queue<T>::ReceiveFromQueue(T& data, std::chrono::milliseconds timeout_ms)
{
  return etcpal_queue_timed_receive(&m_queueHandle, &data, timeout_ms.count());
}

/// @brief Get an item from the queue - block until it works
///
/// @param data A reference to the data that will receive the item from the queue
///
/// @return The result of the attempt to get an item from the queue
template <class T>
inline bool Queue<T>::ReceiveFromQueueBlocking(T& data)
{
  return etcpal_queue_receive(&m_queueHandle, &data);
}

/// @brief Check if a queue is empty
///
/// @return True if queue is empty, False otherwise
template <class T>
inline bool Queue<T>::IsEmpty() const
{
  return etcpal_queue_is_empty(&m_queueHandle);
};

/// @brief Check if a queue is empty from an interrupt service routin
///
/// @return True if queue is empty, False otherwise
template <class T>
inline bool Queue<T>::IsEmptyFromIsr() const
{
  return etcpal_queue_is_empty_from_isr(&m_queueHandle);
};

};  // namespace etcpal

#endif  // ETCPAL_CPP_RTOS_ERROR_H
