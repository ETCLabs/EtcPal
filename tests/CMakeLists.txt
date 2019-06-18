################################### testing ###################################

if(LWPA_TARGET_OS STREQUAL windows OR LWPA_TARGET_OS STREQUAL macos OR LWPA_TARGET_OS STREQUAL linux)
  message(STATUS "Adding GoogleTest dependency...")
  include(AddGoogleTest)

  set(TEST_SRC_DIR ${CMAKE_CURRENT_LIST_DIR}/unit)

  add_executable(test_lwpa
    # lwpa unit test sources
    ${TEST_SRC_DIR}/main.cpp
    ${TEST_SRC_DIR}/test_inet.cpp
    ${TEST_SRC_DIR}/test_log.cpp
    ${TEST_SRC_DIR}/test_mempool.cpp
    ${TEST_SRC_DIR}/test_mutex.cpp
    ${TEST_SRC_DIR}/test_netint.cpp
    ${TEST_SRC_DIR}/test_pack.cpp
    ${TEST_SRC_DIR}/test_rbtree.cpp
    ${TEST_SRC_DIR}/test_rwlock.cpp
    ${TEST_SRC_DIR}/test_signal.cpp
    ${TEST_SRC_DIR}/test_socket.cpp
    ${TEST_SRC_DIR}/test_socket_poll.cpp
    ${TEST_SRC_DIR}/test_thread.cpp
    ${TEST_SRC_DIR}/test_timer.cpp
    ${TEST_SRC_DIR}/test_uuid.cpp
  )
  set_target_properties(test_lwpa PROPERTIES CXX_STANDARD 14)
  target_link_libraries(test_lwpa lwpa gmock)
  if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(test_lwpa PRIVATE -Wno-format-security)
  endif()
endif()
