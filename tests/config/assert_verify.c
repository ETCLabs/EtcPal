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

#define MSG_BUF_SIZE 1000

bool EtcPalTestingAssertVerify(bool condition, const char* expr, const char* file, const char* func, unsigned int line)
{
  if (!condition)
  {
    char msg[MSG_BUF_SIZE] = {0};
    sprintf_s(msg, MSG_BUF_SIZE,
              "Assertion failure from inside EtcPal library. Expression: %s File: %s Function: %s Line: %d", expr, file,
              func, line);
    TEST_FAIL_MESSAGE(msg);
  }
  return condition;
}
