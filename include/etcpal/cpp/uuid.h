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

/// \file etcpal/cpp/uuid.h
/// \brief C++ wrapper and utilities for etcpal/uuid.h

#ifndef ETCPAL_CPP_UUID_H_
#define ETCPAL_CPP_UUID_H_

#include <string>
#include "etcpal/uuid.h"

namespace etcpal
{
/// \defgroup etcpal_cpp_uuid etcpal_cpp_uuid
/// \ingroup etcpal_cpp
/// \brief C++ utilities for the \ref etcpal_uuid module.

/// \brief A wrapper class for the EtcPal UUID type.
/// \ingroup etcpal_cpp_uuid
///
class Uuid
{
public:
  Uuid() = default;
  Uuid(const EtcPalUuid& c_uuid);
  Uuid(EtcPalUuid&& c_uuid);
  Uuid& operator=(const EtcPalUuid& c_uuid);
  Uuid& operator=(EtcPalUuid&& c_uuid);

  EtcPalUuid get() const;
  std::string ToString() const;
  bool IsNull() const;

  static Uuid FromString(const std::string& uuid_str);
  static Uuid V1();
  static Uuid V3();
  static Uuid V4();
  static Uuid OsPreferred();

private:
  EtcPalUuid uuid_{kEtcPalNullUuid};
};

inline Uuid::Uuid(const EtcPalUuid& c_uuid) : uuid_(c_uuid)
{
}

inline Uuid::Uuid(EtcPalUuid&& c_uuid) : uuid_(c_uuid)
{
}

inline Uuid& Uuid::operator=(const EtcPalUuid& c_uuid)
{
  uuid_ = c_uuid;
  return *this;
}

// TODO evaluate/measure, I actually think the rvalue-reference overloads might not be necessary or
// help
inline Uuid& Uuid::operator=(EtcPalUuid&& c_uuid)
{
  uuid_ = c_uuid;
  return *this;
}

inline EtcPalUuid Uuid::get() const
{
  return uuid_;
}

inline std::string Uuid::ToString() const
{
  char str_buf[ETCPAL_UUID_STRING_BYTES];
  if (etcpal_uuid_to_string(&uuid_, str_buf))
    return str_buf;
  else
    return std::string();
}

inline bool Uuid::IsNull() const
{
  return ETCPAL_UUID_IS_NULL(&uuid_);
}

inline Uuid Uuid::FromString(const std::string& uuid_str)
{
  EtcPalUuid uuid;
  etcpal_string_to_uuid(uuid_str.c_str(), &uuid);
  return Uuid(std::move(uuid));
}

};  // namespace etcpal

#endif  // ETCPAL_CPP_UUID_H_
