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

/* etcpal/bool.h: Provide a C boolean type for non-C99 environments. */

#ifndef ETCPAL_BOOL_H_
#define ETCPAL_BOOL_H_

/*!
 * \defgroup etcpal_bool C Boolean Type (bool)
 * \ingroup etcpal
 * \brief Compatibility layer for the C boolean type.
 *
 * ```c
 * #include "etcpal/bool.h"
 * ```
 *
 * In the before-times, C had no boolean type. Then, C99 gave us stdbool.h. But there still exist
 * environments where a C99 compiler is not available or practical, and one allegedly modern
 * toolchain (it rhymes with "Nicrosoft Fishual D++") picks and chooses the modern C features it
 * supports seemingly at random.
 *
 * This header attempts to detect if it is being compiled with C99 or later (or C++). If it is
 * being compiled with C99 or later it simply includes stdbool.h. If it is being compiled with C++
 * it does nothing (bool is a native type in C++). If none of the above are true, it creates a
 * boolean type the old-fashioned way: \#defines.
 *
 * @{
 */

#if (!defined(__cplusplus)) || defined(DOXYGEN)

#if ((defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L) || (defined(_MSC_VER) && _MSC_VER >= 1800) || \
     HAVE_STDBOOL_H)
/* We are being compiled with C99 or later, Visual Studio 2013 or later, or the user has defined
 * HAVE_STDBOOL_H to 1 to force inclusion of stdbool.h */
#include <stdbool.h>
#else /* __STDC_VERSION__ */

/*! Char is used for the fake boolean type as it is guaranteed to be the smallest available type. */
#ifndef bool
#define bool char
#endif

/*! False is 0. Not much to see here. */
#ifndef false
#define false 0
#endif

/*! True is 1. Not much to see here. */
#ifndef true
#define true 1
#endif

#endif /* __STDC_VERSION__ */

#endif /* __cplusplus || DOXYGEN */

/*!
 * @}
 */

#endif /* ETCPAL_BOOL_H_ */
