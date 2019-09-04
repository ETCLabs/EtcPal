# MQX compilation support for EtcPal
set(MQX_BOARD_DIR "" CACHE STRING "Location of the MQX compiled libraries, e.g. \"MQX/v4.2.0/lib/[your board name]\"")
if(NOT MQX_BOARD_DIR)
  message(FATAL_ERROR "You must provide the option MQX_BOARD_DIR to indicate the location of your MQX libraries.")
endif()

# Include the debug versions of the MQX libs if a Debug configuration is specified.
# I think there's probably a better way of doing this and I'm missing something here.
if(CMAKE_BUILD_TYPE STREQUAL Debug)
  set(MQX_BOARD_INT_DIR debug)
else()
  set(MQX_BOARD_INT_DIR release)
endif()

# Depend on the MQX libs.
set(ETCPAL_OS_ADDITIONAL_INCLUDE_DIRS
  ${MQX_BOARD_DIR}/${MQX_BOARD_INT_DIR}
  ${MQX_BOARD_DIR}/${MQX_BOARD_INT_DIR}/bsp
  ${MQX_BOARD_DIR}/${MQX_BOARD_INT_DIR}/bsp/Generated_Code
  ${MQX_BOARD_DIR}/${MQX_BOARD_INT_DIR}/bsp/Sources
  ${MQX_BOARD_DIR}/${MQX_BOARD_INT_DIR}/psp
  ${MQX_BOARD_DIR}/${MQX_BOARD_INT_DIR}/rtcs
)
set(ETCPAL_OS_ADDITIONAL_LIBS
  ${MQX_BOARD_DIR}/${MQX_BOARD_INT_DIR}/bsp/bsp.a
  ${MQX_BOARD_DIR}/${MQX_BOARD_INT_DIR}/psp/psp.a
  ${MQX_BOARD_DIR}/${MQX_BOARD_INT_DIR}/rtcs/rtcs.a
)
