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

/// @file etcpal/cpp/timer.h
/// @brief C++ wrapper and utilities for etcpal/timer.h

#ifndef ETCPAL_CPP_TIMER_H_
#define ETCPAL_CPP_TIMER_H_

#include <algorithm>
#include <chrono>
#include <limits>
#include "etcpal/timer.h"
#include "etcpal/cpp/common.h"

namespace etcpal
{
/// @defgroup etcpal_cpp_timer timer (Timers)
/// @ingroup etcpal_cpp
/// @brief C++ utilities for the @ref etcpal_timer module.
///
/// **WARNING:** This module must be explicitly initialized before use. Initialize the module by
/// calling etcpal_init() with the relevant feature mask:
/// @code
/// etcpal_init(ETCPAL_FEATURE_TIMERS);
/// @endcode
///
/// **NOTE:** The constructs in this module are available in similar form in the C++ standard
/// library using the &lt;chrono&gt; header, with the advantage that this module will work on any
/// platform for which EtcPal is ported, including embedded RTOS platforms, using their native tick
/// count. If your application or library does not need to run on these platforms, consider using
/// &lt;chrono&gt; instead.
///
/// Provides a class for representing points in time (TimePoint) and one which implements a passive
/// monotonic timer (Timer).

/// @ingroup etcpal_cpp_timer
/// @brief Represents a point in time.
///
/// More specifically, stores the number of milliseconds elapsed since an arbitrary point in the
/// past. This number is stored as a 32-bit unsigned integer and thus wraps every 4,294,967,295
/// milliseconds, or approximately 49.7 days. Because of this, it's recommended to only compare
/// TimePoints on a time scale which is shorter than that by at least an order of magnitude. This
/// reduces the likelihood that wrapping will be an issue.
///
/// @code
/// auto start_time = etcpal::TimePoint::Now();
///
/// // Do some work...
///
/// auto end_time = etcpal::TimePoint::Now();
/// EXPECT_GT(end_time, start_time);
/// int32_t interval = end_time - start_time; // How long did it take, in milliseconds?
/// @endcode
///
/// TimePoints are compared by comparing the signed difference of their respective values with 0.
/// This creates a "half the range" rule of thumb for comparison; TimePoints will compare
/// accurately as long as they were sampled during the same process execution and within 2^31
/// milliseconds (24.9 days) of each other.
///
/// This can be unintuitive to understand since TimePoints are based on unsigned 32-bit values
/// which are compared as if they are signed. Here are some examples:
///
/// @code
/// etcpal::TimePoint(0u) < etcpal::TimePoint(0x7fffffffu) // true
/// etcpal::TimePoint(0u) < etcpal::TimePoint(0x80000000u) // true
/// etcpal::TimePoint(0u) < etcpal::TimePoint(0x80000001u) // false
///
/// // You can shift both of these points by any constant interval and it will still hold true. For
/// // example:
///
/// etcpal::TimePoint(0xc0000000u) < etcpal::TimePoint(0x3fffffffu) // true
/// etcpal::TimePoint(0xc0000000u) < etcpal::TimePoint(0x40000000u) // true
/// etcpal::TimePoint(0xc0000000u) < etcpal::TimePoint(0x40000001u) // false
///
/// // And the same principle applies to using operator-:
///
/// etcpal::TimePoint(0x7fffffffu) - etcpal::TimePoint(0u) // 2,147,483,647; makes sense
/// etcpal::TimePoint(0x80000000u) - etcpal::TimePoint(0u) // -2,147,483,648; doesn't make sense
/// etcpal::TimePoint(0x80000001u) - etcpal::TimePoint(0u) // -2,147,483,647; doesn't make sense
/// @endcode
///
/// In order to avoid headaches of the kind that often arise when thinking about two's complement,
/// again, it's recommended to only compare TimePoints on a scale much less than days.
class TimePoint
{
public:
  /// @brief Construct a TimePoint with a value of 0 by default.
  ///
  /// Be careful when default-constructing TimePoints; since TimePoints store the number of
  /// milliseconds elapsed since an arbitrary point in the past, a default-constructed TimePoint
  /// will compare equal to etcpal::TimePoint::Now() every 49.7 days.
  TimePoint() = default;
  constexpr TimePoint(uint32_t ms);

