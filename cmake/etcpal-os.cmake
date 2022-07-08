
# The set of supported target OS
set(VALID_ETCPAL_OS_TARGETS
  freertos
  ios
  linux
  macos
  mqx
  windows
  zephyr
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

# Assume defaults for ETCPAL_OS_TARGET
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
  endif()

  if(ETCPAL_OS_TARGET)
    message(STATUS "Assuming OS target '${ETCPAL_OS_TARGET}' based on current CMake configuration...")
  else()
    message(STATUS "Assuming no OS support based on current CMake configuration...")
  endif()
endif()

# Assume defaults for ETCPAL_NET_TARGET
if(NOT ETCPAL_NET_TARGET)
  message(STATUS "ETCPAL_NET_TARGET not supplied.")
  if(WIN32)
    set(ETCPAL_NET_TARGET windows)
  elseif(APPLE)
    if(IOS)
      set(ETCPAL_NET_TARGET ios)
    else()
      set(ETCPAL_NET_TARGET macos)
    endif()
  elseif(UNIX)
    set(ETCPAL_NET_TARGET linux)
  endif()

  if(ETCPAL_NET_TARGET)
    message(STATUS "Assuming network target '${ETCPAL_NET_TARGET}' based on current CMake configuration...")
  else()
    message(STATUS "Assuming no networking support based on current CMake configuration...")
  endif()
endif()

# Check ETCPAL_OS_TARGET and include its configuration.
if(ETCPAL_OS_TARGET AND NOT ETCPAL_OS_TARGET STREQUAL "none")
  if(NOT ${ETCPAL_OS_TARGET} IN_LIST VALID_ETCPAL_OS_TARGETS)
    message(FATAL_ERROR "${ETCPAL_OS_TARGET} is not a valid target OS. "
      "Specify ETCPAL_OS_TARGET from these options: ${VALID_ETCPAL_OS_TARGETS}"
      "Or leave it empty to build without OS abstraction support."
    )
  endif()

  set(ETCPAL_HAVE_OS_SUPPORT TRUE)

  if(EXISTS ${CMAKE_CURRENT_LIST_DIR}/os/${ETCPAL_OS_TARGET}.cmake)
    include(${CMAKE_CURRENT_LIST_DIR}/os/${ETCPAL_OS_TARGET}.cmake)
  endif()
else()
  set(ETCPAL_OS_ADDITIONAL_DEFINES ETCPAL_NO_OS_SUPPORT)
endif()

# Check ETCPAL_NET_TARGET and include its configuration.
if(ETCPAL_NET_TARGET AND NOT ETCPAL_NET_TARGET STREQUAL "none")
  if(NOT ${ETCPAL_NET_TARGET} IN_LIST VALID_ETCPAL_NET_TARGETS)
    message(FATAL_ERROR "\"${ETCPAL_NET_TARGET}\" is not a valid target network stack. "
            "Specify ETCPAL_NET_TARGET from these options: ${VALID_ETCPAL_NET_TARGETS}")
  endif()

  set(ETCPAL_HAVE_NETWORKING_SUPPORT TRUE)

  if(EXISTS ${CMAKE_CURRENT_LIST_DIR}/net/${ETCPAL_NET_TARGET}.cmake)
    include(${CMAKE_CURRENT_LIST_DIR}/net/${ETCPAL_NET_TARGET}.cmake)
  endif()
else()
  set(ETCPAL_NET_ADDITIONAL_DEFINES ETCPAL_NO_NETWORKING_SUPPORT)
endif()
