# ETC Platform Abstraction Layer                                    {#mainpage}

## Introduction

EtcPal is a C-language library for platform abstraction. It supports the
writing of platform-neutral C and C++ libraries by providing a set of
modules to abstract common system calls. There are also a few
platform-neutral utilities (i.e. data structures, logging) thrown in here
and there for convenience.

EtcPal modules are designed to run with low overhead on embedded platforms, and
thus are designed to be as thin a wrapper over the underlying OS functionality
as possible.

To jump right into the documentation, check out the
[Modules Overview](@ref etcpal).

## Source Location

The sources for EtcPal are located in the heirarchy:
```
src/
  etcpal/
    [platform-neutral EtcPal sources]
  os/
    [OS or network stack provider]/
      [platform-specific EtcPal sources]
  [platform-neutral EtcPal sources]
```

The includes are in the heirarchy:
```
include/
  etcpal/
    [platform-neutral EtcPal headers]
  os/
    [OS or network stack provider]/
      [platform-specific EtcPal headers]
  [platform-neutral EtcPal headers]
```

Include the `etcpal/` prefix when including an EtcPal header:
```C
#include "etcpal/lock.h"
```

Some EtcPal headers are platform-specific and duplicated for each platform.
Platform-specific headers for the same module will always conform to an
identical interface, as documented in that module's documentation.

## Supported Platforms

EtcPal distinguishes between two major constructs which provide services to
applications. 'OS' targets provide threading, synchronization and other
utilities not provided by the C standard library. 'Network' targets provide a
network stack.

EtcPal is currently ported for the following OS targets:

+ FreeRTOS
+ macOS
+ Microsoft Windows
+ Linux
+ MQX

And the following network targets:

+ macOS
+ Microsoft Windows
+ Linux
+ lwIP
+ MQX (RTCS)

### Building EtcPal for Your Platform

EtcPal is built with [CMake](https://cmake.org), a popular cross-platform build
system generator. CMake can also be used to include EtcPal as a dependency to
other projects, i.e. using the `add_subdirectory()` command.

To configure and build EtcPal on its own using CMake, follow these steps:

1. [Download and install](https://cmake.org/download/) CMake version 3.3 or
   higher.
2. Create a directory in your location of choice to hold your build projects or
   Makefiles:
   ```
   $ mkdir build
   $ cd build
   ```
   This directory can be inside or outside the EtcPal repository.
3. Optionally, provide a CMake toolchain file for cross-compilation. Those
   building native code can skip this step. There are some examples in
   `tools/cmake/cross-toolchains`.
3. Run CMake to configure the EtcPal project:
   ```
   $ cmake [-DCMAKE_TOOLCHAIN_FILE=path/to/toolchain/file] path/to/etcpal/root
   ```
   You can optionally specify your build system with the `-G` option;
   otherwise, CMake will choose a system-appropriate default. Use `cmake --help`
   to see all available options.
4. Use CMake to invoke the generated build system to build the EtcPal library
   and unit tests:
   ```
   $ cmake --build .
   ```
   If you are generating IDE project files, you can use CMake to open the
   projects in the IDE:
   ```
   $ cmake --open .
   ```
5. To run the unit tests after building:
   ```
   $ ctest [-C Configuration]
   ```
   Or, if you are generating IDE project files, simply run the projects in the
   'tests' folder in your IDE.

   Alternatively, you can define `ETCPAL_TEST_BUILD_AS_LIBRARIES=ON` to compile
   the unit and integration tests into static libraries, which is often useful
   for running tests on embedded targets.

Alternatively, if you don't want to use CMake, your project can simply build in
the EtcPal sources directly using the appropriate directories for your target
platform.

### Platform Dependencies

The platform ports of EtcPal have the following dependencies:
+ FreeRTOS
  - Tested back to FreeRTOS 9.0.0. Not guaranteed to work with older versions.
  - Optional Features:
    * etcpal_uuid: Creation of V1 and V4 UUIDs not implemented (not provided by
      FreeRTOS).
+ macOS
  - Tested back to macOS 10.11. Not guaranteed to work with older versions.
  - If compiling with macOS 10.7 or earlier, you might need to install a UUID
    package like `ossp-uuid`.
+ Microsoft Windows
  - Windows XP SP1 or later
  - Windows SDK library dependencies for optional features (if building using
    CMake, these dependencies are handled automatically):
    * etcpal_netint (network interfaces): Iphlpapi.lib
    * etcpal_socket (socket interface): Iphlpapi.lib, Ws2_32.lib
    * etcpal_uuid (UUID creation): Rpcrt4.lib
    * etcpal_timer (system timers): Winmm.lib
+ MQX
  - MQX 4.2.0
  - Optional Features:
    * etcpal_uuid: Creation of V1 and V4 UUIDs not implemented (not provided by
      MQX)
+ Linux
  - Optional Features:
    * etcpal_netint (network interfaces): Linux 2.2, glibc 2.3.3
    * etcpal_socket (socket interface): Linux 2.6
    * etcpal_uuid (UUID creation): libuuid (if compiling EtcPal, use
      `sudo apt-get install uuid-dev` or the equivalent method for your
      distribution)
+ lwIP
  - lwIP 2.1.0 or later
