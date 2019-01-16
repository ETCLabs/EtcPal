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
#include "lwpatest.h"

#include <mqx.h>
#include <bsp.h>
#include <rtcs.h>

/* NETWORK SETTINGS TO WRITE TO THE DEVICE */
#define ENET_IPADDR IPADDR(10, 101, 2, 125)
#define ENET_IPMASK IPADDR(255, 255, 0, 0)
#define ENET_IPGATEWAY IPADDR(10, 101, 0, 1)

// MQX task template
void main_task(uint32_t);
void watchdog_task(uint32_t);
// This is so we can create lower-priority tasks than the main task.
void dummy_task_fn(uint32_t initial_data)
{
}

/* clang-format off */
TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
  /* Task Index,   Function,    Stack,  Priority, Name,       Attributes,          Param, Time Slice */
  { 1,            main_task,     8000,         9, "main",     MQX_AUTO_START_TASK,     0,          0 },
  { 2,            watchdog_task, 8000,         5, "watchdog", MQX_AUTO_START_TASK,     0,          0 },
  { 3,            dummy_task_fn,  500,        15, "dummy",                      0,     0,          0 },
  { 0 }
};

/* Hacky way to redirect stdout via semihosting over the I-jet debugger.
 * This symbol replaces the default MQX_init_struct via a linker override. */
const MQX_INITIALIZATION_STRUCT  MY_MQX_init_struct =
{
   /* PROCESSOR_NUMBER                */  BSP_DEFAULT_PROCESSOR_NUMBER,
   /* START_OF_KERNEL_MEMORY          */  BSP_DEFAULT_START_OF_KERNEL_MEMORY,
   /* END_OF_KERNEL_MEMORY            */  BSP_DEFAULT_END_OF_KERNEL_MEMORY,
   /* INTERRUPT_STACK_SIZE            */  BSP_DEFAULT_INTERRUPT_STACK_SIZE,
   /* TASK_TEMPLATE_LIST              */  MQX_template_list,
   /* MQX_HARDWARE_INTERRUPT_LEVEL_MAX*/  BSP_DEFAULT_MQX_HARDWARE_INTERRUPT_LEVEL_MAX,
   /* MAX_MSGPOOLS                    */  BSP_DEFAULT_MAX_MSGPOOLS,
   /* MAX_MSGQS                       */  BSP_DEFAULT_MAX_MSGQS,
   /* IO_CHANNEL                      */  "iodebug:",
   /* IO_OPEN_MODE                    */  BSP_DEFAULT_IO_OPEN_MODE,
   0,
   0,
#if MQX_ENABLE_USER_MODE
   BSP_DEFAULT_START_OF_KERNEL_AREA,
   BSP_DEFAULT_END_OF_KERNEL_AREA,

   BSP_DEFAULT_START_OF_USER_DEFAULT_MEMORY,
   BSP_DEFAULT_END_OF_USER_DEFAULT_MEMORY,

   BSP_DEFAULT_START_OF_USER_HEAP,
   BSP_DEFAULT_END_OF_USER_HEAP,

   BSP_DEFAULT_START_OF_USER_RW_MEMORY,
   BSP_DEFAULT_END_OF_USER_RW_MEMORY,

   BSP_DEFAULT_START_OF_USER_RO_MEMORY,
   BSP_DEFAULT_END_OF_USER_RO_MEMORY,

   BSP_DEFAULT_START_OF_USER_NO_MEMORY,
   BSP_DEFAULT_END_OF_USER_NO_MEMORY,

   BSP_DEFAULT_MAX_USER_TASK_PRIORITY,
   BSP_DEFAULT_MAX_USER_TASK_COUNT,
#endif
};
/* clang-format on */

/* Write network settings to device */
uint32_t setup_network()
{
  uint32_t error = 0;
  IPCFG_IP_ADDRESS_DATA ip_data;
  _enet_address enet_address;
  _enet_handle ehandle;
  _rtcs_if_handle ihandle;
  _rtcs_if_handle lbhandle;

  ip_data.ip = ENET_IPADDR;
  ip_data.mask = ENET_IPMASK;
  ip_data.gateway = ENET_IPGATEWAY;

  /* Create TCP/IP task */
  error = RTCS_create();
  if (error)
    return error;

  /* Get the Ethernet address of the device */
  ENET_get_mac_address(BSP_DEFAULT_ENET_DEVICE, ENET_IPADDR, enet_address);

  error = ENET_initialize(BSP_DEFAULT_ENET_DEVICE, enet_address, 0, &ehandle);
  if (error == 0)
  {
    error = RTCS_if_add(ehandle, RTCS_IF_ENET, &ihandle);
    if (error == 0)
    {
      error = ipcfg_init_interface(BSP_DEFAULT_ENET_DEVICE, ihandle);
      if (error != 0)
      {
        RTCS_if_remove(ihandle);
        ENET_shutdown(ehandle);
        printf("IPCFG init error: %08x\n", error);
      }
    }
    else
    {
      ENET_shutdown(ehandle);
      printf("ENET Interface add error: %08x\n", error);
    }
    /* This voodoo was taken from the ACN C++ test app, and apparently is
     * necessary in order to receive your own multicast traffic. */
    error = RTCS_if_add(ehandle, RTCS_IF_LOCALHOST, &lbhandle);
    if (error == 0)
    {
      error = RTCS_if_bind(lbhandle, 0x7F000001, 0xFF000000);
      if (error != 0)
      {
        RTCS_if_remove(lbhandle);
        ENET_shutdown(ehandle);
        printf("Loopback interface bind error: %08x\n", error);
      }
    }
    else
    {
      ENET_shutdown(ehandle);
      printf("LOCALHOST Interface add error: %08x\n", error);
    }
  }
  else if (error == ENETERR_INIT_DEVICE)
    printf("Device reinitialization... \n");
  else
    printf("Device initialization error: %08x\n", error);

  /* Bind Ethernet device to network using constant (static) IP address information */
  error = ipcfg_bind_staticip(BSP_DEFAULT_ENET_DEVICE, &ip_data);
  return error;
}

uint32_t getms()
{
  TIME_STRUCT ts;
  _time_get_elapsed(&ts);
  return (ts.SECONDS * 1000 + ts.MILLISECONDS);
}

bool watchdog_enabled;
uint32_t last_kick_time;

void watchdog_kick()
{
  watchdog_enabled = true;
  last_kick_time = getms();
}

void watchdog_disable()
{
  watchdog_enabled = false;
}

void watchdog_task(uint32_t initial_data)
{
  while (1)
  {
    _time_delay(100);
    if (watchdog_enabled && getms() - last_kick_time > 5000)
      break;
  }

  /* Watchdog timed out! Signal that the test is over. */
  OUTPUT_TEST_SUITE_TIMEOUT();
  _time_delay(1000);
  _mqx_exit(1);
}

void main_task(uint32_t initial_data)
{
  setup_network();
  OUTPUT_TEST_SUITE_BEGIN();
  test_lock();
  test_thread();
  test_mempool();
  test_rbtree();
  test_log();
  test_pack();
  test_timer();
  test_cid();
  test_netint();
  test_socket();
  watchdog_disable();
  OUTPUT_TEST_SUITE_END();
}
