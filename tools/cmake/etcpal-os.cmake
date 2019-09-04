
# The set of supported target OS
set(VALID_ETCPAL_OS_TARGETS
  freertos
  linux
  macos
  mqx
  windows
)
set(VALID_ETCPAL_NET_TARGETS
  linux
  lwip
  macos
  mqx
  windows
)

set(ETCPAL_OS_TARGET "" CACHE STRING "OS provider for EtcPal. See tools/cmake/etcpal-os.cmake for valid values.")
set(ETCPAL_NET_TARGET "" CACHE STRING "Network stack provider for EtcPal. Often (but not always) the same as ETCPAL_OS_TARGET. See src/CMakeLists.txt for valid values.")

if(NOT ETCPAL_OS_TARGET)
  message(STATUS "ETCPAL_OS_TARGET not supplied, assuming native compile by default...")
  if(WIN32)
    set(ETCPAL_OS_TARGET windows)
  elseif(APPLE)
    set(ETCPAL_OS_TARGET macos)
  elseif(UNIX)
    set(ETCPAL_OS_TARGET linux)
  else()
    # The error check below will catch this and print the fatal error.
    set(ETCPAL_OS_TARGET ${CMAKE_HOST_SYSTEM_NAME})
  endif()
endif()

if(NOT ETCPAL_NET_TARGET)
  if(${ETCPAL_OS_TARGET} IN_LIST VALID_ETCPAL_NET_TARGETS)
    set(ETCPAL_NET_TARGET ${ETCPAL_OS_TARGET})
  endif()
endif()

if(NOT ${ETCPAL_OS_TARGET} IN_LIST VALID_ETCPAL_OS_TARGETS)
  message(FATAL_ERROR "${ETCPAL_OS_TARGET} is not a valid target OS. "
          "Specify ETCPAL_OS_TARGET from these options: ${VALID_ETCPAL_OS_TARGETS}")
endif()

if(NOT ${ETCPAL_NET_TARGET} IN_LIST VALID_ETCPAL_NET_TARGETS)
  message(FATAL_ERROR "${ETCPAL_NET_TARGET} is not a valid target network stack. "
          "Specify ETCPAL_NET_TARGET from these options: ${VALID_ETCPAL_NET_TARGETS}")
endif()

if(EXISTS ${PROJECT_SOURCE_DIR}/tools/cmake/os/${ETCPAL_OS_TARGET}.cmake)
  include(${PROJECT_SOURCE_DIR}/tools/cmake/os/${ETCPAL_OS_TARGET}.cmake)
endif()
if(EXISTS ${PROJECT_SOURCE_DIR}/tools/cmake/net/${ETCPAL_NET_TARGET}.cmake)
  include(${PROJECT_SOURCE_DIR}/tools/cmake/net/${ETCPAL_NET_TARGET}.cmake)
endif()
