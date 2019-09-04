# FreeRTOS compilation support for EtcPal
set(ETCPAL_FREERTOS_INCLUDE_DIRS "" CACHE STRING "The include directory for the FreeRTOS headers")
set(ETCPAL_FREERTOSCONFIG_DIR "" CACHE STRING "The directory of the project's FreeRTOSConfig.h file")
if(NOT ETCPAL_FREERTOS_INCLUDE_DIRS OR NOT ETCPAL_FREERTOSCONFIG_DIR)
  message(FATAL_ERROR "You must provide the options ETCPAL_FREERTOS_INCLUDE_DIRS to specify the FreeRTOS include directories "
                      "and ETCPAL_FREERTOSCONFIG_DIR to specify the location of the FreeRTOSConfig.h.")
endif()

set(ETCPAL_OS_ADDITIONAL_INCLUDE_DIRS ${ETCPAL_FREERTOS_INCLUDE_DIRS} ${ETCPAL_FREERTOSCONFIG_DIR})
