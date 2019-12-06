
set(ETCPAL_CONFIG_FILE_DIR "" CACHE STRING "The directory path of the etcpal_config.h file, if provided")
if(ETCPAL_CONFIG_FILE_DIR)
  set(${ETCPAL_HAVE_CONFIG_H} ETCPAL_HAVE_CONFIG_H)
endif()

###############################################################################
# Core EtcPal library
###############################################################################

add_library(EtcPalInterface INTERFACE)
target_include_directories(EtcPalInterface INTERFACE 
  ${ETCPAL_ROOT}/include
  ${ETCPAL_ROOT}/include/os/${ETCPAL_OS_TARGET}
  ${ETCPAL_ROOT}/include/os/${ETCPAL_NET_TARGET}
  ${ETCPAL_OS_ADDITIONAL_INCLUDE_DIRS}
  ${ETCPAL_NET_ADDITIONAL_INCLUDE_DIRS}
)

add_library(EtcPal
  # EtcPal headers
  ${ETCPAL_ROOT}/include/etcpal/acn_prot.h
  ${ETCPAL_ROOT}/include/etcpal/bool.h
  ${ETCPAL_ROOT}/include/etcpal/uuid.h
  ${ETCPAL_ROOT}/include/etcpal/common.h
  ${ETCPAL_ROOT}/include/etcpal/error.h
  ${ETCPAL_ROOT}/include/etcpal/inet.h
  ${ETCPAL_ROOT}/include/etcpal/int.h
  ${ETCPAL_ROOT}/include/etcpal/log.h
  ${ETCPAL_ROOT}/include/etcpal/mempool.h
  ${ETCPAL_ROOT}/include/etcpal/pack.h
  ${ETCPAL_ROOT}/include/etcpal/pdu.h
  ${ETCPAL_ROOT}/include/etcpal/rbtree.h
  ${ETCPAL_ROOT}/include/etcpal/root_layer_pdu.h
  ${ETCPAL_ROOT}/include/etcpal/socket.h
  ${ETCPAL_ROOT}/include/etcpal/timer.h

  ${ETCPAL_ROOT}/include/os/${ETCPAL_OS_TARGET}/etcpal/os_lock.h
  ${ETCPAL_ROOT}/include/os/${ETCPAL_OS_TARGET}/etcpal/os_thread.h

  ${ETCPAL_ROOT}/include/os/${ETCPAL_NET_TARGET}/etcpal/os_socket.h
  ${ETCPAL_ROOT}/include/os/${ETCPAL_NET_TARGET}/etcpal/os_inet.h

  # EtcPal sources
  ${ETCPAL_ROOT}/src/etcpal/common.c
  ${ETCPAL_ROOT}/src/etcpal/error.c
  ${ETCPAL_ROOT}/src/etcpal/inet.c
  ${ETCPAL_ROOT}/src/etcpal/log.c
  ${ETCPAL_ROOT}/src/etcpal/md5.h
  ${ETCPAL_ROOT}/src/etcpal/md5.c
  ${ETCPAL_ROOT}/src/etcpal/mempool.c
  ${ETCPAL_ROOT}/src/etcpal/pack.c
  ${ETCPAL_ROOT}/src/etcpal/pdu.c
  ${ETCPAL_ROOT}/src/etcpal/rbtree.c
  ${ETCPAL_ROOT}/src/etcpal/root_layer_pdu.c
  ${ETCPAL_ROOT}/src/etcpal/timer.c
  ${ETCPAL_ROOT}/src/etcpal/uuid.c

  ${ETCPAL_ROOT}/src/os/${ETCPAL_OS_TARGET}/etcpal/os_lock.c
  ${ETCPAL_ROOT}/src/os/${ETCPAL_OS_TARGET}/etcpal/os_thread.c
  ${ETCPAL_ROOT}/src/os/${ETCPAL_OS_TARGET}/etcpal/os_timer.c
  ${ETCPAL_ROOT}/src/os/${ETCPAL_OS_TARGET}/etcpal/os_uuid.c

  ${ETCPAL_ROOT}/src/os/${ETCPAL_NET_TARGET}/etcpal/os_inet.c
  ${ETCPAL_ROOT}/src/os/${ETCPAL_NET_TARGET}/etcpal/os_socket.c
  
  ${ETCPAL_OS_ADDITIONAL_SOURCES}
  ${ETCPAL_NET_ADDITIONAL_SOURCES}
)

# Temporary - TODO fix netints on iOS
if(IOS)
  target_compile_definitions(EtcPal PRIVATE ETCPAL_NO_NETINTS)
else()
  target_sources(EtcPal PRIVATE
    ${ETCPAL_ROOT}/include/etcpal/netint.h
    ${ETCPAL_ROOT}/src/etcpal/netint.c
    ${ETCPAL_ROOT}/src/os/${ETCPAL_NET_TARGET}/etcpal/os_netint.c
  )
endif()

# EtcPal public include directories and library dependencies
target_include_directories(EtcPal PUBLIC ${ETCPAL_CONFIG_FILE_DIR})
target_include_directories(EtcPal PRIVATE ${ETCPAL_ROOT}/src)
target_compile_definitions(EtcPal PRIVATE ${ETCPAL_HAVE_CONFIG_H})
if(ETCPAL_NETINT_DEBUG_OUTPUT)
  target_compile_definitions(EtcPal PRIVATE ETCPAL_NETINT_DEBUG_OUTPUT)
