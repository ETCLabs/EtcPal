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

/* lwpa/common.h: Common definitions for Lightweight Platform Abstraction modules. */
#ifndef _LWPA_COMMON_H_
#define _LWPA_COMMON_H_

#include "lwpa/int.h"
#include "lwpa/error.h"

/*! \defgroup lwpa lwpa
 *  \brief Lightweight Platform Abstraction (lwpa): A set of platform abstraction and utility
 *         modules.
 *
 *  Lightweight Platform Abstraction (lwpa) supports the writing of platform-neutral C and C++
 *  libraries by providing a set of modules to abstract common system calls. There are also a few
 *  platform-neutral utilities (e.g. data structures, logging) thrown in here and there for
 *  convenience. lwpa headers can be either platform-neutral (contained in include/) or
 *  platform-specific (contained in include/[platform]). All platform-specific headers of the same
 *  lwpa module will conform to an identical interface.
 *
 * @{
 */

/*! For lwpa_ functions that take a millisecond timeout, this means to wait indefinitely. */
#define LWPA_WAIT_FOREVER -1

typedef uint32_t lwpa_features_t;

#define LWPA_FEATURE_SOCKETS_OFFSET 0
#define LWPA_FEATURE_NETINTS_OFFSET 1
#define LWPA_FEATURE_TIMERS_OFFSET  2
#define LWPA_FEATURE_LOGGING_OFFSET 3
#define LWPA_NUM_FEATURES 4

/*! \name lwpa feature masks
 *  Pass one or more of these to lwpa_init() to initialize the relevant lwpa feature.
 *  lwpa modules not represented here require no initialization and are enabled by default.
 *  @{
 */
#define LWPA_FEATURE_SOCKETS ((lwpa_features_t)(1u << LWPA_FEATURE_SOCKETS_OFFSET)) /*!< Use the lwpa_socket module. */
#define LWPA_FEATURE_NETINTS ((lwpa_features_t)(1u << LWPA_FEATURE_NETINTS_OFFSET)) /*!< Use the lwpa_netint module. */
#define LWPA_FEATURE_TIMERS  ((lwpa_features_t)(1u << LWPA_FEATURE_TIMERS_OFFSET)) /*!< Use the lwpa_timer module. */
#define LWPA_FEATURE_LOGGING ((lwpa_features_t)(1u << LWPA_FEATURE_LOGGING_OFFSET)) /*!< Use the lwpa_log module. */
#define LWPA_FEATURES_ALL 0xffffffffu /*! Use every available module. */

/*! \brief Use every available module except the ones passed in mask.
 *  \param mask Mask of LWPA_FEATURE_* macros to not include in the feature mask.
 *  \return Resulting lwpa feature mask to pass to lwpa_init().
 */
#define LWPA_FEATURES_ALL_BUT(mask) (((uint32_t)LWPA_FEATURES_ALL) & ((uint32_t)(~((uint32_t)(mask)))))
/*! @} */


#ifdef __cplusplus
extern "C" {
#endif

lwpa_error_t lwpa_init(lwpa_features_t features);
void lwpa_deinit(lwpa_features_t features);

#ifdef __cplusplus
}
#endif

/*!@}*/

#endif /* _LWPA_COMMON_H_ */
