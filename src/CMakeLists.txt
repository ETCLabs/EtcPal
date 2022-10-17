
set(ETCPAL_CONFIG_FILE_DIR "" CACHE STRING "The directory path of the etcpal_config.h file, if provided")

if(ETCPAL_CONFIG_FILE_DIR)
  set(${ETCPAL_HAVE_CONFIG_H} ETCPAL_HAVE_CONFIG_H)
endif()

# ##############################################################################
# Core EtcPal library
# ##############################################################################

# Embedded third party sources are in a separate object library to avoid linting them with
# clang-tidy
add_library(EtcPalThirdParty OBJECT
  ${ETCPAL_ROOT}/src/etcpal/thirdparty/md5.h
  ${ETCPAL_ROOT}/src/etcpal/thirdparty/md5.c
  ${ETCPAL_ROOT}/src/etcpal/thirdparty/sha1.h
  ${ETCPAL_ROOT}/src/etcpal/thirdparty/sha1.c
)

set(ETCPAL_LIB_TARGET_NAME EtcPal)

set(ETCPAL_CORE_SOURCES

  # EtcPal headers
  ${ETCPAL_ROOT}/include/etcpal/acn_pdu.h
  ${ETCPAL_ROOT}/include/etcpal/acn_prot.h
  ${ETCPAL_ROOT}/include/etcpal/acn_rlp.h
  ${ETCPAL_ROOT}/include/etcpal/common.h
  ${ETCPAL_ROOT}/include/etcpal/error.h
  ${ETCPAL_ROOT}/include/etcpal/handle_manager.h
  ${ETCPAL_ROOT}/include/etcpal/log.h
  ${ETCPAL_ROOT}/include/etcpal/mempool.h
  ${ETCPAL_ROOT}/include/etcpal/pack.h
  ${ETCPAL_ROOT}/include/etcpal/pack64.h
  ${ETCPAL_ROOT}/include/etcpal/rbtree.h
  ${ETCPAL_ROOT}/include/etcpal/timer.h
  ${ETCPAL_ROOT}/include/etcpal/uuid.h
  ${ETCPAL_ROOT}/include/etcpal/version.h

  # EtcPal sources
  ${ETCPAL_ROOT}/src/etcpal/acn_pdu.c
  ${ETCPAL_ROOT}/src/etcpal/acn_rlp.c
  ${ETCPAL_ROOT}/src/etcpal/common.c
  ${ETCPAL_ROOT}/src/etcpal/error.c
  ${ETCPAL_ROOT}/src/etcpal/handle_manager.c
  ${ETCPAL_ROOT}/src/etcpal/log.c
  ${ETCPAL_ROOT}/src/etcpal/mempool.c
  ${ETCPAL_ROOT}/src/etcpal/pack.c
  ${ETCPAL_ROOT}/src/etcpal/rbtree.c
  ${ETCPAL_ROOT}/src/etcpal/timer.c
  ${ETCPAL_ROOT}/src/etcpal/uuid.c
)

if(ETCPAL_HAVE_OS_SUPPORT)
  set(ETCPAL_CORE_SOURCES ${ETCPAL_CORE_SOURCES}
    ${ETCPAL_ROOT}/include/etcpal/mutex.h
    ${ETCPAL_ROOT}/include/etcpal/queue.h
    ${ETCPAL_ROOT}/include/etcpal/rwlock.h
    ${ETCPAL_ROOT}/include/etcpal/sem.h
    ${ETCPAL_ROOT}/include/etcpal/signal.h
    ${ETCPAL_ROOT}/include/etcpal/thread.h
  )
endif()

if(ETCPAL_HAVE_NETWORKING_SUPPORT)
  set(ETCPAL_CORE_SOURCES ${ETCPAL_CORE_SOURCES}
    ${ETCPAL_ROOT}/include/etcpal/inet.h
    ${ETCPAL_ROOT}/include/etcpal/netint.h
    ${ETCPAL_ROOT}/include/etcpal/socket.h
    ${ETCPAL_ROOT}/src/etcpal/inet.c
    ${ETCPAL_ROOT}/src/etcpal/netint.c
  )
