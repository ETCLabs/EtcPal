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

/* etcpal/error.h: Platform-neutral error codes. */

#ifndef ETCPAL_ERROR_H_
#define ETCPAL_ERROR_H_

#include <stdlib.h>

/**
 * @defgroup etcpal_error error (Error Handling)
 * @ingroup etcpal
 * @brief Platform-neutral error codes.
 *
 * ```c
 * include "etcpal/error.h"
 * ```
 *
 * Provides a set of error codes that can be returned by library functions. For platform
 * abstraction modules, error codes are translated from their platform-specific counterparts. Use
 * etcpal_strerror() to get a string representation of any error code.
 *
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/** A set of error codes that can be returned by library functions. */
typedef enum
{
  /** The call was successful, no error occurred. */
  kEtcPalErrOk = 0,
  /** An identifier or handle passed to a function was not valid or not previously initialized. */
  kEtcPalErrNotFound = -1,
  /** A dynamic memory allocation failed, or there is no space left in a statically allocated
   *  array. */
  kEtcPalErrNoMem = -2,
  /** The resource being requested is temporarily unavailable. */
  kEtcPalErrBusy = -3,
  /** The item being created already exists. */
  kEtcPalErrExists = -4,
  /** An invalid argument was provided to an API function. */
  kEtcPalErrInvalid = -5,
  /** The requested operation would block. */
  kEtcPalErrWouldBlock = -6,
  /** Returned from a function that is meant to be called repeatedly to indicate that there is no
   *  more data available. */
  kEtcPalErrNoData = -7,
  /** A protocol parsing function encountered a malformed packet. */
  kEtcPalErrProtocol = -8,
  /** Message too long. */
  kEtcPalErrMsgSize = -9,
  /** The requested address is already in use. */
  kEtcPalErrAddrInUse = -10,
  /** Cannot assign the requested address. */
  kEtcPalErrAddrNotAvail = -11,
  /** The operation failed because a network was down or unreachable. */
  kEtcPalErrNetwork = -12,
  /** A connection was reset (hard/abortive close) by the remote peer. */
  kEtcPalErrConnReset = -13,
  /** A connection was gracefully closed by the remote peer. This code is only used for protocols
   *  above the transport layer. */
  kEtcPalErrConnClosed = -14,
  /** Transport endpoint is already connected. */
  kEtcPalErrIsConn = -15,
  /** Transport endpoint is not connected. */
  kEtcPalErrNotConn = -16,
  /** Transport endpoint is shut down. */
  kEtcPalErrShutdown = -17,
  /** A connection timed out. */
  kEtcPalErrTimedOut = -18,
  /** Host is unreachable. */
  kEtcPalErrHostUnreach = -19,
  /** A connection has been aborted. */
  kEtcPalErrConnAborted = -20,
  /** A connection was refused. */
  kEtcPalErrConnRefused = -21,
  /** The operation requested is already in progress. */
  kEtcPalErrAlready = -22,
  /** The operation requested is now in progress and will complete later. */
  kEtcPalErrInProgress = -23,
  /** A buffer provided to a function was not big enough to hold the data that needed to be packed
   *  into it. */
  kEtcPalErrBufSize = -24,
  /** An API function was called from a module that was not previously initialized. */
  kEtcPalErrNotInit = -25,
  /** No network interfaces were found on the system, or there are no network interfaces of a type
   *  that can satisfy the call being made. */
  kEtcPalErrNoNetints = -26,
  /** No sockets have been added to the context. */
  kEtcPalErrNoSockets = -27,
  /** A function or specific use of a function is not implemented yet. */
  kEtcPalErrNotImpl = -28,
  /** The operation is not permitted. */
  kEtcPalErrPerm = -29,
  /** A system call or C library call failed in a way not covered by other errors. */
  kEtcPalErrSys = -30,
} etcpal_error_t;

/** The total number of error codes currently defined. */
#define ETCPAL_NUM_ERROR_CODES 31

const char* etcpal_strerror(etcpal_error_t code);

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif /* ETCPAL_ERROR_H_ */
