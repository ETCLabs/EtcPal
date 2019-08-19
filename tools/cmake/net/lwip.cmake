
# lwIP compilation support for lwpa
set(LWPA_LWIP_INCLUDE_DIR "" CACHE STRING "The include directory for the lwIP headers")
set(LWPA_LWIPOPTS_DIR "" CACHE STRING "The directory of the project's lwipopts.h file")
if(NOT LWPA_LWIP_INCLUDE_DIR OR NOT LWPA_LWIPOPTS_DIR)
  message(FATAL_ERROR "You must provide the options LWPA_LWIP_INCLUDE_DIR to specify the lwIP include directory "
                      "and LWPA_LWIPOPTS_DIR to specify the location of the lwipopts.h file."
  )
endif()

set(LWPA_NETWORK_ADDITIONAL_INCLUDE_DIRS ${LWPA_LWIP_INCLUDE_DIR} ${LWPA_LWIPOPTS_DIR})
