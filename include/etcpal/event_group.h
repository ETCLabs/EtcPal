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

#ifndef ETCPAL_EVENT_GROUP_H_
#define ETCPAL_EVENT_GROUP_H_

/**
 * @addtogroup etcpal_event_group
 * @{
 */

/**
 * @name Flags for etcpal_event_group_wait() and etcpal_event_group_timed_wait().
 * @{
 */

/**
 * @brief Clear bits automatically when they are returned from a wait function.
 *
 * If this flag is not specified, bits will remain set in an event group after the wait function
 * returns. They can be cleared by waiting again with this flag specified, or explicitly cleared
 * with etcpal_event_group_clear_bits().
 */
#define ETCPAL_EVENT_GROUP_AUTO_CLEAR 0x1

/**
 * @brief Wait for all specified bits.
 *
 * Do not return from etcpal_event_group_wait() until all bits specified in the bits argument are
 * received. Do not return from etcpal_event_group_timed_wait() until either all bits specified in
 * the bits argument are received or the timeout occurs.
 *
 * If this flag was not specified for an event group, etcpal_event_group_wait() and
 * etcpal_event_group_timed_wait() will return when at least one bit in the bit mask is matched.
 */
#define ETCPAL_EVENT_GROUP_WAIT_FOR_ALL 0x2

/**
 * @}
 */

/**
 * @}
 */

#include "etcpal/os_event_group.h"

#endif /* ETCPAL_EVENT_GROUP_H_ */
