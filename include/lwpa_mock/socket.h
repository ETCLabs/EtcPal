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

/* lwpa_mock/socket.h: Mock functions for the lwpa_socket API. */
#ifndef _LWPA_MOCK_SOCKET_H_
#define _LWPA_MOCK_SOCKET_H_

#include "lwpa/socket.h"
#include "fff.h"

// We don't mock:
// lwpa_inet_pton()
// lwpa_inet_ntop()
// sockaddr_plat_to_lwpa()
// sockaddr_lwpa_to_plat()

#ifdef __cplusplus
extern "C" {
#endif

DECLARE_FAKE_VALUE_FUNC(lwpa_error_t, lwpa_accept, lwpa_socket_t, LwpaSockaddr*, lwpa_socket_t*);
DECLARE_FAKE_VALUE_FUNC(lwpa_error_t, lwpa_bind, lwpa_socket_t, const LwpaSockaddr*);
DECLARE_FAKE_VALUE_FUNC(lwpa_error_t, lwpa_close, lwpa_socket_t);
DECLARE_FAKE_VALUE_FUNC(lwpa_error_t, lwpa_connect, lwpa_socket_t, const LwpaSockaddr*);
DECLARE_FAKE_VALUE_FUNC(lwpa_error_t, lwpa_getpeername, lwpa_socket_t, LwpaSockaddr*);
DECLARE_FAKE_VALUE_FUNC(lwpa_error_t, lwpa_getsockname, lwpa_socket_t, LwpaSockaddr*);
DECLARE_FAKE_VALUE_FUNC(lwpa_error_t, lwpa_getsockopt, lwpa_socket_t, int, int, void*, size_t*);
DECLARE_FAKE_VALUE_FUNC(lwpa_error_t, lwpa_listen, lwpa_socket_t, int);
DECLARE_FAKE_VALUE_FUNC(int, lwpa_recv, lwpa_socket_t, void*, size_t, int);
DECLARE_FAKE_VALUE_FUNC(int, lwpa_recvfrom, lwpa_socket_t, void*, size_t, int, LwpaSockaddr*);
DECLARE_FAKE_VALUE_FUNC(int, lwpa_send, lwpa_socket_t, const void*, size_t, int);
DECLARE_FAKE_VALUE_FUNC(int, lwpa_sendto, lwpa_socket_t, const void*, size_t, int, const LwpaSockaddr*);
DECLARE_FAKE_VALUE_FUNC(lwpa_error_t, lwpa_setsockopt, lwpa_socket_t, int, int, const void*, size_t);
DECLARE_FAKE_VALUE_FUNC(lwpa_error_t, lwpa_shutdown, lwpa_socket_t, int);
DECLARE_FAKE_VALUE_FUNC(lwpa_error_t, lwpa_socket, unsigned int, unsigned int, lwpa_socket_t*);

DECLARE_FAKE_VALUE_FUNC(lwpa_error_t, lwpa_setblocking, lwpa_socket_t, bool);

DECLARE_FAKE_VALUE_FUNC(lwpa_error_t, lwpa_poll_context_init, LwpaPollContext*);
DECLARE_FAKE_VOID_FUNC(lwpa_poll_context_deinit, LwpaPollContext*);
DECLARE_FAKE_VALUE_FUNC(lwpa_error_t, lwpa_poll_add_socket, LwpaPollContext*, lwpa_socket_t, lwpa_poll_events_t, void*);
DECLARE_FAKE_VALUE_FUNC(lwpa_error_t, lwpa_poll_modify_socket, LwpaPollContext*, lwpa_socket_t, lwpa_poll_events_t,
                        void*);
DECLARE_FAKE_VOID_FUNC(lwpa_poll_remove_socket, LwpaPollContext*, lwpa_socket_t);
DECLARE_FAKE_VALUE_FUNC(lwpa_error_t, lwpa_poll_wait, LwpaPollContext*, LwpaPollEvent*, int);

DECLARE_FAKE_VALUE_FUNC(lwpa_error_t, lwpa_getaddrinfo, const char*, const char*, const LwpaAddrinfo*, LwpaAddrinfo*);
DECLARE_FAKE_VALUE_FUNC(bool, lwpa_nextaddr, LwpaAddrinfo*);
DECLARE_FAKE_VOID_FUNC(lwpa_freeaddrinfo, LwpaAddrinfo*);

#define LWPA_SOCKET_DO_FOR_ALL_FAKES(operation) \
  operation(lwpa_accept);                       \
  operation(lwpa_bind);                         \
  operation(lwpa_close);                        \
  operation(lwpa_connect);                      \
  operation(lwpa_getpeername);                  \
  operation(lwpa_getsockname);                  \
  operation(lwpa_getsockopt);                   \
  operation(lwpa_listen);                       \
  operation(lwpa_recv);                         \
  operation(lwpa_recvfrom);                     \
  operation(lwpa_send);                         \
  operation(lwpa_sendto);                       \
  operation(lwpa_setsockopt);                   \
  operation(lwpa_shutdown);                     \
  operation(lwpa_socket);                       \
  operation(lwpa_setblocking);                  \
  operation(lwpa_poll_context_init);            \
  operation(lwpa_poll_context_deinit);          \
  operation(lwpa_poll_add_socket);              \
  operation(lwpa_poll_modify_socket);           \
  operation(lwpa_poll_remove_socket);           \
  operation(lwpa_poll_wait);                    \
  operation(lwpa_getaddrinfo);                  \
  operation(lwpa_nextaddr);                     \
  operation(lwpa_freeaddrinfo);

#ifdef __cplusplus
}
#endif

#endif /* _LWPA_MOCK_SOCKET_H_ */
