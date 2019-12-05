# This file contains specific settings for compiling EtcPal for a specific target, using a specific
# SDK and compiler. This is a convenient place to set up some global compile settings needed by
# everything compiled for this target with this SDK and toolchain.

# This example uses an MCUXpresso SDK (mcuxpresso.nxp.com) built for the K65.
# Some of the options set below are required by the EtcPal CMake target config.

set(MCUX_SDK_DIR path/to/k65/sdk)

set(ETCPAL_FREERTOS_INCLUDE_DIRS "${MCUX_SDK_DIR}/amazon-freertos/include;${MCUX_SDK_DIR}/amazon-freertos/freertos/portable" CACHE STRING "Override from toolchain file" FORCE)
set(ETCPAL_FREERTOSCONFIG_DIR path/to/freertosconfig/dot/h CACHE STRING "Override from toolchain file" FORCE)

set(ETCPAL_LWIP_INCLUDE_DIRS "${MCUX_SDK_DIR}/lwip/src/include;${MCUX_SDK_DIR}/lwip/port" CACHE STRING "Override from toolchain file" FORCE)
set(ETCPAL_LWIPOPTS_DIR path/to/lwipopts/dot/h CACHE STRING "Override from toolchain file" FORCE)

# Set up a global include of the SDK directories we are going to need.
include_directories(
  ${MCUX_SDK_DIR}/utilities
  ${MCUX_SDK_DIR}/drivers
  ${MCUX_SDK_DIR}/CMSIS
  ${MCUX_SDK_DIR}/component/serial_manager
  ${MCUX_SDK_DIR}/device
)

# Compile settings required to compile for this target with the MCUXpresso ARM GCC and Newlib
# toolchain. It's highly recommended that you make these match your project's build settings as
# closely as possible. In the Eclipse-based embedded IDEs that I typically use, I generally just
# grab a line of build output and add all the flags I see to these lines.
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
)
add_compile_options(
  -fno-common
  -Wall
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
