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

#ifndef ETCPAL_PRIVATE_OPTS_H_
#define ETCPAL_PRIVATE_OPTS_H_

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
 * @}
 */

#endif /* ETCPAL_PRIVATE_OPTS_H_ */
