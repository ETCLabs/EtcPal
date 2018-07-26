#include "lwpatest.h"
#include <stdlib.h>
#include "lwpa_mempool.h"

struct test_elem
{
  int val1;
  char val2;
};

#define TEST_ALLOC_MEMP_SIZE 500
LWPA_MEMPOOL_DEFINE(alloc_test, struct test_elem, TEST_ALLOC_MEMP_SIZE);
struct alloctest
{
  struct test_elem *elem;
  bool freed;
} test_array[TEST_ALLOC_MEMP_SIZE];

bool test_alloc()
{
  bool ok;

  ok = (LWPA_OK == lwpa_mempool_init(alloc_test));
  if (ok)
    ok = (TEST_ALLOC_MEMP_SIZE == lwpa_mempool_size(alloc_test));
  if (ok)
  {
    int i;
    /* Allocate the entire pool. */
    for (i = 0; i < TEST_ALLOC_MEMP_SIZE; ++i)
    {
      test_array[i].elem = (struct test_elem *)lwpa_mempool_alloc(alloc_test);
      if (!test_array[i].elem)
      {
        ok = false;
        break;
      }
      test_array[i].freed = false;
    }
  }
  if (ok)
    ok = (TEST_ALLOC_MEMP_SIZE == lwpa_mempool_used(alloc_test));
  if (ok)
  {
    int num_freed = 0;
    /* Free the elements back in random order. */
    while (num_freed < TEST_ALLOC_MEMP_SIZE)
    {
      int i = (rand() % TEST_ALLOC_MEMP_SIZE);
      if (!test_array[i].freed)
      {
        lwpa_mempool_free(alloc_test, test_array[i].elem);
        test_array[i].freed = true;
        ++num_freed;
      }
    }
    ok = (0 == lwpa_mempool_used(alloc_test));
  }
  if (ok)
  {
    int i;
    /* Make sure we can allocate the entire pool again. */
    for (i = 0; i < TEST_ALLOC_MEMP_SIZE; ++i)
    {
      test_array[i].elem = (struct test_elem *)lwpa_mempool_alloc(alloc_test);
      if (!test_array[i].elem)
      {
        ok = false;
        break;
      }
    }
  }
  return ok;
}

void test_mempool()
{
  bool ok;
  OUTPUT_TEST_MODULE_BEGIN("mempool");
  watchdog_kick();
  OUTPUT_TEST_BEGIN("mempool_alloc");
  OUTPUT_TEST_RESULT((ok = test_alloc()));
  OUTPUT_TEST_MODULE_END("mempool", ok);
}
