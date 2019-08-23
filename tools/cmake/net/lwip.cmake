
# lwIP compilation support for lwpa
set(LWPA_LWIP_INCLUDE_DIRS "" CACHE STRING "The include directories for the lwIP headers")
set(LWPA_LWIPOPTS_DIR "" CACHE STRING "The directory of the project's lwipopts.h file")
if(NOT LWPA_LWIP_INCLUDE_DIRS OR NOT LWPA_LWIPOPTS_DIR)
  message(FATAL_ERROR "You must provide the options LWPA_LWIP_INCLUDE_DIRS to specify the lwIP include directories "
                      "and LWPA_LWIPOPTS_DIR to specify the location of the lwipopts.h file."
  )
endif()

set(LWPA_NET_ADDITIONAL_SOURCES
  ${LWPA_ROOT}/src/os/lwip/lwpa/os_error.h
  ${LWPA_ROOT}/src/os/lwip/lwpa/os_error.c
)
set(LWPA_NET_ADDITIONAL_INCLUDE_DIRS ${LWPA_LWIP_INCLUDE_DIRS} ${LWPA_LWIPOPTS_DIR})
