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
 * This file is a part of lwpa. For more information, go to:
 * https://github.com/ETCLabs/lwpa
 ******************************************************************************/

#include "etcpal/uuid.h"

#include <uuid/uuid.h>

// Use the native UUID functionality in the macOS SDK to generate UUIDs.
// https://developer.apple.com/library/archive/documentation/System/Conceptual/ManPages_iPhoneOS/man3/uuid.3.html

lwpa_error_t lwpa_generate_v1_uuid(LwpaUuid* uuid)
{
  if (!uuid)
    return kLwpaErrInvalid;

  uuid_t os_uuid;
  uuid_generate_time(os_uuid);
  memcpy(uuid->data, os_uuid, LWPA_UUID_BYTES);
  return kLwpaErrOk;
}

lwpa_error_t lwpa_generate_v4_uuid(LwpaUuid* uuid)
{
  if (!uuid)
    return kLwpaErrInvalid;

  uuid_t os_uuid;
  uuid_generate_random(os_uuid);
  memcpy(uuid->data, os_uuid, LWPA_UUID_BYTES);
  return kLwpaErrOk;
}

lwpa_error_t lwpa_generate_os_preferred_uuid(LwpaUuid* uuid)
{
  if (!uuid)
    return kLwpaErrInvalid;

  uuid_t os_uuid;
  uuid_generate(os_uuid);
  memcpy(uuid->data, os_uuid, LWPA_UUID_BYTES);
  return kLwpaErrOk;
}
