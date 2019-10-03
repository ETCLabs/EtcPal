# The "live" EtcPal tests, all built as one executable or library for now.

etcpal_add_live_test(etcpal_live_unit_tests
  test_common.c
  test_inet.c
  test_lock.c
  test_log.c
  test_main.c
  test_mempool.c
  test_pack.c
  test_rbtree.c
  test_thread.c
  test_timer.c
  test_uuid.c
)

if(IOS)
  target_compile_definitions(etcpal_live_unit_tests PRIVATE DISABLE_SOCKET_TESTS)
else()
  target_sources(etcpal_live_unit_tests PRIVATE
    test_netint.c
    test_socket.c
  )
endif()
