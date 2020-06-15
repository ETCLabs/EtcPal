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

#include "os_error.h"

#include <errno.h>
#include <WinSock2.h>
#include <Windows.h>

// Convert Windows sockets errors to etcpal_error_t values.
etcpal_error_t err_winsock_to_etcpal(int wsaerror)
{
  // The Winsock error codes are not even close to contiguous in defined value, so a giant switch
  // statement is the only solution here...
  switch (wsaerror)
  {
    case WSAEBADF:
    case WSAENOTSOCK:
    case WSATYPE_NOT_FOUND:
    case WSAHOST_NOT_FOUND:
    case WSAESTALE:
      return kEtcPalErrNotFound;
    case WSA_INVALID_HANDLE:
    case WSA_INVALID_PARAMETER:
    case WSAEFAULT:
    case WSAEINVAL:
    case WSAEDESTADDRREQ:
    case WSAENOPROTOOPT:
      return kEtcPalErrInvalid;
    case WSA_NOT_ENOUGH_MEMORY:
    case WSAEMFILE:
    case WSAETOOMANYREFS:
    case WSAEPROCLIM:
    case WSAEUSERS:
      return kEtcPalErrNoMem;
    case WSA_IO_PENDING:
    case WSAEINPROGRESS:
      return kEtcPalErrInProgress;
    case WSA_IO_INCOMPLETE:
    case WSAEALREADY:
      return kEtcPalErrAlready;
    case WSAEWOULDBLOCK:
      return kEtcPalErrWouldBlock;
    case WSAEMSGSIZE:
      return kEtcPalErrMsgSize;
    case WSAEADDRINUSE:
      return kEtcPalErrAddrInUse;
    case WSAEADDRNOTAVAIL:
      return kEtcPalErrAddrNotAvail;
    case WSAENETDOWN:
    case WSAENETUNREACH:
    case WSAENETRESET:
    case WSAEHOSTDOWN:
    case WSAEHOSTUNREACH:
      return kEtcPalErrNetwork;
    case WSAECONNRESET:
    case WSAECONNABORTED:
      return kEtcPalErrConnReset;
    case WSAEISCONN:
      return kEtcPalErrIsConn;
    case WSAENOTCONN:
      return kEtcPalErrNotConn;
    case WSAESHUTDOWN:
      return kEtcPalErrShutdown;
    case WSAETIMEDOUT:
      return kEtcPalErrTimedOut;
    case WSAECONNREFUSED:
      return kEtcPalErrConnRefused;
    case WSAENOBUFS:
      return kEtcPalErrBufSize;
    case WSANOTINITIALISED:
    case WSASYSNOTREADY:
      return kEtcPalErrNotInit;
    case WSAEACCES:
      return kEtcPalErrPerm;
    case WSA_OPERATION_ABORTED:
    case WSAEPROTOTYPE:
    case WSAEPROTONOSUPPORT:
    case WSAESOCKTNOSUPPORT:
    case WSAEOPNOTSUPP:
    case WSAEPFNOSUPPORT:
    case WSAEAFNOSUPPORT:
    case WSASYSCALLFAILURE:
    default:
      return kEtcPalErrSys;
  }
}

etcpal_error_t err_os_to_etcpal(int error)
{
  switch (error)
  {
    case EPERM:
    case EACCES:
      return kEtcPalErrPerm;
    case ENOENT:
    case ESRCH:
    case ENXIO:
    case ENODEV:
      return kEtcPalErrNotFound;
    case E2BIG:
    case EBADF:
    case ENOTDIR:
    case EISDIR:
    case EINVAL:
    case ENAMETOOLONG:
      return kEtcPalErrInvalid;
    case EAGAIN:
      return kEtcPalErrWouldBlock;
    case ENOMEM:
    case ENOSPC:
      return kEtcPalErrNoMem;
    case EBUSY:
      return kEtcPalErrBusy;
    case EEXIST:
      return kEtcPalErrExists;
    case ENOSYS:
      return kEtcPalErrNotImpl;
    default:
      return kEtcPalErrSys;
  }
}
