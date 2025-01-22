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

/// @file etcpal/cpp/common.h
/// @brief Common definitions used by EtcPal C++ wrappers.

#ifndef ETCPAL_CPP_COMMON_H_
#define ETCPAL_CPP_COMMON_H_

#include <memory>

#if (__cplusplus >= 201703)
#include <memory_resource>
#endif  // #if (__cplusplus >= 201703)

/// @defgroup etcpal_cpp C++ Wrappers
/// @ingroup etcpal
/// @brief C++ Wrappers for EtcPal modules.
///
/// EtcPal is primarily a C-language library, but C++ language wrappers for certain modules are
/// provided for convenience when writing C++ code that uses EtcPal. To include a C++ wrapper for a
/// given module, where you would normally include `"etcpal/[module].h"`, instead include
/// `"etcpal/cpp/[module].h"`.
///
/// Some C++ wrapper modules contain types that directly wrap a type defined in a core EtcPal
/// module. These types follow a C++ namespace convention which matches the C-style namespacing
/// of the items they are wrapping. For example, the struct EtcPalIpAddr has a C++ wrapper class
/// called etcpal::IpAddr. It takes up the same space in memory as an EtcPalIpAddr and can be
/// compared to and converted implicitly from EtcPalIpAddr. Explicit conversions can be made in the
/// opposite direction using etcpal::IpAddr::get().
///
/// @{

/// @def ETCPAL_CONSTEXPR_14
/// @brief Stand-in for "constexpr" on entities that can only be defined "constexpr" in C++14 or later.
/// @def ETCPAL_CONSTEXPR_14_OR_INLINE
/// @brief Defined to "constexpr" in C++14 or later, "inline" earlier.

#if ((defined(_MSC_VER) && (_MSC_VER > 1900)) || (__cplusplus >= 201402L))
#define ETCPAL_CONSTEXPR_14           constexpr
#define ETCPAL_CONSTEXPR_14_OR_INLINE constexpr
#else
#define ETCPAL_CONSTEXPR_14
#define ETCPAL_CONSTEXPR_14_OR_INLINE inline
#endif

#if (__cplusplus >= 201703L)
#define ETCPAL_INLINE_VARIABLE inline
#else
#define ETCPAL_INLINE_VARIABLE static
#endif

/// @def ETCPAL_NO_EXCEPTIONS
/// @brief Explicitly removes all "throw" statements from EtcPal C++ headers.
///
/// The EtcPal headers also attempt to organically detect whether exceptions are enabled using
/// various standard defines. This definition is available to explicitly disable exceptions if
/// that method does not work.
#if DOXYGEN
#define ETCPAL_NO_EXCEPTIONS
#endif

/// @cond Internal Exception Macros

#if (defined(__cpp_exceptions) || defined(__EXCEPTIONS) || defined(_CPPUNWIND)) && !defined(ETCPAL_NO_EXCEPTIONS)
#define ETCPAL_THROW(exception)         throw exception
#define ETCPAL_BUILDING_WITH_EXCEPTIONS 1
#else
#include <cstdlib>
#define ETCPAL_THROW(exception)         std::abort()
#define ETCPAL_BUILDING_WITH_EXCEPTIONS 0
#endif

/// @endcond

/// @}

namespace etcpal
{

using DefaultAllocator =
#if (__cplusplus >= 201703)
    std::pmr::polymorphic_allocator<std::byte>
#else   // #if (__cplusplus >= 201703)
    std::allocator<unsigned char>
#endif  // #if (__cplusplus >= 201703)
    ;

template <typename...>
using void_t = void;

struct InPlace_t
{
};

ETCPAL_INLINE_VARIABLE constexpr auto in_place = InPlace_t{};

/// @cond detail

namespace detail
{
enum class Enabler
{
};

#define ETCPAL_ENABLE_IF_ARG(...)      typename std::enable_if<(__VA_ARGS__)>::type* = nullptr
#define ETCPAL_ENABLE_IF_TEMPLATE(...) typename = typename std::enable_if<(__VA_ARGS__), detail::Enabler>::type

template <typename First, typename...>
using FirstOf = First;

template <typename F, typename, typename... Args>
struct IsCallableImpl : public std::false_type
{
};

template <typename F, typename... Args>
struct IsCallableImpl<F, void_t<decltype(std::declval<F>()(std::declval<Args>()...))>, Args...> : public std::true_type
{
};

template <typename F, typename... Args>
using IsCallable = IsCallableImpl<F, void, Args...>;

template <typename R, typename F, typename, typename... Args>
struct IsCallableRImpl : public std::false_type
{
};

template <typename R, typename F, typename... Args>
struct IsCallableRImpl<
    R,
    F,
    std::enable_if_t<std::is_convertible<decltype(std::declval<F>()(std::declval<Args>()...)), R>::value>,
    Args...> : public std::true_type
{
};

template <typename R, typename F, typename... Args>
using IsCallableR = IsCallableRImpl<R, F, void, Args...>;

template <typename F, typename, typename... Args>
struct IsNothrowCallableImpl : public std::false_type
{
};

template <typename F, typename... Args>
struct IsNothrowCallableImpl<F, std::enable_if_t<noexcept(std::declval<F>()(std::declval<Args>()...))>, Args...>
    : public std::true_type
{
};

template <typename F, typename... Args>
using IsNothrowCallable = IsNothrowCallableImpl<F, void, Args...>;

template <typename R, typename F, typename, typename... Args>
struct IsNothrowCallableRImpl : public std::false_type
{
};

template <typename R, typename F, typename... Args>
struct IsNothrowCallableImpl<R, F, std::enable_if_t<noexcept(R{std::declval<F>()(std::declval<Args>()...)})>, Args...>
    : public std::true_type
{
};

template <typename F, typename... Args>
struct IsNothrowCallableImpl<void, F, std::enable_if_t<noexcept(std::declval<F>()(std::declval<Args>()...))>, Args...>
    : public std::true_type
{
};

template <typename R, typename F, typename... Args>
using IsNothrowCallableR = IsNothrowCallableRImpl<R, F, void, Args...>;

template <typename F, typename, typename... Args>
struct CallResult
{
};

template <typename F, typename... Args>
struct CallResult<F, void_t<decltype(std::declval<F>()(std::declval<Args>()...))>, Args...>
{
  using type = decltype(std::declval<F>()(std::declval<Args>()...));
};

template <typename F, typename... Args>
using CallResult_t = typename CallResult<F, void, Args...>::type;

}  // namespace detail

/// @endcond
}  // namespace etcpal

#endif  // ETCPAL_CPP_COMMON_H_
