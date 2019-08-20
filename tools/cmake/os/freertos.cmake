
# FreeRTOS compilation support for lwpa
set(LWPA_FREERTOS_INCLUDE_DIRS "" CACHE STRING "The include directory for the FreeRTOS headers")
set(LWPA_FREERTOSCONFIG_DIR "" CACHE STRING "The directory of the project's FreeRTOSConfig.h file")
if(NOT LWPA_FREERTOS_INCLUDE_DIRS OR NOT LWPA_FREERTOSCONFIG_DIR)
  message(FATAL_ERROR "You must provide the options LWPA_FREERTOS_INCLUDE_DIRS to specify the FreeRTOS include directories "
                      "and LWPA_FREERTOSCONFIG_DIR to specify the location of the FreeRTOSConfig.h."
  )
endif()

set(LWPA_OS_ADDITIONAL_INCLUDE_DIRS ${LWPA_FREERTOS_INCLUDE_DIRS} ${LWPA_FREERTOSCONFIG_DIR})
