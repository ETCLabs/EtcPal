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

/// \file etcpal/cpp/common.h
/// \brief Common definitions used by EtcPal C++ wrappers.

#ifndef ETCPAL_CPP_COMMON_H_
#define ETCPAL_CPP_COMMON_H_

/// \defgroup etcpal_cpp EtcPal C++ Wrappers
/// \brief C++ Wrappers for EtcPal modules.
///
/// EtcPal is primarly a C-language library, but C++ language wrappers for certain modules are
/// provided for convenience when writing C++ code that uses EtcPal. To include a C++ wrapper for a
/// given module, where you would normally include "etcpal/[module].h", instead include
/// "etcpal/cpp/[module].h".
/// 
/// @{

/// \def ETCPAL_CONSTEXPR_14
/// \brief Stand-in for "constexpr" on entities that can only be defined "constexpr" in C++14 or later.
/// \def ETCPAL_CONSTEXPR_14_OR_INLINE
/// \brief Defined to "constexpr" in C++14 or later, "inline" earlier.

#if ((defined(_MSC_VER) && (_MSC_VER > 1900)) || (__cplusplus >= 201402L))
#define ETCPAL_CONSTEXPR_14 constexpr
#define ETCPAL_CONSTEXPR_14_OR_INLINE constexpr
#else
#define ETCPAL_CONSTEXPR_14
#define ETCPAL_CONSTEXPR_14_OR_INLINE inline
#endif

/// @}

#endif // ETCPAL_CPP_COMMON_H_
