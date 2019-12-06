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

/// \file etcpal/cpp/error.h
/// \brief C++ wrapper and utilities for etcpal/error.h

#ifndef ETCPAL_CPP_ERROR_H_
#define ETCPAL_CPP_ERROR_H_

#include <cassert>
#include <stdexcept>
#include <string>
#include "etcpal/error.h"
#include "etcpal/cpp/common.h"

namespace etcpal
{
/// \defgroup etcpal_cpp_error Error Handling (error)
/// \ingroup etcpal_cpp
/// \brief C++ utilities for the \ref etcpal_error module.

////////////////////////////////////////////////////////////////////////////////////////////////////
// etcpal::Result
////////////////////////////////////////////////////////////////////////////////////////////////////

/// \brief A wrapper class for the EtcPal error type.
/// \ingroup etcpal_cpp_error
/// \details
/// Provides C++ syntactic sugar on top of #etcpal_error_t codes. A Result can be created from an
/// #etcpal_error_t, and contains the code and a string representation. If the Result contains
/// #kEtcPalErrOk, it will evaluate to true in expressions (and IsOk() will return true).
/// Otherwise, the Result evaluates to false.
///
/// The human-readable string associated with the Result can be retrieved with ToString() and
/// ToCString(); this will be the same string available from etcpal_strerror() for the given error
/// code.
///
/// Example with an "OK" error code:
/// \code
/// auto result = etcpal::Result(kEtcPalErrOk); // or etcpal::Result::Ok()
/// if (result)
/// {
///    // This block will be entered...
/// }
/// \endcode
///
/// Example with a non-"OK" error code:
/// \code
/// auto result = etcpal::Result(kEtcPalErrNoMem);
/// if (result)
/// {
///   // This block will not be entered
/// }
/// else
/// {
///   // This block will be entered
///   printf("Error: '%s'", result.ToCString());
///   // Or
///   std::cout << "Error: '" << result.ToString() << "'\n";
///   // Prints "Error: 'Out of memory'"
/// }
/// \endcode
///
/// Result is often used as the return type for functions; it has an implicit constructor from
/// #etcpal_error_t to simplify return statements:
/// \code
/// etcpal::Result DoSomething()
/// {
///   // ... After we did something successfully...
///   return kEtcPalErrOk;
/// }
/// \endcode
class Result
{
public:
  Result() = delete;
  // Note: this constructor is not explicit by design, to allow implicit conversions e.g.
  //   etcpal::Result FunctionReturningResult()
  //   {
  //     return kEtcPalErrOk;
  //   }
  constexpr Result(etcpal_error_t code) noexcept;
  Result& operator=(etcpal_error_t code) noexcept;

  constexpr bool IsOk() const noexcept;
  constexpr etcpal_error_t code() const noexcept;
  std::string ToString() const;
  const char* ToCString() const noexcept;

  constexpr explicit operator bool() const noexcept;

