# A CMake toolchain file for cross-compiling for NXP Kinetis (ARM Cortex-M4) with MQX RTOS and the
# CodeWarrior 10.6 gcc toolchain.
#
# On Windows, you need to use a non-Visual-Studio generator. This requires you to have some kind of
# non-VS build system installed. Some options are:
# - MinGW64 (must be available on PATH)
# - Strawberry Perl (use 'Unix Makefiles' generator)
# - NMake (comes with VS, but must be in a developer command prompt)
#
# Usage: cmake -G "Unix Makefiles" \
#              -DCMAKE_TOOLCHAIN_FILE=../tools/cmake/cross-toolchains/codewarrior-kinetis-mqx.cmake \
#              -DMQX_BOARD_DIR=path/to/mqx/board/dir ..
#
# MQX_BOARD_DIR can also be set in this file so that it does not have to be provided at the command
# line, like so:
# set(MQX_BOARD_DIR ${CMAKE_CURRENT_LIST_DIR}/path/to/mqx/board/dir)

set(CMAKE_SYSTEM_NAME Generic) # This is CMake's name for 'bare-metal' or RTOS environments
set(CMAKE_SYSTEM_PROCESSOR ARM)

if(NOT ${CMAKE_HOST_SYSTEM_NAME} STREQUAL Windows)
  message(FATAL_ERROR "CodeWarrior is Windows-only.")
endif()

if(NOT DEFINED CODEWARRIOR_INSTALL_DIR)
  # The default install location on Windows.
  # You may need to tweak the CodeWarrior version.
  set(CODEWARRIOR_INSTALL_DIR "C:/Freescale/CW MCU v10.6")
endif()

set(TOOLS_DIR ${CODEWARRIOR_INSTALL_DIR}/Cross_Tools/arm-none-eabi-gcc-4_7_3)
set(CMAKE_C_COMPILER ${TOOLS_DIR}/bin/arm-none-eabi-gcc.exe)
set(CMAKE_CXX_COMPILER ${TOOLS_DIR}/bin/arm-none-eabi-g++.exe)

set(LWPA_TARGET_OS mqx)
add_compile_options(
  -nostdinc
  -ffunction-sections
  -fdata-sections
  -Wall
  -std=gnu99
  -fno-strict-aliasing
  -fmessage-length=0
  -specs=ewl_c9x.specs
)
add_compile_definitions(_EWL_C99 _DEBUG)
include_directories(
  ${CODEWARRIOR_INSTALL_DIR}/MCU/ARM_GCC_Support/ewl/EWL_C/include
  ${CODEWARRIOR_INSTALL_DIR}/MCU/ARM_GCC_Support/ewl/EWL_Runtime/include
)

# This tells CMake to use a static library program to do its minimal compiler
# test instead of an executable - this translates well to embedded toolchains.
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# If that doesn't work, you might need to comment the line above and uncomment
# this line to bypass the compiler checks.
# set(CMAKE_C_COMPILER_WORKS 1)
# set(CMAKE_CXX_COMPILER_WORKS 1)
