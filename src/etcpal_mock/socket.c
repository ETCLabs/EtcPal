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

#include "etcpal_mock/socket.h"

DEFINE_FAKE_VALUE_FUNC(etcpal_error_t, etcpal_accept, etcpal_socket_t, LwpaSockaddr*, etcpal_socket_t*);
DEFINE_FAKE_VALUE_FUNC(etcpal_error_t, etcpal_bind, etcpal_socket_t, const LwpaSockaddr*);
DEFINE_FAKE_VALUE_FUNC(etcpal_error_t, etcpal_close, etcpal_socket_t);
DEFINE_FAKE_VALUE_FUNC(etcpal_error_t, etcpal_connect, etcpal_socket_t, const LwpaSockaddr*);
DEFINE_FAKE_VALUE_FUNC(etcpal_error_t, etcpal_getpeername, etcpal_socket_t, LwpaSockaddr*);
DEFINE_FAKE_VALUE_FUNC(etcpal_error_t, etcpal_getsockname, etcpal_socket_t, LwpaSockaddr*);
DEFINE_FAKE_VALUE_FUNC(etcpal_error_t, etcpal_getsockopt, etcpal_socket_t, int, int, void*, size_t*);
DEFINE_FAKE_VALUE_FUNC(etcpal_error_t, etcpal_listen, etcpal_socket_t, int);
DEFINE_FAKE_VALUE_FUNC(int, etcpal_recv, etcpal_socket_t, void*, size_t, int);
DEFINE_FAKE_VALUE_FUNC(int, etcpal_recvfrom, etcpal_socket_t, void*, size_t, int, LwpaSockaddr*);
DEFINE_FAKE_VALUE_FUNC(int, etcpal_send, etcpal_socket_t, const void*, size_t, int);
DEFINE_FAKE_VALUE_FUNC(int, etcpal_sendto, etcpal_socket_t, const void*, size_t, int, const LwpaSockaddr*);
DEFINE_FAKE_VALUE_FUNC(etcpal_error_t, etcpal_setsockopt, etcpal_socket_t, int, int, const void*, size_t);
DEFINE_FAKE_VALUE_FUNC(etcpal_error_t, etcpal_shutdown, etcpal_socket_t, int);
DEFINE_FAKE_VALUE_FUNC(etcpal_error_t, etcpal_socket, unsigned int, unsigned int, etcpal_socket_t*);

DEFINE_FAKE_VALUE_FUNC(etcpal_error_t, etcpal_setblocking, etcpal_socket_t, bool);

DEFINE_FAKE_VALUE_FUNC(etcpal_error_t, etcpal_poll_context_init, LwpaPollContext*);
DEFINE_FAKE_VOID_FUNC(etcpal_poll_context_deinit, LwpaPollContext*);
DEFINE_FAKE_VALUE_FUNC(etcpal_error_t, etcpal_poll_add_socket, LwpaPollContext*, etcpal_socket_t, etcpal_poll_events_t, void*);
DEFINE_FAKE_VALUE_FUNC(etcpal_error_t, etcpal_poll_modify_socket, LwpaPollContext*, etcpal_socket_t, etcpal_poll_events_t,
                       void*);
DEFINE_FAKE_VOID_FUNC(etcpal_poll_remove_socket, LwpaPollContext*, etcpal_socket_t);
DEFINE_FAKE_VALUE_FUNC(etcpal_error_t, etcpal_poll_wait, LwpaPollContext*, LwpaPollEvent*, int);

DEFINE_FAKE_VALUE_FUNC(etcpal_error_t, etcpal_getaddrinfo, const char*, const char*, const LwpaAddrinfo*, LwpaAddrinfo*);
DEFINE_FAKE_VALUE_FUNC(bool, etcpal_nextaddr, LwpaAddrinfo*);
DEFINE_FAKE_VOID_FUNC(etcpal_freeaddrinfo, LwpaAddrinfo*);
