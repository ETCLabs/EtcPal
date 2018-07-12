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
#include <cstdio>
#include <cstdint>
#include "gtest/gtest.h"
#include <Ws2tcpip.h>

// Need to pass this from the command line to a test case; there doesn't seem
// to be a better way to do this than using a global variable.
in_addr g_netint;

int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);

  if (argc < 2 || 0 >= inet_pton(AF_INET, argv[1], &g_netint))
  {
    printf("Usage: %s <interface_addr>\n"
           "  interface_addr: IP address of network interface to use for "
             "test.\n", argv[0]);
    return 1;
  }

  printf("Starting googletest...\n");
  return RUN_ALL_TESTS();
}
