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
 * This file is a part of lwpa. For more information, go to:
 * https://github.com/ETCLabs/lwpa
 ******************************************************************************/
#include <FreeRTOS.h>
#include <task.h>

#include <lwip/netif.h>
#include "ethernetif.h"
#include "lwip/tcpip.h"

#include "fsl_debug_console.h"
#include "board.h"
#include "fsl_phy.h"
#include "pin_mux.h"
#include "clock_config.h"

#include "unity_fixture.h"

extern void run_all_tests(void);

void main_task(void* pvParameters)
{
  const char* fake_argv[] = {"freertos_test_runner", "-v"};

  UnityMain(2, fake_argv, run_all_tests);
}

void main(void)
{
  ip4_addr_t fsl_netif0_ipaddr, fsl_netif0_netmask, fsl_netif0_gw;
  SYSMPU_Type* base = SYSMPU;
  ethernetif_config_t enet_config0;

  /* Init board hardware. */
  BOARD_InitPins();
  BOARD_BootClockRUN();
  BOARD_InitDebugConsole();

  /* Disable MPU. */
  base->CESR &= ~SYSMPU_CESR_VLD_MASK;

  PRINTF("TCP/IP initializing...\n");
  tcpip_init(NULL, NULL);
  PRINTF("TCP/IP initialized.\n");

  IP4_ADDR(&fsl_netif0_ipaddr, configIP_ADDR0, configIP_ADDR1, configIP_ADDR2, configIP_ADDR3);
  IP4_ADDR(&fsl_netif0_netmask, configNET_MASK0, configNET_MASK1, configNET_MASK2, configNET_MASK3);
  IP4_ADDR(&fsl_netif0_gw, configGW_ADDR0, configGW_ADDR1, configGW_ADDR2, configGW_ADDR3);

  enet_config0.phyAddress = ENET_PHY_ADDRESS;
  enet_config0.clockName = kCLOCK_CoreSysClk;
  memcpy(enet_config0.macAddress, g_EnetMacAddr, sizeof g_EnetMacAddr);

  netif_add(&fsl_netif0, &fsl_netif0_ipaddr, &fsl_netif0_netmask, &fsl_netif0_gw, &enet_config0, ethernetif0_init,
            tcpip_input);
  netif_set_default(&fsl_netif0);
  netif_set_up(&fsl_netif0);

  phy_speed_t speed;
  phy_duplex_t duplex;
  if (kStatus_Success == PHY_GetLinkSpeedDuplex(ENET, ENET_PHY_ADDRESS, &speed, &duplex))
  {
    PRINTF("Ethernet link: %s %s\n", speed == kPHY_Speed10M ? "10M" : "100M",
           duplex == kPHY_HalfDuplex ? "half duplex" : "full duplex");
  }

  /* Create RTOS tasks */
  xTaskCreate(main_task, "test", 2000, NULL, 4, NULL);
  vTaskStartScheduler();

  for (;;)
  {               /* Infinite loop to avoid leaving the main function */
    __asm("NOP"); /* something to use as a breakpoint stop while looping */
  }
}
