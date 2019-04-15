################################### testing ###################################

if(LWPA_PLATFORM STREQUAL windows OR LWPA_PLATFORM STREQUAL linux)
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
    ${TEST_SRC_DIR}/test_thread.cpp
    ${TEST_SRC_DIR}/test_timer.cpp
    ${TEST_SRC_DIR}/test_uuid.cpp
  )

  target_link_libraries(test_lwpa lwpa gmock)
endif()
