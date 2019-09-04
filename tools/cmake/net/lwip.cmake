# lwIP compilation support for EtcPal
set(ETCPAL_LWIP_INCLUDE_DIRS "" CACHE STRING "The include directories for the lwIP headers")
set(ETCPAL_LWIPOPTS_DIR "" CACHE STRING "The directory of the project's lwipopts.h file")
if(NOT ETCPAL_LWIP_INCLUDE_DIRS OR NOT ETCPAL_LWIPOPTS_DIR)
  message(FATAL_ERROR "You must provide the options ETCPAL_LWIP_INCLUDE_DIRS to specify the lwIP include directories "
                      "and ETCPAL_LWIPOPTS_DIR to specify the location of the lwipopts.h file.")
endif()

set(ETCPAL_NET_ADDITIONAL_SOURCES
  ${ETCPAL_ROOT}/src/os/lwip/etcpal/os_error.h
  ${ETCPAL_ROOT}/src/os/lwip/etcpal/os_error.c
)
set(ETCPAL_NET_ADDITIONAL_INCLUDE_DIRS ${ETCPAL_LWIP_INCLUDE_DIRS} ${ETCPAL_LWIPOPTS_DIR})
