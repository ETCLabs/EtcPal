
# The set of supported target OS
set(VALID_LWPA_OS_TARGETS
  freertos
  linux
  macos
  mqx
  windows
)
set(VALID_LWPA_NET_TARGETS
  linux
  lwip
  macos
  mqx
  windows
)

set(LWPA_OS_TARGET "" CACHE STRING "OS provider for lwpa. See tools/cmake/lwpa-os.cmake for valid values.")
set(LWPA_NET_TARGET "" CACHE STRING "Network stack provider for lwpa. Often (but not always) the same as LWPA_OS_TARGET. See src/CMakeLists.txt for valid values.")

if(NOT LWPA_OS_TARGET)
  message(STATUS "LWPA_OS_TARGET not supplied, assuming native compile by default...")
  if(WIN32)
    set(LWPA_OS_TARGET windows)
  elseif(APPLE)
    set(LWPA_OS_TARGET macos)
  elseif(UNIX)
    set(LWPA_OS_TARGET linux)
  else()
    # The error check below will catch this and print the fatal error.
    set(LWPA_OS_TARGET ${CMAKE_HOST_SYSTEM_NAME})
  endif()
endif()

if(NOT LWPA_NET_TARGET)
  if(${LWPA_OS_TARGET} IN_LIST VALID_LWPA_NET_TARGETS)
    set(LWPA_NET_TARGET ${LWPA_OS_TARGET})
  endif()
endif()

if(NOT ${LWPA_OS_TARGET} IN_LIST VALID_LWPA_OS_TARGETS)
  message(FATAL_ERROR "${LWPA_OS_TARGET} is not a valid target OS. "
          "Specify LWPA_OS_TARGET from these options: ${VALID_LWPA_OS_TARGETS}")
endif()

if(NOT ${LWPA_NET_TARGET} IN_LIST VALID_LWPA_NET_TARGETS)
  message(FATAL_ERROR "${LWPA_NET_TARGET} is not a valid target network stack. "
          "Specify LWPA_NET_TARGET from these options: ${VALID_LWPA_NET_TARGETS}")
endif()

if(EXISTS ${PROJECT_SOURCE_DIR}/tools/cmake/os/${LWPA_OS_TARGET}.cmake)
  include(${PROJECT_SOURCE_DIR}/tools/cmake/os/${LWPA_OS_TARGET}.cmake)
endif()
if(EXISTS ${PROJECT_SOURCE_DIR}/tools/cmake/net/${LWPA_NET_TARGET}.cmake)
  include(${PROJECT_SOURCE_DIR}/tools/cmake/net/${LWPA_NET_TARGET}.cmake)
endif()