endif()

# This is used for creating the main EtcPal target as well as the one used by the live unit tests
function(add_etcpal_library target_name)
  add_library(${target_name}
    ${ETCPAL_CORE_SOURCES}
    $<TARGET_OBJECTS:EtcPalThirdParty>
    ${ETCPAL_OS_ADDITIONAL_SOURCES}
    ${ETCPAL_NET_ADDITIONAL_SOURCES}
  )

  # EtcPal public include directories and library dependencies
  target_include_directories(${target_name} PUBLIC
    ${ETCPAL_CONFIG_FILE_DIR}
    ${ETCPAL_ROOT}/include
    ${ETCPAL_OS_INCLUDE_DIR}
    ${ETCPAL_NET_INCLUDE_DIR}
  )

  target_include_directories(${target_name} PRIVATE ${ETCPAL_ROOT}/src)
  target_compile_definitions(${target_name} PRIVATE
    ${ETCPAL_OS_ADDITIONAL_DEFINES}
    ${ETCPAL_NET_ADDITIONAL_DEFINES}
  )

  target_link_libraries(${target_name} PUBLIC ${ETCPAL_OS_ADDITIONAL_LIBS} ${ETCPAL_NET_ADDITIONAL_LIBS})

  if(NOT MSVC)
    set_target_properties(${target_name} PROPERTIES C_STANDARD 99)
  endif()
endfunction()

# Set up the main EtcPal target here
add_etcpal_library(${ETCPAL_LIB_TARGET_NAME})

if(ETCPAL_EXPLICITLY_DISABLE_EXCEPTIONS)
  target_compile_definitions(${ETCPAL_LIB_TARGET_NAME} PUBLIC ETCPAL_NO_EXCEPTIONS)
endif()

target_compile_definitions(${ETCPAL_LIB_TARGET_NAME} PRIVATE
  ${ETCPAL_HAVE_CONFIG_H}
)

if(ETCPAL_NETINT_DEBUG_OUTPUT)
  target_compile_definitions(${ETCPAL_LIB_TARGET_NAME} PRIVATE ETCPAL_NETINT_DEBUG_OUTPUT)
endif()

if(NOT COMPILING_AS_OSS)
  target_clang_tidy(${ETCPAL_LIB_TARGET_NAME})
endif()

