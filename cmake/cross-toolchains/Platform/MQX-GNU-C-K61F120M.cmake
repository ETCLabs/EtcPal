# This file contains specific settings for compiling EtcPal for a specific target, using a specific
# SDK and compiler. This is a convenient place to set up some global compile settings needed by
# everything compiled for this target with this SDK and toolchain.

# This example uses MQX compiled for the K61 and a specific board.
# Some of the options set below are required by the EtcPal CMake target config.

# The MQX board directory usually takes the form [MQX_DIR]/lib/[board_name].cw10gcc
set(MQX_BOARD_DIR path/to/mqx/board/dir)

# Compile settings required to compile for this target with the Codewarrior GCC and library
# toolchain. It's highly recommended that you make these match your project's build settings as
# closely as possible. In the Eclipse-based embedded IDEs that I typically use, I generally just
# grab a line of build output and add all the flags I see to these lines.
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
