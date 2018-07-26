/******************************************************************************
 * Copyright 2018 ETC Inc.
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
#include "lwpatest.h"
#include <string.h>
#include <mqx.h>
#include <bsp.h>
#include <rtcs.h>
#include "lwpa_socket.h"

bool test_inet_xtox()
{
  bool ok;
  LwpaIpAddr addr;
  char str[LWPA_INET6_ADDRSTRLEN];
  const char *test_ip4_1 = "0.0.0.0";
  const char *test_ip4_2 = "255.255.255.255";
  const char *test_ip4_fail = "256.256.256.256";
  const char *test_ip6_1 = "::";
  const uint8_t test_ip6_1_bin[IPV6_BYTES] = {0};
  const char *test_ip6_2 = "::1";
  const uint8_t test_ip6_2_bin[IPV6_BYTES] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1};
  const char *test_ip6_3 = "ffff:FFFF:ffff:FFFF:ffff:FFFF:ffff:FFFF";
  const uint8_t test_ip6_3_bin[IPV6_BYTES] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                                              0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
  const char *test_ip6_fail = "abcd::ef01::2345";

  /* Test lwpa_inet_pton() */
  ok = (LWPA_OK == lwpa_inet_pton(LWPA_IPV4, test_ip4_1, &addr));
  if (ok)
    ok = (lwpaip_v4_address(&addr) == 0);
  if (ok)
    ok = (LWPA_OK == lwpa_inet_pton(LWPA_IPV4, test_ip4_2, &addr));
  if (ok)
    ok = (lwpaip_v4_address(&addr) == 0xffffffff);
  if (ok)
    ok = (LWPA_OK != lwpa_inet_pton(LWPA_IPV4, test_ip4_fail, &addr));
  if (ok)
    ok = (LWPA_OK == lwpa_inet_pton(LWPA_IPV6, test_ip6_1, &addr));
  if (ok)
    ok = (0 == memcmp(lwpaip_v6_address(&addr), test_ip6_1_bin, IPV6_BYTES));
  if (ok)
    ok = (LWPA_OK == lwpa_inet_pton(LWPA_IPV6, test_ip6_2, &addr));
  if (ok)
    ok = (0 == memcmp(lwpaip_v6_address(&addr), test_ip6_2_bin, IPV6_BYTES));
  if (ok)
    ok = (LWPA_OK == lwpa_inet_pton(LWPA_IPV6, test_ip6_3, &addr));
  if (ok)
    ok = (0 == memcmp(lwpaip_v6_address(&addr), test_ip6_3_bin, IPV6_BYTES));
  if (ok)
    ok = (LWPA_OK != lwpa_inet_pton(LWPA_IPV6, test_ip6_fail, &addr));

  /* Test lwpa_inet_ntop() */
  if (ok)
  {
    lwpaip_set_v4_address(&addr, 0);
    ok = (LWPA_OK == lwpa_inet_ntop(&addr, str, LWPA_INET_ADDRSTRLEN));
  }
  if (ok)
    ok = (0 == strcmp(str, test_ip4_1));
  if (ok)
  {
    lwpaip_set_v4_address(&addr, 0xffffffff);
    ok = (LWPA_OK == lwpa_inet_ntop(&addr, str, LWPA_INET_ADDRSTRLEN));
  }
  if (ok)
    ok = (0 == strcmp(str, test_ip4_2));
  if (ok)
  {
    lwpaip_set_v6_address(&addr, test_ip6_1_bin);
    ok = (LWPA_OK == lwpa_inet_ntop(&addr, str, LWPA_INET6_ADDRSTRLEN));
  }
  if (ok)
    ok = (0 == strcmp(str, test_ip6_1));
  if (ok)
  {
    lwpaip_set_v6_address(&addr, test_ip6_2_bin);
    ok = (LWPA_OK == lwpa_inet_ntop(&addr, str, LWPA_INET6_ADDRSTRLEN));
  }
  if (ok)
    ok = (0 == strcmp(str, test_ip6_2));
  if (ok)
  {
    lwpaip_set_v6_address(&addr, test_ip6_3_bin);
    ok = (LWPA_OK == lwpa_inet_ntop(&addr, str, LWPA_INET6_ADDRSTRLEN));
  }
  if (ok)
  {
    ok = (0 == strcmp(str, "ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff")) ||
         (0 == strcmp(str, "FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF"));
  }
  return ok;
}

