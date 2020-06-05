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

/* etcpal/common.h: Common definitions for EtcPal modules. */

#ifndef ETCPAL_COMMON_H_
#define ETCPAL_COMMON_H_

#include <stdint.h>
#include "etcpal/error.h"

/**
 * @defgroup etcpal etcpal (Core Modules)
 * @brief ETC Platform Abstraction Layer (EtcPal): A set of platform abstraction and utility
 *        modules used by ETC software libraries.
 *
 * EtcPal supports the writing of platform-neutral C and C++ libraries by providing a set of
 * modules to abstract common system calls. There are also a few platform-neutral utilities
 * (e.g. data structures, logging) thrown in here and there for convenience. EtcPal headers can be
 * either platform-neutral (contained in include/) or platform-specific (contained in
 * include/os/[platform]). All platform-specific headers of the same EtcPal module will conform to
 * an identical interface.
 *
 * Some EtcPal modules must be initialized before use. These modules have a prominent notice in
 * the "detailed description" portion of their documentation. To initialize the EtcPal library,
 * call etcpal_init() with a mask of your desired features. The feature masks can be combined with
 * a bitwise OR.
 *
 * @code
 * #include "etcpal/common.h"
 *
 * etcpal_error_t result = etcpal_init(ETCPAL_FEATURE_SOCKETS | ETCPAL_FEATURE_LOGGING);
 * @endcode
 *
 * etcpal_init() can be called multiple times for the same or different sets of features, with no
 * adverse effects. At deinitialization time, there should be a paired call to etcpal_deinit() with
 * the same mask that was passed to the corresponding call to etcpal_init().
 *
 * @code
 * etcpal_deinit(ETCPAL_FEATURE_SOCKETS | ETCPAL_FEATURE_LOGGING);
 * @endcode
 *
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/** @cond utilities used by library implementation */

/* Suppress deprecated function warnings on Windows/MSVC. This is mostly used in situations where
 * Microsoft warns us that a function like strncpy() could be unsafe, but we want to be portable
 * and have made sure that we're using it in a safe way (e.g. by manually inserting null
 * terminators). */
#ifdef _MSC_VER

#define ETCPAL_MSVC_NO_DEP_WRN __pragma(warning(suppress : 4996))

#define ETCPAL_MSVC_BEGIN_NO_DEP_WARNINGS() __pragma(warning(push)) __pragma(warning(disable : 4996))
#define ETCPAL_MSVC_END_NO_DEP_WARNINGS() __pragma(warning(pop))

#else /* _MSC_VER */

#define ETCPAL_MSVC_NO_DEP_WRN
#define ETCPAL_MSVC_BEGIN_NO_DEP_WARNINGS()
#define ETCPAL_MSVC_END_NO_DEP_WARNINGS()

#endif /* _MSC_VER */

/* Meaningful macro to suppress warnings on unused arguments */
#define ETCPAL_UNUSED_ARG(arg) ((void)arg)

/** @endcond */

/** For etcpal_ functions that take a millisecond timeout, this means to wait indefinitely. */
#define ETCPAL_WAIT_FOREVER -1

/** A mask of desired EtcPal features. See "EtcPal feature masks". */
typedef uint32_t etcpal_features_t;

/** @cond feature_offset_values */

#define ETCPAL_FEATURE_SOCKETS_OFFSET 0
#define ETCPAL_FEATURE_NETINTS_OFFSET 1
#define ETCPAL_FEATURE_TIMERS_OFFSET 2
#define ETCPAL_FEATURE_LOGGING_OFFSET 3
#define ETCPAL_NUM_FEATURES 4

/** @endcond */

/**
 * @name EtcPal feature masks
 *
 * Pass one or more of these to etcpal_init() to initialize the relevant EtcPal feature. Multiple
 * features can be requested using logical OR.
 *
 * EtcPal modules not represented here require no initialization and are enabled by default.
 *
 * @{
 */

#define ETCPAL_FEATURE_SOCKETS \
  ((etcpal_features_t)(1u << ETCPAL_FEATURE_SOCKETS_OFFSET)) /**< Use the etcpal/socket module. */
#define ETCPAL_FEATURE_NETINTS \
  ((etcpal_features_t)(1u << ETCPAL_FEATURE_NETINTS_OFFSET)) /**< Use the etcpal/netint module. */
#define ETCPAL_FEATURE_TIMERS \
  ((etcpal_features_t)(1u << ETCPAL_FEATURE_TIMERS_OFFSET)) /**< Use the etcpal/timer module. */
#define ETCPAL_FEATURE_LOGGING \
  ((etcpal_features_t)(1u << ETCPAL_FEATURE_LOGGING_OFFSET)) /**< Use the etcpal/log module. */
#define ETCPAL_FEATURES_ALL 0xffffffffu                      /**< Use every available module. */

/**
 * @brief Use every available module except the ones passed in mask.
 * @param mask Mask of ETCPAL_FEATURE_* macros to not include in the feature mask.
 * @return Resulting EtcPal feature mask to pass to etcpal_init().
 */
#define ETCPAL_FEATURES_ALL_BUT(mask) (((uint32_t)ETCPAL_FEATURES_ALL) & ((uint32_t)(~((uint32_t)(mask)))))

/**
 * @}
 */

etcpal_error_t etcpal_init(etcpal_features_t features);
void           etcpal_deinit(etcpal_features_t features);

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif /* ETCPAL_COMMON_H_ */
