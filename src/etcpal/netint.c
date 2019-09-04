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

#include "etcpal/netint.h"
#include "etcpal/private/netint.h"

#include <stdlib.h>

/**************************** Private variables ******************************/

static unsigned int init_count;
static CachedNetintInfo netint_cache;

/*********************** Private function prototypes *************************/

static int compare_netints(const void* a, const void* b);

/*************************** Function definitions ****************************/

etcpal_error_t etcpal_netint_init()
{
  etcpal_error_t res = kLwpaErrOk;
  if (init_count == 0)
  {
    res = os_enumerate_interfaces(&netint_cache);
    if (res == kLwpaErrOk)
    {
      // Sort the interfaces by OS index
      qsort(netint_cache.netints, netint_cache.num_netints, sizeof(LwpaNetintInfo), compare_netints);
    }
  }

  if (res == kLwpaErrOk)
    ++init_count;

  return res;
}

void etcpal_netint_deinit()
{
  if (--init_count == 0)
  {
    os_free_interfaces(&netint_cache);
    memset(&netint_cache, 0, sizeof(netint_cache));
  }
}

/*! \brief Get the number of network interfaces present on the system.
 *  \return Number of interfaces present.
 */
size_t etcpal_netint_get_num_interfaces()
{
  return (init_count ? netint_cache.num_netints : 0);
}

/*! \brief Get a list of network interfaces on the system.
 *
 *  For NICs with multiple IP addresses assigned, this module separates each address into its own
 *  entry in the netint array. Because of this, multiple array entries could have the same value
 *  for the index, mac and name parameters.
 *
 *  \return Pointer to an array of network interfaces of length etcpal_netint_get_num_interfaces(),
 *          or NULL if there are no interfaces present or the module is not initialized.
 */
const LwpaNetintInfo* etcpal_netint_get_interfaces()
{
  return (init_count ? netint_cache.netints : NULL);
}

/*! \brief Copy the list of network interfaces on the system into an array.
 *
 *  For NICs with multiple IP addresses assigned, this module separates each address into its own
 *  entry in the netint array. Because of this, multiple array entries could have the same value
 *  for the index, mac and name parameters.
 *
 *  \param[out] netint_arr Array of network interface description structs to fill in with interface
 *                         info.
 *  \param[in] netint_arr_size Size of the netint array.
 *  \return Number of network interfaces that were copied, up to a maximum of netint_arr_size,
 *          or 0 if there are no interfaces present or the module is not initialized.
 */
size_t etcpal_netint_copy_interfaces(LwpaNetintInfo* netint_arr, size_t netint_arr_size)
{
  if (!init_count || !netint_arr || netint_arr_size == 0)
    return 0;

  size_t addrs_copied = (netint_arr_size < netint_cache.num_netints ? netint_arr_size : netint_cache.num_netints);
  memcpy(netint_arr, netint_cache.netints, addrs_copied * sizeof(LwpaNetintInfo));
  return addrs_copied;
}

/*! \brief Get a set of network interface addresses that have the index specified.
 *
 *  See \ref interface_indexes for more information.
 *
 *  \param[in] index Index for which to get interfaces.
 *  \param[out] netint_arr Filled in on success with the array of matching interfaces.
 *  \param[out] netint_arr_size Filled in on success with the size of the matching interface array.
 *  \return #kLwpaErrOk: netint_arr and netint_arr_size were filled in.
 *  \return #kLwpaErrInvalid: Invalid argument provided.
 *  \return #kLwpaErrNotFound: No interfaces found for this index.
 */
etcpal_error_t etcpal_netint_get_interfaces_by_index(unsigned int index, const LwpaNetintInfo** netint_arr,
                                                 size_t* netint_arr_size)
{
  if (index == 0 || !netint_arr || !netint_arr_size)
    return kLwpaErrInvalid;

  size_t arr_size = 0;
  for (const LwpaNetintInfo* netint = netint_cache.netints; netint < netint_cache.netints + netint_cache.num_netints;
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

  if (arr_size != 0)
  {
    *netint_arr_size = arr_size;
    return kLwpaErrOk;
  }
  else
  {
    return kLwpaErrNotFound;
  }
}

/*! \brief Get information about the default network interface.
 *
 *  For our purposes, the 'default' network interface is defined as the interface that is chosen
 *  for the default IP route. The default interface is given as an OS network interface index - see
 *  \ref interface_indexes for more information. Note that since network interfaces can have
 *  multiple IP addresses assigned, this index may be shared by many entries returned by
 *  etcpal_netint_get_interfaces().
 *
 *  \param[in] type The IP protocol for which to get the default network interface, either
 *                  #kLwpaIpTypeV4 or #kLwpaIpTypeV6. A separate default interface is maintained for
 *                  each.
 *  \param[out] netint_index Pointer to value to fill with the index of the default interface.
 *  \return #kLwpaErrOk: netint was filled in.
 *  \return #kLwpaErrInvalid: Invalid argument provided.
 *  \return #kLwpaErrNotFound: No default interface found for this type.
 */
etcpal_error_t etcpal_netint_get_default_interface(etcpal_iptype_t type, unsigned int* netint_index)
{
  if (init_count && netint_index)
  {
    if (type == kLwpaIpTypeV4)
    {
      if (netint_cache.def.v4_valid)
      {
        *netint_index = netint_cache.def.v4_index;
        return kLwpaErrOk;
      }
      else
      {
        return kLwpaErrNotFound;
      }
    }
    else if (type == kLwpaIpTypeV6)
    {
      if (netint_cache.def.v6_valid)
      {
        *netint_index = netint_cache.def.v6_index;
        return kLwpaErrOk;
      }
      else
      {
        return kLwpaErrNotFound;
      }
    }
  }
  return kLwpaErrInvalid;
}

/*! \brief Get the network interface that the system will choose when routing an IP packet to the
 *         specified destination.
 *
 *  \param[in] dest IP address of the destination.
 *  \param[out] netint Pointer to network interface description struct to fill in with information
 *                     about the chosen interface.
 *  \return #kLwpaErrOk: Netint filled in successfully.
 *  \return #kLwpaErrInvalid: Invalid argument provided.
 *  \return #kLwpaErrNotInit: Module not initialized.
 *  \return #kLwpaErrNoNetints: No network interfaces found on system.
 *  \return #kLwpaErrNotFound: No route was able to be resolved to the destination.
 */
etcpal_error_t etcpal_netint_get_interface_for_dest(const LwpaIpAddr* dest, unsigned int* netint_index)
{
  if (!dest || !netint_index)
    return kLwpaErrInvalid;
  if (!init_count)
    return kLwpaErrNotInit;
  if (netint_cache.num_netints == 0)
    return kLwpaErrNoNetints;

  return os_resolve_route(dest, &netint_cache, netint_index);
}

int compare_netints(const void* a, const void* b)
{
  LwpaNetintInfo* netint1 = (LwpaNetintInfo*)a;
  LwpaNetintInfo* netint2 = (LwpaNetintInfo*)b;

  return (netint1->index > netint2->index) - (netint1->index < netint2->index);
}
