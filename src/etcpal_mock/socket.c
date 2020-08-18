/******************************************************************************
 * Copyright 2020 ETC Inc.
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

#include "etcpal_mock/socket.h"

DEFINE_FAKE_VALUE_FUNC(etcpal_error_t, etcpal_accept, etcpal_socket_t, EtcPalSockAddr*, etcpal_socket_t*);
DEFINE_FAKE_VALUE_FUNC(etcpal_error_t, etcpal_bind, etcpal_socket_t, const EtcPalSockAddr*);
DEFINE_FAKE_VALUE_FUNC(etcpal_error_t, etcpal_close, etcpal_socket_t);
DEFINE_FAKE_VALUE_FUNC(etcpal_error_t, etcpal_connect, etcpal_socket_t, const EtcPalSockAddr*);
DEFINE_FAKE_VALUE_FUNC(etcpal_error_t, etcpal_getpeername, etcpal_socket_t, EtcPalSockAddr*);
DEFINE_FAKE_VALUE_FUNC(etcpal_error_t, etcpal_getsockname, etcpal_socket_t, EtcPalSockAddr*);
DEFINE_FAKE_VALUE_FUNC(etcpal_error_t, etcpal_getsockopt, etcpal_socket_t, int, int, void*, size_t*);
DEFINE_FAKE_VALUE_FUNC(etcpal_error_t, etcpal_listen, etcpal_socket_t, int);
DEFINE_FAKE_VALUE_FUNC(int, etcpal_recv, etcpal_socket_t, void*, size_t, int);
DEFINE_FAKE_VALUE_FUNC(int, etcpal_recvfrom, etcpal_socket_t, void*, size_t, int, EtcPalSockAddr*);
DEFINE_FAKE_VALUE_FUNC(int, etcpal_send, etcpal_socket_t, const void*, size_t, int);
DEFINE_FAKE_VALUE_FUNC(int, etcpal_sendto, etcpal_socket_t, const void*, size_t, int, const EtcPalSockAddr*);
DEFINE_FAKE_VALUE_FUNC(etcpal_error_t, etcpal_setsockopt, etcpal_socket_t, int, int, const void*, size_t);
DEFINE_FAKE_VALUE_FUNC(etcpal_error_t, etcpal_shutdown, etcpal_socket_t, int);
DEFINE_FAKE_VALUE_FUNC(etcpal_error_t, etcpal_socket, unsigned int, unsigned int, etcpal_socket_t*);

DEFINE_FAKE_VALUE_FUNC(etcpal_error_t, etcpal_setblocking, etcpal_socket_t, bool);

DEFINE_FAKE_VALUE_FUNC(etcpal_error_t, etcpal_poll_context_init, EtcPalPollContext*);
DEFINE_FAKE_VOID_FUNC(etcpal_poll_context_deinit, EtcPalPollContext*);
DEFINE_FAKE_VALUE_FUNC(etcpal_error_t,
                       etcpal_poll_add_socket,
                       EtcPalPollContext*,
                       etcpal_socket_t,
                       etcpal_poll_events_t,
                       void*);
DEFINE_FAKE_VALUE_FUNC(etcpal_error_t,
                       etcpal_poll_modify_socket,
                       EtcPalPollContext*,
                       etcpal_socket_t,
                       etcpal_poll_events_t,
                       void*);
DEFINE_FAKE_VOID_FUNC(etcpal_poll_remove_socket, EtcPalPollContext*, etcpal_socket_t);
DEFINE_FAKE_VALUE_FUNC(etcpal_error_t, etcpal_poll_wait, EtcPalPollContext*, EtcPalPollEvent*, int);

DEFINE_FAKE_VALUE_FUNC(etcpal_error_t,
                       etcpal_getaddrinfo,
                       const char*,
                       const char*,
                       const EtcPalAddrinfo*,
                       EtcPalAddrinfo*);
DEFINE_FAKE_VALUE_FUNC(bool, etcpal_nextaddr, EtcPalAddrinfo*);
DEFINE_FAKE_VOID_FUNC(etcpal_freeaddrinfo, EtcPalAddrinfo*);

void etcpal_socket_reset_all_fakes(void)
{
  RESET_FAKE(etcpal_accept);
  RESET_FAKE(etcpal_bind);
  RESET_FAKE(etcpal_close);
  RESET_FAKE(etcpal_connect);
  RESET_FAKE(etcpal_getpeername);
  RESET_FAKE(etcpal_getsockname);
  RESET_FAKE(etcpal_getsockopt);
  RESET_FAKE(etcpal_listen);
  RESET_FAKE(etcpal_recv);
  RESET_FAKE(etcpal_recvfrom);
  RESET_FAKE(etcpal_send);
  RESET_FAKE(etcpal_sendto);
  RESET_FAKE(etcpal_setsockopt);
  RESET_FAKE(etcpal_shutdown);
  RESET_FAKE(etcpal_socket);
  RESET_FAKE(etcpal_setblocking);
  RESET_FAKE(etcpal_poll_context_init);
  RESET_FAKE(etcpal_poll_context_deinit);
  RESET_FAKE(etcpal_poll_add_socket);
  RESET_FAKE(etcpal_poll_modify_socket);
  RESET_FAKE(etcpal_poll_remove_socket);
  RESET_FAKE(etcpal_poll_wait);
  RESET_FAKE(etcpal_getaddrinfo);
  RESET_FAKE(etcpal_nextaddr);
  RESET_FAKE(etcpal_freeaddrinfo);
}
