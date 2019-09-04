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

#include "etcpal/uuid.h"

#include <uuid/uuid.h>

// Use libuuid on Linux to generate UUIDs.
// https://linux.die.net/man/3/uuid_generate

etcpal_error_t etcpal_generate_v1_uuid(LwpaUuid* uuid)
{
  if (!uuid)
    return kEtcPalErrInvalid;

  uuid_t os_uuid;
  uuid_generate_time(os_uuid);
  memcpy(uuid->data, os_uuid, ETCPAL_UUID_BYTES);
  return kEtcPalErrOk;
}

etcpal_error_t etcpal_generate_v4_uuid(LwpaUuid* uuid)
{
  if (!uuid)
    return kEtcPalErrInvalid;

  uuid_t os_uuid;
  uuid_generate_random(os_uuid);
  memcpy(uuid->data, os_uuid, ETCPAL_UUID_BYTES);
  return kEtcPalErrOk;
}

etcpal_error_t etcpal_generate_os_preferred_uuid(LwpaUuid* uuid)
{
  if (!uuid)
    return kEtcPalErrInvalid;

  uuid_t os_uuid;
  uuid_generate(os_uuid);
  memcpy(uuid->data, os_uuid, ETCPAL_UUID_BYTES);
  return kEtcPalErrOk;
}
