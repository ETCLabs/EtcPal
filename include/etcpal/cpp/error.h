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

#include <exception>
#include <string>
#include "etcpal/error.h"

namespace etcpal
{
/// \defgroup etcpal_cpp_error etcpal_cpp_error
/// \ingroup etcpal_cpp
/// \brief C++ utilities for the \ref etcpal_error module.

/// \brief A wrapper class for the EtcPal error type.
/// \ingroup etcpal_cpp_error
///
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
class Result
{
public:
  Result() = delete;
  // Note: this constructor is not explicit by design, to allow implicit conversions e.g.
  //   etcpal::Result res = etcpal_c_function_that_returns_error_t();
  Result(etcpal_error_t code);
  Result& operator=(etcpal_error_t code);

  bool IsOk() const;
  etcpal_error_t code() const;
  std::string ToString() const;
  const char* ToCString() const;

  explicit operator bool() const;

  static Result Ok();

private:
  etcpal_error_t code_{kEtcPalErrOk};
  const char* str_{nullptr};

  void SetCode(etcpal_error_t code);
};

/// \brief Construct a Result from an error code.
inline Result::Result(etcpal_error_t code)
{
  SetCode(code);
}

/// \brief Assign an error code to this Result.
inline Result& Result::operator=(etcpal_error_t code)
{
  SetCode(code);
  return *this;
}

/// \brief Whether this Result contains the code #kEtcPalErrOk.
/// \details #kEtcPalErrOk is the only #etcpal_error_t code that does not indicate an error.
/// \return Whether the result is OK (not an error).
inline bool Result::IsOk() const
{
  return code_ == kEtcPalErrOk;
}

/// \brief Get the underlying code from a Result.
inline etcpal_error_t Result::code() const
{
  return code_;
}

/// \brief Get a descriptive string for this Result as a std::string.
inline std::string Result::ToString() const
{
  return str_;
}

/// \brief Get a descriptive string for this Result as a C string.
inline const char* Result::ToCString() const
{
  return str_;
}

/// \brief Evaluate the Result inline - evaluates true if the result is #kEtcPalErrOk, false
///        otherwise.
inline Result::operator bool() const
{
  return IsOk();
}

/// \brief Construct a result containing #kEtcPalErrOk.
inline Result Result::Ok()
{
  return Result(kEtcPalErrOk);
}

// Internal function to set the internal code and string for a given code.
inline void Result::SetCode(etcpal_error_t code)
{
  code_ = code;
  str_ = etcpal_strerror(code);
}

// equality operators
inline bool operator==(etcpal_error_t code, const Result& result)
{
  return code == result.code();
}

inline bool operator!=(etcpal_error_t code, const Result& result)
{
  return !(code == result);
}

inline bool operator==(const Result& result, etcpal_error_t code)
{
  return result.code() == code;
}

inline bool operator!=(const Result& result, etcpal_error_t code)
{
  return !(result == code);
}

inline bool operator==(const Result& a, const Result& b)
{
  return a.code() == b.code();
}

inline bool operator!=(const Result& a, const Result& b)
{
  return !(a == b);
}

class BadExpectedAccess : public std::exception
{
public:
  explicit BadExpectedAccess(etcpal_error_t err) noexcept;
  virtual const char* what() const noexcept override;
  etcpal_error_t code() const noexcept;

private:
  etcpal_error_t err_;
};

BadExpectedAccess::BadExpectedAccess(etcpal_error_t err) noexcept : err_(err)
{
}

const char* BadExpectedAccess::what() const noexcept
{
  return "Bad access to etcpal::Expected::value()";
}

etcpal_error_t BadExpectedAccess::code() const noexcept
{
  return err_;
}

template <typename T>
class Expected
{
public:
  static_assert(!std::is_reference<T>::value, "T must not be a reference");
  static_assert(!std::is_same<T, etcpal_error_t>::value, "T must not be etcpal_error_t");
  static_assert(!std::is_same<T, Result>::value, "T must not be etcpal::Result");

  using ValueType = T;

  constexpr Expected() = default;
  constexpr Expected(const Expected& other);
  Expected(T&& value);
  Expected(etcpal_error_t error);

  ~Expected();

  // TODO: fill out class from http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2017/p0323r3.pdf

  // constexpr explicit operator bool() const noexcept;
  // constexpr bool has_value() const noexcept;
  // constexpr const T& value() const&;
  constexpr ValueType& value() &;
  // constexpr const T&& value() const&&;
  // constexpr T&& value() &&;
  // constexpr const E& error() const&;
  // constexpr E& error() &;
  // constexpr const E&& error() const&&;
  // constexpr E&& error() &&;
  // template <class U>
  // constexpr T value_or(U&&) const&;
  // template <class U>
  // T value_or(U&&) &&;

private:
  bool has_value_{true};
  union
  {
    ValueType value_{};
    etcpal_error_t error_;
  };
};

template <typename T>
Expected<T>::Expected(T&& value) : value(std::forward<T>(value))
{
}

template <typename T>
Expected<T>::Expected(etcpal_error_t error) : has_value_(false), error_(error)
{
}

template <typename T>
Expected<T>::~Expected()
{
  if (has_value_)
    value_.~ValueType();
}

template <typename T>
constexpr T& Expected<T>::value() &
{
  if (has_value_)
    return value_;
  else
    throw BadExpectedAccess(error_);
}

};  // namespace etcpal

#endif  // ETCPAL_CPP_ERROR_H_
