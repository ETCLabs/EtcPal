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

static int compare_netints(const void* a, const void* b);

/*************************** Function definitions ****************************/

etcpal_error_t etcpal_netint_init(void)
{
  etcpal_error_t res = kEtcPalErrSys;

  if (etcpal_mutex_create(&mutex))
  {
    res = os_enumerate_interfaces(&netint_cache);

    if (res == kEtcPalErrOk)
    {
      // Sort the interfaces by OS index
      qsort(netint_cache.netints, netint_cache.num_netints, sizeof(EtcPalNetintInfo), compare_netints);

      initialized = true;
    }
  }

  return res;
}

void etcpal_netint_deinit(void)
{
  os_free_interfaces(&netint_cache);
  memset(&netint_cache, 0, sizeof(netint_cache));
  etcpal_mutex_destroy(&mutex);
  initialized = false;
}

/**
 * @brief Get the number of network interfaces present on the system.
 * @return Number of interfaces present.
 */
size_t etcpal_netint_get_num_interfaces(void)
{
  size_t res = 0;

  if (etcpal_mutex_lock(&mutex))
  {
    if (initialized)
      res = netint_cache.num_netints;

    etcpal_mutex_unlock(&mutex);
  }

  return res;
}

/**
 * @brief Get a list of network interfaces on the system.
 *
 * For NICs with multiple IP addresses assigned, this module separates each address into its own
 * entry in the netint array. Because of this, multiple array entries could have the same value
 * for the index, mac and id parameters.
 *
 * @return Pointer to an array of network interfaces of length etcpal_netint_get_num_interfaces(),
 *         or NULL if there are no interfaces present or the module is not initialized.
 */
const EtcPalNetintInfo* etcpal_netint_get_interfaces(void)
{
  const EtcPalNetintInfo* res = NULL;

  if (etcpal_mutex_lock(&mutex))
  {
    if (initialized)
      res = netint_cache.netints;

    etcpal_mutex_unlock(&mutex);
  }

  return res;
}

/**
 * @brief Get a set of network interface addresses that have the index specified.
 *
 * See @ref interface_indexes for more information.
 *
 * @param[in] index Index for which to get interfaces.
 * @param[out] netint_arr Filled in on success with the array of matching interfaces.
 * @param[out] netint_arr_size Filled in on success with the size of the matching interface array.
 * @return #kEtcPalErrOk: netint_arr and netint_arr_size were filled in.
 * @return #kEtcPalErrInvalid: Invalid argument provided.
 * @return #kEtcPalErrNotInit: Module not initialized.
 * @return #kEtcPalErrNotFound: No interfaces found for this index.
 */
etcpal_error_t etcpal_netint_get_interfaces_by_index(unsigned int             index,
                                                     const EtcPalNetintInfo** netint_arr,
                                                     size_t*                  netint_arr_size)
{
  if (index == 0 || !netint_arr || !netint_arr_size)
    return kEtcPalErrInvalid;
  if (!initialized)
    return kEtcPalErrNotInit;

  if (!etcpal_mutex_lock(&mutex))
    return kEtcPalErrSys;

  size_t arr_size = 0;
  for (const EtcPalNetintInfo* netint = netint_cache.netints; netint < netint_cache.netints + netint_cache.num_netints;
       ++netint)
  {
    if (netint->index == index)
    {
      if (arr_size == 0)
      {
        // Found the beginning of the array slice.
        *netint_arr = netint;
      }
      ++arr_size;
    }
    else if (netint->index > index)
    {
      // Done.
      break;
    }
    // Else we haven't gotten there yet, continue
  }

  etcpal_error_t res = kEtcPalErrOk;
  if (arr_size == 0)
    res = kEtcPalErrNotFound;
  else
    *netint_arr_size = arr_size;

  etcpal_mutex_unlock(&mutex);
  return res;
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
  os_free_interfaces(&netint_cache);
  memset(&netint_cache, 0, sizeof(netint_cache));

  // Now re-populate the new cache
  etcpal_error_t res = os_enumerate_interfaces(&netint_cache);

  if (res == kEtcPalErrOk)  // Sort the interfaces by OS index
    qsort(netint_cache.netints, netint_cache.num_netints, sizeof(EtcPalNetintInfo), compare_netints);

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