  static constexpr Result Ok() noexcept;

private:
  etcpal_error_t code_{kEtcPalErrOk};
};

/// \brief Construct a Result from an error code.
constexpr Result::Result(etcpal_error_t code) noexcept : code_(code)
{
}

/// \brief Assign an error code to this Result.
inline Result& Result::operator=(etcpal_error_t code) noexcept
{
  code_ = code;
  return *this;
}

/// \brief Whether this Result contains the code #kEtcPalErrOk.
/// \details #kEtcPalErrOk is the only #etcpal_error_t code that does not indicate an error.
/// \return Whether the result is OK (not an error).
constexpr bool Result::IsOk() const noexcept
{
  return code_ == kEtcPalErrOk;
}

/// \brief Get the underlying code from a Result.
constexpr etcpal_error_t Result::code() const noexcept
{
  return code_;
}

/// \brief Get a descriptive string for this Result as a std::string.
inline std::string Result::ToString() const
{
  return etcpal_strerror(code_);
}

/// \brief Get a descriptive string for this Result as a C string.
inline const char* Result::ToCString() const noexcept
{
  return etcpal_strerror(code_);
}

/// \brief Evaluate the Result inline - evaluates true if the result is #kEtcPalErrOk, false
///        otherwise.
constexpr Result::operator bool() const noexcept
{
  return IsOk();
}

/// \brief Construct a result containing #kEtcPalErrOk.
constexpr Result Result::Ok() noexcept
{
  return Result(kEtcPalErrOk);
}

/// \addtogroup etcpal_cpp_error
/// @{

/// \name Result Relational Operators
/// @{

constexpr bool operator==(etcpal_error_t code, const Result& result)
{
  return code == result.code();
}

constexpr bool operator!=(etcpal_error_t code, const Result& result)
{
  return !(code == result);
}

constexpr bool operator==(const Result& result, etcpal_error_t code)
{
  return result.code() == code;
}

constexpr bool operator!=(const Result& result, etcpal_error_t code)
{
  return !(result == code);
}

constexpr bool operator==(const Result& a, const Result& b)
{
  return a.code() == b.code();
}

constexpr bool operator!=(const Result& a, const Result& b)
{
  return !(a == b);
}

/// @}
/// @}

////////////////////////////////////////////////////////////////////////////////////////////////////
// etcpal::BadExpectedAccess
////////////////////////////////////////////////////////////////////////////////////////////////////

/// \brief Exception representing bad access to an Expected instance.
/// \ingroup etcpal_cpp_error
/// \details
/// Thrown when attempting to access Expected<T>::value() when Expected<T>::has_value() is false.
/// Holds an etcpal::Result; this allows nicer access to error information than just having the
/// #etcpal_error_t code.
class BadExpectedAccess : public std::logic_error
{
public:
  explicit BadExpectedAccess(Result res);
  Result result() const noexcept;

private:
  Result res_;
};

/// \brief Construct from a Result.
/// \throw May throw std::bad_alloc.
inline BadExpectedAccess::BadExpectedAccess(Result res)
    : std::logic_error("Bad access to etcpal::Expected::value(); the Expected instance contained error '" +
                       res.ToString() + "'.")
    , res_(res)
{
}

/// \brief Get the error code which was contained in the associated Expected when the exception occurred.
inline Result BadExpectedAccess::result() const noexcept
{
  return res_;
}

/// \cond detail_expected

////////////////////////////////////////////////////////////////////////////////////////////////////
// etcpal::Expected internals
////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
class Expected;

namespace detail
{
// This method of implementing Expected's storage, with copy and move functionality, was borrowed
// with love from a generic header-only expected implementation by martinmoene:
// https://github.com/martinmoene/expected-lite

template <typename T>
class ExpectedStorageImpl
{
public:
  using ValueType = T;

  // Do not construct or destruct members on construction or destruction
  ExpectedStorageImpl() {}
  ~ExpectedStorageImpl() {}
  explicit ExpectedStorageImpl(bool has_value) : has_value_(has_value) {}

  void ConstructValue(const ValueType& v) { new (&value_) ValueType(v); }
  void ConstructValue(ValueType&& v) { new (&value_) ValueType(std::move(v)); }
  void SetError(etcpal_error_t e) { error_ = e; }

  void DestructValue() { value_.~ValueType(); }

  constexpr const ValueType& value() const& { return value_; }
  ValueType& value() & { return value_; }
  constexpr const ValueType&& value() const&& { return std::move(value_); }
  ETCPAL_CONSTEXPR_14 ValueType&& value() && { return std::move(value_); }
  const ValueType* value_ptr() const { return &value_; }
  ValueType* value_ptr() { return &value_; }

  etcpal_error_t error() const { return error_; }

  bool has_value() const { return has_value_; }
  void SetHasValue(bool v) { has_value_ = v; }

private:
  bool has_value_{false};
  union
  {
    ValueType value_;
    etcpal_error_t error_;
  };
};

// The ExpectedStorage class provides a way to conditionally delete the copy and/or move
// constructors of etcpal::Expected based on the copyability and moveability of the T that we are
// being specialized with.
//
// We provide a generic template where the copy and move constructors are both deleted, then 3
// specializations, for (copyable and movable), (copyable and not moveable), and (moveable and not
// copyable) respectively.
template <typename T, bool IsCopyConstructible, bool IsMoveConstructible>
class ExpectedStorage : public ExpectedStorageImpl<T>
{
public:
  ExpectedStorage() = default;
  ~ExpectedStorage() = default;

  explicit ExpectedStorage(bool has_value) : ExpectedStorageImpl<T>(has_value) {}

  ExpectedStorage(const ExpectedStorage& other) = delete;
  ExpectedStorage(ExpectedStorage&& other) = delete;
};

template <typename T>
class ExpectedStorage<T, true, true> : public ExpectedStorageImpl<T>
{
public:
  ExpectedStorage() = default;
  ~ExpectedStorage() = default;

