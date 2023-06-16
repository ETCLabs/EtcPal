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

/* The os_netint implementation for Linux.
 *
 * Uses getifaddrs() to get an initial list of network interfaces. More info:
 * http://man7.org/linux/man-pages/man3/getifaddrs.3.html
 *
 * Then uses netlink sockets (specifically RTNETLINK) to fill out missing information about each
 * interface. More info:
 *
 * Netlink sockets: http://man7.org/linux/man-pages/man7/netlink.7.html
 * Netlink macros, for decoding netlink messages: http://man7.org/linux/man-pages/man3/netlink.3.html
 * RtNetlink sockets: http://man7.org/linux/man-pages/man7/rtnetlink.7.html
 * Some sample RtNetlink code: https://www.linuxjournal.com/article/8498
 */

#include "etcpal/netint.h"

#include <stdlib.h>
#include <limits.h>
#include <string.h>

#ifdef ETCPAL_NETINT_DEBUG_OUTPUT
#include <stdio.h>
#endif

#include <errno.h>
#include <net/net_if.h>
#include <sys/types.h>
//#include <sys/ioctl.h>
#include <net/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "etcpal/common.h"
#include "etcpal/socket.h"
#include "etcpal/private/netint.h"

#include "logging/log.h"
LOG_MODULE_REGISTER(ETCPAL_OS_NETINT);

//#include "os_error.h"

/***************************** Private types *********************************/

typedef struct RoutingTableEntry
{
  EtcPalIpAddr addr;
  EtcPalIpAddr mask;
  EtcPalIpAddr gateway;
  int          interface_index;
  int          metric;
} RoutingTableEntry;

typedef struct RoutingTable
{
  RoutingTableEntry* entries;
  RoutingTableEntry* default_route;
  size_t             size;
} RoutingTable;

/**************************** Private variables ******************************/

RoutingTable routing_table_v4;
RoutingTable routing_table_v6;

/*********************** Private function prototypes *************************/
#ifdef NAM
// Functions for building the routing tables
static etcpal_error_t build_routing_tables(void);
static etcpal_error_t build_routing_table(int family, RoutingTable* table);
static void           free_routing_tables(void);
static void           free_routing_table(RoutingTable* table);

// Interacting with RTNETLINK
static etcpal_error_t send_netlink_route_request(int socket, int family);
static etcpal_error_t receive_netlink_route_reply(int sock, int family, size_t buf_size, RoutingTable* table);
static etcpal_error_t parse_netlink_route_reply(int           family,
                                                const char*   buffer,
                                                size_t        nl_msg_size,
                                                RoutingTable* table);

// Manipulating routing table entries
static void init_routing_table_entry(RoutingTableEntry* entry);
static int  compare_routing_table_entries(const void* a, const void* b);
#endif // NAM

#if ETCPAL_NETINT_DEBUG_OUTPUT
static void debug_print_routing_table(RoutingTable* table);
#endif

/*************************** Function definitions ****************************/
void _if_counter(struct net_if *iface, void *user_data)
{
	CachedNetintInfo* cache = (CachedNetintInfo*)user_data;
	cache->num_netints++;
}

typedef struct CacheAndIfaceIndex
{
	uint32_t index;
	CachedNetintInfo* cache;
}CacheAndIfaceIndex;

void _if_lister(struct net_if *iface, void *user_data)
{
	CacheAndIfaceIndex* ud = (CacheAndIfaceIndex*)user_data;
	EtcPalNetintInfo* netIntInfo = &ud->cache->netints[ud->index];
  
	struct in_addr* addr = &iface->config.ip.ipv4->unicast[0].address.in_addr;
	struct in_addr* mask = &iface->config.ip.ipv4->netmask;
	netIntInfo->index = net_if_get_by_iface(iface);
	
	// Right now, only support v4. Can add v6 later
	// Zephyr also has the ability to assign more than one ip configuration
	// to a single iface; this does not seem to be exposed in etcpal networking
	// for now, we'll only look a the first ip configuration on the iface.
	netIntInfo->addr.addr.v4 = addr->s4_addr32[0];
	netIntInfo->addr.type = kEtcPalIpTypeV4;

	netIntInfo->mask.addr.v4 = mask->s4_addr32[0];
	netIntInfo->mask.type = kEtcPalIpTypeV4;
	
	netIntInfo->mac.data[0] = iface->if_dev->link_addr.addr[0];
	netIntInfo->mac.data[1] = iface->if_dev->link_addr.addr[1];
	netIntInfo->mac.data[2] = iface->if_dev->link_addr.addr[2];
	netIntInfo->mac.data[3] = iface->if_dev->link_addr.addr[3];
	netIntInfo->mac.data[4] = iface->if_dev->link_addr.addr[4];
	netIntInfo->mac.data[5] = iface->if_dev->link_addr.addr[5];
	
	strncpy(netIntInfo->id, iface->if_dev->dev->name, ETCPAL_NETINTINFO_ID_LEN);
	strncpy(netIntInfo->friendly_name, iface->if_dev->dev->name, ETCPAL_NETINTINFO_ID_LEN);

	if (net_if_get_default() == iface)
	{
		netIntInfo->is_default = true;
	}
	else
	{
		netIntInfo->is_default = false;
	}

	ud->index++;
}

