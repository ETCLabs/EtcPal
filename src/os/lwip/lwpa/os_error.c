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
#include <errno.h>

lwpa_error_t errno_lwip_to_lwpa(int lwip_errno)
{
  switch (lwip_errno)
  {
    case EPERM:
    case EACCES:
      return kLwpaErrPerm;
    case ENOENT:
    case EBADF:
    case ENOTSOCK:
      return kLwpaErrNotFound;
    case EFAULT:
    case EINVAL:
    case EDESTADDRREQ:
    case EPROTOTYPE:
    case EOPNOTSUPP:
      return kLwpaErrInvalid;
      // case EAGAIN: // EWOULDBLOCK defined to EAGAIN
    case EWOULDBLOCK:
      return kLwpaErrWouldBlock;
    case EBUSY:
      return kLwpaErrBusy;
    case ENOMEM:
    case ENOBUFS:
      return kLwpaErrNoMem;
    case EEXIST:
      return kLwpaErrExists;
    case ENOSYS:
      return kLwpaErrNotImpl;
    case EPROTO:
      return kLwpaErrProtocol;
    case EMSGSIZE:
      return kLwpaErrMsgSize;
    case EADDRINUSE:
      return kLwpaErrAddrInUse;
    case EADDRNOTAVAIL:
      return kLwpaErrAddrNotAvail;
    case ENETDOWN:
    case ENETUNREACH:
    case ENETRESET:
      return kLwpaErrNetwork;
    case ECONNABORTED:
      return kLwpaErrConnAborted;
    case ECONNRESET:
      return kLwpaErrConnReset;
    case EISCONN:
      return kLwpaErrIsConn;
    case ENOTCONN:
      return kLwpaErrNotConn;
    case ESHUTDOWN:
      return kLwpaErrShutdown;
    case ETIMEDOUT:
      return kLwpaErrTimedOut;
    case ECONNREFUSED:
      return kLwpaErrConnRefused;
    case EALREADY:
      return kLwpaErrAlready;
    case EINPROGRESS:
      return kLwpaErrInProgress;
    case EINTR:
    case ENFILE:
    case EMFILE:
    case ENOSPC:
    case EPFNOSUPPORT:
    case EAFNOSUPPORT:
    case EPROTONOSUPPORT:
    case ESOCKTNOSUPPORT:
    default:
      return kLwpaErrSys;
  }
}
