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

/// @file etcpal/cpp/event_group.h
/// @brief C++ wrapper and utilities for etcpal/event_group.h

#ifndef ETCPAL_CPP_EVENT_GROUP_H_
#define ETCPAL_CPP_EVENT_GROUP_H_

#include "etcpal/cpp/common.h"
#include "etcpal/event_group.h"

namespace etcpal
{
/// @defgroup etcpal_cpp_event_group event_group (Event Groups)
/// @ingroup etcpal_cpp
/// @brief C++ utilities for the @ref etcpal_event_group module.
///
/// Provides a C++ wrapper for the EtcPal event group type.

/// @copydoc etcpal_event_bits_t
using EventBits = etcpal_event_bits_t;

/// @ingroup etcpal_cpp_event_group
/// @brief A wrapper class for the EtcPal event group type.
///
/// Example usage:
/// @code
/// etcpal::EventGroup event_group;
/// constexpr etcpal::EventBits kEventMessageReceived = 0x1;
/// constexpr etcpal::EventBits kEventMessageError = 0x2;
///
/// void event_handler_thread()
/// {
///   while (keep_running)
///   {
///     etcpal::EventBits event = event_group.Wait(kEventMessageReceived | kEventMessageError,
///                                                ETCPAL_EVENT_GROUP_AUTO_CLEAR);
///     if (event & kEventMessageReceived)
///     {
///       // Handle message received...
///     }
///     // Note: Depending on the user implementation, kEventMessageReceived and kEventMessageError
///     // could be set at the same time.
///     if (event & kEventMessageError)
///     {
///       // Handle message error...
///     }
///
///     // Since we passed ETCPAL_EVENT_GROUP_AUTO_CLEAR to Wait(), the event bits do not need to
///     // be explicitly cleared before the next iteration.
///   }
/// }
///
/// void message_listener_thread()
/// {
///   while (keep_running)
///   {
///     // Get message...
///     etcpal::EventBits event_bits = kEventMessageReceived;
///     if (message_error)
///     {
///       event_bits |= kEventMessageError;
///     }
///     event_group.SetBits(event_bits);
///   }
/// }
/// @endcode
///
/// See @ref etcpal_event_group for more information.
class EventGroup
{
public:
  EventGroup();
  ~EventGroup();

  EventGroup(const EventGroup& other) = delete;
  EventGroup& operator=(const EventGroup& other) = delete;
  EventGroup(EventGroup&& other)                 = delete;
  EventGroup& operator=(EventGroup&& other) = delete;

  EventBits Wait(EventBits bits, int flags = 0);
  EventBits TryWait(EventBits bits, int flags = 0, int timeout_ms = 0);
  void      SetBits(EventBits bits_to_set);
  EventBits GetBits();
  void      ClearBits(EventBits bits_to_clear);
  void      SetBitsFromIsr(EventBits bits_to_set);
  EventBits GetBitsFromIsr();
  void      ClearBitsFromIsr(EventBits bits_to_clear);

  etcpal_event_group_t& get();

private:
  etcpal_event_group_t event_group_{};
};

/// @brief Create a new event group.
inline EventGroup::EventGroup()
{
  (void)etcpal_event_group_create(&event_group_);
}

/// @brief Destroy the event group.
inline EventGroup::~EventGroup()
{
  etcpal_event_group_destroy(&event_group_);
}

/// @brief Wait for one or more bits in an event group.
/// @param bits Mask of bits in the event group to wait for.
/// @param flags Flags that define waiting options.
/// @return The result of etcpal_event_group_wait() on the underlying event group.
inline EventBits EventGroup::Wait(EventBits bits, int flags)
{
  return etcpal_event_group_wait(&event_group_, bits, flags);
}

/// @brief Wait for one or more bits in an event group until a timeout expires.
///
/// **NOTE**: Timeouts other than 0 or #ETCPAL_WAIT_FOREVER are typically only honored on real-time
/// platforms. See the table in @ref etcpal_event_group for more information. On platforms where
/// timeouts are not honored, passing 0 for timeout_ms returns the state of the event bits
/// immediately, while any other value executes the equivalent of Wait().
///
/// @param bits Mask of bits in the event group to wait for.
/// @param flags Flags that define waiting options.
/// @param timeout_ms Maximum amount of time to wait for the event bits, in milliseconds. If
///                   #ETCPAL_WAIT_FOREVER is given, the result is the same as if Wait() was called.
inline EventBits EventGroup::TryWait(EventBits bits, int flags, int timeout_ms)
{
  return etcpal_event_group_timed_wait(&event_group_, bits, flags, timeout_ms);
}

/// @brief Set one or more bits in an event group.
///
/// See etcpal_event_group_set_bits() for more information.
///
/// @param bits_to_set Mask of bits in the event group to set.
inline void EventGroup::SetBits(EventBits bits_to_set)
{
  etcpal_event_group_set_bits(&event_group_, bits_to_set);
}

/// @brief Get the current bitfield from an event group.
inline EventBits EventGroup::GetBits()
{
  return etcpal_event_group_get_bits(&event_group_);
}

/// @brief Clear one or more bits in an event group.
/// @param bits_to_clear Mask of bits to clear.
inline void EventGroup::ClearBits(EventBits bits_to_clear)
{
  etcpal_event_group_clear_bits(&event_group_, bits_to_clear);
}

/// @brief Set one or more bits in an event group from an interrupt context.
///
/// **NOTE**: Only meaningful on some platforms. See the table in @ref etcpal_event_group and the
/// description of etcpal_event_group_set_bits_from_isr() for more information. On platforms on
/// which it is not meaningful, executes the equivalent of SetBits().
///
/// @param bits_to_set Mask of bits in the event group to set.
inline void EventGroup::SetBitsFromIsr(EventBits bits_to_set)
{
  etcpal_event_group_set_bits_from_isr(&event_group_, bits_to_set);
}

/// @brief Get the current bitfield from an event group from an interrupt context.
///
/// **NOTE**: Only meaningful on some platforms. See the table in @ref etcpal_event_group and the
/// description of etcpal_event_group_get_bits_from_isr() for more information. On platforms on
/// which it is not meaningful, executes the equivalent of GetBits().
inline EventBits EventGroup::GetBitsFromIsr()
{
  return etcpal_event_group_get_bits_from_isr(&event_group_);
}

/// @brief Clear one or more bits in an event group from an interrupt context.
///
/// **NOTE**: Only meaningful on some platforms. See the table in @ref etcpal_event_group and the
/// description of etcpal_event_group_clear_bits_from_isr() for more information. On platforms on
/// which it is not meaningful, executes the equivalent of ClearBits().
///
/// @param bits_to_clear Mask of bits to clear.
inline void EventGroup::ClearBitsFromIsr(EventBits bits_to_clear)
{
  etcpal_event_group_clear_bits_from_isr(&event_group_, bits_to_clear);
}

/// @brief Get a reference to the underlying etcpal_event_group_t type.
inline etcpal_event_group_t& EventGroup::get()
{
  return event_group_;
}

};  // namespace etcpal

#endif  // ETCPAL_CPP_EVENT_GROUP_H_
