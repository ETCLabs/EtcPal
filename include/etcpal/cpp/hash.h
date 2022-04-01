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

/// @file etcpal/cpp/opaque_id.h
/// @brief Provides the OpaqueId template type and function definitions.

#ifndef ETCPAL_CPP_HASH_H_
#define ETCPAL_CPP_HASH_H_

#include <cstddef>
#include <functional>

namespace etcpal
{
/// @defgroup etcpal_cpp_hash hash (Hashing Utilities)
/// @ingroup etcpal_cpp
/// @brief C++ utilities for hashing.

/// @ingroup etcpal_cpp_hash
/// @brief A function that combines the hash of a new value into an existing seed, based on boost::hash_combine.
///
/// Example:
///
/// @code
/// int val1 = 1234;
/// std::string val2("5678");
///
/// size_t seed = std::hash<int>()(val1);
/// etcpal::HashCombine(seed, val2);
///
/// // seed now containes a hash uniquely based on the combination of val1 and val2.
/// @endcode
/// @param seed The seed which will be updated.
/// @param val The value to hash and combine into the seed.
template <class T>
inline void HashCombine(size_t& seed, const T& val)
{
  std::hash<T> hasher;
  seed ^= hasher(val) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

};  // namespace etcpal

#endif /* ETCPAL_CPP_HASH_H_ */
