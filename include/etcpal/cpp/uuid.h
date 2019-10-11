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

#include <cstdint>
#include <cstring>
#include <array>
#include <initializer_list>
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
  Uuid(const std::array<uint8_t, ETCPAL_UUID_BYTES>& bytes);
  Uuid& operator=(const EtcPalUuid& c_uuid);

  const EtcPalUuid& get() const;
  std::string ToString() const;
  bool IsNull() const;

  static Uuid FromString(const std::string& uuid_str);
  static Uuid V1();
  static Uuid V3(const std::string& device_str, const std::array<uint8_t, 6>& mac_addr, uint32_t uuid_num);
  static Uuid V4();
  static Uuid OsPreferred();

private:
  EtcPalUuid uuid_{kEtcPalNullUuid};
};

inline Uuid::Uuid(const EtcPalUuid& c_uuid) : uuid_(c_uuid)
{
}

inline Uuid::Uuid(const std::array<uint8_t, ETCPAL_UUID_BYTES>& bytes)
{
  std::memcpy(uuid_.data, bytes.data(), ETCPAL_UUID_BYTES);
}

inline Uuid& Uuid::operator=(const EtcPalUuid& c_uuid)
{
  uuid_ = c_uuid;
  return *this;
}

inline const EtcPalUuid& Uuid::get() const
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
  Uuid uuid;
  etcpal_string_to_uuid(uuid_str.c_str(), &uuid.uuid_);
  return uuid;
}

Uuid Uuid::V1()
{
  Uuid uuid;
  etcpal_generate_v1_uuid(&uuid.uuid_);
  return uuid;
}

Uuid Uuid::V3(const std::string& device_str, const std::array<uint8_t, 6>& mac_addr, uint32_t uuid_num)
{
  Uuid uuid;
  etcpal_generate_v3_uuid(device_str.c_str(), mac_addr.data(), uuid_num, &uuid.uuid_);
  return uuid;
}

Uuid Uuid::V4()
{
  Uuid uuid;
  etcpal_generate_v4_uuid(&uuid.uuid_);
  return uuid;
}

Uuid Uuid::OsPreferred()
{
  Uuid uuid;
  etcpal_generate_os_preferred_uuid(&uuid.uuid_);
  return uuid;
}

// operators
inline bool operator==(const EtcPalUuid& c_uuid, const Uuid& uuid) noexcept
{
  return c_uuid == uuid.get();
}

inline bool operator==(const Uuid& uuid, const EtcPalUuid& c_uuid) noexcept
{
  return uuid.get() == c_uuid;
}

inline bool operator==(const Uuid& a, const Uuid& b) noexcept
{
  return a.get() == b.get();
}

inline bool operator<(const Uuid& a, const Uuid& b) noexcept
{
  return a.get() < b.get();
}

};  // namespace etcpal

#endif  // ETCPAL_CPP_UUID_H_
