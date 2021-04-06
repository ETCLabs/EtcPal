// etcpal_windows_queue.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "queue_tests.h"

static void enable_console_colors()
{
  DWORD console_mode;
  GetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), &console_mode);
  console_mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
  SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), console_mode);
}


int main()
{
  enable_console_colors();

  TEST(create_queue, "Simple Queue Create")
  TEST(simple_send_receive, "Simple send and receive")
  TEST(simple_fill_and_empty, "Simple fill and empty")

  init_concurrency_test();

  TEST(concurrent_small_queue_few_writers, "Small Queue, Few items, few writers")
  TEST(concurrent_large_queue_few_writers_many_items, "Huge Queue, Many items, few writers")
  TEST(concurrent_large_queue_many_writers_few_items, "Huge Queue, Few items, Many writers")

  TEST(concurrent_small_queue_one_reader_few_writers_few_items, "Small Queue, Few Writers, One Reader")
  TEST(concurrent_small_queue_few_readers_few_writers_few_items, "Small Queue, Few Writers, Few Readers")
  TEST(concurrent_small_queue_few_readers_many_writers_many_items, "Small Queue, Many Writers, Few Readers, Many Items")
  TEST(concurrent_small_queue_many_readers_few_writers_many_items, "Small Queue, Few Writers, Many Readers, Many Items")

  TEST(send_to_empty_from_isr, "ISR Send to empty queue");
  TEST(send_to_full_from_isr, "ISR Send to full queue");
  TEST(receive_from_empty_from_isr, "ISR Receive from empty queue");
  TEST(receive_from_full_from_isr, "ISR Receive from full queue");

  TEST(leak_check_send_receive, "Send/Receive Leak Check");
  TEST(leak_check_create_destroy, "Create/Destroy Leak Check");
  Sleep(10);
}