/*!
    \brief Get the number of network interfaces from Zephyr
    \param cache The cache data, to be referenced in order to more quickly get interface data without going through the operating system.
           The num_netints field is modified!
    \return The number of network interfaces that Zephyr is aware of.
*/
unsigned os_count_interfaces(CachedNetintInfo* cache)
{
	cache->num_netints = 0;
	net_if_foreach(_if_counter, cache);
	return cache->num_netints;
}

etcpal_error_t os_enumerate_interfaces(CachedNetintInfo* cache)
{
	etcpal_error_t err = kEtcPalErrOk;
	CacheAndIfaceIndex cache_and_index;
	cache_and_index.cache = cache;
	cache_and_index.index = 0;
	
  // Populates cache->num_netints
	os_count_interfaces(cache_and_index.cache);

	// Throw out old data
	if (cache->netints)
	{
		free(cache->netints);
	}

	// Allocate an array large enough to hold pointers to each NetInt structure
	cache->netints = calloc(sizeof(CachedNetintInfo), cache->num_netints);
	if (cache->netints)
	{
		// Go through all interfaces and get data about each iface
		net_if_foreach(_if_lister, &cache_and_index);
	}
	else
	{
		LOG_ERR("Could not allocate network interface cache!");
		err = kEtcPalErrNoMem;
	}
	
	return err;
}

void os_free_interfaces(CachedNetintInfo* cache)
{
  if (cache->netints)
  {
    free(cache->netints);
    cache->netints = NULL;
  }
}

etcpal_error_t os_resolve_route(const EtcPalIpAddr* dest, const CachedNetintInfo* cache, unsigned int* index)
{
  ETCPAL_UNUSED_ARG(cache);

  RoutingTable* table_to_use = (ETCPAL_IP_IS_V6(dest) ? &routing_table_v6 : &routing_table_v4);

  unsigned int index_found = 0;
  for (RoutingTableEntry* entry = table_to_use->entries; entry < table_to_use->entries + table_to_use->size; ++entry)
  {
    if (entry->interface_index <= 0 || ETCPAL_IP_IS_INVALID(&entry->mask))
      continue;

    // Check each route to see if it matches the destination address explicitly
    if (etcpal_ip_network_portions_equal(&entry->addr, dest, &entry->mask))
    {
      index_found = (unsigned int)entry->interface_index;
      break;
    }
  }

  // Fall back to the default route
  if (index_found == 0 && table_to_use->default_route)
    index_found = (unsigned int)table_to_use->default_route->interface_index;

  if (index_found > 0)
  {
    *index = index_found;
    return kEtcPalErrOk;
  }
  return kEtcPalErrNotFound;
}

bool os_netint_is_up(unsigned int index, const CachedNetintInfo* cache)
{
  ETCPAL_UNUSED_ARG(cache);

  return false;
}

#if ETCPAL_NETINT_DEBUG_OUTPUT
void debug_print_routing_table(RoutingTable* table)
{
  printf("%-40s %-40s %-40s %-10s %s\n", "Address", "Mask", "Gateway", "Metric", "Index");
  for (RoutingTableEntry* entry = table->entries; entry < table->entries + table->size; ++entry)
  {
    char addr_str[ETCPAL_IP_STRING_BYTES];
    char mask_str[ETCPAL_IP_STRING_BYTES];
    char gw_str[ETCPAL_IP_STRING_BYTES];

    if (!ETCPAL_IP_IS_INVALID(&entry->addr))
      etcpal_ip_to_string(&entry->addr, addr_str);
    else
      strcpy(addr_str, "0.0.0.0");

    if (!ETCPAL_IP_IS_INVALID(&entry->mask))
      etcpal_ip_to_string(&entry->mask, mask_str);
    else
      strcpy(mask_str, "0.0.0.0");

    if (!ETCPAL_IP_IS_INVALID(&entry->gateway))
      etcpal_ip_to_string(&entry->gateway, gw_str);
    else
      strcpy(gw_str, "0.0.0.0");

    printf("%-40s %-40s %-40s %-10d %d\n", addr_str, mask_str, gw_str, entry->metric, entry->interface_index);
  }
}
#endif
