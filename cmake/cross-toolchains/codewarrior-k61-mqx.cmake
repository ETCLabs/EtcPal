# An example CMake toolchain file for cross-compiling using the CodeWarrior 10.6 gcc toolchain.
#
# You need to pick the type of build files that CMake generates (the Generator). CodeWarrior 10.6
# comes with a GNU Make utility, so it's convenient to use the "Unix Makefiles" generator in most
# cases; a line below tells CMake where to find CodeWarrior's make utility.
#
# Usage: cmake -G "Unix Makefiles" -DCMAKE_TOOLCHAIN_FILE=[path/to/this/file] [path/to/sources] 

set(CMAKE_SYSTEM_NAME Generic) # This is CMake's name for 'bare-metal' or RTOS environments
set(CMAKE_SYSTEM_PROCESSOR arm)

if(NOT ${CMAKE_HOST_SYSTEM_NAME} STREQUAL Windows)
  message(FATAL_ERROR "CodeWarrior is Windows-only.")
endif()

# The CodeWarrior installation directory, highly likely to require customization
set(CODEWARRIOR_INSTALL_DIR "C:/Freescale/CW MCU v10.6" CACHE STRING "The CodeWarrior install directory")

set(TOOLS_DIR ${CODEWARRIOR_INSTALL_DIR}/Cross_Tools/arm-none-eabi-gcc-4_7_3)
set(CMAKE_C_COMPILER ${TOOLS_DIR}/bin/arm-none-eabi-gcc.exe CACHE FILEPATH "CodeWarrior/gcc C Compiler")
set(CMAKE_CXX_COMPILER ${TOOLS_DIR}/bin/arm-none-eabi-g++.exe CACHE FILEPATH "CodeWarrior/gcc C++ Compiler")
set(CMAKE_MAKE_PROGRAM ${CODEWARRIOR_INSTALL_DIR}/gnu/bin CACHE FILEPATH "CodeWarrior Make utility")

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
# that builds for a NXP Kinetis K61 processor using MQX.
add_compile_definitions(_EWL_C99 _DEBUG)
add_compile_options(
  -nostdinc
  -ffunction-sections
  -fdata-sections
  -fno-strict-aliasing
  -fmessage-length=0
  -specs=ewl_c9x.specs
)
