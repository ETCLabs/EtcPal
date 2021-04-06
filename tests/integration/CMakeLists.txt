# EtcPal integration tests
# These are "live" tests which use multiple parts of the EtcPal library. Most of what is tested
# here are the locking, threading and synchronization functions.

if(ETCPAL_HAVE_OS_SUPPORT)
  etcpal_add_live_test(etcpal_integration_tests C
    mutex_integration_test.c
    rwlock_integration_test.c
    sem_integration_test.c
    signal_integration_test.c
    queue_integration_test.cpp
    queue_tests/queue_simple_concurrency_test.cpp
    test_main.c
  )

  # Temporary - TODO fix netints on iOS
  if(IOS OR NOT ETCPAL_HAVE_NETWORKING_SUPPORT)
    target_compile_definitions(etcpal_integration_tests PRIVATE DISABLE_SOCKET_INTEGRATION_TESTS)
  else()
    target_sources(etcpal_integration_tests PRIVATE 
      socket_integration_test.c
    )
  endif()

  # The macOS hosted agents have some unknown restriction on multicast traffic which causes the
  # multicast tests to fail.
  if(APPLE AND ETCPAL_BUILDING_FOR_AZURE_PIPELINES_CI)
    target_compile_definitions(etcpal_integration_tests PRIVATE ETCPAL_TEST_DISABLE_MCAST_INTEGRATION_TESTS)
  endif()

  # Recursive mutexes and event groups not supported on MQX
  if (ETCPAL_OS_TARGET STREQUAL "mqx")
    target_compile_definitions(etcpal_integration_tests PRIVATE
      DISABLE_EVENT_GROUP_TESTS
      DISABLE_RECURSIVE_MUTEX_TESTS
    )
  else()
    target_sources(etcpal_integration_tests PRIVATE
      event_group_integration_test.c
      recursive_mutex_integration_test.c
    )
  endif()
endif()
