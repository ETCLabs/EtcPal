#include "lwpatest.h"
#include <mqx.h>
#include <string.h>
#include "lwpa_netint.h"

static LwpaNetintInfo *g_interfaces;
static size_t g_num_interfaces;

bool test_enumerate()
{
  bool ok;
  /* We are making an assumption that this test will be run on a system with at least one network interface. */
  g_num_interfaces = netint_get_num_interfaces();
  ok = (g_num_interfaces > 0);
  if (ok)
  {
    g_interfaces = _mem_alloc_zero(g_num_interfaces * sizeof(LwpaNetintInfo));
    ok = (g_interfaces != NULL);
  }
  if (ok)
  {
    ok = (g_num_interfaces == netint_get_interfaces(g_interfaces, g_num_interfaces));
  }
  if (ok)
  {
    size_t i;
    size_t num_defaults = 0;
    for (i = 0; i < g_num_interfaces; ++i)
    {
      ok = (0 < strlen(g_interfaces[i].name));
      if (!ok)
        break;
      if (g_interfaces[i].is_default)
        ++num_defaults;
    }
    ok = (num_defaults == 1);
  }
  return ok;
}

bool test_default()
{
  bool ok;
  LwpaNetintInfo def_int;

  memset(&def_int, 0, sizeof def_int);
  ok = netint_get_default_interface(&def_int);
  if (ok)
    ok = def_int.is_default;
  if (ok)
  {
    size_t i;
    for (i = 0; i < g_num_interfaces; ++i)
    {
      if (g_interfaces[i].is_default)
      {
        ok = (0 == memcmp(&def_int, &g_interfaces[i], sizeof def_int));
        if (!ok)
          break;
      }
    }
  }
  return ok;
}

/* TODO - test IPv6 */
bool test_route()
{
  bool ok = true;
  LwpaNetintInfo *iface;

  for (iface = g_interfaces; iface < g_interfaces + g_num_interfaces; ++iface)
  {
    uint32_t host;
    uint32_t net;
    uint32_t neighbor;
    uint32_t remote;
    uint32_t mask;
    LwpaIpAddr dest_ip;
    const LwpaNetintInfo *dest;

    if (lwpaip_is_v6(&iface->addr))
      continue;

    mask = lwpaip_v4_address(&iface->mask);
    host = lwpaip_v4_address(&iface->addr) & (~mask);
    net = lwpaip_v4_address(&iface->addr) & mask;

    /* Simulate another host on the same subnet. */
    neighbor = net | ((host + 1) & (~mask));

    /* Simulate a host on a remote network by flipping the bottom bit of the network portion. */
    remote = 0x1u | (net ^ ((~mask) + 1));

    /* Test the local neighbor */
    lwpaip_set_v4_address(&dest_ip, neighbor);
    dest = netint_get_iface_for_dest(&dest_ip, g_interfaces, g_num_interfaces);
    ok = (dest == iface);
    if (!ok)
      break;

    /* Test the remote host */
    lwpaip_set_v4_address(&dest_ip, remote);
    dest = netint_get_iface_for_dest(&dest_ip, g_interfaces, g_num_interfaces);
    ok = (dest->is_default);
    if (!ok)
      break;
  }

  return ok;
}

void test_netint()
{
  bool ok;

  OUTPUT_TEST_MODULE_BEGIN("netint");
  watchdog_kick();
  OUTPUT_TEST_BEGIN("enumerate");
  OUTPUT_TEST_RESULT((ok = test_enumerate()));
  if (ok)
  {
    watchdog_kick();
    OUTPUT_TEST_BEGIN("default");
    OUTPUT_TEST_RESULT((ok = test_default()));
  }
  if (ok)
  {
    watchdog_kick();
    OUTPUT_TEST_BEGIN("route");
    OUTPUT_TEST_RESULT((ok = test_route()));
  }
  if (g_interfaces)
    _mem_free(g_interfaces);
  OUTPUT_TEST_MODULE_END("netint", ok);
}
