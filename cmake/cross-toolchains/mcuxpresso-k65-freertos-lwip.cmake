# An example CMake toolchain file for cross-compiling for NXP devices with NXP's MCUXpresso gcc
# toolchain.
#
# You need to pick the type of build files that CMake generates (the Generator). MCUXpresso comes
# with a GNU Make utility, so it's convenient to use the "Unix Makefiles" generator in most cases;
# a line below tells CMake where to find MCUXpresso's make utility.
#
# Usage: cmake -G "Unix Makefiles" -DCMAKE_TOOLCHAIN_FILE=[path/to/this/file] [path/to/sources]

set(CMAKE_SYSTEM_NAME Generic) # This is CMake's name for 'bare-metal' or RTOS environments
set(CMAKE_SYSTEM_PROCESSOR arm)

# The MCUXpresso installation directory, highly likely to require customization
if(WIN32)
  set(MCUXPRESSO_DEFAULT_INSTALL_DIR "C:/nxp/MCUXpressoIDE_11.1.0_3209")
endif()
set(MCUXPRESSO_INSTALL_DIR "${MCUXPRESSO_DEFAULT_INSTALL_DIR}" CACHE STRING "The MCUXpresso install directory")

set(CMAKE_C_COMPILER ${MCUXPRESSO_INSTALL_DIR}/ide/tools/bin/arm-none-eabi-gcc.exe CACHE FILEPATH "MCUXpresso ARM GCC Compiler")
set(CMAKE_MAKE_PROGRAM ${MCUXPRESSO_INSTALL_DIR}/ide/buildtools/bin/make.exe CACHE FILEPATH "MCUXpresso Make utility")

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
  __NEWLIB__
  __STRICT_ANSI__
  CPU_MK65FN2M0VMI18
  CPU_MK65FN2M0VMI18_cm4
  SDK_DEBUGCONSOLE=1
  CR_INTEGER_PRINTF
  PRINTF_FLOAT_ENABLE=0
  FSL_RTOS_FREE_RTOS
  SDK_RTOS_FREE_RTOS
  __MCUXPRESSO
  __USE_CMSIS
  USE_RTOS
  __LINUX_ERRNO_EXTENSIONS__
)
add_compile_options(
  -fno-common
  -ffunction-sections
  -fdata-sections
  -ffreestanding
  -fno-builtin
  -mcpu=cortex-m4
  -mfpu=fpv4-sp-d16
  -mfloat-abi=hard
  -mthumb
  -fstack-usage
)