set_target_properties(${ETCPAL_LIB_TARGET_NAME} PROPERTIES COMPILE_PDB_NAME "${ETCPAL_LIB_TARGET_NAME}")
set_target_properties(${ETCPAL_LIB_TARGET_NAME} PROPERTIES COMPILE_PDB_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")

# Write a pkg-config file
set(ETCPAL_PKGCONFIG_INTERMEDIATE_FILE ${CMAKE_BINARY_DIR}/${ETCPAL_LIB_TARGET_NAME}.pc)
configure_file(${ETCPAL_ROOT}/cmake/EtcPal.pc.in ${ETCPAL_PKGCONFIG_INTERMEDIATE_FILE} @ONLY)

# Installation
install(TARGETS ${ETCPAL_LIB_TARGET_NAME} ARCHIVE DESTINATION lib)
install(DIRECTORY ${ETCPAL_ROOT}/include/etcpal DESTINATION include FILES_MATCHING PATTERN "*.h")
install(DIRECTORY ${ETCPAL_OS_INCLUDE_DIR}/etcpal DESTINATION include FILES_MATCHING PATTERN "*.h")

if(ETCPAL_NET_INCLUDE_DIR)
  install(DIRECTORY ${ETCPAL_NET_INCLUDE_DIR}/etcpal DESTINATION include FILES_MATCHING PATTERN "*.h")
endif()

install(DIRECTORY ${ETCPAL_ROOT}/include/etcpal/cpp DESTINATION include/etcpal FILES_MATCHING PATTERN "*.h")
install(FILES ${ETCPAL_PKGCONFIG_INTERMEDIATE_FILE} DESTINATION lib/pkgconfig)

if(ETCPAL_INSTALL_PDBS)
  get_target_property(ETCPAL_PDB_OUTPUT_DIRECTORY ${ETCPAL_LIB_TARGET_NAME} COMPILE_PDB_OUTPUT_DIRECTORY)
  get_target_property(ETCPAL_PDB_NAME ${ETCPAL_LIB_TARGET_NAME} COMPILE_PDB_NAME)
  install(FILES "${ETCPAL_PDB_OUTPUT_DIRECTORY}/Debug/${ETCPAL_PDB_NAME}.pdb" DESTINATION lib CONFIGURATIONS Debug OPTIONAL)
  install(FILES "${ETCPAL_PDB_OUTPUT_DIRECTORY}/RelWithDebInfo/${ETCPAL_PDB_NAME}.pdb" DESTINATION lib CONFIGURATIONS RelWithDebInfo OPTIONAL)
endif()

# ##############################################################################
# Mock EtcPal library
# ##############################################################################
if(ETCPAL_BUILD_MOCK_LIB)
  add_library(EtcPalMock
    $<TARGET_OBJECTS:EtcPalThirdParty>

    # EtcPal headers
    ${ETCPAL_ROOT}/include/etcpal/acn_pdu.h
    ${ETCPAL_ROOT}/include/etcpal/acn_prot.h
    ${ETCPAL_ROOT}/include/etcpal/acn_rlp.h
    ${ETCPAL_ROOT}/include/etcpal/uuid.h
    ${ETCPAL_ROOT}/include/etcpal/error.h
    ${ETCPAL_ROOT}/include/etcpal/handle_manager.h
    ${ETCPAL_ROOT}/include/etcpal/inet.h
    ${ETCPAL_ROOT}/include/etcpal/log.h
    ${ETCPAL_ROOT}/include/etcpal/mempool.h
    ${ETCPAL_ROOT}/include/etcpal/pack.h
    ${ETCPAL_ROOT}/include/etcpal/rbtree.h

    ${ETCPAL_ROOT}/include/etcpal_mock/common.h
    ${ETCPAL_ROOT}/include/etcpal_mock/netint.h
    ${ETCPAL_ROOT}/include/etcpal_mock/socket.h
    ${ETCPAL_ROOT}/include/etcpal_mock/timer.h
    ${ETCPAL_ROOT}/include/etcpal_mock/thread.h

    # EtcPal sources
    # We will gradually substitute these with mocks as needed
    ${ETCPAL_ROOT}/src/etcpal/acn_pdu.c
    ${ETCPAL_ROOT}/src/etcpal/acn_rlp.c
    ${ETCPAL_ROOT}/src/etcpal/error.c
    ${ETCPAL_ROOT}/src/etcpal/handle_manager.c
    ${ETCPAL_ROOT}/src/etcpal/inet.c
    ${ETCPAL_ROOT}/src/etcpal/log.c
    ${ETCPAL_ROOT}/src/etcpal/mempool.c
    ${ETCPAL_ROOT}/src/etcpal/pack.c
    ${ETCPAL_ROOT}/src/etcpal/rbtree.c
    ${ETCPAL_ROOT}/src/etcpal/timer.c
    ${ETCPAL_ROOT}/src/etcpal/uuid.c

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
    ${ETCPAL_OS_INCLUDE_DIR}
    ${ETCPAL_NET_INCLUDE_DIR}
  )
  target_include_directories(EtcPalMock PRIVATE ${ETCPAL_ROOT}/src)
  target_compile_definitions(EtcPalMock PRIVATE ETCPAL_BUILDING_MOCK_LIB)
  target_link_libraries(EtcPalMock PUBLIC meekrosoft::fff)
  target_link_libraries(EtcPalMock PUBLIC ${ETCPAL_OS_ADDITIONAL_LIBS} ${ETCPAL_NET_ADDITIONAL_LIBS})

  if(NOT MSVC)
    set_target_properties(EtcPalMock PROPERTIES C_STANDARD 99)
  endif()
endif()