  constexpr uint32_t value() const noexcept;

  ETCPAL_CONSTEXPR_14 TimePoint& operator+=(uint32_t duration) noexcept;
  ETCPAL_CONSTEXPR_14 TimePoint& operator-=(uint32_t duration) noexcept;

  static TimePoint Now() noexcept;

private:
  uint32_t ms_{0};
};

/// @brief Construct a TimePoint from a number of milliseconds elapsed since a point in the past.
constexpr TimePoint::TimePoint(uint32_t ms) : ms_(ms)
{
}

/// @brief Get the raw millisecond value from a TimePoint.
constexpr uint32_t TimePoint::value() const noexcept
{
  return ms_;
}

/// @brief Add a millisecond duration to a TimePoint.
ETCPAL_CONSTEXPR_14_OR_INLINE TimePoint& TimePoint::operator+=(uint32_t duration) noexcept
{
  ms_ += duration;
  return *this;
}

/// @brief Subtract a millisecond duration from a TimePoint.
ETCPAL_CONSTEXPR_14_OR_INLINE TimePoint& TimePoint::operator-=(uint32_t duration) noexcept
{
  ms_ -= duration;
  return *this;
}

/// @brief Get a TimePoint representing the current time.
inline TimePoint TimePoint::Now() noexcept
{
  return etcpal_getms();
}

/// @ingroup etcpal_cpp_timer
/// @brief A wrapper class for the EtcPal timer type.
///
/// See the description for @ref etcpal_timer for details about the time points and intervals used
/// by this class.
///
/// Use this class to time an interval.
///
/// @code
/// etcpal::Timer timer(100); // Start a 100-millisecond timer.
///
/// // Some time later...
/// uint32_t elapsed = timer.GetElapsed(); // How many milliseconds since the timer was started
/// uint32_t remaining = timer.GetRemaining(); // How much time is remaining in the interval.
/// bool is_expired = timer.IsExpired(); // Whether the interval has expired
///
/// timer.Reset(); // Reset the timer for another 100-millisecond interval
/// // Or
/// timer.Start(1000); // Reuse the timer for a different interval, in this case 1 second.
/// @endcode
class Timer
{
public:
  /// @brief Creates an expired timer with an interval of 0.
  Timer() = default;
  // Note: this constructor is not explicit by design, to allow implicit conversions e.g.
  //   etcpal::Timer timer = etcpal_c_function_that_returns_timer();
  constexpr Timer(const EtcPalTimer& c_timer) noexcept;
  Timer& operator=(const EtcPalTimer& c_timer) noexcept;
  explicit Timer(uint32_t interval) noexcept;
  template <typename Rep, typename Period>
  explicit Timer(const std::chrono::duration<Rep, Period>& interval) noexcept;

  constexpr const EtcPalTimer& get() const noexcept;
  ETCPAL_CONSTEXPR_14 EtcPalTimer& get() noexcept;

  TimePoint GetStartTime() const noexcept;
  uint32_t  GetInterval() const noexcept;
  uint32_t  GetElapsed() const noexcept;
  uint32_t  GetRemaining() const noexcept;
  bool      IsExpired() const noexcept;

