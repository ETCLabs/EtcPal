# A CMake toolchain file for cross-compiling for NXP Kinetis (ARM Cortex-M4) with MQX RTOS and the
# IAR Embedded Workbench toolchain.
#
# On Windows, you need to use a non-Visual-Studio generator. This requires you to have some kind of
# non-VS build system installed. Some options are:
# - MinGW64 (must be available on PATH)
# - Strawberry Perl (use 'Unix Makefiles' generator)
# - NMake (comes with VS, but must be in a developer command prompt)
#
# Usage: cmake -G "Unix Makefiles" \
#              -DCMAKE_TOOLCHAIN_FILE=../tools/cmake/cross-toolchains/iar-kinetis-mqx.cmake \
#              -DMQX_BOARD_DIR=path/to/mqx/board/dir ..
#
# MQX_BOARD_DIR can also be set in this file so that it does not have to be provided at the command
# line, like so:
# set(MQX_BOARD_DIR ${CMAKE_CURRENT_LIST_DIR}/path/to/mqx/board/dir)

set(CMAKE_SYSTEM_NAME Generic) # This is CMake's name for 'bare-metal' or RTOS environments
set(CMAKE_SYSTEM_PROCESSOR ARM)

if(NOT ${CMAKE_HOST_SYSTEM_NAME} STREQUAL Windows)
  message(FATAL_ERROR "IAR is Windows-only.")
endif()

if(NOT DEFINED IAR_INSTALL_DIR)
  # The default install location on Windows.
  # You may need to tweak the IAR version.
  set(IAR_INSTALL_DIR "C:/Program Files (x86)/IAR Systems/Embedded Workbench 7.5")
endif()

set(CMAKE_C_COMPILER ${IAR_INSTALL_DIR}/arm/bin/iccarm.exe)
set(CMAKE_CXX_COMPILER ${IAR_INSTALL_DIR}/arm/bin/iccarm.exe)

set(LWPA_TARGET_OS mqx)
add_compile_options(
  --endian=little
  --cpu=Cortex-M4
  --fpu=VFPv4_sp
  --dlib_config normal
)

# This tells CMake to use a static library program to do its minimal compiler
# test instead of an executable - this translates well to embedded toolchains.
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# If that doesn't work, you might need to comment the line above and uncomment
# this line to bypass the compiler checks.
# set(CMAKE_C_COMPILER_WORKS 1)
# set(CMAKE_CXX_COMPILER_WORKS 1)
