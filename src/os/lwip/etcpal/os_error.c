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

#include "os_error.h"
#include <lwip/netdb.h>
#include <lwip/errno.h>

#if !LWIP_PROVIDE_ERRNO && !LWIP_ERRNO_STDINCLUDE
#if defined(LWIP_ERRNO_INCLUDE)
#include LWIP_ERRNO_INCLUDE
#else
#include <errno.h>
#endif
#endif

etcpal_error_t errno_lwip_to_etcpal(int lwip_errno)
{
  switch (lwip_errno)
  {
    case EPERM:
    case EACCES:
      return kEtcPalErrPerm;
    case ENOENT:
    case EBADF:
    case ENOTSOCK:
      return kEtcPalErrNotFound;
    case EFAULT:
    case EINVAL:
    case EDESTADDRREQ:
    case EPROTOTYPE:
    case EOPNOTSUPP:
      return kEtcPalErrInvalid;
      // case EAGAIN: // EWOULDBLOCK defined to EAGAIN
    case EWOULDBLOCK:
      return kEtcPalErrWouldBlock;
    case EBUSY:
      return kEtcPalErrBusy;
    case ENOMEM:
    case ENOBUFS:
      return kEtcPalErrNoMem;
    case EEXIST:
      return kEtcPalErrExists;
    case ENOSYS:
      return kEtcPalErrNotImpl;
    case EPROTO:
      return kEtcPalErrProtocol;
    case EMSGSIZE:
      return kEtcPalErrMsgSize;
    case EADDRINUSE:
      return kEtcPalErrAddrInUse;
    case EADDRNOTAVAIL:
      return kEtcPalErrAddrNotAvail;
    case ENETDOWN:
    case ENETUNREACH:
    case ENETRESET:
      return kEtcPalErrNetwork;
    case ECONNABORTED:
      return kEtcPalErrConnAborted;
    case ECONNRESET:
      return kEtcPalErrConnReset;
    case EISCONN:
      return kEtcPalErrIsConn;
    case ENOTCONN:
      return kEtcPalErrNotConn;
#ifdef ESHUTDOWN
    case ESHUTDOWN:
      return kEtcPalErrShutdown;
#endif
    case ETIMEDOUT:
      return kEtcPalErrTimedOut;
    case ECONNREFUSED:
      return kEtcPalErrConnRefused;
    case EALREADY:
      return kEtcPalErrAlready;
    case EINPROGRESS:
      return kEtcPalErrInProgress;
    case EINTR:
    case ENFILE:
    case EMFILE:
    case ENOSPC:
#ifdef EPFNOSUPPORT
    case EPFNOSUPPORT:
#endif
    case EAFNOSUPPORT:
    case EPROTONOSUPPORT:
#ifdef ESOCKTNOSUPPORT
    case ESOCKTNOSUPPORT:
#endif
    default:
      return kEtcPalErrSys;
  }
}

etcpal_error_t err_gai_to_etcpal(int gai_error)
{
#if LWIP_DNS
  switch (gai_error)
  {
    case EAI_SERVICE:
    case EAI_FAIL:
    case EAI_NONAME:
      return kEtcPalErrNotFound;
    case EAI_MEMORY:
      return kEtcPalErrNoMem;
    case EAI_FAMILY:
      return kEtcPalErrInvalid;
    default:
      return kEtcPalErrSys;
  }
#else
  (void)gai_error;  // unused
  return kEtcPalErrSys;
#endif
}
