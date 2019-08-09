# test_lwpa
# Main lwpa test module, does "live" tests which use system calls of the underlying OS to ensure
# consistent behavior across platforms.

add_library(lib_test_lwpa
  # lwpa unit test sources
  test_common.c
#  test_inet.c
#  test_log.c
#  test_mempool.c
#  test_mutex.c
#  test_netint.c
#  test_pack.c
#  test_rbtree.c
#  test_rwlock.c
#  test_signal.c
#  test_socket.c
#  test_socket_poll.c
#  test_thread.c
#  test_timer.c
#  test_uuid.c
  test_main.c
)
target_include_directories(lib_test_lwpa PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(lib_test_lwpa PUBLIC lwpa ThrowTheSwitch::Unity meekrosoft::fff)
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  target_compile_options(lib_test_lwpa PRIVATE -Wno-format-security)
endif()

# TODO diagnose consistent failure of these tests on the Azure Pipelines macOS hosted agent.
# if(APPLE)
#   set(LWPA_MACOS_FAILING_TESTS SocketTest.multicast_udp SocketPollTest.udp_in)
#   set_tests_properties(${LWPA_MACOS_FAILING_TESTS} PROPERTIES WILL_FAIL TRUE)
# endif()
