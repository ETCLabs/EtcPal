# Building EtcPal                                                                {#building_etcpal}

EtcPal uses [CMake](https://cmake.org), a popular cross-platform build system generator, to
generate build files. CMake can also be used to include EtcPal as a dependency to other projects,
i.e. using the `add_subdirectory()` command.

CMake can be downloaded [here](https://cmake.org/download/). EtcPal requires version 3.3 or higher.

If you are building a native project, you can use the typical CMake build process outlined below.
If you are cross-compiling and/or building for an embedded target, there is a little bit more
configuration needed - see @subpage building_for_embedded for more information.

1. Create a directory in your location of choice to hold your build projects or Makefiles:
   ```
   $ mkdir build
   $ cd build
   ```
   This directory can be inside or outside the EtcPal repository.
2. Run CMake to configure the EtcPal project:
   ```
   $ cmake path/to/etcpal/root
   ```
   You can optionally specify your build system with the `-G` option; otherwise, CMake will choose
   a system-appropriate default. Use `cmake --help` to see all available options.
3. Use CMake to invoke the generated build system to build the EtcPal library:
   ```
   $ cmake --build .
   ```
   If you are generating IDE project files, you can use CMake to open the projects in the IDE:
   ```
   $ cmake --open .
   ```
4. To run the unit tests after building:
   ```
   $ ctest [-C Configuration]
   ```
   Or, if you are generating IDE project files, simply run the projects in the 'tests' folder in
   your IDE.

   Alternatively, you can define `ETCPAL_TEST_BUILD_AS_LIBRARIES=ON` to compile the unit and
   integration tests into static libraries, which is often useful for running tests on embedded
   targets.

Alternatively, if you don't want to use CMake, your project can simply build in the EtcPal sources
directly using the appropriate directories for your target platform.

3. Optionally, provide a CMake toolchain file for cross-compilation. Those building native code can
   skip this step. There are some examples in `tools/cmake/cross-toolchains`.
