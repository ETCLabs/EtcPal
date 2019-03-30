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

#include <string.h>
#include <rpc.h>
#include "lwpa/pack.h"

/* The Windows implementation of generating a V1 UUID. */
lwpa_error_t lwpa_generate_v1_uuid(LwpaUuid *uuid)
{
  UUID plat_uuid;

  if (!uuid)
    return kLwpaErrInvalid;

  if (RPC_S_OK == UuidCreateSequential(&plat_uuid))
  {
    lwpa_pack_32b(&uuid->data[0], plat_uuid.Data1);
    lwpa_pack_16b(&uuid->data[4], plat_uuid.Data2);
    lwpa_pack_16b(&uuid->data[6], plat_uuid.Data3);
    memcpy(&uuid->data[8], plat_uuid.Data4, 8);
    return kLwpaErrOk;
  }
  else
  {
    return kLwpaErrSys;
  }
}

lwpa_error_t lwpa_generate_v4_uuid(LwpaUuid *uuid)
{
  UUID plat_uuid;

  if (!uuid)
    return kLwpaErrInvalid;

  if (RPC_S_OK == UuidCreate(&plat_uuid))
  {
    lwpa_pack_32b(&uuid->data[0], plat_uuid.Data1);
    lwpa_pack_16b(&uuid->data[4], plat_uuid.Data2);
    lwpa_pack_16b(&uuid->data[6], plat_uuid.Data3);
    memcpy(&uuid->data[8], plat_uuid.Data4, 8);
    return kLwpaErrOk;
  }
  else
  {
    return kLwpaErrSys;
  }
}
