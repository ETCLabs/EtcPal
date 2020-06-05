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

/* etcpal_mock/socket.h: Mock functions for the etcpal_socket API. */

#ifndef ETCPAL_MOCK_SOCKET_H_
#define ETCPAL_MOCK_SOCKET_H_

#include "etcpal/socket.h"
#include "fff.h"

// We don't mock:
// etcpal_ip_to_string()
// etcpal_string_to_ip()
// sockaddr_os_to_etcpal()
// sockaddr_etcpal_to_os()

#ifdef __cplusplus
extern "C" {
#endif

DECLARE_FAKE_VALUE_FUNC(etcpal_error_t, etcpal_accept, etcpal_socket_t, EtcPalSockAddr*, etcpal_socket_t*);
DECLARE_FAKE_VALUE_FUNC(etcpal_error_t, etcpal_bind, etcpal_socket_t, const EtcPalSockAddr*);
DECLARE_FAKE_VALUE_FUNC(etcpal_error_t, etcpal_close, etcpal_socket_t);
DECLARE_FAKE_VALUE_FUNC(etcpal_error_t, etcpal_connect, etcpal_socket_t, const EtcPalSockAddr*);
DECLARE_FAKE_VALUE_FUNC(etcpal_error_t, etcpal_getpeername, etcpal_socket_t, EtcPalSockAddr*);
DECLARE_FAKE_VALUE_FUNC(etcpal_error_t, etcpal_getsockname, etcpal_socket_t, EtcPalSockAddr*);
DECLARE_FAKE_VALUE_FUNC(etcpal_error_t, etcpal_getsockopt, etcpal_socket_t, int, int, void*, size_t*);
DECLARE_FAKE_VALUE_FUNC(etcpal_error_t, etcpal_listen, etcpal_socket_t, int);
DECLARE_FAKE_VALUE_FUNC(int, etcpal_recv, etcpal_socket_t, void*, size_t, int);
DECLARE_FAKE_VALUE_FUNC(int, etcpal_recvfrom, etcpal_socket_t, void*, size_t, int, EtcPalSockAddr*);
DECLARE_FAKE_VALUE_FUNC(int, etcpal_send, etcpal_socket_t, const void*, size_t, int);
DECLARE_FAKE_VALUE_FUNC(int, etcpal_sendto, etcpal_socket_t, const void*, size_t, int, const EtcPalSockAddr*);
DECLARE_FAKE_VALUE_FUNC(etcpal_error_t, etcpal_setsockopt, etcpal_socket_t, int, int, const void*, size_t);
DECLARE_FAKE_VALUE_FUNC(etcpal_error_t, etcpal_shutdown, etcpal_socket_t, int);
DECLARE_FAKE_VALUE_FUNC(etcpal_error_t, etcpal_socket, unsigned int, unsigned int, etcpal_socket_t*);

DECLARE_FAKE_VALUE_FUNC(etcpal_error_t, etcpal_setblocking, etcpal_socket_t, bool);

DECLARE_FAKE_VALUE_FUNC(etcpal_error_t, etcpal_poll_context_init, EtcPalPollContext*);
DECLARE_FAKE_VOID_FUNC(etcpal_poll_context_deinit, EtcPalPollContext*);
DECLARE_FAKE_VALUE_FUNC(etcpal_error_t,
                        etcpal_poll_add_socket,
                        EtcPalPollContext*,
                        etcpal_socket_t,
                        etcpal_poll_events_t,
                        void*);
DECLARE_FAKE_VALUE_FUNC(etcpal_error_t,
                        etcpal_poll_modify_socket,
                        EtcPalPollContext*,
                        etcpal_socket_t,
                        etcpal_poll_events_t,
                        void*);
DECLARE_FAKE_VOID_FUNC(etcpal_poll_remove_socket, EtcPalPollContext*, etcpal_socket_t);
DECLARE_FAKE_VALUE_FUNC(etcpal_error_t, etcpal_poll_wait, EtcPalPollContext*, EtcPalPollEvent*, int);

DECLARE_FAKE_VALUE_FUNC(etcpal_error_t,
                        etcpal_getaddrinfo,
                        const char*,
                        const char*,
                        const EtcPalAddrinfo*,
                        EtcPalAddrinfo*);
DECLARE_FAKE_VALUE_FUNC(bool, etcpal_nextaddr, EtcPalAddrinfo*);
DECLARE_FAKE_VOID_FUNC(etcpal_freeaddrinfo, EtcPalAddrinfo*);

void etcpal_socket_reset_all_fakes(void);

#ifdef __cplusplus
}
#endif

#endif /* ETCPAL_MOCK_SOCKET_H_ */
