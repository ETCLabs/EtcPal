/******************************************************************************
 * Copyright 2018 ETC Inc.
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

/* lwpa/error.h: Platform-neutral error codes. */
#ifndef _LWPA_ERROR_H_
#define _LWPA_ERROR_H_

#include <stdlib.h>

/*! \defgroup lwpa_error lwpa_error
 *  \ingroup lwpa
 *  \brief Platform-neutral error codes.
 *
 *  \#include "lwpa/error.h"
 *
 *  @{
 */

/*! A set of error codes that can be returned by library functions. */
typedef enum
{
  /*! The call was successful, no error occurred. */
  LWPA_OK = 0,
  /*! An identifier or handle passed to a function was not valid or not previously initialized. */
  LWPA_NOTFOUND = -1,
  /*! A dynamic memory allocation failed, or there is no space left in a statically allocated
   *  array. */
  LWPA_NOMEM = -2,
  /*! The resource being requested is temporarily unavailable. */
  LWPA_BUSY = -3,
  /*! The item being created already exists. */
  LWPA_EXIST = -4,
  /*! An invalid argument was provided to an API function. */
  LWPA_INVALID = -5,
  /*! The requested operation would block. */
  LWPA_WOULDBLOCK = -6,
  /*! Returned from a function that is meant to be called repeatedly to indicate that there is no
   *  more data available. */
  LWPA_NODATA = -7,
  /*! A protocol parsing function encountered a malformed packet. */
  LWPA_PROTERR = -8,
  /*! Message too long. */
  LWPA_MSGSIZE = -9,
  /*! The requested address is already in use. */
  LWPA_ADDRINUSE = -10,
  /*! Cannot assign the requested address. */
  LWPA_ADDRNOTAVAIL = -11,
  /*! The operation failed because a network was down or unreachable. */
  LWPA_NETERR = -12,
  /*! A connection was reset (hard/abortive close) by the remote peer. */
  LWPA_CONNRESET = -13,
  /*! A connection was gracefully closed by the remote peer. This code is only used for protocols
   *  above the transport layer. */
  LWPA_CONNCLOSED = -14,
  /*! Transport endpoint is already connected. */
  LWPA_ISCONN = -15,
  /*! Transport endpoint is not connected. */
  LWPA_NOTCONN = -16,
  /*! Transport endpoint is shut down. */
  LWPA_SHUTDOWN = -17,
  /*! A connection timed out. */
  LWPA_TIMEDOUT = -18,
  /*! A connection was refused. */
  LWPA_CONNREFUSED = -19,
  /*! The operation requested is already in progress. */
  LWPA_ALREADY = -20,
  /*! The operation requested is now in progress and will complete later. */
  LWPA_INPROGRESS = -21,
  /*! A buffer provided to a function was not big enough to hold the data that needed to be packed
   *  into it. */
  LWPA_BUFSIZE = -22,
  /*! An API function was called from a module that was not previously initialized. */
  LWPA_NOTINIT = -23,
  /*! No network interfaces were found on the system, or there are no network interfaces of a type
   *  that can satisfy the call being made. */
  LWPA_NOIFACES = -24,
  /*! A function or specific use of a function is not implemented yet. */
  LWPA_NOTIMPL = -25,
  /*! A system call or C library call failed in a way not covered by other errors. */
  LWPA_SYSERR = -26,
} lwpa_error_t;

#define LWPA_NUM_ERROR_CODES 27

#ifdef __cplusplus
extern "C" {
#endif

extern const char *lwpa_error_strings[LWPA_NUM_ERROR_CODES];

/*! \brief Get a string representation of an error code.
 *  \param errcode lwpa error code.
 *  \return Error string (char *) (valid error code) or NULL (invalid error code).
 */
#define lwpa_strerror(errcode) \
  (((int)errcode <= 0 && (int)errcode > -LWPA_NUM_ERROR_CODES) ? lwpa_error_strings[-((int)errcode)] : NULL)

#ifdef __cplusplus
}
#endif

/*!@}*/

#endif /* _LWPA_ERROR_H_ */