bool test_sockopts()
{
  /* TODO, need getsockopt() implemented for this */
  return true;
}

#define NUM_TEST_PACKETS 1000
#define SEND_MSG "testtesttest"
#define SEND_MSG_LEN 12
struct sendthread_inf
{
  lwpa_socket_t send_sock;
  LwpaSockaddr send_addr;
};

void send_thread(uint32_t initial_data)
{
  struct sendthread_inf *inf = (struct sendthread_inf *)initial_data;
  const uint8_t *send_buf = (const uint8_t *)SEND_MSG;
  size_t i;

  for (i = 0; i < NUM_TEST_PACKETS; ++i)
    lwpa_sendto(inf->send_sock, send_buf, SEND_MSG_LEN, 0, &inf->send_addr);
}

/* TODO test IPv6 */
bool test_unicast_udp()
{
  bool ok;
  lwpa_socket_t rcvsock1, rcvsock2, rcvsock3;
  struct sendthread_inf stinf;
  LwpaSockaddr bind_addr;
  IPCFG_IP_ADDRESS_DATA ip_data;
  TASK_TEMPLATE_STRUCT ststruct;
  _task_id id;

  /* We'll just use the default interface for this test. */
  ipcfg_get_ip(BSP_DEFAULT_ENET_DEVICE, &ip_data);

  rcvsock1 = lwpa_socket(LWPA_AF_INET, LWPA_DGRAM);
  ok = (rcvsock1 != LWPA_SOCKET_INVALID);
  if (ok)
  {
    rcvsock2 = lwpa_socket(LWPA_AF_INET, LWPA_DGRAM);
    ok = (rcvsock2 != LWPA_SOCKET_INVALID);
  }
  if (ok)
  {
    int intval = 1;
    ok = (0 == lwpa_setsockopt(rcvsock2, LWPA_SOL_SOCKET, LWPA_SO_RCVTIMEO, &intval, sizeof(int)));
  }
  if (ok)
  {
    rcvsock3 = lwpa_socket(LWPA_AF_INET, LWPA_DGRAM);
    ok = (rcvsock3 != LWPA_SOCKET_INVALID);
  }
  if (ok)
  {
    stinf.send_sock = lwpa_socket(LWPA_AF_INET, LWPA_DGRAM);
    ok = (stinf.send_sock != LWPA_SOCKET_INVALID);
  }
  if (ok)
    ok = (0 == lwpa_close(rcvsock3));
  if (ok)
  {
    lwpaip_make_any_v4(&bind_addr.ip);
    bind_addr.port = 8888;
    /* Shouldn't be able to bind to a closed socket. */
    ok = (0 != lwpa_bind(rcvsock3, &bind_addr));
  }
  if (ok)
  {
    /* Bind socket 1 to the wildcard address and port 8888. */
    ok = (0 == lwpa_bind(rcvsock1, &bind_addr));
  }
  if (ok)
  {
    /* Bind socket 2 to the wildcard address and port 9999. */
    bind_addr.port = 9999;
    ok = (0 == lwpa_bind(rcvsock2, &bind_addr));
  }
  if (ok)
  {
    lwpaip_set_v4_address(&stinf.send_addr.ip, ip_data.ip);
    stinf.send_addr.port = 8888;

    /* Start the send thread. */
    ststruct.TASK_ADDRESS = send_thread;
    ststruct.TASK_NAME = "socket_test";
    ststruct.TASK_PRIORITY = 10;
    ststruct.TASK_STACKSIZE = 1000;
    ststruct.TASK_ATTRIBUTES = 0;
    ststruct.DEFAULT_TIME_SLICE = 1;
    ststruct.CREATION_PARAMETER = (uint32_t)&stinf;
    id = _task_create(0, 0, (uint32_t)&ststruct);
    if (id == MQX_NULL_TASK_ID)
      ok = false;
  }
  if (ok)
  {
    size_t i;
    for (i = 0; i < NUM_TEST_PACKETS; ++i)
    {
      LwpaSockaddr from_addr;
      uint8_t buf[SEND_MSG_LEN + 1];
      ok = (SEND_MSG_LEN == lwpa_recvfrom(rcvsock1, buf, SEND_MSG_LEN, 0, &from_addr));
      if (!ok)
        break;
      ok = (lwpaip_equal(&stinf.send_addr.ip, &from_addr.ip) && (from_addr.port != 8888));
      if (!ok)
        break;
      buf[SEND_MSG_LEN] = '\0';
      ok = (0 == strcmp((char *)buf, SEND_MSG));
      if (!ok)
        break;
    }
  }
  if (ok)
  {
    LwpaSockaddr from_addr;
    uint8_t buf[SEND_MSG_LEN + 1];
    /* recvfrom should time out because this socket is bound to a different
     * port and we set the timeout option on this socket. */
    ok = (0 >= lwpa_recvfrom(rcvsock2, buf, SEND_MSG_LEN, 0, &from_addr));
  }
  if (ok)
  {
    /* Let the send thread end */
    _time_delay(100);
    ok = (0 == lwpa_close(rcvsock1));
  }
  if (ok)
    ok = (0 == lwpa_close(rcvsock2));
  if (ok)
    ok = (0 == lwpa_close(stinf.send_sock));
  return ok;
}

