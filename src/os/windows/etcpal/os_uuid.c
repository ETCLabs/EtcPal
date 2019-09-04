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

etcpal_error_t etcpal_generate_v1_uuid(EtcPalUuid* uuid)
{
  if (!uuid)
    return kEtcPalErrInvalid;

  UUID os_uuid;
  if (RPC_S_OK == UuidCreateSequential(&os_uuid))
  {
    etcpal_pack_32b(&uuid->data[0], os_uuid.Data1);
    etcpal_pack_16b(&uuid->data[4], os_uuid.Data2);
    etcpal_pack_16b(&uuid->data[6], os_uuid.Data3);
    memcpy(&uuid->data[8], os_uuid.Data4, 8);
    return kEtcPalErrOk;
  }
  else
  {
    return kEtcPalErrSys;
  }
}

etcpal_error_t etcpal_generate_v4_uuid(EtcPalUuid* uuid)
{
  if (!uuid)
    return kEtcPalErrInvalid;

  UUID os_uuid;
  if (RPC_S_OK == UuidCreate(&os_uuid))
  {
    etcpal_pack_32b(&uuid->data[0], os_uuid.Data1);
    etcpal_pack_16b(&uuid->data[4], os_uuid.Data2);
    etcpal_pack_16b(&uuid->data[6], os_uuid.Data3);
    memcpy(&uuid->data[8], os_uuid.Data4, 8);
    return kEtcPalErrOk;
  }
  else
  {
    return kEtcPalErrSys;
  }
}

etcpal_error_t etcpal_generate_os_preferred_uuid(EtcPalUuid* uuid)
{
  return etcpal_generate_v4_uuid(uuid);
}
