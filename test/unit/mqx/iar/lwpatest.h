#ifndef _LWPATEST_H_
#define _LWPATEST_H_

#include <mqx.h>
#include <bsp.h>
#include "testio.h"

uint32_t getms();
void watchdog_kick();
void watchdog_disable();

/* Lock tests */
void test_lock();
bool test_mutex();
bool test_signal();
bool test_rwlock();

/* Thread test */
void test_thread();

/* Mempool test */
void test_mempool();

/* Red-black tree test */
void test_rbtree();

/* Log test */
void test_log();

/* Pack test */
void test_pack();

/* Timer test */
void test_timer();

/* CID test */
void test_cid();

/* Netint test */
void test_netint();

/* Socket test */
void test_socket();

#endif /* _LWPATEST_H_ */
