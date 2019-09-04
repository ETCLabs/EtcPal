# LightWeight Platform Abstraction                                  {#mainpage}

## Introduction

lwpa is a C-language library for platform abstraction. It supports the
writing of platform-neutral C and C++ libraries by providing a set of
modules to abstract common system calls. There are also a few
platform-neutral utilities (i.e. data structures, logging) thrown in here
and there for convenience.

lwpa modules are designed to run with low overhead on embedded platforms, and
thus are designed to be as thin a wrapper over the underlying OS functionality
as possible.

To jump right into the documentation, check out the
[Modules Overview](@ref lwpa).

## Source Location

the sources for lwpa are located in the heirarchy:
```
src/
  os/
    [OS or network stack provider]/
      [platform-specific lwpa sources]
  [platform-neutral lwpa sources]
```

The includes are in the heirarchy:
```
include/
  os/
    [OS or network stack provider]/
      [platform-specific lwpa headers]
  [platform-neutral lwpa headers]
```
Some lwpa headers are platform-specific and duplicated for each platform.
Platform-specific headers for the same module will always conform to an
identical interface, as documented in that module's documentation.

## Supported Platforms

lwpa distinguishes between two major constructs which provide services to
applications. 'OS' targets provide threading, synchronization and other
utilities not provided by the C standard library. 'Network' targets provide a
network stack.

lwpa is currently ported for the following OS targets:

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

### Building lwpa for Your Platform

lwpa is built with [CMake](https://cmake.org), a popular cross-platform build
system generator. CMake can also be used to include lwpa as a dependency to
other projects, i.e. using the `add_subdirectory()` command.

To configure and build lwpa on its own using CMake, follow these steps:

1. [Download and install](https://cmake.org/download/) CMake version 3.3 or higher.
2. Create a directory in your location of choice to hold your build projects or
   Makefiles:
   ```
   $ mkdir build
   $ cd build
   ```
   This directory can be inside or outside the lwpa repository.
3. Optionally, provide a CMake toolchain file for cross-compilation. Those
   building native code can skip this step. There are some examples in
   `tools/cmake/cross-toolchains`.
3. Run CMake to configure the lwpa project:
   ```
   $ cmake [-DCMAKE_TOOLCHAIN_FILE=path/to/toolchain/file] path/to/lwpa/root
   ```
   You can optionally specify your build system with the `-G` option;
   otherwise, CMake will choose a system-appropriate default. Use `cmake --help`
   to see all available options.
4. Use CMake to invoke the generated build system to build the lwpa library and
   unit tests:
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

   Alternatively, you can define `LWPA_TEST_BUILD_AS_LIBRARIES=ON` to compile
   the unit and integration tests into static libraries, which is often useful
   for running tests on embedded targets.

Alternatively, if you don't want to use CMake, your project can simply build in
the lwpa sources directly using the appropriate directories for your target
platform.

### Platform Dependencies

The platform ports of lwpa have the following dependencies:
+ FreeRTOS
  - Tested back to FreeRTOS 9.0.0. Not guaranteed to work with older versions.
  - Optional Features:
    * lwpa_uuid: Creation of V1 and V4 UUIDs not implemented (not provided by
      FreeRTOS).
+ macOS
  - Tested back to macOS 10.11. Not guaranteed to work with older versions.
  - If compiling with macOS 10.7 or earlier, you might need to install a UUID
    package like `ossp-uuid`.
+ Microsoft Windows
  - Windows XP SP1 or later
  - Windows SDK library dependencies for optional features (if building using
    CMake, these dependencies are handled automatically):
    * lwpa_netint (network interfaces): Iphlpapi.lib
    * lwpa_socket (socket interface): Iphlpapi.lib, Ws2_32.lib
    * lwpa_uuid (UUID creation): Rpcrt4.lib
    * lwpa_timer (system timers): Winmm.lib
+ MQX
  - MQX 4.2.0
  - Optional Features:
    * lwpa_uuid: Creation of V1 and V4 UUIDs not implemented (not provided by
      MQX)
+ Linux
  - Optional Features:
    * lwpa_netint (network interfaces): Linux 2.2, glibc 2.3.3
    * lwpa_socket (socket interface): Linux 2.6
    * lwpa_uuid (UUID creation): libuuid (if compiling lwpa, use
      `sudo apt-get install uuid-dev` or the equivalent method for your
      distribution)
+ lwIP
  - lwIP 2.1.0 or later
