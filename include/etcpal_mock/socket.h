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
 * This file is a part of EtcPal. For more information, go to:
 * https://github.com/ETCLabs/EtcPal
 ******************************************************************************/

/* etcpal_mock/socket.h: Mock functions for the etcpal_socket API. */
#ifndef _LWPA_MOCK_SOCKET_H_
#define _LWPA_MOCK_SOCKET_H_

#include "etcpal/socket.h"
#include "fff.h"

// We don't mock:
// etcpal_inet_pton()
// etcpal_inet_ntop()
// sockaddr_os_to_lwpa()
// sockaddr_etcpal_to_os()

#ifdef __cplusplus
extern "C" {
#endif

DECLARE_FAKE_VALUE_FUNC(etcpal_error_t, etcpal_accept, etcpal_socket_t, LwpaSockaddr*, etcpal_socket_t*);
DECLARE_FAKE_VALUE_FUNC(etcpal_error_t, etcpal_bind, etcpal_socket_t, const LwpaSockaddr*);
DECLARE_FAKE_VALUE_FUNC(etcpal_error_t, etcpal_close, etcpal_socket_t);
DECLARE_FAKE_VALUE_FUNC(etcpal_error_t, etcpal_connect, etcpal_socket_t, const LwpaSockaddr*);
DECLARE_FAKE_VALUE_FUNC(etcpal_error_t, etcpal_getpeername, etcpal_socket_t, LwpaSockaddr*);
DECLARE_FAKE_VALUE_FUNC(etcpal_error_t, etcpal_getsockname, etcpal_socket_t, LwpaSockaddr*);
DECLARE_FAKE_VALUE_FUNC(etcpal_error_t, etcpal_getsockopt, etcpal_socket_t, int, int, void*, size_t*);
DECLARE_FAKE_VALUE_FUNC(etcpal_error_t, etcpal_listen, etcpal_socket_t, int);
DECLARE_FAKE_VALUE_FUNC(int, etcpal_recv, etcpal_socket_t, void*, size_t, int);
DECLARE_FAKE_VALUE_FUNC(int, etcpal_recvfrom, etcpal_socket_t, void*, size_t, int, LwpaSockaddr*);
DECLARE_FAKE_VALUE_FUNC(int, etcpal_send, etcpal_socket_t, const void*, size_t, int);
DECLARE_FAKE_VALUE_FUNC(int, etcpal_sendto, etcpal_socket_t, const void*, size_t, int, const LwpaSockaddr*);
DECLARE_FAKE_VALUE_FUNC(etcpal_error_t, etcpal_setsockopt, etcpal_socket_t, int, int, const void*, size_t);
DECLARE_FAKE_VALUE_FUNC(etcpal_error_t, etcpal_shutdown, etcpal_socket_t, int);
DECLARE_FAKE_VALUE_FUNC(etcpal_error_t, etcpal_socket, unsigned int, unsigned int, etcpal_socket_t*);

DECLARE_FAKE_VALUE_FUNC(etcpal_error_t, etcpal_setblocking, etcpal_socket_t, bool);

DECLARE_FAKE_VALUE_FUNC(etcpal_error_t, etcpal_poll_context_init, LwpaPollContext*);
DECLARE_FAKE_VOID_FUNC(etcpal_poll_context_deinit, LwpaPollContext*);
DECLARE_FAKE_VALUE_FUNC(etcpal_error_t, etcpal_poll_add_socket, LwpaPollContext*, etcpal_socket_t, etcpal_poll_events_t, void*);
DECLARE_FAKE_VALUE_FUNC(etcpal_error_t, etcpal_poll_modify_socket, LwpaPollContext*, etcpal_socket_t, etcpal_poll_events_t,
                        void*);
DECLARE_FAKE_VOID_FUNC(etcpal_poll_remove_socket, LwpaPollContext*, etcpal_socket_t);
DECLARE_FAKE_VALUE_FUNC(etcpal_error_t, etcpal_poll_wait, LwpaPollContext*, LwpaPollEvent*, int);

DECLARE_FAKE_VALUE_FUNC(etcpal_error_t, etcpal_getaddrinfo, const char*, const char*, const LwpaAddrinfo*, LwpaAddrinfo*);
DECLARE_FAKE_VALUE_FUNC(bool, etcpal_nextaddr, LwpaAddrinfo*);
DECLARE_FAKE_VOID_FUNC(etcpal_freeaddrinfo, LwpaAddrinfo*);

#define LWPA_SOCKET_DO_FOR_ALL_FAKES(operation) \
  operation(etcpal_accept);                       \
  operation(etcpal_bind);                         \
  operation(etcpal_close);                        \
  operation(etcpal_connect);                      \
  operation(etcpal_getpeername);                  \
  operation(etcpal_getsockname);                  \
  operation(etcpal_getsockopt);                   \
  operation(etcpal_listen);                       \
  operation(etcpal_recv);                         \
  operation(etcpal_recvfrom);                     \
  operation(etcpal_send);                         \
  operation(etcpal_sendto);                       \
  operation(etcpal_setsockopt);                   \
  operation(etcpal_shutdown);                     \
  operation(etcpal_socket);                       \
  operation(etcpal_setblocking);                  \
  operation(etcpal_poll_context_init);            \
  operation(etcpal_poll_context_deinit);          \
  operation(etcpal_poll_add_socket);              \
  operation(etcpal_poll_modify_socket);           \
  operation(etcpal_poll_remove_socket);           \
  operation(etcpal_poll_wait);                    \
  operation(etcpal_getaddrinfo);                  \
  operation(etcpal_nextaddr);                     \
  operation(etcpal_freeaddrinfo);

#ifdef __cplusplus
}
#endif

#endif /* _LWPA_MOCK_SOCKET_H_ */
