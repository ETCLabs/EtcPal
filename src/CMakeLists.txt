
set(ETCPAL_CONFIG_FILE_DIR "" CACHE STRING "The directory path of the lwpa_config.h file, if provided")
if(ETCPAL_CONFIG_FILE_DIR)
  set(${ETCPAL_HAVE_CONFIG_H} ETCPAL_HAVE_CONFIG_H)
endif()

###############################################################################
# Core lwpa library
###############################################################################

add_library(EtcPal
  # EtcPal headers
  ${ETCPAL_ROOT}/include/lwpa/acn_prot.h
  ${ETCPAL_ROOT}/include/lwpa/bool.h
  ${ETCPAL_ROOT}/include/lwpa/uuid.h
  ${ETCPAL_ROOT}/include/lwpa/common.h
  ${ETCPAL_ROOT}/include/lwpa/error.h
  ${ETCPAL_ROOT}/include/lwpa/inet.h
  ${ETCPAL_ROOT}/include/lwpa/int.h
  ${ETCPAL_ROOT}/include/lwpa/log.h
  ${ETCPAL_ROOT}/include/lwpa/mempool.h
  ${ETCPAL_ROOT}/include/lwpa/netint.h
  ${ETCPAL_ROOT}/include/lwpa/pack.h
  ${ETCPAL_ROOT}/include/lwpa/pdu.h
  ${ETCPAL_ROOT}/include/lwpa/rbtree.h
  ${ETCPAL_ROOT}/include/lwpa/root_layer_pdu.h
  ${ETCPAL_ROOT}/include/lwpa/socket.h
  ${ETCPAL_ROOT}/include/lwpa/timer.h

  ${ETCPAL_ROOT}/include/os/${ETCPAL_OS_TARGET}/lwpa/os_lock.h
  ${ETCPAL_ROOT}/include/os/${ETCPAL_OS_TARGET}/lwpa/os_thread.h

  ${ETCPAL_ROOT}/include/os/${ETCPAL_NET_TARGET}/lwpa/os_socket.h
  ${ETCPAL_ROOT}/include/os/${ETCPAL_NET_TARGET}/lwpa/os_inet.h

  # EtcPal sources
  ${ETCPAL_ROOT}/src/lwpa/common.c
  ${ETCPAL_ROOT}/src/lwpa/error.c
  ${ETCPAL_ROOT}/src/lwpa/inet.c
  ${ETCPAL_ROOT}/src/lwpa/log.c
  ${ETCPAL_ROOT}/src/lwpa/md5.h
  ${ETCPAL_ROOT}/src/lwpa/md5.c
  ${ETCPAL_ROOT}/src/lwpa/mempool.c
  ${ETCPAL_ROOT}/src/lwpa/netint.c
  ${ETCPAL_ROOT}/src/lwpa/pack.c
  ${ETCPAL_ROOT}/src/lwpa/pdu.c
  ${ETCPAL_ROOT}/src/lwpa/rbtree.c
  ${ETCPAL_ROOT}/src/lwpa/root_layer_pdu.c
  ${ETCPAL_ROOT}/src/lwpa/timer.c
  ${ETCPAL_ROOT}/src/lwpa/uuid.c

  ${ETCPAL_ROOT}/src/os/${ETCPAL_OS_TARGET}/lwpa/os_lock.c
  ${ETCPAL_ROOT}/src/os/${ETCPAL_OS_TARGET}/lwpa/os_thread.c
  ${ETCPAL_ROOT}/src/os/${ETCPAL_OS_TARGET}/lwpa/os_timer.c
  ${ETCPAL_ROOT}/src/os/${ETCPAL_OS_TARGET}/lwpa/os_uuid.c

  ${ETCPAL_ROOT}/src/os/${ETCPAL_NET_TARGET}/lwpa/os_inet.c
  ${ETCPAL_ROOT}/src/os/${ETCPAL_NET_TARGET}/lwpa/os_netint.c
  ${ETCPAL_ROOT}/src/os/${ETCPAL_NET_TARGET}/lwpa/os_socket.c
  
  ${ETCPAL_OS_ADDITIONAL_SOURCES}
  ${ETCPAL_NET_ADDITIONAL_SOURCES}
)

# EtcPal public include directories and library dependencies
target_include_directories(EtcPal PUBLIC
  ${ETCPAL_ROOT}/include
  ${ETCPAL_ROOT}/include/os/${ETCPAL_OS_TARGET}
  ${ETCPAL_ROOT}/include/os/${ETCPAL_NET_TARGET}
  ${ETCPAL_OS_ADDITIONAL_INCLUDE_DIRS}
  ${ETCPAL_NET_ADDITIONAL_INCLUDE_DIRS}
  ${ETCPAL_CONFIG_FILE_DIR}
)
target_include_directories(EtcPal PRIVATE ${ETCPAL_ROOT}/src)
target_compile_definitions(EtcPal PRIVATE ${ETCPAL_HAVE_CONFIG_H})
if(ETCPAL_NETINT_DEBUG_OUTPUT)
  target_compile_definitions(EtcPal PRIVATE ETCPAL_NETINT_DEBUG_OUTPUT)
endif()
target_link_libraries(EtcPal PUBLIC ${ETCPAL_OS_ADDITIONAL_LIBS} ${ETCPAL_NET_ADDITIONAL_LIBS})
set_target_properties(EtcPal PROPERTIES DEBUG_POSTFIX d)

if(NOT MSVC)
  set_target_properties(EtcPal PROPERTIES C_STANDARD 99)
endif()

