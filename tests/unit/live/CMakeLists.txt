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
    test_event_group.c
    test_mutex.c
    test_rwlock.c
    test_sem.c
    test_signal.c
    test_timer.c
    test_thread.c
  )

  # freertos is the only platform that supports queues right now
  if (ETCPAL_OS_TARGET STREQUAL "freertos")
    target_sources(etcpal_live_unit_tests PRIVATE test_queue.c)
  else()
    # we need a way to keep the queue tests from running in the code for non queue cabaple platforms
    target_compile_definitions(etcpal_live_unit_tests PRIVATE DISABLE_QUEUE_TESTS)
  endif()

  # Recursive mutexes not supported on MQX
  if (ETCPAL_OS_TARGET STREQUAL "mqx")
    target_compile_definitions(etcpal_live_unit_tests PRIVATE DISABLE_RECURSIVE_MUTEX_TESTS)
  else()
    target_sources(etcpal_live_unit_tests PRIVATE test_recursive_mutex.c)
  endif()
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
