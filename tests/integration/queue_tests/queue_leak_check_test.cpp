#include "queue_tests.h"

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifdef _DEBUG
#define DBG_NEW new (_NORMAL_BLOCK, __FILE__, __LINE__)
// Replace _NORMAL_BLOCK with _CLIENT_BLOCK if you want the
// allocations to be of _CLIENT_BLOCK type
#else
#define DBG_NEW new
#endif

#define START_LEAK_SNAPSHOT                                   \
_CrtMemState s1;                                              \
_CrtMemState s2;                                              \
_CrtMemState s3;                                              \
                                                              \
_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);              \
  _CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDOUT);          \
  _CrtMemCheckpoint(&s1);                                     \

#define END_LEAK_SNAPSHOT(LEAK)                               \
_CrtMemCheckpoint(&s2);                                       \
                                                              \
LEAK = false;                                                 \
if (_CrtMemDifference(&s3, &s1, &s2))                         \
{                                                             \
  printf("\r\n----------------------------------------\r\n"); \
  LEAK = true;                                                \
  _CrtMemDumpStatistics(&s3);                                 \
  printf("----------------------------------------\r\n");     \
}                                                             \

bool leak_check_send_receive()
{
  bool true_if_leak;
  long long int                 val;
  etcpal::Queue<long long int>* q = new etcpal::Queue<long long int>(3);

  START_LEAK_SNAPSHOT
  q->Send(10);
  q->Receive(val);
  END_LEAK_SNAPSHOT(true_if_leak)
  return !true_if_leak;
}

bool leak_check_create_destroy()
{
  bool                          true_if_leak;

  START_LEAK_SNAPSHOT
  etcpal::Queue<long long int>* q = new etcpal::Queue<long long int>(10);
  q->Send(10);
  delete q;
  END_LEAK_SNAPSHOT(true_if_leak)
  return !true_if_leak;
}
