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

#include <string.h>
#include <rpc.h>
#include "etcpal/pack.h"

// Use Windows APIs to generate UUIDs.
// https://docs.microsoft.com/en-us/windows/desktop/api/rpcdce/nf-rpcdce-uuidcreate

lwpa_error_t lwpa_generate_v1_uuid(LwpaUuid* uuid)
{
  if (!uuid)
    return kLwpaErrInvalid;

  UUID os_uuid;
  if (RPC_S_OK == UuidCreateSequential(&os_uuid))
  {
    lwpa_pack_32b(&uuid->data[0], os_uuid.Data1);
    lwpa_pack_16b(&uuid->data[4], os_uuid.Data2);
    lwpa_pack_16b(&uuid->data[6], os_uuid.Data3);
    memcpy(&uuid->data[8], os_uuid.Data4, 8);
    return kLwpaErrOk;
  }
  else
  {
    return kLwpaErrSys;
  }
}

lwpa_error_t lwpa_generate_v4_uuid(LwpaUuid* uuid)
{
  if (!uuid)
    return kLwpaErrInvalid;

  UUID os_uuid;
  if (RPC_S_OK == UuidCreate(&os_uuid))
  {
    lwpa_pack_32b(&uuid->data[0], os_uuid.Data1);
    lwpa_pack_16b(&uuid->data[4], os_uuid.Data2);
    lwpa_pack_16b(&uuid->data[6], os_uuid.Data3);
    memcpy(&uuid->data[8], os_uuid.Data4, 8);
    return kLwpaErrOk;
  }
  else
  {
    return kLwpaErrSys;
  }
}

lwpa_error_t lwpa_generate_os_preferred_uuid(LwpaUuid* uuid)
{
  return lwpa_generate_v4_uuid(uuid);
}
