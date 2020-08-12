# The "live" EtcPal tests, all built as one executable or library for now.

etcpal_add_live_test(etcpal_live_unit_tests C
  test_common.c
  test_log.c
  test_main.c
  test_mempool.c
  test_pack.c
  test_rbtree.c
  test_uuid.c
)

if(ETCPAL_HAVE_OS_SUPPORT)
  target_sources(etcpal_live_unit_tests PRIVATE
    test_lock.c
    test_thread.c
    test_timer.c
    test_queue.c
  )
endif()

if(ETCPAL_HAVE_NETWORKING_SUPPORT)
  # Temporary - TODO fix netints in iOS
  if(IOS)
    target_compile_definitions(etcpal_live_unit_tests PRIVATE ETCPAL_NO_NETWORKING_SUPPORT)
  else()
    target_sources(etcpal_live_unit_tests PRIVATE
      test_inet.c
      test_netint.c
      test_socket.c
    )
  endif()
endif()
