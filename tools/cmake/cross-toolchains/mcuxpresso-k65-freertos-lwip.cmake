# A CMake toolchain file for cross-compiling for NXP Kinetis K65 (ARM Cortex-M4) with FreeRTOS, lwIP
# and the MCUXpresso gcc toolchain.
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
# Platform/FreeRTOS-lwIP.cmake
# Platform/FreeRTOS-lwIP-GNU-C-K65F180M.cmake
set(CMAKE_SYSTEM_NAME FreeRTOS-lwIP)
set(CMAKE_SYSTEM_PROCESSOR K65F180M)

# Set this to the location of the Platform subdirectory containing the platform files.
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR})

# The MCUXpresso installation directory, highly likely to require customization
if(WIN32)
  set(MCUXPRESSO_DEFAULT_INSTALL_DIR "C:/nxp/MCUXpressoIDE_11.0.0_2516")
endif()
set(MCUXPRESSO_INSTALL_DIR "${MCUXPRESSO_DEFAULT_INSTALL_DIR}" CACHE STRING "The MCUXpresso install directory")

set(CMAKE_C_COMPILER ${MCUXPRESSO_INSTALL_DIR}/ide/tools/bin/arm-none-eabi-gcc.exe CACHE FILEPATH "MCUXpresso ARM GCC Compiler")

# This tells CMake to use a static library program to do its minimal compiler
# test instead of an executable - this translates well to embedded toolchains.
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# If that doesn't work, you might need to comment the line above and uncomment
# this line to bypass the compiler checks.
# set(CMAKE_C_COMPILER_WORKS 1)
# set(CMAKE_CXX_COMPILER_WORKS 1)
