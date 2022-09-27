/******************************************************************************
 * Copyright 2022 ETC Inc.
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

#ifndef ETCPAL_NO_NETWORKING_SUPPORT

#include "etcpal/netint.h"

#include <stdlib.h>
#include "etcpal/common.h"
#include "etcpal/mutex.h"
#include "etcpal/private/netint.h"

/**************************** Private variables ******************************/

static bool             initialized = false;
static CachedNetintInfo netint_cache;
etcpal_mutex_t          mutex;

/*********************** Private function prototypes *************************/

static int            compare_netints(const void* a, const void* b);
static etcpal_error_t populate_netint_cache();
static void           clear_netint_cache();
static etcpal_error_t get_interfaces(EtcPalNetintInfo* netints,
                                     size_t*           num_netints,
                                     bool              specific_index,
                                     unsigned int      index);

/*************************** Function definitions ****************************/

etcpal_error_t etcpal_netint_init(void)
{
  etcpal_error_t res = kEtcPalErrSys;

  if (etcpal_mutex_create(&mutex))
  {
    res = populate_netint_cache();

    if (res == kEtcPalErrOk)
      initialized = true;
  }

  return res;
}

void etcpal_netint_deinit(void)
{
  clear_netint_cache();
  etcpal_mutex_destroy(&mutex);
  initialized = false;
}

/**
 * @brief Get a list of network interfaces on the system (or just the number of interfaces).
 *
 * For NICs with multiple IP addresses assigned, this module separates each address into its own
 * entry in the netint array. Because of this, multiple array entries could have the same value
 * for the index, mac and id parameters.
 *
 * @param[out] netints Application-provided array to be filled in on success with the array of network interfaces.
 * This can be set to NULL if only interested in the number of interfaces.
 * @param[in,out] num_netints Initialize this with the size of the netints array (0 if it's NULL). Filled in on
 * success with the number of network interfaces on the system (reallocate the netints array if this ends up being
 * larger). Cannot be a NULL pointer.
 * @return #kEtcPalErrOk: netints and num_netints were filled in with all system interfaces.
 * @return #kEtcPalErrInvalid: Invalid argument provided.
 * @return #kEtcPalErrNotInit: Module not initialized.
 * @return #kEtcPalErrBufSize: The netints array was not large enough to hold all of the network interfaces. See the
 * value of num_netints to determine how large the array needs to be.
 * @return #kEtcPalErrNotFound: No system interfaces were found.
 */
etcpal_error_t etcpal_netint_get_interfaces(EtcPalNetintInfo* netints, size_t* num_netints)
{
  return get_interfaces(netints, num_netints, false, 0);
}

/**
 * @brief Get a list of network interfaces (or just the number of interfaces) that have the index specified.
 *
 * See @ref interface_indexes for more information.
 *
 * @param[in] index Index for which to get interfaces.
 * @param[out] netints Application-provided array to be filled in on success with the array of network interfaces.
 * This can be set to NULL if only interested in the number of interfaces.
 * @param[in,out] num_netints Initialize this with the size of the netints array (0 if it's NULL). Filled in on
 * success with the number of network interfaces that match the index (reallocate the netints array if this ends up
 * being larger). Cannot be NULL.
 * @return #kEtcPalErrOk: netints and num_netints were filled in with all matching interfaces.
 * @return #kEtcPalErrInvalid: Invalid argument provided.
 * @return #kEtcPalErrNotInit: Module not initialized.
 * @return #kEtcPalErrBufSize: The netints array was not large enough to hold all of the network interfaces. See the
 * value of num_netints to determine how large the array needs to be.
 * @return #kEtcPalErrNotFound: No interfaces found for this index.
 */
etcpal_error_t etcpal_netint_get_interfaces_for_index(unsigned int      netint_index,
                                                      EtcPalNetintInfo* netints,
                                                      size_t*           num_netints)
{
  return get_interfaces(netints, num_netints, true, netint_index);
}

/**
 * @brief Get information about the default network interface.
 *
 * For our purposes, the 'default' network interface is defined as the interface that is chosen
 * for the default IP route. The default interface is given as an OS network interface index - see
 * @ref interface_indexes for more information. Note that since network interfaces can have
 * multiple IP addresses assigned, this index may be shared by many entries returned by
 * etcpal_netint_get_interfaces().
 *
 * @param[in] type The IP protocol for which to get the default network interface, either
 *                 #kEtcPalIpTypeV4 or #kEtcPalIpTypeV6. A separate default interface is maintained for
 *                 each.
 * @param[out] netint_index Pointer to value to fill with the index of the default interface.
 * @return #kEtcPalErrOk: netint was filled in.
 * @return #kEtcPalErrInvalid: Invalid argument provided.
 * @return #kEtcPalErrNotInit: Module not initialized.
 * @return #kEtcPalErrNotFound: No default interface found for this type.
 */