#define TEST_MCAST_ADDR 0xec02054d /* 236.2.5.77 */

bool test_multicast_udp()
{
  bool ok;
  lwpa_socket_t rcvsock1, rcvsock2;
  struct sendthread_inf stinf;
  LwpaSockaddr bind_addr;
  IPCFG_IP_ADDRESS_DATA ip_data;
  TASK_TEMPLATE_STRUCT ststruct;
  _task_id id;

  /* We'll just use the default interface for this test. */
  ipcfg_get_ip(BSP_DEFAULT_ENET_DEVICE, &ip_data);

  rcvsock1 = lwpa_socket(LWPA_AF_INET, LWPA_DGRAM);
  ok = (rcvsock1 != LWPA_SOCKET_INVALID);
  if (ok)
  {
    int intval = 1;
    ok = (0 == lwpa_setsockopt(rcvsock1, LWPA_SOL_SOCKET, LWPA_SO_REUSEADDR, &intval, sizeof(int)));
  }
  if (ok)
  {
    rcvsock2 = lwpa_socket(LWPA_AF_INET, LWPA_DGRAM);
    ok = (rcvsock2 != LWPA_SOCKET_INVALID);
  }
  if (ok)
  {
    int intval = 1;
    ok = (0 == lwpa_setsockopt(rcvsock2, LWPA_SOL_SOCKET, LWPA_SO_RCVTIMEO, &intval, sizeof(int)));
  }
  if (ok)
  {
    int intval = 1;
    ok = (0 == lwpa_setsockopt(rcvsock2, LWPA_SOL_SOCKET, LWPA_SO_REUSEADDR, &intval, sizeof(int)));
  }
  if (ok)
  {
    stinf.send_sock = lwpa_socket(LWPA_AF_INET, LWPA_DGRAM);
    ok = (stinf.send_sock != LWPA_SOCKET_INVALID);
  }
  if (ok)
  {
    /* Bind socket 1 to the multicast address and port 8888. */
    lwpaip_set_v4_address(&bind_addr.ip, TEST_MCAST_ADDR);
    bind_addr.port = 8888;
    ok = (0 == lwpa_bind(rcvsock1, &bind_addr));
  }
  if (ok)
  {
    /* Bind socket 2 to the multicast address and port 9999. */
    bind_addr.port = 9999;
    ok = (0 == lwpa_bind(rcvsock2, &bind_addr));
  }
  if (ok)
  {
    /* Subscribe socket 1 to the multicast address. */
    LwpaMreq mreq;
    lwpaip_set_v4_address(&mreq.netint, ip_data.ip);
    lwpaip_set_v4_address(&mreq.group, TEST_MCAST_ADDR);
    ok = (0 == lwpa_setsockopt(rcvsock1, LWPA_IPPROTO_IP, LWPA_MCAST_JOIN_GROUP, &mreq, sizeof mreq));
    /* Subscribe socket 2 to the multicast address */
    if (ok)
    {
      ok = (0 == lwpa_setsockopt(rcvsock2, LWPA_IPPROTO_IP, LWPA_MCAST_JOIN_GROUP, &mreq, sizeof mreq));
    }
  }
  if (ok)
  {
    lwpaip_set_v4_address(&stinf.send_addr.ip, TEST_MCAST_ADDR);
    stinf.send_addr.port = 8888;

    /* Start the send thread. */
    ststruct.TASK_ADDRESS = send_thread;
    ststruct.TASK_NAME = "socket_test";
    ststruct.TASK_PRIORITY = 10;
    ststruct.TASK_STACKSIZE = 1000;
    ststruct.TASK_ATTRIBUTES = 0;
    ststruct.DEFAULT_TIME_SLICE = 1;
    ststruct.CREATION_PARAMETER = (uint32_t)&stinf;
    id = _task_create(0, 0, (uint32_t)&ststruct);
    if (id == MQX_NULL_TASK_ID)
      ok = false;
  }
  if (ok)
  {
    size_t i;
    for (i = 0; i < NUM_TEST_PACKETS; ++i)
    {
      LwpaSockaddr from_addr;
      uint8_t buf[SEND_MSG_LEN + 1];
      ok = (SEND_MSG_LEN == lwpa_recvfrom(rcvsock1, buf, SEND_MSG_LEN, 0, &from_addr));
      if (!ok)
        break;
      ok = (from_addr.port != 8888);
      if (!ok)
        break;
      buf[SEND_MSG_LEN] = '\0';
      ok = (0 == strcmp((char *)buf, SEND_MSG));
      if (!ok)
        break;
    }
  }
  if (ok)
  {
    LwpaSockaddr from_addr;
    uint8_t buf[SEND_MSG_LEN + 1];
    /* recvfrom should time out because this socket is bound to a different
     * port and we set the timeout option on this socket. */
    ok = (0 >= lwpa_recvfrom(rcvsock2, buf, SEND_MSG_LEN, 0, &from_addr));
  }
  if (ok)
  {
    /* Let the send thread end */
    _time_delay(100);
    ok = (0 == lwpa_close(rcvsock1));
  }
  if (ok)
    ok = (0 == lwpa_close(rcvsock2));
  if (ok)
    ok = (0 == lwpa_close(stinf.send_sock));
  return ok;
}

