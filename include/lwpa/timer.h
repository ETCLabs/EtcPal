/******************************************************************************
 * Copyright 2018 ETC Inc.
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

/* lwpa_timer.h: Platform-neutral implementation of system timers. */
#ifndef _LWPA_TIMER_H_
#define _LWPA_TIMER_H_

#include "lwpa/int.h"
#include "lwpa/bool.h"

/*! \defgroup lwpa_timer lwpa_timer
 *  \ingroup lwpa
 *  \brief Platform-neutral system timers.
 *
 *  \#include "lwpa/timer.h"
 *
 *  @{
 */

/*! \brief A millisecond-resolution timer.
 *
 *  The times are represented in milliseconds by a 32-bit unsigned integer. Since the timer is
 *  monotonically-increasing, wraparound is handled by doing comparisons of the form
 *  \code timeA - timeB > 0 \endcode rather than \code timeA > timeB \endcode.
 */
typedef struct LwpaTimer
{
  uint32_t reset_time; /*!< The time at which this timer was reset. */
  uint32_t interval;   /*!< This timer's timeout interval. */
} LwpaTimer;

#ifdef __cplusplus
extern "C" {
#endif

/*! \brief Get a monotonically-increasing millisecond value
 *  \return The current timestamp in milliseconds.
 */
uint32_t lwpa_getms();

/*! \brief Start a timer.
 *  \param lwpatimerptr Pointer to the LwpaTimer to start.
 *  \param intval Timer interval in milliseconds. An interval of 0 will result in a timer that is
 *                always expired.
 */
#define lwpa_timer_start(lwpatimerptr, intval) \
  do                                           \
  {                                            \
    (lwpatimerptr)->reset_time = lwpa_getms(); \
    (lwpatimerptr)->interval = intval;         \
  } while (0)

/*! \brief Reset a timer while keeping the same interval.
 *  \param lwpatimerptr Pointer to the LwpaTimer to reset.
 */
#define lwpa_timer_reset(lwpatimerptr) (((lwpatimerptr)->reset_time) = lwpa_getms())

/*! \brief Get the time since a timer was reset.
 *  \param lwpatimerptr Pointer to the LwpaTimer of which to get the elapsed time.
 *  \return Number of milliseconds since the timer was reset.
 */
#define lwpa_timer_elapsed(lwpatimerptr) (lwpa_getms() - (lwpatimerptr)->reset_time)

/*! \brief Check to see if a timer is expired.
 *  \param lwpatimerptr Pointer to the LwpaTimer of which to check the expiration.
 *  \return true (more than \link LwpaTimer::interval interval \endlink milliseconds have passed
 *          since the timer was started/reset) or false (less than or equal to
 *          \link LwpaTimer::interval interval \endlink milliseconds have passed since the timer was
 *          started/reset)
 */
#define lwpa_timer_isexpired(lwpatimerptr) \
  (((lwpatimerptr)->interval == 0) || (lwpa_timer_elapsed(lwpatimerptr) > (lwpatimerptr)->interval))

/*! \brief Get the amount of time remaining in a timer.
 *  \param timer Pointer to the LwpaTimer of which to get the remaining time.
 *  \return Remaining time in milliseconds or 0 (timer is expired).
 */
uint32_t lwpa_timer_remaining(const LwpaTimer *timer);

#ifdef __cplusplus
}
#endif

/*!@}*/

#endif /* _LWPA_TIMER_H_ */
