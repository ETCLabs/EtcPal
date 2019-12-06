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

/* etcpal/common.h: Common definitions for EtcPal modules. */

#ifndef ETCPAL_COMMON_H_
#define ETCPAL_COMMON_H_

#include "etcpal/int.h"
#include "etcpal/error.h"

/*!
 * \defgroup etcpal EtcPal
 * \brief ETC Platform Abstraction Layer (EtcPal): A set of platform abstraction and utility
 *        modules used by ETC software libraries.
 *
 * EtcPal supports the writing of platform-neutral C and C++ libraries by providing a set of
 * modules to abstract common system calls. There are also a few platform-neutral utilities
 * (e.g. data structures, logging) thrown in here and there for convenience. EtcPal headers can be
 * either platform-neutral (contained in include/) or platform-specific (contained in
 * include/os/[platform]). All platform-specific headers of the same EtcPal module will conform to
 * an identical interface.
 *
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/*! For etcpal_ functions that take a millisecond timeout, this means to wait indefinitely. */
#define ETCPAL_WAIT_FOREVER -1

/*! A mask of desired EtcPal features. See "EtcPal feature masks". */
typedef uint32_t etcpal_features_t;

/*! \cond feature_offset_values */

#define ETCPAL_FEATURE_SOCKETS_OFFSET 0
#define ETCPAL_FEATURE_NETINTS_OFFSET 1
#define ETCPAL_FEATURE_TIMERS_OFFSET 2
#define ETCPAL_FEATURE_LOGGING_OFFSET 3
#define ETCPAL_NUM_FEATURES 4

/*! \endcond */

/*!
 * \name EtcPal feature masks
 *
 * Pass one or more of these to etcpal_init() to initialize the relevant EtcPal feature. Multiple
 * features can be requested using logical OR.
 *
 * EtcPal modules not represented here require no initialization and are enabled by default.
 *
 * @{
 */

#define ETCPAL_FEATURE_SOCKETS \
  ((etcpal_features_t)(1u << ETCPAL_FEATURE_SOCKETS_OFFSET)) /*!< Use the etcpal_socket module. */
#define ETCPAL_FEATURE_NETINTS \
  ((etcpal_features_t)(1u << ETCPAL_FEATURE_NETINTS_OFFSET)) /*!< Use the etcpal_netint module. */
#define ETCPAL_FEATURE_TIMERS \
  ((etcpal_features_t)(1u << ETCPAL_FEATURE_TIMERS_OFFSET)) /*!< Use the etcpal_timer module. */
#define ETCPAL_FEATURE_LOGGING \
  ((etcpal_features_t)(1u << ETCPAL_FEATURE_LOGGING_OFFSET)) /*!< Use the etcpal_log module. */
#define ETCPAL_FEATURES_ALL 0xffffffffu                      /*!< Use every available module. */

/*!
 * \brief Use every available module except the ones passed in mask.
 * \param mask Mask of ETCPAL_FEATURE_* macros to not include in the feature mask.
 * \return Resulting EtcPal feature mask to pass to etcpal_init().
 */
#define ETCPAL_FEATURES_ALL_BUT(mask) (((uint32_t)ETCPAL_FEATURES_ALL) & ((uint32_t)(~((uint32_t)(mask)))))

/*!
 * @}
 */

etcpal_error_t etcpal_init(etcpal_features_t features);
void etcpal_deinit(etcpal_features_t features);

#ifdef __cplusplus
}
#endif

/*!
 * @}
 */

#endif /* ETCPAL_COMMON_H_ */