  explicit ExpectedStorage(bool has_value) : ExpectedStorageImpl<T>(has_value) {}

  ExpectedStorage(const ExpectedStorage& other) : ExpectedStorageImpl<T>(other.has_value())
  {
    if (this->has_value())
      this->ConstructValue(other.value());
    else
      this->SetError(other.error());
  }

  ExpectedStorage(ExpectedStorage&& other) : ExpectedStorageImpl<T>(other.has_value())
  {
    if (this->has_value())
      this->ConstructValue(std::move(other.value()));
    else
      this->SetError(other.error());
  }
};

template <typename T>
class ExpectedStorage<T, true, false> : public ExpectedStorageImpl<T>
{
public:
  ExpectedStorage() = default;
  ~ExpectedStorage() = default;

  explicit ExpectedStorage(bool has_value) : ExpectedStorageImpl<T>(has_value) {}

  ExpectedStorage(const ExpectedStorage& other) : ExpectedStorageImpl<T>(other.has_value())
  {
    if (this->has_value())
      this->ConstructValue(other.value());
    else
      this->SetError(other.error());
  }

  ExpectedStorage(ExpectedStorage&& other) = delete;
};

template <typename T>
class ExpectedStorage<T, false, true> : public ExpectedStorageImpl<T>
{
public:
  ExpectedStorage() = default;
  ~ExpectedStorage() = default;

  explicit ExpectedStorage(bool has_value) : ExpectedStorageImpl<T>(has_value) {}

  ExpectedStorage(ExpectedStorage const& other) = delete;

  ExpectedStorage(ExpectedStorage&& other) : ExpectedStorageImpl<T>(other.has_value())
  {
    if (this->has_value())
      this->ConstructValue(std::move(other.value()));
    else
      this->SetError(other.error());
  }
};

enum class enabler
{
};

};  // namespace detail

#define ETCPAL_ENABLE_IF_ARG(...) typename std::enable_if<(__VA_ARGS__)>::type* = nullptr
#define ETCPAL_ENABLE_IF_TEMPLATE(...) typename = typename std::enable_if<(__VA_ARGS__), detail::enabler>::type

/// \endcond

////////////////////////////////////////////////////////////////////////////////////////////////////
// etcpal::Expected
////////////////////////////////////////////////////////////////////////////////////////////////////

/// \brief A type representing either a value or an #etcpal_error_t code.
/// \ingroup etcpal_cpp_error
/// \details
/// This class is modeled after the [std::expected proposal](http://wg21.link/p0323). It is a
/// useful companion to C++17's std::optional, when one desires a concise representation of either
/// a value or an error code representing why that value is not present.
///
/// The proposed std::expected is generic with respect to both the value and the error type, but
/// EtcPal's version always uses #etcpal_error_t as the error. This makes it useful in the context
/// of EtcPal itself and libraries that depend on EtcPal; if you are looking for a
/// generically-useful `expected` implementation, [martinmoene's version](https://github.com/martinmoene/expected-lite)
/// is recommended. This implementation is based heavily on that one.
///
/// **Usage**
///
/// Expected is almost always used as the return type of an API function. Consider a function that
/// either creates a network socket or returns an error.
///
/// \code
/// etcpal::Expected<etcpal_socket_t> CreateSocket(int family, int type);
/// \endcode
///
/// When using this function, check whether the result contains an error:
/// \code
/// auto socket_res = CreateSocket(AF_INET, SOCK_STREAM);
/// if (socket_res) // or socket_res.has_value()
/// {
///   etcpal_socket_t socket = *socket_res; // or socket_res.value()
///   // Now use the socket...
/// }
/// else
/// {
///   printf("CreateSocket failed with result code %d, description '%s'\n", socket_res.error(),
///          socket_res.result().ToCString());
/// }
/// \endcode
///
/// value() throws BadExpectedAccess if the instance does not have a valid value. So you can also
/// use an exception-handling approach:
///
/// \code
/// try
/// {
///   etcpal_socket_t socket = CreateSocket(AF_INET, SOCK_STREAM).value();
/// }
/// catch (const etcpal::BadExpectedAccess& e)
/// {
///   printf("CreateSocket failed with result code %d, description '%s'\n", e.result().code(),
///          e.result().ToCString());
/// }
/// \endcode
///
/// Note that this only works for the value() function, not any of the other accessors (they assert
/// on the correct state instead).
///
/// Inside functions that return Expected, you can use implicit conversions from the value type or
/// #etcpal_error_t to return success or failure:
///
/// \code
/// // Possible implementation of CreateSocket()...
/// etcpal::Expected<etcpal_socket_t> CreateSocket(int family, int type)
/// {
///    etcpal_socket_t socket;
///    etcpal_error_t res = etcpal_socket(family, type, &socket);
///    if (res == kEtcPalErrOk)
///      return socket; // Implicitly converted to Expected, has_value() is true
///    else
///      return res; // Implicitly converted to Expected, has_value() is false
/// }
/// \endcode
///
/// The value_or() function can be useful when you want to use a default value in case of an error
/// condition:
///
/// \code
/// etcpal::Expected<std::string> ConvertIntToString(int val);
/// // ...
/// std::string conversion = ConvertIntToString(arg).value_or("0");
/// // Conversion contains the successful conversion result, or "0" if the conversion failed.
/// \endcode
template <typename T>
class Expected
{
private:
  template <typename>
  friend class Expected;

public:
  static_assert(!std::is_reference<T>::value, "T must not be a reference");
  static_assert(!std::is_same<T, etcpal_error_t>::value, "T must not be etcpal_error_t");
  static_assert(!std::is_same<T, Result>::value, "T must not be etcpal::Result");
  static_assert(!std::is_void<T>::value, "T must not be void");

