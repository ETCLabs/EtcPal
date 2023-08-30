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

#include "stdbool.h"
#include "stdio.h"
#include "unity_fixture.h"

#ifdef _MSC_VER
#define ETCPAL_SPRINTF __pragma(warning(suppress : 4996)) sprintf
#else
#define ETCPAL_SPRINTF sprintf
#endif

bool EtcPalTestingAssertVerify(bool condition, const char* expr, const char* file, const char* func, unsigned int line)
{
  if (!condition)
  {
    char msg[1000];
    ETCPAL_SPRINTF(msg, "Assertion failure from inside EtcPal library. Expression: %s File: %s Function: %s Line: %d",
                   expr, file, func, line);
    TEST_FAIL_MESSAGE(msg);
  }
  return condition;
}
