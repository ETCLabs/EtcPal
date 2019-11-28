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
 * This file is a part of EtcPal. For more information, go to:
 * https://github.com/ETCLabs/EtcPal
 ******************************************************************************/

/* etcpal_timer.h: Platform-neutral implementation of system timers. */

#ifndef ETCPAL_TIMER_H_
#define ETCPAL_TIMER_H_

#include "etcpal/int.h"
#include "etcpal/bool.h"

/*!
 * \defgroup etcpal_timer Timers (timer)
 * \ingroup etcpal
 * \brief Platform-neutral system timers.
 *
 * ```c
 * #include "etcpal/timer.h"
 * ```
 *
 * @{
 */

/*!
 * \brief A millisecond-resolution timer.
 *
 * The times are represented in milliseconds by a 32-bit unsigned integer. Since the timer is
 * monotonically-increasing, wraparound is handled by doing comparisons of the form
 * \code timeA - timeB > 0 \endcode rather than \code timeA > timeB \endcode.
 */
typedef struct EtcPalTimer
{
  uint32_t reset_time; /*!< The time at which this timer was reset. */
  uint32_t interval;   /*!< This timer's timeout interval. */
} EtcPalTimer;

#ifdef __cplusplus
extern "C" {
#endif

/* Function with platform-specific definition */

/*!
 * \brief Get a monotonically-increasing millisecond value
 * \return The current timestamp in milliseconds.
 */
uint32_t etcpal_getms();

/* Functions with platform-neutral definitions */

void etcpal_timer_start(EtcPalTimer* timer, uint32_t interval);
void etcpal_timer_reset(EtcPalTimer* timer);
uint32_t etcpal_timer_elapsed(const EtcPalTimer* timer);
bool etcpal_timer_is_expired(const EtcPalTimer* timer);
uint32_t etcpal_timer_remaining(const EtcPalTimer* timer);

#ifdef __cplusplus
}
#endif

/*!
 * @}
 */

#endif /* ETCPAL_TIMER_H_ */
