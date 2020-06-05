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

/* etcpal_timer.h: Platform-neutral implementation of system timers. */

#ifndef ETCPAL_TIMER_H_
#define ETCPAL_TIMER_H_

#include <stdbool.h>
#include <stdint.h>

/**
 * @defgroup etcpal_timer timer (Timers)
 * @ingroup etcpal
 * @brief Platform-neutral system timers.
 *
 * ```c
 * #include "etcpal/timer.h"
 * ```
 *
 * **WARNING:** This module must be explicitly initialized before use. Initialize the module by
 * calling etcpal_init() with the relevant feature mask:
 * @code
 * etcpal_init(ETCPAL_FEATURE_TIMERS);
 * @endcode
 *
 * Provides an implementation of a passive monotonic timer, as well as a way to get monotonic time
 * points. Time points and intervals are represented in milliseconds by a 32-bit unsigned int. To
 * get a time point, use etcpal_getms().
 *
 * @code
 * uint32_t time_point = etcpal_getms();
 * @endcode
 *
 * A time point represents the time elapsed in milliseconds since an arbitrary fixed point in the
 * past, independent of any changes in system "wall clock" time. Time points provided by this
 * module wrap every 4,294,967,295 milliseconds, or approximately 49.7 days. Because of this, it's
 * recommended to only use this module to time events which occur on time scales which are shorter
 * than that by at least an order of magnitude. This reduces the likelihood that wrapping will be
 * an issue.
 *
 * Note that this problem only applies to wraparound past an original time point; absolute
 * wraparound of the integer type is handled. For example, using an EtcPalTimer to time a
 * 10-millisecond interval that begins at time point 4,294,967,290 and ends at time point 5 will
 * work as expected.
 *
 * Use the ETCPAL_TIME_ELAPSED_SINCE() macro to determine how much time has elapsed since a time
 * point.
 *
 * @code
 * uint32_t ms_elapsed = ETCPAL_TIME_ELAPSED_SINCE(time_point);
 * @endcode
 *
 * To time an interval, use an EtcPalTimer.
 *
 * @code
 * EtcPalTimer timer;
 * etcpal_timer_start(&timer, 100); // Start a 100-millisecond timer
 *
 * // Some time later...
 *
 * uint32_t elapsed = etcpal_timer_elapsed(&timer); // How many milliseconds since the timer was started
 * uint32_t remaining = etcpal_timer_remaining(&timer); // How much time is remaining in the interval
 * bool is_expired = etcpal_timer_is_expired(&timer); // Whether the interval has expired
 *
 * etcpal_timer_reset(&timer); // Reset the timer for another 100-millisecond interval
 * // Or
 * etcpal_timer_start(&timer, 1000); // Reuse the timer for a different interval, in this case 1 second.
 * @endcode
 *
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief A millisecond-resolution timer.
 *
 * The times are represented in milliseconds by a 32-bit unsigned integer. Since the timer is
 * monotonically-increasing, wraparound is handled by doing comparisons of the form
 * @code timeA - timeB > 0 @endcode rather than @code timeA > timeB @endcode.
 */
typedef struct EtcPalTimer
{
  uint32_t reset_time; /**< The time at which this timer was reset. */
  uint32_t interval;   /**< This timer's timeout interval. */
} EtcPalTimer;

/**
 * @brief Get the amount of time elapsed since the given time point in milliseconds.
 * @param start_time (uint32_t) The start time to measure against.
 * @return The number of milliseconds elapsed since the start time.
 */
#define ETCPAL_TIME_ELAPSED_SINCE(start_time) (uint32_t)(etcpal_getms() - (uint32_t)start_time)

/* Function with platform-specific definition */

/**
 * @brief Get a monotonically-increasing millisecond value
 * @return The current timestamp in milliseconds.
 */
uint32_t etcpal_getms(void);

/* Functions with platform-neutral definitions */

void     etcpal_timer_start(EtcPalTimer* timer, uint32_t interval);
void     etcpal_timer_reset(EtcPalTimer* timer);
uint32_t etcpal_timer_elapsed(const EtcPalTimer* timer);
bool     etcpal_timer_is_expired(const EtcPalTimer* timer);
uint32_t etcpal_timer_remaining(const EtcPalTimer* timer);

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif /* ETCPAL_TIMER_H_ */