etcpal_error_t etcpal_netint_get_default_interface(etcpal_iptype_t type, unsigned int* netint_index)
{
  if (!netint_index)
    return kEtcPalErrInvalid;
  if (!initialized)
    return kEtcPalErrNotInit;

  if (!etcpal_mutex_lock(&mutex))
    return kEtcPalErrSys;

  etcpal_error_t res = kEtcPalErrOk;
  if (type == kEtcPalIpTypeV4)
  {
    if (netint_cache.def.v4_valid)
      *netint_index = netint_cache.def.v4_index;
    else
      res = kEtcPalErrNotFound;
  }
  else if (type == kEtcPalIpTypeV6)
  {
    if (netint_cache.def.v6_valid)
      *netint_index = netint_cache.def.v6_index;
    else
      res = kEtcPalErrNotFound;
  }
  else
  {
    res = kEtcPalErrInvalid;
  }

  etcpal_mutex_unlock(&mutex);
  return res;
}

/**
 * @brief Get the network interface that the system will choose when routing an IP packet to the
 *        specified destination.
 *
 * @param[in] dest IP address of the destination.
 * @param[out] netint_index Pointer to value to fill in with the index of the chosen interface.
 * @return #kEtcPalErrOk: Netint filled in successfully.
 * @return #kEtcPalErrInvalid: Invalid argument provided.
 * @return #kEtcPalErrNotInit: Module not initialized.
 * @return #kEtcPalErrNoNetints: No network interfaces found on system.
 * @return #kEtcPalErrNotFound: No route was able to be resolved to the destination.
 */
etcpal_error_t etcpal_netint_get_interface_for_dest(const EtcPalIpAddr* dest, unsigned int* netint_index)
{
  if (!dest || !netint_index)
    return kEtcPalErrInvalid;
  if (!initialized)
    return kEtcPalErrNotInit;

  if (!etcpal_mutex_lock(&mutex))
    return kEtcPalErrSys;

  etcpal_error_t res = kEtcPalErrOk;
  if (netint_cache.num_netints == 0)
    res = kEtcPalErrNoNetints;

  if (res == kEtcPalErrOk)
    res = os_resolve_route(dest, &netint_cache, netint_index);

  etcpal_mutex_unlock(&mutex);
  return res;
}

int compare_netints(const void* a, const void* b)
{
  EtcPalNetintInfo* netint1 = (EtcPalNetintInfo*)a;
  EtcPalNetintInfo* netint2 = (EtcPalNetintInfo*)b;

  return (netint1->index > netint2->index) - (netint1->index < netint2->index);
}

// Needs lock
etcpal_error_t populate_netint_cache()
{
  etcpal_error_t res = os_enumerate_interfaces(&netint_cache);

  if (res == kEtcPalErrOk)  // Sort the interfaces by OS index
    qsort(netint_cache.netints, netint_cache.num_netints, sizeof(EtcPalNetintInfo), compare_netints);

  return res;
}

// Needs lock
void clear_netint_cache()
{
  os_free_interfaces(&netint_cache);
  memset(&netint_cache, 0, sizeof(netint_cache));
}

// Takes lock
etcpal_error_t get_interfaces(EtcPalNetintInfo* netints, size_t* num_netints, bool specific_index, unsigned int index)
{
  if (!num_netints)
    return kEtcPalErrInvalid;
  if ((!netints && (*num_netints > 0)) && (netints && (*num_netints == 0)))
    return kEtcPalErrInvalid;
  if (!initialized)
    return kEtcPalErrNotInit;

  if (!etcpal_mutex_lock(&mutex))
    return kEtcPalErrSys;

  etcpal_error_t res          = kEtcPalErrNotFound;
  size_t         netint_count = 0;
  for (size_t i = 0; i < netint_cache.num_netints; ++i)
  {
    if (!specific_index || (index == netint_cache.netints[i].index))
    {
      res = kEtcPalErrOk;
      if (netints && (netint_count < *num_netints))
        netints[netint_count] = netint_cache.netints[i];
      else
        res = kEtcPalErrBufSize;

      ++netint_count;
    }
  }

  *num_netints = netint_count;

  etcpal_mutex_unlock(&mutex);
  return res;
}

/**
 * @brief Refresh the list of network interfaces.
 *
 * Rebuilds the cached array of network interfaces that is returned via the
 * etcpal_netint_get_interfaces() function.
 *
 * @return #kEtcPalErrOk: Interfaces refreshed.
 * @return Other error codes from the underlying platform are possible here.
 */
etcpal_error_t etcpal_netint_refresh_interfaces()
{
  if (!initialized)
    return kEtcPalErrNotInit;

  if (!etcpal_mutex_lock(&mutex))
    return kEtcPalErrSys;

  // First clean up the old cache
  clear_netint_cache();

  // Now re-populate the new cache
  etcpal_error_t res = populate_netint_cache();

  etcpal_mutex_unlock(&mutex);
  return res;
}

/**
 * @brief Determine whether a network interface is currently up and running.
 *
 * @note On Windows, cached network interface information is used to determine this, so the result
 *       for a given index will not change until etcpal_netint_refresh_interfaces() is called.
 *
 * @param netint_index Index of the interface to check.
 * @return true: The interface indicated by netint_index is up.
 * @return false: The interface indicated by netint_index is down, or netint_index is invalid.
 */
bool etcpal_netint_is_up(unsigned int netint_index)
{
  if (!initialized || netint_index == 0)
    return false;

  if (!etcpal_mutex_lock(&mutex))
    return false;

  bool res = os_netint_is_up(netint_index, &netint_cache);

  etcpal_mutex_unlock(&mutex);
  return res;
}

#endif  // ETCPAL_NO_NETWORKING_SUPPORT
