#pragma once
#include "etcpal/cpp/queue.h"

#define TEST(FUNC, NAME)                                              \
{                                                                     \
  printf("\x1b[39m ");                                                 \
  printf("%s: ... ", NAME);                                          \
  printf("\r \x1b[60C");                                              \
  if (FUNC())                                                         \
  {                                                                   \
    printf("\x1b[92m");                                               \
    printf("SUCCESS\r\n");                                            \
  }else{                                                              \
    printf("\x1b[91m");                                               \
    printf("FAIL\r\n");                                               \
  }                                                                   \
}

void init_concurrency_test();
bool create_queue();
bool simple_send_receive();
bool simple_fill_and_empty();

bool concurrent_small_queue_few_writers();
bool concurrent_large_queue_few_writers_many_items();
bool concurrent_large_queue_many_writers_few_items();

bool concurrent_small_queue_one_reader_few_writers_few_items();
bool concurrent_small_queue_few_readers_few_writers_few_items();
bool concurrent_small_queue_few_readers_many_writers_many_items();
bool concurrent_small_queue_many_readers_few_writers_many_items();

bool send_to_empty_from_isr();
bool send_to_full_from_isr();
bool receive_from_empty_from_isr();
bool receive_from_full_from_isr();

bool leak_check_send_receive();
bool leak_check_create_destroy();
