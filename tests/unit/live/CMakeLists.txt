# The "live" EtcPal tests, all built as one executable or library for now.

etcpal_add_live_test(etcpal_live_unit_tests C
  test_common.c
  test_handle_manager.c
  test_log.c
  test_main.c
  test_mempool.c
  test_pack.c
  test_rbtree.c
  test_uuid.c
)

if(ETCPAL_HAVE_OS_SUPPORT)
  target_sources(etcpal_live_unit_tests PRIVATE
    test_mutex.c
    test_rwlock.c
    test_sem.c
    test_signal.c
    test_timer.c
    test_thread.c
  )

  # Recursive mutexes and event groups not supported on MQX
  if (ETCPAL_OS_TARGET STREQUAL "mqx")
    target_compile_definitions(etcpal_live_unit_tests PRIVATE
      DISABLE_EVENT_GROUP_TESTS
      DISABLE_RECURSIVE_MUTEX_TESTS
    )
  else()
    target_sources(etcpal_live_unit_tests PRIVATE
      test_event_group.c
      test_recursive_mutex.c
    )
    if (ETCPAL_OS_TARGET STREQUAL "linux")  # Fix undefined reference to `log' error in dev container
      target_link_libraries(etcpal_live_unit_tests PUBLIC m)
    endif()
  endif()

  # Queues not supported on MQX or Apple platforms
  if(ETCPAL_OS_TARGET STREQUAL "mqx" OR ETCPAL_OS_TARGET STREQUAL "ios" OR ETCPAL_OS_TARGET STREQUAL "macos")
    target_compile_definitions(etcpal_live_unit_tests PRIVATE DISABLE_QUEUE_TESTS)
  else()
    target_sources(etcpal_live_unit_tests PRIVATE test_queue.c)
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

  if(ETCPAL_NET_TARGET STREQUAL "lwip")
    add_etcpal_test_library(LiveTestEtcPalWithMalloc ${ETCPAL_TEST}/config/embos_use_malloc)

    etcpal_add_custom_test(etcpal_live_unit_tests_with_malloc C
      test_netint.c
      test_with_malloc_main.c
    )
    target_link_libraries(etcpal_live_unit_tests_with_malloc PUBLIC LiveTestEtcPalWithMalloc)
  endif()
endif()
