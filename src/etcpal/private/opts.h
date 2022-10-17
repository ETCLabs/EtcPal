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

#ifndef ETCPAL_PRIVATE_OPTS_H_
#define ETCPAL_PRIVATE_OPTS_H_

#include "stdbool.h"

#if ETCPAL_HAVE_CONFIG_H
#include "etcpal_config.h"
#endif

/**
 * @defgroup etcpal_opts EtcPal config options
 * @ingroup etcpal
 * @brief Compile-time configuration options for EtcPal
 *
 * Options starting with "ETCPAL_EMBOS_" are ignored on Windows, macOS and Linux.
 *
 * To override any of these options, either define them on your compiler command line, or provide a
 * file called etcpal_config.h and:
 *   * If not building EtcPal with CMake: add ETCPAL_HAVE_CONFIG_H to your preprocessor
 *     definitions, and add the path to etcpal_config.h as an include path
 *   * If building with CMake: use the CMake option ETCPAL_CONFIG_FILE_LOC
 *
 * In the etcpal_config.h file, simply \#define any overridden options to the desired value.
 *
 * @{
 */

/**
 * @brief Whether to build the etcpal/pack64.h functions.
 *
 * This can be defined to 0 if you are on a platform that does not provide a int64_t/uint64_t type.
 */
#ifndef ETCPAL_INCLUDE_PACK_64
#define ETCPAL_INCLUDE_PACK_64 1
#endif

/**
 * @brief Whether a malloc() implementation is available.
 *
 * Currently this only affects network interface enumeration using lwIP.
 */
#ifndef ETCPAL_EMBOS_USE_MALLOC
#define ETCPAL_EMBOS_USE_MALLOC 0
#endif

/**
 * @brief The maximum number of network interfaces that can be tracked by the @ref etcpal_netint module.
 *
 * Currently this limit is not necessary and therefore ignored on all targets except lwIP.
 */
#ifndef ETCPAL_EMBOS_MAX_NETINTS
#define ETCPAL_EMBOS_MAX_NETINTS 5
#endif

/**
 * @brief Enable message logging from the EtcPal library.
 *
 * If defined nonzero, the etcpal_init_log_handler() function can be used to register the application for logging
 * messages from EtcPal. These messages typically indicate critical assertion failures that may not otherwise be visible
 * in a release environment.
 *
 * Note that this only toggles logging from the EtcPal library and does not control if the logging API is enabled as a
 * feature (that is determined by the features passed to etcpal_init()).
 */
#ifndef ETCPAL_LOGGING_ENABLED
#define ETCPAL_LOGGING_ENABLED 1
#endif

/**
 * @brief A string which will be prepended to all log messages from the EtcPal library.
 */
#ifndef ETCPAL_LOG_MSG_PREFIX
#define ETCPAL_LOG_MSG_PREFIX "EtcPal: "
#endif

/* Assertion failure handler */
bool etcpal_assert_verify_fail(const char* exp, const char* file, const char* func, const int line);

/**
 * @brief The assertion handler used by the EtcPal library.
 *
 * By default, evaluates to true on success, or false on failure (additionally asserting and logging). If redefining
 * this, it must be redefined as a macro taking a single argument (the assertion expression).
 */
#ifndef ETCPAL_ASSERT_VERIFY
#define ETCPAL_ASSERT_VERIFY(exp) ((exp) ? true : etcpal_assert_verify_fail(#exp, __FILE__, __func__, __LINE__))
#endif

/**
 * @brief The lower-level debug assert used by the EtcPal library.
 *
 * This is the assertion that gets called by #ETCPAL_ASSERT_VERIFY on failure. Redefine this to retain the logging done
 * by the default #ETCPAL_ASSERT_VERIFY macro.
 *
 * By default, just uses the C library assert. If redefining this, it must be redefined as a macro taking a single
 * argument (the assertion expression).
 */
#ifndef ETCPAL_ASSERT
#include <assert.h>
#define ETCPAL_ASSERT(expr) assert(expr)
#endif

/**
 * @}
 */

#endif /* ETCPAL_PRIVATE_OPTS_H_ */
