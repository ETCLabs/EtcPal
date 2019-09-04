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

#ifndef _LWPA_OS_ERROR_H_
#define _LWPA_OS_ERROR_H_

#include "lwpa/error.h"
#include <lwip/errno.h>

#if !LWIP_PROVIDE_ERRNO && !LWIP_ERRNO_STDINCLUDE && !defined(LWIP_ERRNO_INCLUDE)
#include <errno.h>
#endif

lwpa_error_t errno_lwip_to_lwpa(int lwip_errno);
lwpa_error_t err_gai_to_lwpa(int gai_error);

#endif /* _LWPA_OS_ERROR_H_ */