# Installation
install(TARGETS EtcPal ARCHIVE DESTINATION lib)
install(DIRECTORY ${ETCPAL_ROOT}/include/lwpa DESTINATION include FILES_MATCHING PATTERN "*.h")
install(DIRECTORY ${ETCPAL_ROOT}/include/os/${ETCPAL_OS_TARGET}/lwpa DESTINATION include FILES_MATCHING PATTERN "*.h")
install(DIRECTORY ${ETCPAL_ROOT}/include/os/${ETCPAL_NET_TARGET}/lwpa DESTINATION include FILES_MATCHING PATTERN "*.h")

###############################################################################
# Mock lwpa library
###############################################################################

if(ETCPAL_BUILD_MOCK_LIB)
  add_library(EtcPalMock
    # EtcPal headers
    ${ETCPAL_ROOT}/include/lwpa/acn_prot.h
    ${ETCPAL_ROOT}/include/lwpa/bool.h
    ${ETCPAL_ROOT}/include/lwpa/uuid.h
    ${ETCPAL_ROOT}/include/lwpa/common.h
    ${ETCPAL_ROOT}/include/lwpa/error.h
    ${ETCPAL_ROOT}/include/lwpa/inet.h
    ${ETCPAL_ROOT}/include/lwpa/int.h
    ${ETCPAL_ROOT}/include/lwpa/log.h
    ${ETCPAL_ROOT}/include/lwpa/mempool.h
    ${ETCPAL_ROOT}/include/lwpa/netint.h
    ${ETCPAL_ROOT}/include/lwpa/pack.h
    ${ETCPAL_ROOT}/include/lwpa/pdu.h
    ${ETCPAL_ROOT}/include/lwpa/rbtree.h
    ${ETCPAL_ROOT}/include/lwpa/root_layer_pdu.h
    ${ETCPAL_ROOT}/include/lwpa/socket.h
    ${ETCPAL_ROOT}/include/lwpa/timer.h

    ${ETCPAL_ROOT}/include/os/${ETCPAL_OS_TARGET}/lwpa/os_lock.h
    ${ETCPAL_ROOT}/include/os/${ETCPAL_OS_TARGET}/lwpa/os_thread.h

    ${ETCPAL_ROOT}/include/os/${ETCPAL_NET_TARGET}/lwpa/os_socket.h
    ${ETCPAL_ROOT}/include/os/${ETCPAL_NET_TARGET}/lwpa/os_inet.h

    ${ETCPAL_ROOT}/include/lwpa_mock/socket.h

    # EtcPal sources
    # We will gradually substitute these with mocks as needed
    ${ETCPAL_ROOT}/src/lwpa/common.c
    ${ETCPAL_ROOT}/src/lwpa/error.c
    ${ETCPAL_ROOT}/src/lwpa/inet.c
    ${ETCPAL_ROOT}/src/lwpa/log.c
    ${ETCPAL_ROOT}/src/lwpa/md5.h
    ${ETCPAL_ROOT}/src/lwpa/md5.c
    ${ETCPAL_ROOT}/src/lwpa/mempool.c
    ${ETCPAL_ROOT}/src/lwpa/netint.c
    ${ETCPAL_ROOT}/src/lwpa/pack.c
    ${ETCPAL_ROOT}/src/lwpa/pdu.c
    ${ETCPAL_ROOT}/src/lwpa/rbtree.c
    ${ETCPAL_ROOT}/src/lwpa/root_layer_pdu.c
    ${ETCPAL_ROOT}/src/lwpa/timer.c
    ${ETCPAL_ROOT}/src/lwpa/uuid.c

    ${ETCPAL_ROOT}/src/os/${ETCPAL_OS_TARGET}/lwpa/os_lock.c
    ${ETCPAL_ROOT}/src/os/${ETCPAL_OS_TARGET}/lwpa/os_thread.c
    ${ETCPAL_ROOT}/src/os/${ETCPAL_OS_TARGET}/lwpa/os_timer.c
    ${ETCPAL_ROOT}/src/os/${ETCPAL_OS_TARGET}/lwpa/os_uuid.c

    ${ETCPAL_ROOT}/src/os/${ETCPAL_NET_TARGET}/lwpa/os_inet.c
    ${ETCPAL_ROOT}/src/os/${ETCPAL_NET_TARGET}/lwpa/os_netint.c
    ${ETCPAL_ROOT}/src/os/${ETCPAL_NET_TARGET}/lwpa/os_socket.c

    ${ETCPAL_ROOT}/src/lwpa_mock/socket.c
  )
  target_include_directories(EtcPalMock PUBLIC
    ${ETCPAL_ROOT}/include
    ${ETCPAL_ROOT}/include/os/${ETCPAL_OS_TARGET}
    ${ETCPAL_ROOT}/include/os/${ETCPAL_NET_TARGET}
  )
  target_include_directories(EtcPalMock PRIVATE ${ETCPAL_ROOT}/src)
  target_compile_definitions(EtcPalMock PRIVATE ETCPAL_BUILDING_MOCK_LIB)
  target_link_libraries(EtcPalMock PUBLIC meekrosoft::fff)
  target_link_libraries(EtcPalMock PUBLIC ${ETCPAL_OS_ADDITIONAL_LIBS} ${ETCPAL_NET_ADDITIONAL_LIBS})
  set_target_properties(EtcPalMock PROPERTIES DEBUG_POSTFIX d)
  if(NOT MSVC)
    set_target_properties(EtcPalMock PROPERTIES C_STANDARD 99)
  endif()
endif()
