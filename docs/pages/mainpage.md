# ETC Platform Abstraction Layer                                                        {#mainpage}

## Introduction

EtcPal is a C99 library for platform abstraction. It supports the writing of platform-neutral C and
C++ libraries by providing a set of modules to abstract common system calls. There are also a few
platform-neutral utilities (i.e. data structures, logging) thrown in here and there for convenience.

EtcPal modules are designed to run with low overhead on embedded platforms, and thus are designed
to be as thin a wrapper over the underlying OS functionality as possible.

EtcPal also contains some header-only C++11 wrapper modules which wrap the functionality of a
corresponding C module in a nicer API for C++ users.

@htmlonly
To jump right into the documentation, check out the <a href="modules.html">Modules Overview</a>.
@endhtmlonly

## Supported Platforms

EtcPal distinguishes between two major constructs which provide services to applications. 'OS'
targets provide threading, synchronization and other utilities not provided by the C standard
library. 'Network' targets provide a network stack.

EtcPal is not written with any hardware architecture assumptions or dependencies, so it should be
able to run on any target hardware or architecture.

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

See @ref building_etcpal for instructions on building EtcPal for your platform.

### Platform Dependencies

The platform ports of EtcPal have the following dependencies:
+ FreeRTOS
  - Tested back to FreeRTOS 9.0.0. Not guaranteed to work with older versions.
  - Optional Features:
    * etcpal/uuid: Creation of V1 and V4 UUIDs not implemented (not provided by FreeRTOS).
+ macOS
  - Tested back to macOS 10.11. Not guaranteed to work with older versions.
  - If compiling with macOS 10.7 or earlier, you might need to install a UUID package like
    `ossp-uuid`.
+ Microsoft Windows
  - Windows 7 or later
  - Windows SDK library dependencies for optional features (if building using CMake, these
    dependencies are handled automatically):
    * etcpal/netint (network interfaces): Iphlpapi.lib
    * etcpal/socket (socket interface): Iphlpapi.lib, Ws2_32.lib
    * etcpal/uuid (UUID creation): Rpcrt4.lib
    * etcpal/timer (system timers): Winmm.lib
+ MQX
  - MQX 4.2.0
  - Optional Features:
    * etcpal/uuid: Creation of V1 and V4 UUIDs not implemented (not provided by MQX)
+ Linux
  - Optional Features:
    * etcpal/netint (network interfaces): Linux 2.2, glibc 2.3.3
    * etcpal/socket (socket interface): Linux 2.6
    * etcpal/uuid (UUID creation): libuuid (if compiling EtcPal, use `sudo apt-get install uuid-dev`
      or the equivalent method for your distribution)
+ lwIP
  - lwIP 2.1.0 or later
