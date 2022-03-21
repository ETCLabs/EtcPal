/******************************************************************************
 * Copyright 2022 ETC Inc.
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
 * This file is a part of EtcPal. For more information, go to:
 * https://github.com/ETCLabs/EtcPal
 ******************************************************************************/

/*
 * A platform-neutral example application showing usage of the etcpal_recvmsg and etcpal_cmsg_ functions.
 */

#include <stdio.h>
#include "etcpal/common.h"
#include "etcpal/socket.h"

#define RECVMSG_EXAMPLE_PORT 9000
#define RECVMSG_EXAMPLE_MTU  10000

etcpal_error_t init_receive_socket(etcpal_socket_t* socket)
{
  etcpal_error_t res = etcpal_socket(ETCPAL_AF_INET, ETCPAL_SOCK_DGRAM, socket);

  // Configure socket options.
  if (res == kEtcPalErrOk)
  {
    int intval = 1;
    res        = etcpal_setsockopt(*socket, ETCPAL_IPPROTO_IP, ETCPAL_IP_PKTINFO, &intval, sizeof(int));
  }

  // Bind socket to the wildcard address and a specific port.
  if (res == kEtcPalErrOk)
  {
    EtcPalSockAddr bind_addr;
    etcpal_ip_set_wildcard(kEtcPalIpTypeV4, &bind_addr.ip);
    bind_addr.port = RECVMSG_EXAMPLE_PORT;
    res            = etcpal_bind(*socket, &bind_addr);
  }

  return res;
}

etcpal_error_t init_poll_context(etcpal_socket_t socket, EtcPalPollContext* context)
{
  etcpal_error_t res = etcpal_poll_context_init(context);

  // Get the poll context set up
  if (res == kEtcPalErrOk)
    res = etcpal_poll_add_socket(context, socket, ETCPAL_POLL_IN, NULL);

  return res;
}

etcpal_error_t print_packet_info(int bytes_received, EtcPalMsgHdr* msg)
{
  char     source_ip_str[ETCPAL_IP_STRING_BYTES] = {0};
  uint8_t* buf                                   = (uint8_t*)msg->buf;

  etcpal_error_t res = etcpal_ip_to_string(&msg->name.ip, source_ip_str);

  if (res == kEtcPalErrOk)
  {
    printf("New packet received!\nSource IP: %s\nSource Port: %d\n", source_ip_str, msg->name.port);

    printf("Packet data dump:");
    for (int i = 0; i < bytes_received; ++i)
      printf(" 0x%02x", buf[i]);

    printf("\n");

    EtcPalCMsgHdr cmsg = {0};
    if (etcpal_cmsg_firsthdr(msg, &cmsg))
    {
      do
      {
        if (cmsg.type == ETCPAL_IP_PKTINFO)
        {
          EtcPalPktInfo pktinfo = {0};
          if (etcpal_cmsg_to_pktinfo(&cmsg, &pktinfo))
          {
            char dest_ip_str[ETCPAL_IP_STRING_BYTES] = {0};
            res                                      = etcpal_ip_to_string(&pktinfo.addr, dest_ip_str);
            printf("New packet info!\nDestination IP: %s\nDestination Interface Index: %d\n", dest_ip_str,
                   pktinfo.ifindex);
          }
          else
          {
            res = kEtcPalErrSys;
          }
        }
      } while ((res == kEtcPalErrOk) && etcpal_cmsg_nxthdr(msg, &cmsg, &cmsg));
    }
  }

  return res;
}

int main(void)
{
  // The socket module requires explicit initialization
  etcpal_error_t res = etcpal_init(ETCPAL_FEATURE_SOCKETS);

  // Initialize receive socket and poll context.
  etcpal_socket_t rcvsock = ETCPAL_SOCKET_INVALID;
  if (res == kEtcPalErrOk)
    res = init_receive_socket(&rcvsock);

  EtcPalPollContext context;
  if (res == kEtcPalErrOk)
    res = init_poll_context(rcvsock, &context);

  // Poll and receive one message.
  printf("Waiting for a new packet to be received on port %d...\n", RECVMSG_EXAMPLE_PORT);

  EtcPalPollEvent event;
  if (res == kEtcPalErrOk)
    res = etcpal_poll_wait(&context, &event, ETCPAL_WAIT_FOREVER);

  uint8_t      buf[RECVMSG_EXAMPLE_MTU]                = {0};
  uint8_t      control[ETCPAL_CONTROL_SIZE_IP_PKTINFO] = {0};
  EtcPalMsgHdr msg                                     = {0};
  int          bytes_received                          = 0;
  if (res == kEtcPalErrOk)
  {
    msg.buf        = buf;
    msg.buflen     = RECVMSG_EXAMPLE_MTU;
    msg.control    = control;
    msg.controllen = ETCPAL_CONTROL_SIZE_IP_PKTINFO;

    int recvmsg_res = etcpal_recvmsg(rcvsock, &msg, 0);
    if (recvmsg_res < 0)
      res = (etcpal_error_t)recvmsg_res;
    else
      bytes_received = recvmsg_res;
  }

  // Parse and print the packet data and info.
  if (res == kEtcPalErrOk)
    res = print_packet_info(bytes_received, &msg);

  if (res != kEtcPalErrOk)
  {
    printf("Error occurred: '%s'\n", etcpal_strerror(res));
    return 1;
  }

  // Must always deinit with the same feature macro combination that was passed to init().
  etcpal_deinit(ETCPAL_FEATURE_SOCKETS);
  return 0;
}
