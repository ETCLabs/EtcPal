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

#include "etcpal/pack.h"
#include "etcpal/pack64.h"
#include "unity_fixture.h"

#include <stdbool.h>
#include <stddef.h>
#include <string.h>

// What we pack and unpack from
uint8_t test_buf[100];

// Test utility functions that are used by the actual tests

static bool test_packu16b(uint8_t* buffer)
{
  const uint16_t tst = 0x1234;
  const uint8_t* ptst = (const uint8_t*)(&tst);
  memset(buffer, 0, sizeof(uint16_t));

  etcpal_pack_u16b(buffer, tst);
  if ((ptst[0] == buffer[1]) && (ptst[1] == buffer[0]))
    return true;
  return false;
}

static bool test_packu16l(uint8_t* buffer)
{
  const uint16_t tst = 0x1234;
  const uint8_t* ptst = (const uint8_t*)(&tst);
  memset(buffer, 0, sizeof(uint16_t));

  etcpal_pack_u16l(buffer, tst);
  if ((ptst[0] == buffer[0]) && (ptst[1] == buffer[1]))
    return true;
  return false;
}

static bool test_unpacku16b(uint8_t* buffer)
{
  const uint16_t tst = 0x1234;
  memset(buffer, 0, sizeof(uint16_t));

  etcpal_pack_u16b(buffer, tst);
  uint16_t var = etcpal_unpack_u16b(buffer);
  if (tst == var)
    return true;
  return false;
}

static bool test_unpacku16l(uint8_t* buffer)
{
  const uint16_t tst = 0x1234;
  memset(buffer, 0, sizeof(uint16_t));

  etcpal_pack_u16l(buffer, tst);
  uint16_t var = etcpal_unpack_u16l(buffer);
  if (tst == var)
    return true;
  return false;
}

static bool test_packu32b(uint8_t* buffer)
{
  const uint32_t tst = 0x12345678;
  const uint8_t* ptst = (const uint8_t*)(&tst);
  memset(buffer, 0, sizeof(uint32_t));

  etcpal_pack_u32b(buffer, tst);
  if ((ptst[0] == buffer[3]) && (ptst[1] == buffer[2]) && (ptst[2] == buffer[1]) && (ptst[3] == buffer[0]))
  {
    return true;
  }
  return false;
}

static bool test_packu32l(uint8_t* buffer)
{
  const uint32_t tst = 0x12345678;
  const uint8_t* ptst = (const uint8_t*)(&tst);
  memset(buffer, 0, sizeof(uint32_t));

  etcpal_pack_u32l(buffer, tst);
  if ((ptst[0] == buffer[0]) && (ptst[1] == buffer[1]) && (ptst[2] == buffer[2]) && (ptst[3] == buffer[3]))
  {
    return true;
  }
  return false;
}

static bool test_unpacku32b(uint8_t* buffer)
{
  const uint32_t tst = 0x12345678;
  memset(buffer, 0, sizeof(uint32_t));

  etcpal_pack_u32b(buffer, tst);
  uint32_t var = etcpal_unpack_u32b(buffer);
  if (tst == var)
    return true;
  return false;
}

static bool test_unpacku32l(uint8_t* buffer)
{
  const uint32_t tst = 0x12345678;
  memset(buffer, 0, sizeof(uint32_t));

  etcpal_pack_u32l(buffer, tst);
  uint32_t var = etcpal_unpack_u32l(buffer);
  if (tst == var)
    return true;
  return false;
}

static bool test_packu64b(uint8_t* buffer)
{
  const uint64_t tst = 0x1234567890abcdef;
  const uint8_t* ptst = (const uint8_t*)(&tst);
  memset(buffer, 0, sizeof(uint64_t));

  etcpal_pack_u64b(buffer, tst);
  if ((ptst[0] == buffer[7]) && (ptst[1] == buffer[6]) && (ptst[2] == buffer[5]) && (ptst[3] == buffer[4]) &&
      (ptst[4] == buffer[3]) && (ptst[5] == buffer[2]) && (ptst[6] == buffer[1]) && (ptst[7] == buffer[0]))
  {
    return true;
  }
  return false;
}

static bool test_packu64l(uint8_t* buffer)
{
  const uint64_t tst = 0x1234567890abcdef;
  const uint8_t* ptst = (const uint8_t*)(&tst);
  memset(buffer, 0, sizeof(uint64_t));

  etcpal_pack_u64l(buffer, tst);
  if ((ptst[0] == buffer[0]) && (ptst[1] == buffer[1]) && (ptst[2] == buffer[2]) && (ptst[3] == buffer[3]) &&
      (ptst[4] == buffer[4]) && (ptst[5] == buffer[5]) && (ptst[6] == buffer[6]) && (ptst[7] == buffer[7]))
  {
    return true;
  }
  return false;
}

static bool test_unpacku64b(uint8_t* buffer)
{
  const uint64_t tst = 0x1234567890abcdef;
  memset(buffer, 0, sizeof(uint64_t));

  etcpal_pack_u64b(buffer, tst);
  uint64_t var = etcpal_unpack_u64b(buffer);
  if (tst == var)
    return true;
  return false;
}

static bool test_unpacku64l(uint8_t* buffer)
{
  const uint64_t tst = 0x1234567890abcdef;
  memset(buffer, 0, sizeof(uint64_t));

  etcpal_pack_u64l(buffer, tst);
  uint64_t var = etcpal_unpack_u64l(buffer);
  if (tst == var)
    return true;
  return false;
}

// The tests themselves

TEST_GROUP(etcpal_pack);

TEST_SETUP(etcpal_pack)
{
}

TEST_TEAR_DOWN(etcpal_pack)
{
}

TEST(etcpal_pack, pack_16_functions_work)
{
  // Test packing and unpacking values at varying memory offsets.
  for (size_t offset = 0; offset <= 60; ++offset)
  {
    TEST_ASSERT(test_packu16b(test_buf + offset));
    TEST_ASSERT(test_packu16l(test_buf + offset));
    TEST_ASSERT(test_unpacku16b(test_buf + offset));
    TEST_ASSERT(test_unpacku16l(test_buf + offset));
  }
}

TEST(etcpal_pack, pack_32_functions_work)
{
  // Test packing and unpacking values at varying memory offsets.
  for (size_t offset = 0; offset <= 60; ++offset)
  {
    TEST_ASSERT(test_packu32b(test_buf + offset));
    TEST_ASSERT(test_packu32l(test_buf + offset));
    TEST_ASSERT(test_unpacku32b(test_buf + offset));
    TEST_ASSERT(test_unpacku32l(test_buf + offset));
  }
}

TEST(etcpal_pack, pack_64_functions_work)
{
  // Test packing and unpacking values at varying memory offsets.
  for (size_t offset = 0; offset <= 60; ++offset)
  {
    TEST_ASSERT(test_packu64b(test_buf + offset));
    TEST_ASSERT(test_packu64l(test_buf + offset));
    TEST_ASSERT(test_unpacku64b(test_buf + offset));
    TEST_ASSERT(test_unpacku64l(test_buf + offset));
  }
}

TEST_GROUP_RUNNER(etcpal_pack)
{
  RUN_TEST_CASE(etcpal_pack, pack_16_functions_work);
  RUN_TEST_CASE(etcpal_pack, pack_32_functions_work);
  RUN_TEST_CASE(etcpal_pack, pack_64_functions_work);
}
