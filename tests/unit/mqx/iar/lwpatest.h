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
 * This file is a part of lwpa. For more information, go to:
 * https://github.com/ETCLabs/lwpa
 ******************************************************************************/
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
