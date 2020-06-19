
# The set of supported target OS
set(VALID_ETCPAL_OS_TARGETS
  freertos
  ios
  linux
  macos
  mqx
  windows
)
set(VALID_ETCPAL_NET_TARGETS
  ios
  linux
  lwip
  macos
  mqx
  windows
)

set(ETCPAL_OS_TARGET "" CACHE STRING "OS provider for EtcPal. See tools/cmake/etcpal-os.cmake for valid values.")
set(ETCPAL_NET_TARGET "" CACHE STRING "Network stack provider for EtcPal. Often (but not always) the same as ETCPAL_OS_TARGET. See src/CMakeLists.txt for valid values.")

if(NOT ETCPAL_OS_TARGET)
  message(STATUS "ETCPAL_OS_TARGET not supplied.")

  if(WIN32)
    set(ETCPAL_OS_TARGET windows)
  elseif(APPLE)
    if(IOS)
      set(ETCPAL_OS_TARGET ios)
    else()
      set(ETCPAL_OS_TARGET macos)
    endif()
  elseif(UNIX)
    set(ETCPAL_OS_TARGET linux)
  else()
    # The error check below will catch this and print the fatal error.
    set(ETCPAL_OS_TARGET ${CMAKE_HOST_SYSTEM_NAME})
  endif()

  message(STATUS "Assuming OS target '${ETCPAL_OS_TARGET}' based on current CMake configuration...")
endif()

if(NOT ETCPAL_NET_TARGET)
  if(${ETCPAL_OS_TARGET} IN_LIST VALID_ETCPAL_NET_TARGETS)
    set(ETCPAL_NET_TARGET ${ETCPAL_OS_TARGET})
  endif()
endif()

# ETCPAL_OS_TARGET is required.
if((NOT ETCPAL_OS_TARGET) OR (NOT ${ETCPAL_OS_TARGET} IN_LIST VALID_ETCPAL_OS_TARGETS))
  message(FATAL_ERROR "${ETCPAL_OS_TARGET} is not a valid target OS. "
          "Specify ETCPAL_OS_TARGET from these options: ${VALID_ETCPAL_OS_TARGETS}")
endif()

if(EXISTS ${CMAKE_CURRENT_LIST_DIR}/os/${ETCPAL_OS_TARGET}.cmake)
  include(${CMAKE_CURRENT_LIST_DIR}/os/${ETCPAL_OS_TARGET}.cmake)
endif()

# ETCPAL_NET_TARGET is not required.
if(ETCPAL_NET_TARGET)
  if(NOT ${ETCPAL_NET_TARGET} IN_LIST VALID_ETCPAL_NET_TARGETS)
    message(FATAL_ERROR "\"${ETCPAL_NET_TARGET}\" is not a valid target network stack. "
            "Specify ETCPAL_NET_TARGET from these options: ${VALID_ETCPAL_NET_TARGETS}")
  endif()

  if(EXISTS ${CMAKE_CURRENT_LIST_DIR}/net/${ETCPAL_NET_TARGET}.cmake)
    include(${CMAKE_CURRENT_LIST_DIR}/net/${ETCPAL_NET_TARGET}.cmake)
  endif()
else()
  set(ETCPAL_NET_ADDITIONAL_DEFINES ETCPAL_NO_NETWORKING_SUPPORT)
endif()