  /// The value type.
  using ValueType = T;

  // Typical ETC library style is to place all function definitions outside the class definition;
  // however, this gets nasty with these templates so the constructors are an exception here.

  // clang-format off

  /// \brief Default constructor - enabled if T is default-constructible.
  ///
  /// Default-constructs an instance of T. This Expected instance has a value after this
  /// constructor is called.
  template <bool B = std::is_default_constructible<T>::value, typename std::enable_if<B, int>::type = 0>
  ETCPAL_CONSTEXPR_14 Expected() : contained_(true)
  {
    contained_.ConstructValue(ValueType());
  }

  /// \brief Copy constructor - enabled if T is copy-constructible.
  ///
  /// If other has a value, copies its value into this Expected.
  /// If other does not have a value, copies its error code into this Expected.
  ETCPAL_CONSTEXPR_14 Expected(const Expected& other) = default;

  /// \brief Move constructor - enabled if T is move-constructible.
  ///
  /// If other has a value, moves its value into this Expected.
  /// If other does not have a value, copies its error code into this Expected.
  ETCPAL_CONSTEXPR_14 Expected(Expected&& other) = default;

  /// \brief Explicit conversion copy constructor - enabled for U that can be explicitly converted to T.
  ///
  /// If other has a value, copies its value into this Expected.
  /// If other does not have a value, copies its error code into this Expected.
  template<typename U>
  ETCPAL_CONSTEXPR_14 explicit Expected(const Expected<U>& other,
                                        ETCPAL_ENABLE_IF_ARG(
                                            std::is_constructible<T, const U&>::value &&
                                           !std::is_constructible<T, Expected<U>&>::value &&
                                           !std::is_constructible<T, Expected<U>       &&   >::value &&
                                           !std::is_constructible<T, Expected<U> const &    >::value &&
                                           !std::is_constructible<T, Expected<U> const &&   >::value &&
                                           !std::is_convertible<     Expected<U>       & , T>::value &&
                                           !std::is_convertible<     Expected<U>       &&, T>::value &&
                                           !std::is_convertible<     Expected<U> const & , T>::value &&
                                           !std::is_convertible<     Expected<U> const &&, T>::value &&
                                           !std::is_convertible<const U&, T>::value
                                       ))
      : contained_(other.has_value())
  {
    if (has_value())
      contained_.ConstructValue(T{other.contained_.value()});
    else
      contained_.SetError(other.contained_.error());
  }

