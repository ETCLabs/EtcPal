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

#include "lwpa/netint.h"

#include <sys/types.h>
#include <ifaddrs.h>
#include <errno.h>
#include "lwpa/private/netint.h"
#include "os_error.h"

/**************************** Private variables ******************************/

bool module_initialized;
struct ifaddrs* os_addrs;
size_t num_addrs;

/*********************** Private function prototypes *************************/

static lwpa_error_t re_enumerate_os_addrs();

/*************************** Function definitions ****************************/

lwpa_error_t lwpa_netint_init()
{
  lwpa_error_t res = kLwpaErrOk;
  if (!module_initialized)
  {
    res = re_enumerate_os_addrs();
    if (res == kLwpaErrOk)
      module_initialized = true;
  }
  return res;
}

void lwpa_netint_deinit()
{
  if (module_initialized)
  {
    freeifaddrs(os_addrs);
    module_initialized = false;
  }
}

lwpa_error_t re_enumerate_os_addrs()
{
  if (getifaddrs(&os_addrs) < 0)
  {
    return errno_os_to_lwpa(errno);
  }

  num_addrs = 0;
  for (struct ifaddrs* ifaddr = os_addrs; ifaddr; ifaddr = ifaddr->ifa_next)
  {
    // TODO more logic...
    ++num_addrs;
  }
  return kLwpaErrOk;
}
