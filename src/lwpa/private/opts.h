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

#ifndef _LWPA_PRIVATE_OPTS_H_
#define _LWPA_PRIVATE_OPTS_H_

#if LWPA_HAVE_CONFIG_H
#include "lwpa_config.h"
#endif

/*! \defgroup lwpa_opts lwpa config options
 *  \ingroup lwpa
 *  \brief Compile-time configuration options for lwpa
 *
 *  Options starting with "LWPA_EMBOS_" (currently all of them) are ignored on Windows, macOS and
 *  Linux.
 *
 *  To override any of these options, provide a file called lwpa_config.h and:
 *    * If not building lwpa with CMake: add LWPA_HAVE_CONFIG_H to your preprocessor definitions,
 *      and add the path to lwpa_config.h as an include path
 *    * If building with CMake: use the CMake option LWPA_CONFIG_FILE_LOC
 *
 *  In the lwpa_config.h file, simply #define any overridden options to the desired value.
 *
 *  @{
 */

/*! \brief Whether a malloc() implementation is available.
 *
 *  Currently this only affects network interface enumeration using lwIP.
 */
#ifndef LWPA_EMBOS_USE_MALLOC
#define LWPA_EMBOS_USE_MALLOC 0
#endif

/*! \brief The maximum number of network interfaces that can be tracked by the \ref lwpa_netint module.
 *
 *  Currently this limit is not necessary and therefore ignored on all targets except lwIP.
 */
#ifndef LWPA_EMBOS_MAX_NETINTS
#define LWPA_EMBOS_MAX_NETINTS 5
#endif

/*! @} */

#endif /* _LWPA_PRIVATE_OPTS_H_ */
