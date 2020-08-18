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

/*
 * A platform-neutral example application showing usage of the etcpal_timer functions.
 */

#include <stdio.h>
#include "etcpal/common.h"
#include "etcpal/thread.h"
#include "etcpal/timer.h"

int main(void)
{
  // The timer module requires explicit initialization
  etcpal_error_t res = etcpal_init(ETCPAL_FEATURE_TIMERS);
  if (res != kEtcPalErrOk)
  {
    printf("Couldn't initialize EtcPal: '%s'\n", etcpal_strerror(res));
    return 1;
  }

  EtcPalTimer timer;
  // Do some busy work until a 1-second timer is expired
  unsigned long i = 0;
  printf("Seeing how many times we can increment an integer in one second...\n");
  etcpal_timer_start(&timer, 1000);
  while (!etcpal_timer_is_expired(&timer))
  {
    ++i;
  }
  printf("Wow, we got up to %lu!\n", i);

  // Sleep for some time, then see how long we actually slept for
  printf("Attempting to sleep for 1 second...\n");
  etcpal_timer_reset(&timer);  // Resets the timer for the same interval
  etcpal_thread_sleep(1000);
  printf("We actually slept for %u milliseconds, which is %s what we asked for.\n", etcpal_timer_elapsed(&timer),
         etcpal_timer_is_expired(&timer) ? "greater than" : "less than or equal to");

  // Must always deinit with the same feature macro combination that was passed to init().
  etcpal_deinit(ETCPAL_FEATURE_TIMERS);
  return 0;
}
