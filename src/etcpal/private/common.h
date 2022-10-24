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

/* etcpal/private/common.h: Definitions used throughout the EtcPal library. */

#ifndef ETCPAL_PRIVATE_COMMON_H_
#define ETCPAL_PRIVATE_COMMON_H_

#include "etcpal/private/opts.h"

#include "etcpal/log.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 * Logging
 *****************************************************************************/

#if ETCPAL_LOGGING_ENABLED
#define ETCPAL_PRINT_LOG(pri, ...)    etcpal_log(etcpal_log_params, (pri), ETCPAL_LOG_MSG_PREFIX __VA_ARGS__)
#define ETCPAL_PRINT_LOG_EMERG(...)   ETCPAL_PRINT_LOG(ETCPAL_LOG_EMERG, __VA_ARGS__)
#define ETCPAL_PRINT_LOG_ALERT(...)   ETCPAL_PRINT_LOG(ETCPAL_LOG_ALERT, __VA_ARGS__)
#define ETCPAL_PRINT_LOG_CRIT(...)    ETCPAL_PRINT_LOG(ETCPAL_LOG_CRIT, __VA_ARGS__)
#define ETCPAL_PRINT_LOG_ERR(...)     ETCPAL_PRINT_LOG(ETCPAL_LOG_ERR, __VA_ARGS__)
#define ETCPAL_PRINT_LOG_WARNING(...) ETCPAL_PRINT_LOG(ETCPAL_LOG_WARNING, __VA_ARGS__)
#define ETCPAL_PRINT_LOG_NOTICE(...)  ETCPAL_PRINT_LOG(ETCPAL_LOG_NOTICE, __VA_ARGS__)
#define ETCPAL_PRINT_LOG_INFO(...)    ETCPAL_PRINT_LOG(ETCPAL_LOG_INFO, __VA_ARGS__)
#define ETCPAL_PRINT_LOG_DEBUG(...)   ETCPAL_PRINT_LOG(ETCPAL_LOG_DEBUG, __VA_ARGS__)

#define ETCPAL_CAN_LOG(pri) etcpal_can_log(etcpal_log_params, (pri))
#else
#define ETCPAL_PRINT_LOG(pri, ...)
#define ETCPAL_PRINT_LOG_EMERG(...)
#define ETCPAL_PRINT_LOG_ALERT(...)
#define ETCPAL_PRINT_LOG_CRIT(...)
#define ETCPAL_PRINT_LOG_ERR(...)
#define ETCPAL_PRINT_LOG_WARNING(...)
#define ETCPAL_PRINT_LOG_NOTICE(...)
#define ETCPAL_PRINT_LOG_INFO(...)
#define ETCPAL_PRINT_LOG_DEBUG(...)

#define ETCPAL_CAN_LOG(pri) false
#endif

/******************************************************************************
 * Global variables, functions, and state tracking
 *****************************************************************************/

extern const EtcPalLogParams* etcpal_log_params;
extern bool                   etcpal_init_called;

bool set_etcpal_log_params(const EtcPalLogParams* params);

#ifdef __cplusplus
}
#endif

#endif /* ETCPAL_PRIVATE_COMMON_H_ */
