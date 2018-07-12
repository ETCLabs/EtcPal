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

#include "lwpa_error.h"

/* clang-format off */

/* The lwpa_error string array, for use by lwpa_strerror(). */
const char *lwpa_error_strings[LWPA_NUM_ERROR_CODES] =
{
  "No error",                        /* LWPA_OK */
  "Handle not found",                /* LWPA_NOTFOUND */
  "Out of memory",                   /* LWPA_NOMEM */
  "Resource unavailable",            /* LWPA_BUSY */
  "Already exists",                  /* LWPA_EXIST */
  "Invalid argument",                /* LWPA_INVALID */
  "Operation would block",           /* LWPA_WOULDBLOCK */
  "No data available",               /* LWPA_NODATA */
  "Malformed packet",                /* LWPA_PROTERR */
  "Message too long",                /* LWPA_MSGSIZE */
  "Address already in use",          /* LWPA_ADDRINUSE */
  "Cannot assign requested address", /* LWPA_ADDRNOTAVAIL */
  "Network down or unreachable",     /* LWPA_NETERR */
  "Connection reset by peer",        /* LWPA_CONNRESET */
  "Connection closed by peer",       /* LWPA_CONNCLOSED */
  "Already connected",               /* LWPA_ISCONN */
  "Not connected",                   /* LWPA_NOTCONN */
  "Transport endpoint shut down",    /* LWPA_SHUTDOWN */
  "Operation timed out",             /* LWPA_TIMEDOUT */
  "Connection refused",              /* LWPA_CONNREFUSED */
  "Operation already in progress",   /* LWPA_ALREADY */
  "Operation now in progress",       /* LWPA_INPROGRESS */
  "Buffer not big enough",           /* LWPA_BUFSIZE */
  "Module not initialized",          /* LWPA_NOTINIT */
  "No network interfaces",           /* LWPA_NOIFACES */
  "Not implemented",                 /* LWPA_NOTIMPL */
  "System or library call failed",   /* LWPA_SYSERR */
};
