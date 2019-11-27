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

/* etcpal/int.h: Provide fixed-wid.h C integer types for non-C99 environments. */

#ifndef ETCPAL_INT_H_
#define ETCPAL_INT_H_

/*!
 * \defgroup etcpal_int Fixed-Width Integer Types (int)
 * \ingroup etcpal
 * \brief Compatibility layer for fixed-width integer types.
 *
 * ```c
 * #include "etcpal/int.h"
 * ```
 *
 * In the before-times, C had no fixed-width integer types. Then, C99 gave us stdint.h. But there
 * still exist environments where a C99 compiler is not available or practical, and one allegedly
 * modern toolchain (it rhymes with "Nicrosoft Fishual D++") picks and chooses the modern C
 * features it supports seemingly at random.
 *
 * This header attempts to detect if it is being compiled with C99 or later (or C++). If so, it
 * simply includes stdint.h (or cstdint). If not, it attempts to include a user-created file
 * "stdint_usr.h" which the user provides.
 *
 * @{
 */

/*!
 * \def HAVE_STDINT_H
 * \brief Define this value in your compiler settings if your toolchain provides a <stdint.h>.
 *
 * Some compilers are non-C99 (or non-C++11) and nevertheless provide fixed-width types in
 * <stdint.h>. In that situation, the user can define HAVE_STDINT_H to force etcpal_int to include
 * <stdint.h>.
 */
#if DOXYGEN
#define HAVE_STDINT_H 1
#endif

#ifdef __cplusplus

#if ((__cplusplus >= 201103L) || (defined(_MSC_VER) && _MSC_VER >= 1600) || HAVE_STDINT_H)
/* We are being compiled with C++11 or later, Visual Studio 2010 or later, or the user has defined
 * HAVE_STDINT_H to 1 to force the inclusion of cstdint */
#include <cstdint>
#else
/* A user-provided integer define file which should define the same things typically guaranteed to
 * be in stdint.h */
#include "stdint_usr.h"
#endif

#else /* __cplusplus */

#if ((defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L) || (defined(_MSC_VER) && _MSC_VER >= 1600) || \
     HAVE_STDINT_H)
/* We are being compiled with C99 or later, Visual Studio 2010 or later, or the user has defined
 * HAVE_STDINT_H to 1 to force inclusion of stdint.h */
#include <stdint.h>
#else
/* A user-provided integer define file which should define the same things typically guaranteed to
 * be in stdint.h */
#include "stdint_usr.h"
#endif

#endif /* __cplusplus */

/*!
 * @}
 */

#endif /* ETCPAL_INT_H_ */
