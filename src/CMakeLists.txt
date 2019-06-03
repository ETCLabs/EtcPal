############################### Platform support ############################## 

if(DEFINED LWPA_CONFIG)
  get_filename_component(LWPA_CONFIG_LOC LWPA_CONFIG DIRECTORY)
  set(${LWPA_HAVE_CONFIG_H} LWPA_HAVE_CONFIG_H)
endif()

# The set of supported target OS
set(VALID_LWPA_TARGETS
  windows
  linux
  mqx
)

if(NOT DEFINED LWPA_TARGET_OS)
  message(STATUS "LWPA_TARGET_OS not supplied, assuming native compile by default...")
  if(WIN32)
    set(LWPA_TARGET_OS windows)
  elseif(UNIX AND NOT APPLE)
    set(LWPA_TARGET_OS linux)
  else()
    # The error check below will catch this and print the fatal error.
    set(LWPA_TARGET_OS ${CMAKE_HOST_SYSTEM_NAME})
  endif()
endif()

if(NOT ${LWPA_TARGET_OS} IN_LIST VALID_LWPA_TARGETS)
  message(FATAL_ERROR "${LWPA_TARGET_OS} is not a valid target OS. "
          "Specify LWPA_TARGET_OS from these options: ${VALID_LWPA_TARGETS}")
endif()

# MQX compilation support
if(LWPA_TARGET_OS STREQUAL mqx)
  if(NOT DEFINED MQX_BOARD_DIR)
    message(FATAL_ERROR "You must provide a variable MQX_BOARD_DIR to indicate the location of your MQX libraries.")
  endif()
endif()

set(LWPA_TARGET_OS ${LWPA_TARGET_OS} PARENT_SCOPE)

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
  ${LWPA_ROOT}/include/os/${LWPA_TARGET_OS}/lwpa/os_lock.h
  ${LWPA_ROOT}/include/os/${LWPA_TARGET_OS}/lwpa/os_thread.h
  ${LWPA_ROOT}/include/os/${LWPA_TARGET_OS}/lwpa/os_socket.h
  # lwpa sources
  ${LWPA_ROOT}/src/lwpa/uuid.c
  ${LWPA_ROOT}/src/lwpa/error.c
  ${LWPA_ROOT}/src/lwpa/log.c
  ${LWPA_ROOT}/src/lwpa/mempool.c
  ${LWPA_ROOT}/src/lwpa/pdu.c
  ${LWPA_ROOT}/src/lwpa/rbtree.c
  ${LWPA_ROOT}/src/lwpa/root_layer_pdu.c
  ${LWPA_ROOT}/src/lwpa/md5.h
  ${LWPA_ROOT}/src/lwpa/md5.c
  ${LWPA_ROOT}/src/os/${LWPA_TARGET_OS}/lwpa/os_common.c
  ${LWPA_ROOT}/src/os/${LWPA_TARGET_OS}/lwpa/os_lock.c
  ${LWPA_ROOT}/src/os/${LWPA_TARGET_OS}/lwpa/os_netint.c
  ${LWPA_ROOT}/src/os/${LWPA_TARGET_OS}/lwpa/os_uuid.c
  ${LWPA_ROOT}/src/os/${LWPA_TARGET_OS}/lwpa/os_socket.c
  ${LWPA_ROOT}/src/os/${LWPA_TARGET_OS}/lwpa/os_thread.c
  ${LWPA_ROOT}/src/os/${LWPA_TARGET_OS}/lwpa/os_timer.c
)

# lwpa public include directories and library dependencies
target_include_directories(lwpa PUBLIC
  ${LWPA_ROOT}/include
  ${LWPA_ROOT}/include/os/${LWPA_TARGET_OS}
  ${LWPA_CONFIG_LOC}
)
target_include_directories(lwpa PRIVATE ${LWPA_ROOT}/src)
target_compile_definitions(lwpa PUBLIC ${LWPA_HAVE_CONFIG_H})

# Add include paths and library dependencies based on OS for which we are being compiled
if(LWPA_TARGET_OS STREQUAL windows)
  target_link_libraries(lwpa PUBLIC winmm ws2_32 Iphlpapi Rpcrt4)
elseif(LWPA_TARGET_OS STREQUAL linux)
  target_link_libraries(lwpa PUBLIC uuid)
elseif(LWPA_TARGET_OS STREQUAL mqx)
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

###############################################################################
# Mock lwpa library
###############################################################################

if(LWPA_BUILD_MOCK_LIB)
  add_library(lwpa_mock
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
    ${LWPA_ROOT}/include/os/${LWPA_TARGET_OS}/lwpa/os_lock.h
    ${LWPA_ROOT}/include/os/${LWPA_TARGET_OS}/lwpa/os_socket.h
    ${LWPA_ROOT}/include/os/${LWPA_TARGET_OS}/lwpa/os_thread.h

    ${LWPA_ROOT}/include/lwpa_mock/socket.h

    # lwpa sources
    # We will gradually substitute these with mocks as needed
    ${LWPA_ROOT}/src/lwpa/uuid.c
    ${LWPA_ROOT}/src/lwpa/error.c
    ${LWPA_ROOT}/src/lwpa/log.c
    ${LWPA_ROOT}/src/lwpa/mempool.c
    ${LWPA_ROOT}/src/lwpa/pdu.c
    ${LWPA_ROOT}/src/lwpa/rbtree.c
    ${LWPA_ROOT}/src/lwpa/root_layer_pdu.c
    ${LWPA_ROOT}/src/lwpa/md5.h
    ${LWPA_ROOT}/src/lwpa/md5.c
    ${LWPA_ROOT}/src/os/${LWPA_TARGET_OS}/lwpa/os_lock.c
    ${LWPA_ROOT}/src/os/${LWPA_TARGET_OS}/lwpa/os_netint.c
    ${LWPA_ROOT}/src/os/${LWPA_TARGET_OS}/lwpa/os_uuid.c
    ${LWPA_ROOT}/src/os/${LWPA_TARGET_OS}/lwpa/os_socket.c
    ${LWPA_ROOT}/src/os/${LWPA_TARGET_OS}/lwpa/os_thread.c
    ${LWPA_ROOT}/src/os/${LWPA_TARGET_OS}/lwpa/os_timer.c

    ${LWPA_ROOT}/src/lwpa_mock/socket.c
  )
  target_include_directories(lwpa_mock PUBLIC
    ${LWPA_ROOT}/include
    ${LWPA_ROOT}/include/os/${LWPA_TARGET_OS}
  )
  target_compile_definitions(lwpa_mock PRIVATE LWPA_BUILDING_MOCK_LIB)
  target_link_libraries(lwpa_mock PUBLIC meekrosoft::fff)
  if(LWPA_TARGET_OS STREQUAL windows)
    target_link_libraries(lwpa_mock PUBLIC winmm ws2_32 Iphlpapi Rpcrt4)
  endif()
  set_target_properties(lwpa PROPERTIES DEBUG_POSTFIX d)
endif()
