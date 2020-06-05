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

#include "etcpal/error.h"

/* clang-format off */

/**************************** Private variables ******************************/

/* The etcpal_error string array, for use by etcpal_strerror(). */
static const char *etcpal_error_strings[ETCPAL_NUM_ERROR_CODES] =
{
  "No error",                         /* kEtcPalErrOk */
  "Handle not found",                 /* kEtcPalErrNotFound */
  "Out of memory",                    /* kEtcPalErrNoMem */
  "Resource unavailable",             /* kEtcPalErrBusy */
  "Already exists",                   /* kEtcPalErrExists */
  "Invalid argument",                 /* kEtcPalErrInvalid */
  "Operation would block",            /* kEtcPalErrWouldBlock */
  "No data available",                /* kEtcPalErrNoData */
  "Malformed packet",                 /* kEtcPalErrProtocol */
  "Message too long",                 /* kEtcPalErrMsgSize */
  "Address already in use",           /* kEtcPalErrAddrInUse */
  "Cannot assign requested address",  /* kEtcPalErrAddrNotAvail */
  "Network down or unreachable",      /* kEtcPalErrNetwork */
  "Connection reset by peer",         /* kEtcPalErrConnReset */
  "Connection closed by peer",        /* kEtcPalErrConnClosed */
  "Already connected",                /* kEtcPalErrIsConn */
  "Not connected",                    /* kEtcPalErrNotConn */
  "Transport endpoint shut down",     /* kEtcPalErrShutdown */
  "Operation timed out",              /* kEtcPalErrTimedOut */
  "Host is unreachable",              /* kEtcPalErrHostUnreach */
  "Software caused connection abort", /* kEtcPalErrConnAborted */
  "Connection refused",               /* kEtcPalErrConnRefused */
  "Operation already in progress",    /* kEtcPalErrAlready */
  "Operation now in progress",        /* kEtcPalErrInProgress */
  "Buffer not big enough",            /* kEtcPalErrBufSize */
  "Module not initialized",           /* kEtcPalErrNotInit */
  "No network interfaces",            /* kEtcPalErrNoNetints */
  "No sockets",                       /* kEtcPalErrNoSockets */
  "Not implemented",                  /* kEtcPalErrNotImpl */
  "Operation not permitted",          /* kEtcPalErrPerm */
  "System or library call failed",    /* kEtcPalErrSys */
};

/*************************** Function definitions ****************************/

/**
 * @brief Get a string representation of an error code.
 * @param code EtcPal error code.
 * @return Error string (char *) (valid error code) or NULL (invalid error code).
 */
const char* etcpal_strerror(etcpal_error_t code)
{
  if ((int)code <= 0 && (int)code > -ETCPAL_NUM_ERROR_CODES)
  {
    return etcpal_error_strings[-((int)code)];
  }
  return NULL;
}
