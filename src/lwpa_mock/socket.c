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

#include "lwpa_mock/socket.h"

DEFINE_FAKE_VALUE_FUNC(lwpa_error_t, lwpa_accept, lwpa_socket_t, LwpaSockaddr*, lwpa_socket_t*);
DEFINE_FAKE_VALUE_FUNC(lwpa_error_t, lwpa_bind, lwpa_socket_t, const LwpaSockaddr*);
DEFINE_FAKE_VALUE_FUNC(lwpa_error_t, lwpa_close, lwpa_socket_t);
DEFINE_FAKE_VALUE_FUNC(lwpa_error_t, lwpa_connect, lwpa_socket_t, const LwpaSockaddr*);
DEFINE_FAKE_VALUE_FUNC(lwpa_error_t, lwpa_getpeername, lwpa_socket_t, LwpaSockaddr*);
DEFINE_FAKE_VALUE_FUNC(lwpa_error_t, lwpa_getsockname, lwpa_socket_t, LwpaSockaddr*);
DEFINE_FAKE_VALUE_FUNC(lwpa_error_t, lwpa_getsockopt, lwpa_socket_t, int, int, void*, size_t*);
DEFINE_FAKE_VALUE_FUNC(lwpa_error_t, lwpa_listen, lwpa_socket_t, int);
DEFINE_FAKE_VALUE_FUNC(int, lwpa_recv, lwpa_socket_t, void*, size_t, int);
DEFINE_FAKE_VALUE_FUNC(int, lwpa_recvfrom, lwpa_socket_t, void*, size_t, int, LwpaSockaddr*);
DEFINE_FAKE_VALUE_FUNC(int, lwpa_send, lwpa_socket_t, const void*, size_t, int);
DEFINE_FAKE_VALUE_FUNC(int, lwpa_sendto, lwpa_socket_t, const void*, size_t, int, const LwpaSockaddr*);
DEFINE_FAKE_VALUE_FUNC(lwpa_error_t, lwpa_setsockopt, lwpa_socket_t, int, int, const void*, size_t);
DEFINE_FAKE_VALUE_FUNC(lwpa_error_t, lwpa_shutdown, lwpa_socket_t, int);
DEFINE_FAKE_VALUE_FUNC(lwpa_error_t, lwpa_socket, unsigned int, unsigned int, lwpa_socket_t*);

DEFINE_FAKE_VALUE_FUNC(lwpa_error_t, lwpa_setblocking, lwpa_socket_t, bool);

DEFINE_FAKE_VALUE_FUNC(lwpa_error_t, lwpa_poll_context_init, LwpaPollContext*);
DEFINE_FAKE_VOID_FUNC(lwpa_poll_context_deinit, LwpaPollContext*);
DEFINE_FAKE_VALUE_FUNC(lwpa_error_t, lwpa_poll_add_socket, LwpaPollContext*, lwpa_socket_t, lwpa_poll_events_t, void*);
DEFINE_FAKE_VALUE_FUNC(lwpa_error_t, lwpa_poll_modify_socket, LwpaPollContext*, lwpa_socket_t, lwpa_poll_events_t,
                       void*);
DEFINE_FAKE_VOID_FUNC(lwpa_poll_remove_socket, LwpaPollContext*, lwpa_socket_t);
DEFINE_FAKE_VALUE_FUNC(lwpa_error_t, lwpa_poll_wait, LwpaPollContext*, LwpaPollEvent*, int);

DEFINE_FAKE_VALUE_FUNC(lwpa_error_t, lwpa_getaddrinfo, const char*, const char*, const LwpaAddrinfo*, LwpaAddrinfo*);
DEFINE_FAKE_VALUE_FUNC(bool, lwpa_nextaddr, LwpaAddrinfo*);
DEFINE_FAKE_VOID_FUNC(lwpa_freeaddrinfo, LwpaAddrinfo*);
