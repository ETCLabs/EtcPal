# An example CMake toolchain file for cross-compiling for NXP Kinetis K61 (ARM Cortex-M4) with MQX
# RTOS and the CodeWarrior 10.6 gcc toolchain.
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
# Platform/MQX-GNU-C-K61F120M.cmake
set(CMAKE_SYSTEM_NAME MQX)
set(CMAKE_SYSTEM_PROCESSOR K61F120M)

if(NOT ${CMAKE_HOST_SYSTEM_NAME} STREQUAL Windows)
  message(FATAL_ERROR "CodeWarrior is Windows-only.")
endif()

# The CodeWarrior installation directory, highly likely to require customization
set(CODEWARRIOR_INSTALL_DIR "C:/Freescale/CW MCU v10.6" CACHE STRING "The CodeWarrior install directory")

set(TOOLS_DIR ${CODEWARRIOR_INSTALL_DIR}/Cross_Tools/arm-none-eabi-gcc-4_7_3)
set(CMAKE_C_COMPILER ${TOOLS_DIR}/bin/arm-none-eabi-gcc.exe CACHE FILEPATH "CodeWarrior/gcc C Compiler")
set(CMAKE_CXX_COMPILER ${TOOLS_DIR}/bin/arm-none-eabi-g++.exe CACHE FILEPATH "CodeWarrior/gcc C++ Compiler")

# This tells CMake to use a static library program to do its minimal compiler
# test instead of an executable - this translates well to embedded toolchains.
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# If that doesn't work, you might need to comment the line above and uncomment
# this line to bypass the compiler checks.
# set(CMAKE_C_COMPILER_WORKS 1)
# set(CMAKE_CXX_COMPILER_WORKS 1)