endif()
target_link_libraries(EtcPal PUBLIC EtcPalInterface ${ETCPAL_OS_ADDITIONAL_LIBS} ${ETCPAL_NET_ADDITIONAL_LIBS})

if(NOT MSVC)
  set_target_properties(EtcPal PROPERTIES C_STANDARD 99)
endif()

# Installation
install(TARGETS EtcPal ARCHIVE DESTINATION lib)
install(DIRECTORY ${ETCPAL_ROOT}/include/etcpal DESTINATION include FILES_MATCHING PATTERN "*.h")
install(DIRECTORY ${ETCPAL_ROOT}/include/os/${ETCPAL_OS_TARGET}/etcpal DESTINATION include FILES_MATCHING PATTERN "*.h")
install(DIRECTORY ${ETCPAL_ROOT}/include/os/${ETCPAL_NET_TARGET}/etcpal DESTINATION include FILES_MATCHING PATTERN "*.h")

###############################################################################
# Mock EtcPal library
###############################################################################

if(ETCPAL_BUILD_MOCK_LIB)
  add_library(EtcPalMock
    # EtcPal headers
    ${ETCPAL_ROOT}/include/etcpal/acn_prot.h
    ${ETCPAL_ROOT}/include/etcpal/bool.h
    ${ETCPAL_ROOT}/include/etcpal/uuid.h
    ${ETCPAL_ROOT}/include/etcpal/error.h
    ${ETCPAL_ROOT}/include/etcpal/inet.h
    ${ETCPAL_ROOT}/include/etcpal/int.h
    ${ETCPAL_ROOT}/include/etcpal/log.h
    ${ETCPAL_ROOT}/include/etcpal/mempool.h
    ${ETCPAL_ROOT}/include/etcpal/pack.h
    ${ETCPAL_ROOT}/include/etcpal/pdu.h
    ${ETCPAL_ROOT}/include/etcpal/rbtree.h
    ${ETCPAL_ROOT}/include/etcpal/root_layer_pdu.h

    ${ETCPAL_ROOT}/include/os/${ETCPAL_OS_TARGET}/etcpal/os_lock.h
    ${ETCPAL_ROOT}/include/os/${ETCPAL_OS_TARGET}/etcpal/os_thread.h

    ${ETCPAL_ROOT}/include/os/${ETCPAL_NET_TARGET}/etcpal/os_socket.h
    ${ETCPAL_ROOT}/include/os/${ETCPAL_NET_TARGET}/etcpal/os_inet.h

    ${ETCPAL_ROOT}/include/etcpal_mock/common.h
    ${ETCPAL_ROOT}/include/etcpal_mock/netint.h
    ${ETCPAL_ROOT}/include/etcpal_mock/socket.h
    ${ETCPAL_ROOT}/include/etcpal_mock/timer.h
    ${ETCPAL_ROOT}/include/etcpal_mock/thread.h

    # EtcPal sources
    # We will gradually substitute these with mocks as needed
    ${ETCPAL_ROOT}/src/etcpal/error.c
    ${ETCPAL_ROOT}/src/etcpal/inet.c
    ${ETCPAL_ROOT}/src/etcpal/log.c
    ${ETCPAL_ROOT}/src/etcpal/md5.h
    ${ETCPAL_ROOT}/src/etcpal/md5.c
    ${ETCPAL_ROOT}/src/etcpal/mempool.c
    ${ETCPAL_ROOT}/src/etcpal/pack.c
    ${ETCPAL_ROOT}/src/etcpal/pdu.c
    ${ETCPAL_ROOT}/src/etcpal/rbtree.c
    ${ETCPAL_ROOT}/src/etcpal/root_layer_pdu.c
    ${ETCPAL_ROOT}/src/etcpal/timer.c
    ${ETCPAL_ROOT}/src/etcpal/uuid.c

    ${ETCPAL_ROOT}/src/os/${ETCPAL_OS_TARGET}/etcpal/os_lock.c
    ${ETCPAL_ROOT}/src/os/${ETCPAL_OS_TARGET}/etcpal/os_uuid.c

    ${ETCPAL_ROOT}/src/os/${ETCPAL_NET_TARGET}/etcpal/os_inet.c

    ${ETCPAL_ROOT}/src/etcpal_mock/common.c
    ${ETCPAL_ROOT}/src/etcpal_mock/netint.c
    ${ETCPAL_ROOT}/src/etcpal_mock/socket.c
    ${ETCPAL_ROOT}/src/etcpal_mock/timer.c
    ${ETCPAL_ROOT}/src/etcpal_mock/thread.c

    ${ETCPAL_OS_ADDITIONAL_SOURCES}
    ${ETCPAL_NET_ADDITIONAL_SOURCES}
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
  if(NOT MSVC)
    set_target_properties(EtcPalMock PROPERTIES C_STANDARD 99)
  endif()
endif()

###############################################################################
# EtcPalCpp header-only interface library
###############################################################################

add_library(EtcPalCpp INTERFACE)
target_link_libraries(EtcPalCpp INTERFACE EtcPal)

install(DIRECTORY ${ETCPAL_ROOT}/include/etcpal/cpp DESTINATION include/etcpal FILES_MATCHING PATTERN "*.h")