  void Start(uint32_t interval) noexcept;
  template <typename Rep, typename Period>
  void Start(const std::chrono::duration<Rep, Period>& interval) noexcept;
  void Reset() noexcept;

private:
  EtcPalTimer timer_{};
};

/// @brief Construct a timer copied from an instance of the C EtcPalTimer type.
constexpr Timer::Timer(const EtcPalTimer& c_timer) noexcept : timer_(c_timer)
{
}

/// @brief Assign an instance of the C EtcPalTimer type to an instance of this class.
inline Timer& Timer::operator=(const EtcPalTimer& c_timer) noexcept
{
  timer_ = c_timer;
  return *this;
}

/// @brief Create and start a timer with the given interval in milliseconds.
inline Timer::Timer(uint32_t interval) noexcept
{
  Start(interval);
}

/// @brief Create and start a timer with the given interval.
///
/// Note: interval will be clamped to [0, UINT32_MAX] milliseconds.
template <typename Rep, typename Period>
Timer::Timer(const std::chrono::duration<Rep, Period>& interval) noexcept
{
  Start(interval);
}

/// @brief Get a const reference to the underlying C type.
constexpr const EtcPalTimer& Timer::get() const noexcept
{
  return timer_;
}

/// @brief Get a mutable reference to the underlying C type.
ETCPAL_CONSTEXPR_14_OR_INLINE EtcPalTimer& Timer::get() noexcept
{
  return timer_;
}

/// @brief Get the time when this timer was started or reset.
inline TimePoint Timer::GetStartTime() const noexcept
{
  return timer_.reset_time;
}

/// @brief Get the current interval being timed by the timer.
inline uint32_t Timer::GetInterval() const noexcept
{
  return timer_.interval;
}

/// @brief Get the time since the timer was reset.
inline uint32_t Timer::GetElapsed() const noexcept
{
  return etcpal_timer_elapsed(&timer_);
}

/// @brief Get the amount of time remaining in the timer's interval (returns 0 if the timer is expired).
inline uint32_t Timer::GetRemaining() const noexcept
{
  return etcpal_timer_remaining(&timer_);
}

/// @brief Whether the timer's interval is expired.
inline bool Timer::IsExpired() const noexcept
{
  return etcpal_timer_is_expired(&timer_);
}

/// @brief Start the timer with a new interval.
/// @param interval Interval to time, in milliseconds. An interval of 0 will result in a timer that
///                 is always expired.
inline void Timer::Start(uint32_t interval) noexcept
{
  etcpal_timer_start(&timer_, interval);
}

/// @brief Start the timer with a new interval.
/// @param interval Interval to time. Will be clamped to [0, UINT32_MAX] milliseconds. An interval
///                 of 0 will result in a timer that is always expired.
template <typename Rep, typename Period>
void Timer::Start(const std::chrono::duration<Rep, Period>& interval) noexcept
{
  uint32_t interval_clamped = static_cast<uint32_t>(
      std::min(std::chrono::milliseconds(interval).count(),
               static_cast<std::chrono::milliseconds::rep>(std::numeric_limits<uint32_t>::max())));
  etcpal_timer_start(&timer_, interval_clamped);
}

/// @brief Reset the timer while keeping the same interval.
inline void Timer::Reset() noexcept
{
  etcpal_timer_reset(&timer_);
}

/// @addtogroup etcpal_cpp_timer
/// @{

/// @name TimePoint operators
/// @{
///
constexpr int32_t operator-(const TimePoint& a, const TimePoint& b) noexcept
{
  return static_cast<int32_t>(a.value() - b.value());
}

constexpr bool operator==(const TimePoint& a, const TimePoint& b) noexcept
{
  return (a.value() == b.value());
}

constexpr bool operator!=(const TimePoint& a, const TimePoint& b) noexcept
{
  return !(a == b);
}

constexpr bool operator<(const TimePoint& a, const TimePoint& b) noexcept
{
  return (a - b) < 0;
}

constexpr bool operator>(const TimePoint& a, const TimePoint& b) noexcept
{
  return b < a;
}

constexpr bool operator<=(const TimePoint& a, const TimePoint& b) noexcept
{
  return !(b < a);
}

constexpr bool operator>=(const TimePoint& a, const TimePoint& b) noexcept
{
  return !(a < b);
}

/// @}
/// @}

};  // namespace etcpal

#endif  // ETCPAL_CPP_TIMER_H_