  /// \brief Implicit conversion copy constructor - enabled for U that can be implicitly converted to T.
  ///
  /// If other has a value, copies its value into this Expected.
  /// If other does not have a value, copies its error code into this Expected.
  template<typename U>
  ETCPAL_CONSTEXPR_14 Expected(const Expected<U>& other,
                               ETCPAL_ENABLE_IF_ARG(
                                   std::is_constructible<T, const U&>::value &&
                                  !std::is_constructible<T, Expected<U>       &    >::value &&
                                  !std::is_constructible<T, Expected<U>       &&   >::value &&
                                  !std::is_constructible<T, Expected<U> const &    >::value &&
                                  !std::is_constructible<T, Expected<U> const &&   >::value &&
                                  !std::is_convertible<     Expected<U>       & , T>::value &&
                                  !std::is_convertible<     Expected<U>       &&, T>::value &&
                                  !std::is_convertible<     Expected<U> const  &, T>::value &&
                                  !std::is_convertible<     Expected<U> const &&, T>::value &&
                                  std::is_convertible<const U&, T>::value
                              ))
      : contained_(other.has_value())
  {
    if (has_value())
      contained_.ConstructValue(other.contained_.value());
    else
      contained_.SetError(other.contained_.error());
  }

  /// \brief Explicit conversion move constructor - enabled for U that can be explicitly converted to T.
  ///
  /// If other has a value, moves its value into this Expected.
  /// If other does not have a value, copies its error code into this Expected.
  template<typename U>
  ETCPAL_CONSTEXPR_14 explicit Expected(Expected<U>&& other,
                                        ETCPAL_ENABLE_IF_ARG(
                                            std::is_constructible<T, U>::value &&
                                           !std::is_constructible<T, Expected<U>       &    >::value &&
                                           !std::is_constructible<T, Expected<U>       &&   >::value &&
                                           !std::is_constructible<T, Expected<U> const &    >::value &&
                                           !std::is_constructible<T, Expected<U> const &&   >::value &&
                                           !std::is_convertible<     Expected<U>       & , T>::value &&
                                           !std::is_convertible<     Expected<U>       &&, T>::value &&
                                           !std::is_convertible<     Expected<U> const & , T>::value &&
                                           !std::is_convertible<     Expected<U> const &&, T>::value &&
                                           !std::is_convertible<U, T>::value
                                       ))
      : contained_(other.has_value())
  {
    if (has_value())
      contained_.ConstructValue(T{std::move(other.contained_.value())});
    else
      contained_.SetError(other.contained_.error());
  }

  /// \brief Implicit conversion move constructor - enabled for U that can be implicitly converted to T.
  ///
  /// If other has a value, moves its value into this Expected.
  /// If other does not have a value, copies its error code into this Expected.
  template<typename U>
  ETCPAL_CONSTEXPR_14 Expected(Expected<U>&& other,
                               ETCPAL_ENABLE_IF_ARG(
                                   std::is_constructible<T, U>::value &&
                                  !std::is_constructible<T, Expected<U>      &     >::value &&
                                  !std::is_constructible<T, Expected<U>      &&    >::value &&
                                  !std::is_constructible<T, Expected<U> const &    >::value &&
                                  !std::is_constructible<T, Expected<U> const &&   >::value &&
                                  !std::is_convertible<     Expected<U>       & , T>::value &&
                                  !std::is_convertible<     Expected<U>       &&, T>::value &&
                                  !std::is_convertible<     Expected<U> const & , T>::value &&
                                  !std::is_convertible<     Expected<U> const &&, T>::value &&
                                   std::is_convertible<U, T>::value
                              ))
      : contained_(other.has_value())
  {
    if (has_value())
      contained_.ConstructValue(std::move(other.contained_.value()));
    else
      contained_.SetError(other.contained_.error());
  }

  /// \brief Construct from value, explicit.
  ///
  /// Constructs an Expected from some value U which is explicitly (but not implicitly) convertible
  /// to T. The constructor is made explicit as a result. This Expected instance has a value after
  /// this constructor is called.
  ///
  /// \param value Value to construct from.
  template <typename U = T>
  ETCPAL_CONSTEXPR_14 explicit Expected(U&& value,
                                        ETCPAL_ENABLE_IF_ARG( std::is_constructible<T, U&&>::value &&
                                                             !std::is_convertible<U&&, T>::value
                                                            )) noexcept(std::is_nothrow_move_constructible<U>::value)
      : contained_(true)
  {
    contained_.ConstructValue(T{std::forward<U>(value)});
  }

