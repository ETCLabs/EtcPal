# A CMake toolchain file for cross-compiling for NXP Kinetis (ARM Cortex-M4) with MQX RTOS and the
# IAR Embedded Workbench toolchain.
#
# You need to pick the type of build files that CMake generates (the Generator). If you have a
# build system already that uses GNU Make, or are building on a macOS or Unix-based host,
# "Unix Makefiles" will cover most cases. On Windows, you need to use a non-Visual-Studio generator.
# This requires you to have some kind of non-VS build system installed. Some options are:
# - MinGW64 (must be available on PATH)
# - Strawberry Perl (use 'Unix Makefiles' generator)
# - NMake (comes with VS, but must be in a developer command prompt)
#
# Usage: cmake -G "Unix Makefiles" -DCMAKE_TOOLCHAIN_FILE=path/to/this/file ..

# Setting these names causes CMake to look in the Platform subdirectory for files called:
# Platform/MQX.cmake
# Platform/MQX-IAR-C-K61F120M.cmake
set(CMAKE_SYSTEM_NAME MQX)
set(CMAKE_SYSTEM_PROCESSOR K61F120M)

# Set this to the location of the Platform subdirectory containing the platform files.
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR})

if(NOT ${CMAKE_HOST_SYSTEM_NAME} STREQUAL Windows)
  message(FATAL_ERROR "IAR is Windows-only.")
endif()

# The IAR installation directory, highly likely to require customization
set(IAR_INSTALL_DIR "C:/Program Files (x86)/IAR Systems/Embedded Workbench 7.5" CACHE STRING "The IAR install directory")

set(CMAKE_C_COMPILER ${IAR_INSTALL_DIR}/arm/bin/iccarm.exe CACHE FILEPATH "IAR C Compiler")
set(CMAKE_CXX_COMPILER ${IAR_INSTALL_DIR}/arm/bin/iccarm.exe CACHE FILEPATH "IAR C++ Compiler")

# This tells CMake to use a static library program to do its minimal compiler
# test instead of an executable - this translates well to embedded toolchains.
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# If that doesn't work, you might need to comment the line above and uncomment
# this line to bypass the compiler checks.
# set(CMAKE_C_COMPILER_WORKS 1)
# set(CMAKE_CXX_COMPILER_WORKS 1)
