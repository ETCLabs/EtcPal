############################### Platform support ############################## 

if(DEFINED LWPA_CONFIG)
  get_filename_component(LWPA_CONFIG_LOC LWPA_CONFIG DIRECTORY)
  set(${LWPA_HAVE_CONFIG_H} LWPA_HAVE_CONFIG_H)
endif()

# The set of supported platforms
set(VALID_LWPA_PLATFORMS
  windows
  linux
  mqx
)

if(NOT DEFINED LWPA_PLATFORM)
  message(STATUS "LWPA_PLATFORM not supplied, assuming native compile by default...")
  if(WIN32)
    set(LWPA_PLATFORM windows)
  elseif(UNIX AND NOT APPLE)
    set(LWPA_PLATFORM linux)
  else()
    message(FATAL_ERROR "lwpa is not ported for this platform.")
  endif()
endif()

if(NOT ${LWPA_PLATFORM} IN_LIST VALID_LWPA_PLATFORMS)
  message(FATAL_ERROR "${LWPA_PLATFORM} is not a valid platform.")
endif()

# MQX platform support
if(LWPA_PLATFORM STREQUAL mqx)
  if(NOT DEFINED MQX_BOARD_DIR)
    message(FATAL_ERROR "You must provide a variable MQX_BOARD_DIR to indicate the location of your MQX libraries.")
  endif()
endif()

set(LWPA_PLATFORM ${LWPA_PLATFORM} PARENT_SCOPE)

###############################################################################
# Core lwpa library
###############################################################################

set(LWPA_ROOT ${CMAKE_CURRENT_LIST_DIR}/..)

add_library(lwpa
# lwpa headers
  ${LWPA_ROOT}/include/lwpa/acn_prot.h
  ${LWPA_ROOT}/include/lwpa/bool.h
  ${LWPA_ROOT}/include/lwpa/uuid.h
  ${LWPA_ROOT}/include/lwpa/common.h
  ${LWPA_ROOT}/include/lwpa/error.h
  ${LWPA_ROOT}/include/lwpa/inet.h
  ${LWPA_ROOT}/include/lwpa/int.h
  ${LWPA_ROOT}/include/lwpa/log.h
  ${LWPA_ROOT}/include/lwpa/mempool.h
  ${LWPA_ROOT}/include/lwpa/netint.h
  ${LWPA_ROOT}/include/lwpa/opts.h
  ${LWPA_ROOT}/include/lwpa/pack.h
  ${LWPA_ROOT}/include/lwpa/pdu.h
  ${LWPA_ROOT}/include/lwpa/rbtree.h
  ${LWPA_ROOT}/include/lwpa/root_layer_pdu.h
  ${LWPA_ROOT}/include/lwpa/socket.h
  ${LWPA_ROOT}/include/lwpa/timer.h
  ${LWPA_ROOT}/include/${LWPA_PLATFORM}/lwpa/plat_lock.h
  ${LWPA_ROOT}/include/${LWPA_PLATFORM}/lwpa/plat_thread.h
  ${LWPA_ROOT}/include/${LWPA_PLATFORM}/lwpa/plat_socket.h
# lwpa sources
  ${LWPA_ROOT}/src/lwpa_uuid.c
  ${LWPA_ROOT}/src/lwpa_error.c
  ${LWPA_ROOT}/src/lwpa_log.c
  ${LWPA_ROOT}/src/lwpa_mempool.c
  ${LWPA_ROOT}/src/lwpa_pdu.c
  ${LWPA_ROOT}/src/lwpa_rbtree.c
  ${LWPA_ROOT}/src/lwpa_root_layer_pdu.c
  ${LWPA_ROOT}/src/md5.h
  ${LWPA_ROOT}/src/md5c.c
  ${LWPA_ROOT}/src/${LWPA_PLATFORM}/lwpa_lock.c
  ${LWPA_ROOT}/src/${LWPA_PLATFORM}/lwpa_netint.c
  ${LWPA_ROOT}/src/${LWPA_PLATFORM}/lwpa_socket.c
  ${LWPA_ROOT}/src/${LWPA_PLATFORM}/lwpa_thread.c
  ${LWPA_ROOT}/src/${LWPA_PLATFORM}/lwpa_timer.c
)

# lwpa public include directories and library dependencies
target_include_directories(lwpa PUBLIC
  ${LWPA_ROOT}/include
  ${LWPA_ROOT}/include/${LWPA_PLATFORM}
  ${LWPA_CONFIG_LOC})
target_compile_definitions(lwpa PUBLIC ${LWPA_HAVE_CONFIG_H})

# Add include paths and library dependencies based on platform for which we are being compiled
if(LWPA_PLATFORM STREQUAL windows)
  target_link_libraries(lwpa PUBLIC winmm ws2_32 Iphlpapi)
elseif(LWPA_PLATFORM STREQUAL mqx)
  # Include the debug versions of the MQX libs if a Debug configuration is specified.
  # I think there's probably a better way of doing this and I'm missing something here.
  if(CMAKE_BUILD_TYPE STREQUAL Debug)
    set(MQX_BOARD_INT_DIR debug)
  else()
    set(MQX_BOARD_INT_DIR release)
  endif()

  # Depend on the MQX libs.
  target_include_directories(lwpa PUBLIC ${MQX_BOARD_DIR}/${MQX_BOARD_INT_DIR}
                                         ${MQX_BOARD_DIR}/${MQX_BOARD_INT_DIR}/bsp
                                         ${MQX_BOARD_DIR}/${MQX_BOARD_INT_DIR}/bsp/Generated_Code
                                         ${MQX_BOARD_DIR}/${MQX_BOARD_INT_DIR}/bsp/Sources
                                         ${MQX_BOARD_DIR}/${MQX_BOARD_INT_DIR}/psp
                                         ${MQX_BOARD_DIR}/${MQX_BOARD_INT_DIR}/rtcs)
  target_link_libraries(lwpa PUBLIC ${MQX_BOARD_DIR}/${MQX_BOARD_INT_DIR}/bsp/bsp.a
                                    ${MQX_BOARD_DIR}/${MQX_BOARD_INT_DIR}/psp/psp.a
                                    ${MQX_BOARD_DIR}/${MQX_BOARD_INT_DIR}/rtcs/rtcs.a)
endif()

set_target_properties(lwpa PROPERTIES DEBUG_POSTFIX d)