  /// \brief Construct from value, implicit.
  ///
  /// Constructs an Expected from some value U implicitly convertible to T. The constructor is not
  /// made explicit as a result. This Expected instance has a value after this constructor is
  /// called.
  ///
  /// \param value Value to construct from.
  template <typename U = T>
  ETCPAL_CONSTEXPR_14 Expected(U&& value, ETCPAL_ENABLE_IF_ARG(std::is_constructible<T, U&&>::value &&
                                                               std::is_convertible<U&&, T>::value
                                                              )) noexcept(std::is_nothrow_move_constructible<U>::value)
      : contained_(true)
  {
    contained_.ConstructValue(std::forward<U>(value));
  }

  // clang-format on

  // Always implicit by design
  ETCPAL_CONSTEXPR_14 Expected(etcpal_error_t error);
  ~Expected();

  constexpr const T* operator->() const;
  T* operator->();
  constexpr const T& operator*() const&;
  T& operator*() &;
  constexpr const T&& operator*() const&&;
  ETCPAL_CONSTEXPR_14 T&& operator*() &&;
  constexpr explicit operator bool() const noexcept;
  constexpr bool has_value() const noexcept;
  constexpr const T& value() const&;
  T& value() &;
  constexpr const T&& value() const&&;
  ETCPAL_CONSTEXPR_14 T&& value() &&;
  constexpr etcpal_error_t error() const noexcept;
  constexpr Result result() const noexcept;

  // clang-format off

  /// \brief Get the value, or a default value if this Expected contains an error.
  /// \param def_val The default value to return on error - must be convertible to T.
  template <typename U,
            ETCPAL_ENABLE_IF_TEMPLATE(std::is_copy_constructible<T>::value && std::is_convertible<U&&, T>::value)>
  constexpr T value_or(U&& def_val) const&
  {
    return has_value() ? contained_.value() : static_cast<T>(std::forward<U>(def_val));
  }

  /// \brief Get the value, or a default value if this Expected contains an error.
  /// \param def_val The default value to return on error - must be convertible to T.
  template <typename U,
            ETCPAL_ENABLE_IF_TEMPLATE(std::is_move_constructible<T>::value && std::is_convertible<U&&, T>::value)>
  T value_or(U&& def_val) &&
  {
    return has_value() ? contained_.value() : static_cast<T>(std::forward<U>(def_val));
  }