struct pt_sendthread_inf
{
  lwpa_socket_t send_sock;
  LwpaSockaddr send_addr_1;
  LwpaSockaddr send_addr_2;
};

void polltest_sendthread(uint32_t initial_data)
{
  struct pt_sendthread_inf *inf = (struct pt_sendthread_inf *)initial_data;
  const uint8_t *send_buf = (const uint8_t *)SEND_MSG;

  lwpa_sendto(inf->send_sock, send_buf, SEND_MSG_LEN, 0, &inf->send_addr_1);
  lwpa_sendto(inf->send_sock, send_buf, SEND_MSG_LEN, 0, &inf->send_addr_2);
}

bool test_poll()
{
  bool ok;
  lwpa_socket_t rcvsock1, rcvsock2;
  struct pt_sendthread_inf stinf;
  LwpaSockaddr bind_addr;
  IPCFG_IP_ADDRESS_DATA ip_data;
  TASK_TEMPLATE_STRUCT ststruct;
  _task_id id;

  /* We'll just use the default interface for this test. */
  ipcfg_get_ip(BSP_DEFAULT_ENET_DEVICE, &ip_data);

  rcvsock1 = lwpa_socket(LWPA_AF_INET, LWPA_DGRAM);
  ok = (rcvsock1 != LWPA_SOCKET_INVALID);
  if (ok)
  {
    rcvsock2 = lwpa_socket(LWPA_AF_INET, LWPA_DGRAM);
    ok = (rcvsock2 != LWPA_SOCKET_INVALID);
  }
  if (ok)
  {
    stinf.send_sock = lwpa_socket(LWPA_AF_INET, LWPA_DGRAM);
    ok = (stinf.send_sock != LWPA_SOCKET_INVALID);
  }
  if (ok)
  {
    /* Bind socket 1 to the wildcard address and port 8888. */
    lwpaip_make_any_v4(&bind_addr.ip);
    bind_addr.port = 8888;
    ok = (0 == lwpa_bind(rcvsock1, &bind_addr));
  }
  if (ok)
  {
    /* Bind socket 2 to the wildcard address and port 9999. */
    bind_addr.port = 9999;
    ok = (0 == lwpa_bind(rcvsock2, &bind_addr));
  }
  if (ok)
  {
    /* Test poll with nothing sending - should time out. */
    LwpaPollfd pfds[2];
    pfds[0].fd = rcvsock1;
    pfds[0].events = LWPA_POLLIN;
    pfds[1].fd = rcvsock2;
    pfds[1].events = LWPA_POLLIN;
    ok = (LWPA_TIMEDOUT == lwpa_poll(pfds, 2, 100));
  }
  if (ok)
  {
    lwpaip_set_v4_address(&stinf.send_addr_1.ip, ip_data.ip);
    lwpaip_set_v4_address(&stinf.send_addr_2.ip, ip_data.ip);
    stinf.send_addr_1.port = 8888;
    stinf.send_addr_2.port = 9999;

    /* Start the send thread. */
    ststruct.TASK_ADDRESS = polltest_sendthread;
    ststruct.TASK_NAME = "poll_test";
    ststruct.TASK_PRIORITY = 10;
    ststruct.TASK_STACKSIZE = 1000;
    ststruct.TASK_ATTRIBUTES = 0;
    ststruct.DEFAULT_TIME_SLICE = 1;
    ststruct.CREATION_PARAMETER = (uint32_t)&stinf;
    id = _task_create(0, 0, (uint32_t)&ststruct);
    if (id == MQX_NULL_TASK_ID)
      ok = false;
  }
  if (ok)
  {
    LwpaPollfd pfds[2];
    int poll_res;
    bool already_polled = false;

  poll_again:
    pfds[0].fd = rcvsock1;
    pfds[0].events = LWPA_POLLIN;
    pfds[1].fd = rcvsock2;
    pfds[1].events = LWPA_POLLIN;
    poll_res = lwpa_poll(pfds, 2, LWPA_WAIT_FOREVER);
    ok = (0 < poll_res);
    if (ok)
    {
      int i;
      for (i = 0; i < 2; ++i)
      {
        if (pfds[i].revents & LWPA_POLLIN)
        {
          LwpaSockaddr from_addr;
          uint8_t buf[SEND_MSG_LEN + 1];
          ok = (SEND_MSG_LEN == lwpa_recvfrom(pfds[i].fd, buf, SEND_MSG_LEN, 0, &from_addr));
          if (!ok)
            break;
          ok = (lwpaip_equal(&stinf.send_addr_1.ip, &from_addr.ip) && (from_addr.port != 8888));
          if (!ok)
            break;
          buf[SEND_MSG_LEN] = '\0';
          ok = (0 == strcmp((char *)buf, SEND_MSG));
          if (!ok)
            break;
        }
      }
    }
    if (poll_res < 2 && !already_polled)
    {
      already_polled = true;
      goto poll_again; /* Yeah, yeah, shut up. It's a test app. */
    }
  }
  if (ok)
  {
    /* Let the send thread end */
    _time_delay(100);
    ok = (0 == lwpa_close(rcvsock1));
  }
  if (ok)
    ok = (0 == lwpa_close(rcvsock2));
  if (ok)
    ok = (0 == lwpa_close(stinf.send_sock));
  return ok;
}

void test_socket()
{
  bool ok;

  OUTPUT_TEST_MODULE_BEGIN("socket");
  watchdog_kick();
  OUTPUT_TEST_BEGIN("inet_xtox");
  OUTPUT_TEST_RESULT((ok = test_inet_xtox()));
  if (ok)
  {
    watchdog_kick();
    OUTPUT_TEST_BEGIN("sockopts");
    OUTPUT_TEST_RESULT((ok = test_sockopts()));
  }
  if (ok)
  {
    watchdog_kick();
    OUTPUT_TEST_BEGIN("unicast_udp");
    OUTPUT_TEST_RESULT((ok = test_unicast_udp()));
  }
  if (ok)
  {
    watchdog_kick();
    OUTPUT_TEST_BEGIN("multicast_udp");
    OUTPUT_TEST_RESULT((ok = test_multicast_udp()));
  }
  if (ok)
  {
    watchdog_kick();
    OUTPUT_TEST_BEGIN("poll");
    OUTPUT_TEST_RESULT((ok = test_poll()));
  }
  /* TODO tcp */
  OUTPUT_TEST_MODULE_END("socket", ok);
}
