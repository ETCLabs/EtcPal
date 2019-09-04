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

#include "etcpal/timer.h"

/*************************** Function definitions ****************************/

/*! \brief Start a timer.
 *  \param timer Pointer to the LwpaTimer to start.
 *  \param interval Timer interval in milliseconds. An interval of 0 will result in a timer that is
 *                  always expired.
 */
void lwpa_timer_start(LwpaTimer* timer, uint32_t interval)
{
  if (timer)
  {
    timer->reset_time = lwpa_getms();
    timer->interval = interval;
  }
}

/*! \brief Reset a timer while keeping the same interval.
 *  \param timer Pointer to the LwpaTimer to reset.
 */
void lwpa_timer_reset(LwpaTimer* timer)
{
  if (timer)
  {
    timer->reset_time = lwpa_getms();
  }
}

/*! \brief Get the time since a timer was reset.
 *  \param timer Pointer to the LwpaTimer of which to get the elapsed time.
 *  \return Number of milliseconds since the timer was reset.
 */
uint32_t lwpa_timer_elapsed(const LwpaTimer* timer)
{
  if (timer)
  {
    return lwpa_getms() - timer->reset_time;
  }
  return 0;
}

/*! \brief Check to see if a timer is expired.
 *  \param timer Pointer to the LwpaTimer of which to check the expiration.
 *  \return true: More than \link LwpaTimer::interval interval \endlink milliseconds have passed
 *          since the timer was started/reset.
 *  \return false: Less than or equal to \link LwpaTimer::interval interval \endlink milliseconds
 *          have passed since the timer was started/reset)
 */
bool lwpa_timer_is_expired(const LwpaTimer* timer)
{
  if (timer)
  {
    return ((timer->interval == 0) || ((lwpa_getms() - timer->reset_time) > timer->interval));
  }
  return true;
}

/*! \brief Get the amount of time remaining in a timer.
 *  \param timer Pointer to the LwpaTimer of which to get the remaining time.
 *  \return Remaining time in milliseconds or 0 (timer is expired).
 */
uint32_t lwpa_timer_remaining(const LwpaTimer* timer)
{
  uint32_t res = 0;
  if (timer->interval != 0)
  {
    uint32_t cur_ms = lwpa_getms();
    if (cur_ms - timer->reset_time < timer->interval)
      res = timer->reset_time + timer->interval - cur_ms;
  }
  return res;
}
