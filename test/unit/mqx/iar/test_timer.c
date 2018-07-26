#include "lwpatest.h"
#include "lwpa_timer.h"

bool test_getms()
{
  uint32_t t1, t2;
  t1 = lwpa_getms();
  _time_delay(10);
  t2 = lwpa_getms();

  return (t1 != 0 && t2 != 0 && (((int32_t)t2 - (int32_t)t1) >= 0));
}

bool test_timeouts()
{
  LwpaTimer t1, t2;
  bool ok;

  lwpa_timer_start(&t1, 0);
  lwpa_timer_start(&t2, 100);
  ok = lwpa_timer_isexpired(&t1);
  if (ok)
    ok = !lwpa_timer_isexpired(&t2);
  if (ok)
  {
    _time_delay(110);
    ok = lwpa_timer_isexpired(&t2) && lwpa_timer_elapsed(&t2) >= 100;
  }
  if (ok)
  {
    lwpa_timer_reset(&t2);
    ok = !lwpa_timer_isexpired(&t2);
  }
  if (ok)
  {
    _time_delay(110);
    ok = lwpa_timer_isexpired(&t2) && lwpa_timer_elapsed(&t2) >= 100;
  }
  return ok;
}

void test_timer()
{
  bool ok;

  OUTPUT_TEST_MODULE_BEGIN("timer");
  watchdog_kick();
  OUTPUT_TEST_BEGIN("getms");
  OUTPUT_TEST_RESULT((ok = test_getms()));
  if (ok)
  {
    watchdog_kick();
    OUTPUT_TEST_BEGIN("timeouts");
    OUTPUT_TEST_RESULT((ok = test_timeouts()));
  }
  OUTPUT_TEST_MODULE_END("timer", ok);
}
