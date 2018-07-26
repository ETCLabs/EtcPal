#include "lwpatest.h"
#include "lwpa_pack.h"

bool test_pack16b(uint8_t *buffer)
{
  const uint16_t tst = 0x1234;
  const uint8_t *ptst = (const uint8_t *)(&tst);
  memset(buffer, 0, sizeof(uint16_t));

  pack_16b(buffer, tst);
  if ((ptst[0] == buffer[1]) && (ptst[1] == buffer[0]))
    return true;
  return false;
}

bool test_pack16l(uint8_t *buffer)
{
  const uint16_t tst = 0x1234;
  const uint8_t *ptst = (const uint8_t *)(&tst);
  memset(buffer, 0, sizeof(uint16_t));

  pack_16l(buffer, tst);
  if ((ptst[0] == buffer[0]) && (ptst[1] == buffer[1]))
    return true;
  return false;
}

bool test_upack16b(uint8_t *buffer)
{
  const uint16_t tst = 0x1234;
  uint16_t var;
  memset(buffer, 0, sizeof(uint16_t));

  pack_16b(buffer, tst);
  var = upack_16b(buffer);
  if (tst == var)
    return true;
  return false;
}

bool test_upack16l(uint8_t *buffer)
{
  const uint16_t tst = 0x1234;
  uint16_t var;
  memset(buffer, 0, sizeof(uint16_t));

  pack_16l(buffer, tst);
  var = upack_16l(buffer);
  if (tst == var)
    return true;
  return false;
}

bool test_pack32b(uint8_t *buffer)
{
  const uint32_t tst = 0x12345678;
  const uint8_t *ptst = (const uint8_t *)(&tst);
  memset(buffer, 0, sizeof(uint32_t));

  pack_32b(buffer, tst);
  if ((ptst[0] == buffer[3]) && (ptst[1] == buffer[2]) && (ptst[2] == buffer[1]) && (ptst[3] == buffer[0]))
  {
    return true;
  }
  return false;
}

bool test_pack32l(uint8_t *buffer)
{
  const uint32_t tst = 0x12345678;
  const uint8_t *ptst = (const uint8_t *)(&tst);
  memset(buffer, 0, sizeof(uint32_t));

  pack_32l(buffer, tst);
  if ((ptst[0] == buffer[0]) && (ptst[1] == buffer[1]) && (ptst[2] == buffer[2]) && (ptst[3] == buffer[3]))
  {
    return true;
  }
  return false;
}

bool test_upack32b(uint8_t *buffer)
{
  const uint32_t tst = 0x12345678;
  uint32_t var;
  memset(buffer, 0, sizeof(uint32_t));

  pack_32b(buffer, tst);
  var = upack_32b(buffer);
  if (tst == var)
    return true;
  return false;
}

bool test_upack32l(uint8_t *buffer)
{
  const uint32_t tst = 0x12345678;
  uint32_t var;
  memset(buffer, 0, sizeof(uint32_t));

  pack_32l(buffer, tst);
  var = upack_32l(buffer);
  if (tst == var)
    return true;
  return false;
}

bool test_pack64b(uint8_t *buffer)
{
  const uint64_t tst = 0x1234567890abcdef;
  const uint8_t *ptst = (const uint8_t *)(&tst);
  memset(buffer, 0, sizeof(uint64_t));

  pack_64b(buffer, tst);
  if ((ptst[0] == buffer[7]) && (ptst[1] == buffer[6]) && (ptst[2] == buffer[5]) && (ptst[3] == buffer[4]) &&
      (ptst[4] == buffer[3]) && (ptst[5] == buffer[2]) && (ptst[6] == buffer[1]) && (ptst[7] == buffer[0]))
  {
    return true;
  }
  return false;
}

bool test_pack64l(uint8_t *buffer)
{
  const uint64_t tst = 0x1234567890abcdef;
  const uint8_t *ptst = (const uint8_t *)(&tst);
  memset(buffer, 0, sizeof(uint64_t));

  pack_64l(buffer, tst);
  if ((ptst[0] == buffer[0]) && (ptst[1] == buffer[1]) && (ptst[2] == buffer[2]) && (ptst[3] == buffer[3]) &&
      (ptst[4] == buffer[4]) && (ptst[5] == buffer[5]) && (ptst[6] == buffer[6]) && (ptst[7] == buffer[7]))
  {
    return true;
  }
  return false;
}

bool test_upack64b(uint8_t *buffer)
{
  const uint64_t tst = 0x1234567890abcdef;
  uint64_t var;
  memset(buffer, 0, sizeof(uint64_t));

  pack_64b(buffer, tst);
  var = upack_64b(buffer);
  if (tst == var)
    return true;
  return false;
}

bool test_upack64l(uint8_t *buffer)
{
  const uint64_t tst = 0x1234567890abcdef;
  uint64_t var;
  memset(buffer, 0, sizeof(uint64_t));

  pack_64l(buffer, tst);
  var = upack_64l(buffer);
  if (tst == var)
    return true;
  return false;
}

bool test_pack16(uint8_t *buffer)
{
  int offset = 0;
  bool ok;

  /* We test the packing and unpacking at weird offsets. */
  for (offset = 0; offset <= 60; ++offset)
  {
    ok = test_pack16b(buffer + offset);
    if (!ok)
      break;
    ok = test_pack16l(buffer + offset);
    if (!ok)
      break;
    ok = test_upack16b(buffer + offset);
    if (!ok)
      break;
    ok = test_upack16l(buffer + offset);
    if (!ok)
      break;
  }
  return ok;
}

bool test_pack32(uint8_t *buffer)
{
  int offset = 0;
  bool ok;

  /* We test the packing and unpacking at weird offsets. */
  for (offset = 0; offset <= 60; ++offset)
  {
    ok = test_pack32b(buffer + offset);
    if (!ok)
      break;
    ok = test_pack32l(buffer + offset);
    if (!ok)
      break;
    ok = test_upack32b(buffer + offset);
    if (!ok)
      break;
    ok = test_upack32l(buffer + offset);
    if (!ok)
      break;
  }
  return ok;
}

bool test_pack64(uint8_t *buffer)
{
  int offset = 0;
  bool ok;

  /* We test the packing and unpacking at weird offsets. */
  for (offset = 0; offset <= 60; ++offset)
  {
    ok = test_pack64b(buffer + offset);
    if (!ok)
      break;
    ok = test_pack64l(buffer + offset);
    if (!ok)
      break;
    ok = test_upack64b(buffer + offset);
    if (!ok)
      break;
    ok = test_upack64l(buffer + offset);
    if (!ok)
      break;
  }
  return ok;
}

void test_pack()
{
  uint8_t buffer[100]; /* What we pack and unpack from. */
  bool ok;

  OUTPUT_TEST_MODULE_BEGIN("pack");
  watchdog_kick();
  OUTPUT_TEST_BEGIN("pack_16");
  OUTPUT_TEST_RESULT((ok = test_pack16(buffer)));
  if (ok)
  {
    watchdog_kick();
    OUTPUT_TEST_BEGIN("pack_32");
    OUTPUT_TEST_RESULT((ok = test_pack32(buffer)));
  }
  if (ok)
  {
    watchdog_kick();
    OUTPUT_TEST_BEGIN("pack_64");
    OUTPUT_TEST_RESULT((ok = test_pack64(buffer)));
  }
  OUTPUT_TEST_MODULE_END("pack", ok);
}
