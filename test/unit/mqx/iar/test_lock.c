#include "lwpatest.h"

void test_lock()
{
  bool ok;
  OUTPUT_TEST_MODULE_BEGIN("lock");
  ok = test_mutex();
  if (ok)
    ok = test_signal();
  if (ok)
    ok = test_rwlock();
  OUTPUT_TEST_MODULE_END("lock", ok);
}
