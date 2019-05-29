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

#include "lwpa/uuid.h"

#include <uuid/uuid.h>

// Use Linux APIs to generate UUIDs.
// https://linux.die.net/man/3/uuid_generate

lwpa_error_t lwpa_generate_v1_uuid(LwpaUuid *uuid)
{
  if (!uuid)
    return kLwpaErrInvalid;

  uuid_t plat_uuid;
  uuid_generate_time(plat_uuid);
  memcpy(uuid->data, plat_uuid, LWPA_UUID_BYTES);
  return kLwpaErrOk;
}

lwpa_error_t lwpa_generate_v4_uuid(LwpaUuid *uuid)
{
  if (!uuid)
    return kLwpaErrInvalid;

  uuid_t plat_uuid;
  uuid_generate_random(plat_uuid);
  memcpy(uuid->data, plat_uuid, LWPA_UUID_BYTES);
  return kLwpaErrOk;
}

lwpa_error_t lwpa_generate_os_preferred_uuid(LwpaUuid *uuid)
{
  if (!uuid)
    return kLwpaErrInvalid;

  uuid_t plat_uuid;
  uuid_generate(plat_uuid);
  memcpy(uuid->data, plat_uuid, LWPA_UUID_BYTES);
  return kLwpaErrOk;
}