  // clang-format on

private:
  detail::ExpectedStorage<T, std::is_copy_constructible<T>::value, std::is_move_constructible<T>::value> contained_;
};  // namespace etcpal

/// \brief Construct an Expected instance containing an error code.
template <typename T>
ETCPAL_CONSTEXPR_14 Expected<T>::Expected(etcpal_error_t error) : contained_(false)
{
  contained_.SetError(error);
}

/// \brief Calls the contained value's destructor if and only if has_value() is true.
template <typename T>
Expected<T>::~Expected()
{
  if (has_value())
    contained_.DestructValue();
}

/// \brief Access members of a composite contained value.
///
/// If has_value() is false, the behavior is undefined.
template <typename T>
constexpr const T* Expected<T>::operator->() const
{
  // Comma syntax is to satisfy C++11 requirements for constexpr
  return assert(has_value()), contained_.value_ptr();
}

/// \brief Access members of a composite contained value.
///
/// If has_value() is false, the behavior is undefined.
template <typename T>
T* Expected<T>::operator->()
{
  // Comma syntax is to satisfy C++11 requirements for constexpr
  return assert(has_value()), contained_.value_ptr();
}

/// \brief Get the underlying value.
///
/// If has_value() is false, the behavior is undefined.
template <typename T>
constexpr const T& Expected<T>::operator*() const&
{
  // Comma syntax is to satisfy C++11 requirements for constexpr
  return assert(has_value()), contained_.value();
}

/// \brief Get the underlying value.
///
/// If has_value() is false, the behavior is undefined.
template <typename T>
T& Expected<T>::operator*() &
{
  // Comma syntax is to satisfy C++11 requirements for constexpr
  return assert(has_value()), contained_.value();
}

/// \brief Get the underlying value.
///
/// If has_value() is false, the behavior is undefined.
template <typename T>
constexpr const T&& Expected<T>::operator*() const&&
{
  // Comma syntax is to satisfy C++11 requirements for constexpr
  return assert(has_value()), std::move(contained_.value());
}

/// \brief Get the underlying value.
///
/// If has_value() is false, the behavior is undefined.
template <typename T>
ETCPAL_CONSTEXPR_14 T&& Expected<T>::operator*() &&
{
  // Comma syntax is to satisfy C++11 requirements for constexpr
  return assert(has_value()), std::move(contained_.value());
}

/// \brief Evaluate the Expected instance inline - evaluates to has_value().
template <typename T>
constexpr Expected<T>::operator bool() const noexcept
{
  return has_value();
}

/// \brief Whether this Expected instance contains a valid value. If not, contains an error code.
template <typename T>
constexpr bool Expected<T>::has_value() const noexcept
{
  return contained_.has_value();
}

/// \brief Get the underlying value.
/// \throw BadExpectedAccess if has_value() is false.
template <typename T>
constexpr const T& Expected<T>::value() const&
{
  // Comma syntax is to satisfy C++11 requirements for constexpr
  return has_value() ? (contained_.value()) : (throw BadExpectedAccess(contained_.error()), contained_.value());
}

/// \brief Get the underlying value.
/// \throw BadExpectedAccess if has_value() is false.
template <typename T>
T& Expected<T>::value() &
{
  // Comma syntax is to satisfy C++11 requirements for constexpr
  return has_value() ? (contained_.value()) : (throw BadExpectedAccess(contained_.error()), contained_.value());
}

/// \brief Get the underlying value.
/// \throw BadExpectedAccess if has_value() is false.
template <typename T>
constexpr const T&& Expected<T>::value() const&&
{
  // Comma syntax is to satisfy C++11 requirements for constexpr
  return std::move(has_value() ? (contained_.value())
                               : (throw BadExpectedAccess(contained_.error()), contained_.value()));
}

/// \brief Get the underlying value.
/// \throw BadExpectedAccess if has_value() is false.
template <typename T>
ETCPAL_CONSTEXPR_14 T&& Expected<T>::value() &&
{
  // Comma syntax is to satisfy C++11 requirements for constexpr
  return std::move(has_value() ? (contained_.value())
                               : (throw BadExpectedAccess(contained_.error()), contained_.value()));
}

/// \brief Get the error code.
///
/// If has_value() is true, the behavior is undefined.
template <typename T>
constexpr etcpal_error_t Expected<T>::error() const noexcept
{
  // Comma syntax is to satisfy C++11 requirements for constexpr
  return assert(!has_value()), contained_.error();
}

/// \brief Get the error code as a Result object.
///
/// If has_value() is true, the behavior is undefined.
template <typename T>
constexpr Result Expected<T>::result() const noexcept
{
  // Comma syntax is to satisfy C++11 requirements for constexpr
  return assert(!has_value()), contained_.error();
}

/// \addtogroup etcpal_cpp_error
/// @{

/// \name Expected Relational Operators
/// @{

template <typename T1, typename T2>
constexpr bool operator==(const Expected<T1>& x, const Expected<T2>& y)
{
  return bool(x) != bool(y) ? false : bool(x) == false ? x.error() == y.error() : *x == *y;
}

template <typename T1, typename T2>
constexpr bool operator!=(const Expected<T1>& x, const Expected<T2>& y)
{
  return !(x == y);
}

template <typename T1, typename T2>
constexpr bool operator==(const Expected<T1>& x, const T2& v)
{
  return bool(x) ? *x == v : false;
}

template <typename T1, typename T2>
constexpr bool operator==(const T2& v, const Expected<T1>& x)
{
  return bool(x) ? v == *x : false;
}

template <typename T1, typename T2>
constexpr bool operator!=(const Expected<T1>& x, const T2& v)
{
  return bool(x) ? *x != v : true;
}

template <typename T1, typename T2>
constexpr bool operator!=(const T2& v, const Expected<T1>& x)
{
  return bool(x) ? v != *x : true;
}

template <typename T>
constexpr bool operator==(const Expected<T>& x, etcpal_error_t e)
{
  return (!x) ? x.error() == e : false;
}

template <typename T>
constexpr bool operator==(etcpal_error_t e, const Expected<T>& x)
{
  return (x == e);
}

template <typename T>
constexpr bool operator!=(const Expected<T>& x, etcpal_error_t e)
{
  return !(x == e);
}

template <typename T>
constexpr bool operator!=(etcpal_error_t e, const Expected<T>& x)
{
  return !(x == e);
}

/// @}
/// @}

};  // namespace etcpal

#endif  // ETCPAL_CPP_ERROR_H_
