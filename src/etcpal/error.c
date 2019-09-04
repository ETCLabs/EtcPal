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

#include "etcpal/error.h"

/* clang-format off */

/* The etcpal_error string array, for use by etcpal_strerror(). */
const char *etcpal_error_strings[LWPA_NUM_ERROR_CODES] =
{
  "No error",                         /* kLwpaErrOk */
  "Handle not found",                 /* kLwpaErrNotFound */
  "Out of memory",                    /* kLwpaErrNoMem */
  "Resource unavailable",             /* kLwpaErrBusy */
  "Already exists",                   /* kLwpaErrExists */
  "Invalid argument",                 /* kLwpaErrInvalid */
  "Operation would block",            /* kLwpaErrWouldBlock */
  "No data available",                /* kLwpaErrNoData */
  "Malformed packet",                 /* kLwpaErrProtocol */
  "Message too long",                 /* kLwpaErrMsgSize */
  "Address already in use",           /* kLwpaErrAddrInUse */
  "Cannot assign requested address",  /* kLwpaErrAddrNotAvail */
  "Network down or unreachable",      /* kLwpaErrNetwork */
  "Connection reset by peer",         /* kLwpaErrConnReset */
  "Connection closed by peer",        /* kLwpaErrConnClosed */
  "Already connected",                /* kLwpaErrIsConn */
  "Not connected",                    /* kLwpaErrNotConn */
  "Transport endpoint shut down",     /* kLwpaErrShutdown */
  "Operation timed out",              /* kLwpaErrTimedOut */
  "Software caused connection abort", /* kLwpaErrConnAborted */
  "Connection refused",               /* kLwpaErrConnRefused */
  "Operation already in progress",    /* kLwpaErrAlready */
  "Operation now in progress",        /* kLwpaErrInProgress */
  "Buffer not big enough",            /* kLwpaErrBufSize */
  "Module not initialized",           /* kLwpaErrNotInit */
  "No network interfaces",            /* kLwpaErrNoNetints */
  "No sockets",                       /* kLwpaErrNoSockets */
  "Not implemented",                  /* kLwpaErrNotImpl */
  "Operation not permitted",          /* kLwpaErrPerm */
  "System or library call failed",    /* kLwpaErrSys */
};
