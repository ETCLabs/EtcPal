/******************************************************************************
 * Copyright 2020 ETC Inc.
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

#ifndef ETCPAL_MOCK_COMMON_H_
#define ETCPAL_MOCK_COMMON_H_

#include "etcpal/common.h"
#include "fff.h"

#ifdef __cplusplus
extern "C" {
#endif

DECLARE_FAKE_VALUE_FUNC(etcpal_error_t, etcpal_init, etcpal_features_t);
DECLARE_FAKE_VOID_FUNC(etcpal_deinit, etcpal_features_t);

void etcpal_reset_all_fakes(void);

#ifdef __cplusplus
}
#endif

#endif /* ETCPAL_MOCK_COMMON_H_ */
