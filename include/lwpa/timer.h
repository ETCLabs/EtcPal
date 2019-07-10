/******************************************************************************
 * Copyright 2019 ETC Inc.
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

/* Function with platform-specific definition */

/*! \brief Get a monotonically-increasing millisecond value
 *  \return The current timestamp in milliseconds.
 */
uint32_t lwpa_getms();

/* Functions with platform-neutral definitions */

void lwpa_timer_start(LwpaTimer* timer, uint32_t interval);
void lwpa_timer_reset(LwpaTimer* timer);
uint32_t lwpa_timer_elapsed(const LwpaTimer* timer);
bool lwpa_timer_is_expired(const LwpaTimer* timer);
uint32_t lwpa_timer_remaining(const LwpaTimer* timer);

#ifdef __cplusplus
}
#endif

/*! @} */

#endif /* _LWPA_TIMER_H_ */
