# An example CMake toolchain file for cross-compiling using the IAR Embedded Workbench toolchain.
#
# You need to pick the type of build files that CMake generates (the Generator). Unfortunately,
# CMake does not know how to use IAR's build system, so you need an alternative. Some options are:
#
# - MinGW64 (must be available on PATH, use 'Unix Makefiles' generator)
# - Strawberry Perl (use 'Unix Makefiles' generator)
# - Ninja (must be available on PATH, use 'Ninja' generator)
#
# Usage: cmake -G "Unix Makefiles" -DCMAKE_TOOLCHAIN_FILE=path/to/this/file ..

set(CMAKE_SYSTEM_NAME Generic) # This is CMake's name for 'bare-metal' or RTOS environments
set(CMAKE_SYSTEM_PROCESSOR arm)

if(NOT ${CMAKE_HOST_SYSTEM_NAME} STREQUAL Windows)
  message(FATAL_ERROR "IAR is Windows-only.")
endif()

# The IAR installation directory, highly likely to require customization
set(IAR_INSTALL_DIR "C:/Program Files (x86)/IAR Systems/Embedded Workbench 8.0" CACHE STRING "The IAR install directory")

set(CMAKE_C_COMPILER ${IAR_INSTALL_DIR}/arm/bin/iccarm.exe CACHE FILEPATH "IAR C Compiler")
set(CMAKE_CXX_COMPILER ${IAR_INSTALL_DIR}/arm/bin/iccarm.exe CACHE FILEPATH "IAR C++ Compiler")

# This tells CMake to use a static library program to do its minimal compiler
# test instead of an executable - this translates well to embedded toolchains.
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# If that doesn't work, you might need to comment the line above and uncomment
# this line to bypass the compiler checks.
# set(CMAKE_C_COMPILER_WORKS 1)
# set(CMAKE_CXX_COMPILER_WORKS 1)

# These are global compile definitions and compile options that will be applied to every build step
# when building the libraries. It's highly recommended that you make these match your project's
# build settings as closely as possible. My preferred approach to this is to just grab a line of
# build output and add all the flags I see to these lines. Here are some examples from a project
# that builds for a NXP Kinetis K65 processor using FreeRTOS and lwIP.
add_compile_definitions(
  CPU_MK65FN2M0VMI18
  CPU_MK65FN2M0VMI18_cm4
  SDK_DEBUGCONSOLE=1
  CR_INTEGER_PRINTF
  PRINTF_FLOAT_ENABLE=0
  FSL_RTOS_FREE_RTOS
  SDK_RTOS_FREE_RTOS
  USE_RTOS
)
# IAR doesn't like the command-line format that CMake spits out for add_compile_options() so you
# must do it this way instead.
set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} "--endian=little --cpu=Cortex-M4 --fpu=VFPv4_sp --dlib_config normal")
