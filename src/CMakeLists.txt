
set(LWPA_CONFIG_FILE_LOC "" CACHE STRING "The directory path of the lwpa_config.h file, if provided")
if(LWPA_CONFIG_FILE_LOC)
  set(${LWPA_HAVE_CONFIG_H} LWPA_HAVE_CONFIG_H)
endif()

###############################################################################
# Core lwpa library
###############################################################################

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
  ${LWPA_ROOT}/include/lwpa/pack.h
  ${LWPA_ROOT}/include/lwpa/pdu.h
  ${LWPA_ROOT}/include/lwpa/rbtree.h
  ${LWPA_ROOT}/include/lwpa/root_layer_pdu.h
  ${LWPA_ROOT}/include/lwpa/socket.h
  ${LWPA_ROOT}/include/lwpa/timer.h

  ${LWPA_ROOT}/include/os/${LWPA_OS_TARGET}/lwpa/os_lock.h
  ${LWPA_ROOT}/include/os/${LWPA_OS_TARGET}/lwpa/os_thread.h

  ${LWPA_ROOT}/include/os/${LWPA_NET_TARGET}/lwpa/os_socket.h
  ${LWPA_ROOT}/include/os/${LWPA_NET_TARGET}/lwpa/os_inet.h

  # lwpa sources
  ${LWPA_ROOT}/src/lwpa/common.c
  ${LWPA_ROOT}/src/lwpa/error.c
  ${LWPA_ROOT}/src/lwpa/inet.c
  ${LWPA_ROOT}/src/lwpa/log.c
  ${LWPA_ROOT}/src/lwpa/md5.h
  ${LWPA_ROOT}/src/lwpa/md5.c
  ${LWPA_ROOT}/src/lwpa/mempool.c
  ${LWPA_ROOT}/src/lwpa/netint.c
  ${LWPA_ROOT}/src/lwpa/pack.c
  ${LWPA_ROOT}/src/lwpa/pdu.c
  ${LWPA_ROOT}/src/lwpa/rbtree.c
  ${LWPA_ROOT}/src/lwpa/root_layer_pdu.c
  ${LWPA_ROOT}/src/lwpa/timer.c
  ${LWPA_ROOT}/src/lwpa/uuid.c

  ${LWPA_ROOT}/src/os/${LWPA_OS_TARGET}/lwpa/os_lock.c
  ${LWPA_ROOT}/src/os/${LWPA_OS_TARGET}/lwpa/os_uuid.c
  ${LWPA_ROOT}/src/os/${LWPA_OS_TARGET}/lwpa/os_thread.c
  ${LWPA_ROOT}/src/os/${LWPA_OS_TARGET}/lwpa/os_timer.c

  ${LWPA_ROOT}/src/os/${LWPA_NET_TARGET}/lwpa/os_inet.c
  ${LWPA_ROOT}/src/os/${LWPA_NET_TARGET}/lwpa/os_netint.c
  ${LWPA_ROOT}/src/os/${LWPA_NET_TARGET}/lwpa/os_socket.c
  
  ${LWPA_OS_ADDITIONAL_SOURCES}
  ${LWPA_NET_ADDITIONAL_SOURCES}
)

# lwpa public include directories and library dependencies
target_include_directories(lwpa PUBLIC
  ${LWPA_ROOT}/include
  ${LWPA_ROOT}/include/os/${LWPA_OS_TARGET}
  ${LWPA_ROOT}/include/os/${LWPA_NET_TARGET}
  ${LWPA_OS_ADDITIONAL_INCLUDE_DIRS}
  ${LWPA_NET_ADDITIONAL_INCLUDE_DIRS}
  ${LWPA_TOOLCHAIN_ADDITIONAL_INCLUDE_DIRS}
  ${LWPA_CONFIG_FILE_LOC}
)
target_include_directories(lwpa PRIVATE ${LWPA_ROOT}/src)
target_compile_definitions(lwpa PRIVATE ${LWPA_HAVE_CONFIG_H})
if(LWPA_NETINT_DEBUG_OUTPUT)
  target_compile_definitions(lwpa PRIVATE LWPA_NETINT_DEBUG_OUTPUT)
endif()
target_link_libraries(lwpa PUBLIC ${LWPA_OS_ADDITIONAL_LIBS} ${LWPA_NET_ADDITIONAL_LIBS})

set_target_properties(lwpa PROPERTIES DEBUG_POSTFIX d)

# Installation
install(TARGETS lwpa ARCHIVE DESTINATION lib)
install(DIRECTORY ${LWPA_ROOT}/include/lwpa DESTINATION include FILES_MATCHING PATTERN "*.h")
install(DIRECTORY ${LWPA_ROOT}/include/os/${LWPA_OS_TARGET}/lwpa DESTINATION include FILES_MATCHING PATTERN "*.h")
install(DIRECTORY ${LWPA_ROOT}/include/os/${LWPA_NET_TARGET}/lwpa DESTINATION include FILES_MATCHING PATTERN "*.h")

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
    ${LWPA_ROOT}/include/os/${LWPA_OS_TARGET}/lwpa/os_lock.h
    ${LWPA_ROOT}/include/os/${LWPA_OS_TARGET}/lwpa/os_socket.h
    ${LWPA_ROOT}/include/os/${LWPA_OS_TARGET}/lwpa/os_thread.h

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
    ${LWPA_ROOT}/src/os/${LWPA_OS_TARGET}/lwpa/os_lock.c
    ${LWPA_ROOT}/src/os/${LWPA_OS_TARGET}/lwpa/os_netint.c
    ${LWPA_ROOT}/src/os/${LWPA_OS_TARGET}/lwpa/os_uuid.c
    ${LWPA_ROOT}/src/os/${LWPA_OS_TARGET}/lwpa/os_socket.c
    ${LWPA_ROOT}/src/os/${LWPA_OS_TARGET}/lwpa/os_thread.c
    ${LWPA_ROOT}/src/os/${LWPA_OS_TARGET}/lwpa/os_timer.c

    ${LWPA_ROOT}/src/lwpa_mock/socket.c
  )
  target_include_directories(lwpa_mock PUBLIC
    ${LWPA_ROOT}/include
    ${LWPA_ROOT}/include/os/${LWPA_OS_TARGET}
  )
  target_include_directories(lwpa_mock PRIVATE ${LWPA_ROOT}/src)
  target_compile_definitions(lwpa_mock PRIVATE LWPA_BUILDING_MOCK_LIB)
  target_link_libraries(lwpa_mock PUBLIC meekrosoft::fff)
  if(LWPA_OS_TARGET STREQUAL windows)
    target_link_libraries(lwpa_mock PUBLIC winmm ws2_32 Iphlpapi Rpcrt4)
  endif()
  set_target_properties(lwpa PROPERTIES DEBUG_POSTFIX d)
endif()
