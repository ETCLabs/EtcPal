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
#include <string>
#include "etcpal/uuid.h"
#include "etcpal/cpp/inet.h"

namespace etcpal
{
/// \defgroup etcpal_cpp_uuid uuid (UUIDs)
/// \ingroup etcpal_cpp
/// \brief C++ utilities for the \ref etcpal_uuid module.

/// \ingroup etcpal_cpp_uuid
/// \brief A wrapper class for the EtcPal UUID type.
///
/// Provides C++ syntactic sugar for working with UUIDs. UUIDs are considered immutable after they
/// are created, so there is no non-const access to the underlying data as of now.
class Uuid
{
public:
  /// \brief Constructs a null UUID by default.
  Uuid() = default;
  // Note: this constructor is not explicit by design, to allow implicit conversions e.g.
  //   etcpal::Uuid uuid = etcpal_c_function_that_returns_uuid();
  constexpr Uuid(const EtcPalUuid& c_uuid) noexcept;
  Uuid& operator=(const EtcPalUuid& c_uuid) noexcept;

  constexpr const EtcPalUuid& get() const noexcept;
  std::string ToString() const;
  bool IsNull() const noexcept;

  static Uuid FromString(const char* uuid_str) noexcept;
  static Uuid FromString(const std::string& uuid_str) noexcept;
  static Uuid V1() noexcept;
  static Uuid V3(const Uuid& ns, const void* name, size_t name_len) noexcept;
  static Uuid V4() noexcept;
  static Uuid V5(const Uuid& ns, const void* name, size_t name_len) noexcept;
  static Uuid OsPreferred() noexcept;
  static Uuid Device(const std::string& device_str, const MacAddr& mac_addr, uint32_t uuid_num) noexcept;

private:
  EtcPalUuid uuid_{kEtcPalNullUuid};
};

/// \brief Construct a UUID copied from an instance of the C EtcPalUuid type.
constexpr Uuid::Uuid(const EtcPalUuid& c_uuid) noexcept : uuid_(c_uuid)
{
}

/// \brief Assign an instance of the C EtcPalUuid type to an instance of this class.
inline Uuid& Uuid::operator=(const EtcPalUuid& c_uuid) noexcept
{
  uuid_ = c_uuid;
  return *this;
}

/// \brief Get a reference to the underlying C type.
constexpr const EtcPalUuid& Uuid::get() const noexcept
{
  return uuid_;
}

/// \brief Convert the UUID to a string representation formatted per RFC 4122.
inline std::string Uuid::ToString() const
{
  char str_buf[ETCPAL_UUID_STRING_BYTES];
  if (etcpal_uuid_to_string(&uuid_, str_buf))
    return str_buf;
  else
    return std::string();
}

/// \brief Check if a UUID is null (all 0's).
inline bool Uuid::IsNull() const noexcept
{
  return ETCPAL_UUID_IS_NULL(&uuid_);
}

/// \brief Create a UUID from a string representation.
/// \return A valid UUID on successful parse, or a null UUID on failure.
inline Uuid Uuid::FromString(const char* uuid_str) noexcept
{
  Uuid uuid;
  etcpal_string_to_uuid(uuid_str, &uuid.uuid_);
  return uuid;
}

/// \brief Create a UUID from a string representation.
/// \return A valid UUID on successful parse, or a null UUID on failure.
inline Uuid Uuid::FromString(const std::string& uuid_str) noexcept
{
  return FromString(uuid_str.c_str());
}

/// \brief Generate and return a Version 1 UUID.
///
/// If not implemented, returns a null UUID. See etcpal_generate_v1_uuid() for more information.
inline Uuid Uuid::V1() noexcept
{
  Uuid uuid;
  etcpal_generate_v1_uuid(&uuid.uuid_);
  return uuid;
}

/// \brief Generate and return a Version 3 UUID.
///
/// See etcpal_generate_v3_uuid() for more information.
inline Uuid Uuid::V3(const Uuid& ns, const void* name, size_t name_len) noexcept
{
  Uuid uuid;
  etcpal_generate_v3_uuid(&ns.get(), name, name_len, &uuid.uuid_);
  return uuid;
}

/// \brief Generate and return a Version 4 UUID.
///
/// If not implemented, returns a null UUID. See etcpal_generate_v4_uuid() for more information.
inline Uuid Uuid::V4() noexcept
{
  Uuid uuid;
  etcpal_generate_v4_uuid(&uuid.uuid_);
  return uuid;
}

/// \brief Generate and return a Version 5 UUID.
///
/// See etcpal_generate_v5_uuid() for more information.
inline Uuid Uuid::V5(const Uuid& ns, const void* name, size_t name_len) noexcept
{
  Uuid uuid;
  etcpal_generate_v5_uuid(&ns.get(), name, name_len, &uuid.uuid_);
  return uuid;
}

/// \brief Generate and return a UUID of the version preferred by the underlying OS.
///
/// If not implemented, returns a null UUID. See etcpal_generate_os_preferred_uuid() for more
/// information.
inline Uuid Uuid::OsPreferred() noexcept
{
  Uuid uuid;
  etcpal_generate_os_preferred_uuid(&uuid.uuid_);
  return uuid;
}

/// \brief Generate and return a Device UUID.
///
/// See etcpal_generate_device_uuid() for more information.
inline Uuid Uuid::Device(const std::string& device_str, const MacAddr& mac_addr, uint32_t uuid_num) noexcept
{
  Uuid uuid;
  etcpal_generate_device_uuid(device_str.c_str(), &mac_addr.get(), uuid_num, &uuid.uuid_);
  return uuid;
}

/// \addtogroup etcpal_cpp_uuid
/// @{

/// \name UUID Relational Operators
/// @{

// Special operators for comparing with EtcPalUuids

inline bool operator==(const EtcPalUuid& c_uuid, const Uuid& uuid) noexcept
{
  return c_uuid == uuid.get();
}

inline bool operator!=(const EtcPalUuid& c_uuid, const Uuid& uuid) noexcept
{
  return !(c_uuid == uuid);
}

inline bool operator==(const Uuid& uuid, const EtcPalUuid& c_uuid) noexcept
{
  return uuid.get() == c_uuid;
}

inline bool operator!=(const Uuid& uuid, const EtcPalUuid& c_uuid) noexcept
{
  return !(uuid == c_uuid);
}

// Standard operators

inline bool operator==(const Uuid& a, const Uuid& b) noexcept
{
  return a.get() == b.get();
}

inline bool operator!=(const Uuid& a, const Uuid& b) noexcept
{
  return !(a == b);
}

inline bool operator<(const Uuid& a, const Uuid& b) noexcept
{
  return a.get() < b.get();
}

inline bool operator>(const Uuid& a, const Uuid& b) noexcept
{
  return b < a;
}

inline bool operator<=(const Uuid& a, const Uuid& b) noexcept
{
  return !(b < a);
}

inline bool operator>=(const Uuid& a, const Uuid& b) noexcept
{
  return !(a < b);
}

/// @}
/// @}

};  // namespace etcpal

#endif  // ETCPAL_CPP_UUID_H_
