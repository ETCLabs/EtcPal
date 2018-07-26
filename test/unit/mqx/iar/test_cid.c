#include "lwpatest.h"
#include <string.h>
#include "lwpa_cid.h"

bool test_null()
{
  bool ok;
  LwpaCid cid = {{0}};

  ok = cid_isnull(&cid);
  if (ok)
  {
    size_t i;
    for (i = 0; i < CID_BYTES; ++i)
      cid.data[i] = i;
    ok = !cid_isnull(&cid);
  }
  return ok;
}

bool test_compare()
{
  bool ok;
  LwpaCid cid1 = {{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16}};
  LwpaCid cid1_dup = {{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16}};
  LwpaCid cid2 = {{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 17}};

  ok = (0 == cidcmp(&cid1, &cid1_dup));
  if (ok)
    ok = (0 < cidcmp(&cid2, &cid1));
  if (ok)
    ok = (0 > cidcmp(&cid1, &cid2));
  return ok;
}

bool test_string()
{
  bool ok;
  const char good_str[] = "08090a0b-0C0D-0e0f-1011-121314151617";
  const char short_str[] = "08090a0b-0c0d-0e0f-1011-1213141516";
  const char bad_str[] = "This isn't a CID";
  char str_buf[CID_STRING_BYTES];
  LwpaCid cid = {{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16}};

  cid_to_string(str_buf, &cid);
  ok = (0 == strcmp(str_buf, "01020304-0506-0708-090a-0b0c0d0e0f10"));
  if (ok)
  {
    cid = NULL_CID;
    cid_to_string(str_buf, &cid);
    ok = (0 == strcmp(str_buf, "00000000-0000-0000-0000-000000000000"));
  }
  if (ok)
    ok = string_to_cid(&cid, good_str, strlen(good_str));
  if (ok)
  {
    LwpaCid cid_cmp = {{8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23}};
    ok = (0 == cidcmp(&cid, &cid_cmp));
  }
  if (ok)
  {
    cid = NULL_CID;
    ok = !string_to_cid(&cid, short_str, strlen(short_str));
  }
  if (ok)
    ok = !string_to_cid(&cid, bad_str, strlen(bad_str));
  return ok;
}

bool test_generate()
{
  bool ok;
  uint8_t mac1[6] = {0x00, 0xc0, 0x16, 0xff, 0xef, 0x12};
  uint8_t mac2[6] = {0x00, 0xc0, 0x16, 0xff, 0xef, 0x13};
  LwpaCid cid1, cid2, cid3, cid4, cid1_dup, rdm1, rdm2, rdm1_dup;

  /* Test the generate_cid() function */

  generate_cid(&cid1, "Test Device", mac1, 0);
  generate_cid(&cid2, "Test Device", mac1, 1);
  generate_cid(&cid3, "Tst Device", mac1, 0);
  generate_cid(&cid4, "Test Device", mac2, 0);
  generate_cid(&cid1_dup, "Test Device", mac1, 0);

  ok = (0 != cidcmp(&cid1, &cid2));
  if (ok)
    ok = (0 != cidcmp(&cid1, &cid3));
  if (ok)
    ok = (0 != cidcmp(&cid1, &cid4));
  if (ok)
    ok = (0 != cidcmp(&cid2, &cid3));
  if (ok)
    ok = (0 != cidcmp(&cid2, &cid4));
  if (ok)
    ok = (0 != cidcmp(&cid3, &cid4));
  if (ok)
    ok = (0 == cidcmp(&cid1, &cid1_dup));

  /* Test the generate_rdm_cid() function */

  if (ok)
  {
    /* We'll reuse the MAC addresses as UIDs. */
    generate_rdm_cid(&rdm1, mac1);
    generate_rdm_cid(&rdm2, mac2);
    generate_rdm_cid(&rdm1_dup, mac1);
    ok = (0 != cidcmp(&rdm1, &rdm2));
  }
  if (ok)
    ok = (0 == cidcmp(&rdm1, &rdm1_dup));

  return ok;
}

void test_cid()
{
  bool ok;

  OUTPUT_TEST_MODULE_BEGIN("CID");
  watchdog_kick();
  OUTPUT_TEST_BEGIN("null");
  OUTPUT_TEST_RESULT((ok = test_null()));
  if (ok)
  {
    watchdog_kick();
    OUTPUT_TEST_BEGIN("compare");
    OUTPUT_TEST_RESULT((ok = test_compare()));
  }
  if (ok)
  {
    watchdog_kick();
    OUTPUT_TEST_BEGIN("string");
    OUTPUT_TEST_RESULT((ok = test_string()));
  }
  if (ok)
  {
    watchdog_kick();
    OUTPUT_TEST_BEGIN("generate");
    OUTPUT_TEST_RESULT((ok = test_generate()));
  }
  OUTPUT_TEST_MODULE_END("CID", ok);
}
