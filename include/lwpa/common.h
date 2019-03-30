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

/*!@}*/

#endif /* _LWPA_COMMON_H_ */
