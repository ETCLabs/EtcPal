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

#include "os_error.h"

lwpa_error_t err_os_to_lwpa(int errno)
{
  switch (errno)
  {
    /*
    case WSAEBADF:
    case WSAENOTSOCK:
    case WSATYPE_NOT_FOUND:
    case WSAHOST_NOT_FOUND:
    case WSAESTALE:
      return kLwpaErrNotFound;
    case WSA_INVALID_HANDLE:
    case WSA_INVALID_PARAMETER:
    case WSAEFAULT:
    case WSAEINVAL:
    case WSAEDESTADDRREQ:
    case WSAENOPROTOOPT:
      return kLwpaErrInvalid;
    case WSA_NOT_ENOUGH_MEMORY:
    case WSAEMFILE:
    case WSAETOOMANYREFS:
    case WSAEPROCLIM:
    case WSAEUSERS:
      return kLwpaErrNoMem;
    case WSA_IO_PENDING:
    case WSAEINPROGRESS:
      return kLwpaErrInProgress;
    case WSA_IO_INCOMPLETE:
    case WSAEALREADY:
      return kLwpaErrAlready;
    case WSAEWOULDBLOCK:
      return kLwpaErrWouldBlock;
    case WSAEMSGSIZE:
      return kLwpaErrMsgSize;
    case WSAEADDRINUSE:
      return kLwpaErrAddrInUse;
    case WSAEADDRNOTAVAIL:
      return kLwpaErrAddrNotAvail;
    case WSAENETDOWN:
    case WSAENETUNREACH:
    case WSAENETRESET:
    case WSAEHOSTDOWN:
    case WSAEHOSTUNREACH:
      return kLwpaErrNetwork;
    case WSAECONNRESET:
    case WSAECONNABORTED:
      return kLwpaErrConnReset;
    case WSAEISCONN:
      return kLwpaErrIsConn;
    case WSAENOTCONN:
      return kLwpaErrNotConn;
    case WSAESHUTDOWN:
      return kLwpaErrShutdown;
    case WSAETIMEDOUT:
      return kLwpaErrTimedOut;
    case WSAECONNREFUSED:
      return kLwpaErrConnRefused;
    case WSAENOBUFS:
      return kLwpaErrBufSize;
    case WSANOTIMPLIALISED:
    case WSASYSNOTREADY:
      return kLwpaErrNotImpl;
    case WSAEACCES:
    case WSA_OPERATION_ABORTED:
    case WSAEPROTOTYPE:
    case WSAEPROTONOSUPPORT:
    case WSAESOCKTNOSUPPORT:
    case WSAEOPNOTSUPP:
    case WSAEPFNOSUPPORT:
    case WSAEAFNOSUPPORT:
    case WSASYSCALLFAILURE:
    */
    default:
      return kLwpaErrSys;
  }
}
