# A CMake toolchain file for cross-compiling for NXP Kinetis (ARM Cortex-M4) with MQX RTOS and the
# IAR Embedded Workbench toolchain.
#
# Usage: cmake -DCMAKE_TOOLCHAIN_FILE=../tools/cmake/cross-toolchains/iar-kinetis-mqx.cmake \
#              -DMQX_BOARD_DIR=path/to/mqx/board/dir ..
#
# MQX_BOARD_DIR can also be set in this file so that it does not have to be provided at the command
# line, like so:
# set(MQX_BOARD_DIR path/to/mqx/board/dir)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR ARM)

if(NOT ${CMAKE_HOST_SYSTEM_NAME} STREQUAL Windows)
  message(FATAL_ERROR "IAR is Windows-only.")
endif()

if(NOT DEFINED IAR_INSTALL_DIR)
  set(IAR_INSTALL_DIR "C:/Program Files (x86)/IAR Systems/Embedded Workbench 7.5") # The default install location on Windows.
endif()

set(CMAKE_C_COMPILER ${IAR_INSTALL_DIR}/arm/bin/iccarm.exe)
set(CMAKE_CXX_COMPILER ${IAR_INSTALL_DIR}/arm/bin/iccarm.exe)

set(LWPA_PLATFORM mqx)
add_compile_options(
  --endian=little
  --cpu=Cortex-M4
  --fpu=VFPv4_sp
  --dlib_config normal
)

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
