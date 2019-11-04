# Creating a New EtcPal Port                                   {#porting_guide}

EtcPal is ported for several desktop, mobile and embedded operating systems and
network stacks; however, decisions on what ports to include are heavily
influenced by the platforms ETC uses in our products. Because of this, EtcPal
may not support your OS and/or network stack of choice. Fear not, however,
because making a new port of EtcPal is designed to be simple.

## First Steps 

First, decide if you want to maintain your port as a fork of EtcPal or if you
would like your changes to be accepted into upstream development. There are
good reasons to select either approach; in a fork, you are free to modify
whatever you wish to make EtcPal work with your project, but you might need to
merge downstream to get new EtcPal updates.

If you want to submit your changes as a pull request for acceptance into the
main EtcPal repository, the EtcPal development team will judge whether it is
practical for ETC to build and test the new port as part of their development
process. Although the EtcPal development team reserves the right to decline to
add a port to the core build and test process for any reason, this judgment
will often be made based on factors such as:

* Whether the toolchain required to build the port is free and open or requires
  a license
* Whether good simulation resources are readily available to automate the
  running of unit tests

If it is determined to be impractical for the new port to be added to the
EtcPal automated build and test process, the development team will organize
the code for the new port into a section that is designated as untested. It
will be your responsibility to ensure that this port works as required and
continues to work as changes are made to the core EtcPal code.

## OS vs Network Ports

EtcPal distinguishes between two types of port: _OS_ and _Network_ ports. OS
ports are an abstraction of non-networking operating system functionality, such
as threading, synchronization and timers. Network ports only encompass
networking functionality, such as a socket interface and network interface
enumeration.

These two types of ports are kept separate because it is common for embedded
platforms to mix and match RTOS with network stacks. This makes it convenient
to compile EtcPal for many different combinations of embedded OS and networking
SDKs. This distinction becomes less important on full-featured OS such as
GNU/Linux, macOS and Windows, where networking is always part of the monolithic
OS kernel.

Before making a new port of EtcPal, determine whether your port will be OS
only, network only or both.

## CMake

EtcPal is built using CMake. The CMake files specific to EtcPal's ports are
located in the `tools/cmake` subdirectory.

The file `etcpal-os.cmake` does the determination of the OS and network targets
at configure time. Add your port to the definition of the variables
`VALID_ETCPAL_OS_TARGETS` and `VALID_ETCPAL_NET_TARGETS` as required. Your port
name must be all-lowercase and may contain underscores. The name of your port
will appear this way in CMake files as well as subdirectories of the
`include/etcpal/os` and `src/etcpal/os` source tree organization.

If your port is a network-only port, add a file called `[your-port-name].cmake`
to `tools/cmake/net`. If your file os an OS-only port or a combined OS and
network port, add a file called `[your-port-name].cmake` to `tools/cmake/os`.

These files should define as CMake options any values that should be
configurable by the user to set up compilation of this port. This may include
paths to SDK include directories for the OS or network stack in questions.
Define these variables as CMake _options_ by using a signature similar to:

```cmake
set(MY_OPTION_NAME "" CACHE STRING "Description of my option")
```

There are also some special EtcPal variables that these files can manipulate to
modify the final build process:

* `ETCPAL_OS_ADDITIONAL_SOURCES`: Any additional source files outside the core
  module names that need to be compiled with this OS port. These source files
  should appear under `src/etcpal/os/[your-port-name]/etcpal`.
* `ETCPAL_OS_ADDITIONAL_INCLUDE_DIRS`: Any additional include directory paths
  that need to be referenced during compilation of EtcPal for this OS target.
  These will be added as _public_ include directories to the EtcPal CMake
  target.
* `ETCPAL_OS_ADDITIONAL_LIBS`: Any additional static libraries that need to be
  linked by any application linking EtcPal when built for this OS target. These
  will be added as _public_ link libraries to the EtcPal CMake target.
* `ETCPAL_NET_ADDITIONAL_SOURCES`: Any additional source files outside the core
  module names that need to be compiled with this network port. These source
  files should appear under `src/etcpal/os/[your-port-name]/etcpal`.
* `ETCPAL_NET_ADDITIONAL_INCLUDE_DIRS`: Any additional include directory paths
  that need to be referenced during compilation of EtcPal for this network
  target. These will be added as _public_ include directories to the EtcPal
  CMake target.
* `ETCPAL_NET_ADDITIONAL_LIBS`: Any additional static libraries that need to be
  linked by any application linking EtcPal when built for this network target.
  These will be added as _public_ link libraries to the EtcPal CMake target.

## Modules

To make a new port of EtcPal, you must port all the modules that have 
platform-specific code for that port type. For OS ports, the modules that need
to be ported are:

* `lock`
* `thread`
* `timer`
* `uuid`

For network ports, the modules are:

* `inet`
* `netint`
* `socket`

The platform-specific source files for each module appear in the directory
`src/os/[your-port-name]/etcpal/` for both OS and network ports. Some modules
also have platform-specific public include files, which appear in the same
heirarchy under `include/`.

Consult the documentation for each module and look at comparable code for other
platforms to determine what definitions need to appear in platform-specific
include and source files. Note that a port is not complete unless all modules
have platform-specific source files available - if certain functionality is not
available on your platform, indicate this by creating a stub function which
returns `kEtcPalErrNotImpl`.

## Tests

The unit and integration tests for EtcPal are designed to be platform-neutral.
This means that you should be able to build and run the unit tests on
appropriate target hardware or simulation for your port's platform. The tests
being platform-neutral allow them to enforce the same behavior between your
port and all the other ports. You shouldn't consider your port to be complete
until you can build and run the unit tests and see them all pass.

EtcPal's unit tests are located in the `tests/unit` directory. Unit test
targets are automatically included by CMake when compiling the EtcPal library
on its own.

"Live" unit tests (located in `tests/unit/live`) and the integration tests
(located in `tests/integration`) link the full unmodified EtcPal library and
exercise underlying OS or networking functionality while running. Although this
is typically considered poor practice for unit testing, it's necessary in this
case since we are testing a library which is specifically for OS abstraction -
if the OS were abstracted away, there would be nothing to test. Be aware that
the live tests create and destroy threads, send and receive network traffic,
and do whatever else is necessary to test that OS abstractions behave the same
across platforms.

C++ unit tests (located in the `cpp` directory) test the header-only C++
wrappers. If you have a C++ compiler available for your target platform, you 
should also build and run these tests to test your port, although they are less
important than the live tests.

Other custom tests may appear underneath the `unit` directory - these are
typically custom-linked tests that require some parts of the EtcPal library to
be stubbed in order to test specific edge cases. These typically don't rely on
underlying platform-specific code and thus are less important than the live
tests, though you should run them if you can.
