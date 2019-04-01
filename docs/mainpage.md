# LightWeight Platform Abstraction                                  {#mainpage}

## Introduction

lwpa is a C-language library for platform abstraction. It supports the
writing of platform-neutral C and C++ libraries by providing a set of
modules to abstract common system calls. There are also a few
platform-neutral utilities (i.e. data structures, logging) thrown in here
and there for convenience.

lwpa modules are typically optimized for embedded platforms and thus designed
to be as thin a wrapper over the underlying OS functionality as possible.

To jump right into the documentation, check out the
[Modules Overview](@ref lwpa).

## Source Location

the sources for lwpa are located in the heirarchy:
```
src/
  [platform_name]/
    [platform-specific lwpa sources]
  [platform-neutral lwpa sources]
```

The includes are in the heirarchy:
```
include/
  [platform_name]/
    [platform-specific lwpa headers]
  [platform-neutral lwpa headers]
```
Some lwpa headers are platform-specific and duplicated for each platform.
Platform-specific headers for the same module will always conform to an
identical interface, as documented in that module's documentation.

## Platforms

lwpa is currently ported for the following platforms:

+ Microsoft Windows
+ MQX RTOS

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
3. Run CMake to configure the lwpa project:
   ```
   $ cmake path/to/lwpa/root
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
   # On Windows
   > .\test\[Build_Configuration]\test_lwpa.exe

   # Elsewhere
   $ ./test/test_lwpa
   ```
   Or, if you are generating IDE project files, simply run the test_lwpa
   project from your IDE.

Alternatively, if you don't want to use CMake, your project can simply build in
the lwpa sources directly using the appropriate directories for your target
platform. This is currently the only option for MQX RTOS.

### Platform Dependencies

The platform ports of lwpa have the following dependencies:
+ Microsoft Windows
  - Windows XP or higher
+ MQX RTOS
  - MQX 4.2.0
+ Linux
  - libuuid (`sudo apt-get install uuid-dev`)
