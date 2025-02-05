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
#include <utility>

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

/// @brief Implementation of `std::void_t`.
template <typename...>
using void_t = void;

/// @brief Tag type to indicate a wrapped type should be in-place constructed.
struct InPlace_t
{
};
/// @brief Tag value to indicate a wrapped type should be in-place constructed.
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

template <typename T, typename U, typename Obj, typename... Args>
decltype(auto) invoke_member_function(T U::* f, Obj&& obj, Args&&... args) noexcept(
    noexcept(((*std::forward<Obj>(obj)).*f)(std::forward<Args>(args)...)))
{
  return ((*std::forward<Obj>(obj)).*f)(std::forward<Args>(args)...);
}

template <typename T, typename... Args>
struct IsCallableImpl<T,
                      std::enable_if_t<std::is_member_function_pointer<T>::value,
                                       decltype(invoke_member_function(std::declval<T>(), std::declval<Args>()...))>,
                      Args...> : public std::true_type
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

template <typename R, typename T, typename... Args>
struct IsCallableImpl<
    R,
    T,
    std::enable_if_t<
        std::is_member_function_pointer<T>::value &&
        std::is_convertible<decltype(invoke_member_function(std::declval<T>(), std::declval<Args>()...)), R>::value>,
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

template <typename... T>
struct CommonCVRefType
{
};

template <typename... T>
using CommonCVRefType_t = typename CommonCVRefType<T...>::type;

template <typename T>
struct CommonCVRefType<T>
{
  using type = T;
};

template <typename T1, typename T2>
struct CommonCVRefType<T1, T2>
{
  using type = decltype(true ? std::declval<T1>() : std::declval<T2>());
};

template <typename T1, typename T2, typename T3, typename... Rest>
struct CommonCVRefType<T1, T2, T3, Rest...>
    : public CommonCVRefType<CommonCVRefType_t<T1, T2>, CommonCVRefType_t<T3, Rest...>>
{
};

}  // namespace detail
/// @endcond

/// @brief Implementation of `std::remove_cvref_t`.
/// @tparam T The type to remove reference and cv-qualifiers from.
template <typename T>
using RemoveCVRef_t = std::remove_cv_t<std::remove_reference_t<T>>;

/// @brief Convert an enumeration to its underlying type.
/// @tparam Enum The enumeration type to convert.
/// @param value The enumeration value to convert.
/// @return The underlying integral enumeration value.
template <typename Enum>
[[nodiscard]] constexpr auto to_underlying(Enum value) noexcept
{
  return static_cast<std::underlying_type_t<Enum>>(value);
}

/// @brief A scope-based finalizer.
///
/// The given action type must be invocable with no arguments, and must not throw any exceptions. Its return value is
/// always ignored.
///
/// @tparam Action The type of action to invoke on scope exit.
template <typename Action>
class [[maybe_unused]] FinalAction
{
public:
  constexpr FinalAction(Action&& action) noexcept : action_{std::move(action)}, valid_{true} {}
  constexpr FinalAction(const Action& action) noexcept(std::is_nothrow_copy_constructible<Action>::value)
      : action_{action}, valid_{true}
  {
  }

  FinalAction(const FinalAction& rhs) = delete;
  constexpr FinalAction(FinalAction&& rhs) noexcept
      : action_{std::move(rhs.action_)}, valid_{std::exchange(rhs.valid_, false)}
  {
  }

  ~FinalAction() noexcept { action_(); }

  auto operator=(const FinalAction& rhs) = delete;
  auto operator=(FinalAction&& rhs)      = delete;

private:
  Action action_;
  bool   valid_ = false;
};

#if (__cplusplus >= 201703L)
template <typename Action>
FinalAction(Action&&) -> FinalAction<RemoveCVRef_t<Action>>;
#endif  // #if (__cplusplus >= 201703L)

/// @brief Create a scope-exit finalizer that runs the given action.
/// @tparam Action The type of action to invoke on scope exit.
/// @param action The action to invoke on scope exit.
/// @return A finalizer that runs the given action on destruction.
template <typename Action>
[[nodiscard]] constexpr auto finally(Action&& action) noexcept(
    std::is_nothrow_constructible<FinalAction<RemoveCVRef_t<Action>>>::value)
{
  return FinalAction<RemoveCVRef_t<Action>>{std::forward<Action>(action)};
}

}  // namespace etcpal

#endif  // ETCPAL_CPP_COMMON_H_
