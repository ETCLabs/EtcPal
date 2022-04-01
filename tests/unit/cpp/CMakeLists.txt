# The C++ EtcPal tests, all built as one executable or library for now.

etcpal_add_live_test(etcpal_cpp_unit_tests CXX
  test_error.cpp
  test_hash.cpp
  test_main.cpp
  test_opaque_id.cpp
  test_uuid.cpp
)

if(ETCPAL_HAVE_OS_SUPPORT)
  target_sources(etcpal_cpp_unit_tests PRIVATE
    test_log.cpp
    test_mutex.cpp
    test_rwlock.cpp
    test_sem.cpp
    test_signal.cpp
    test_thread.cpp
    test_timer.cpp
  )

  # Recursive mutexes, queues and event groups not supported on MQX
  if (ETCPAL_OS_TARGET STREQUAL "mqx")
    target_compile_definitions(etcpal_cpp_unit_tests PRIVATE
      DISABLE_EVENT_GROUP_TESTS
      DISABLE_RECURSIVE_MUTEX_TESTS
    )
  else()
    target_sources(etcpal_cpp_unit_tests PRIVATE
      test_event_group.cpp
      test_recursive_mutex.cpp
    )
  endif()

  # Queues not supported on MQX or Apple platforms
  if(ETCPAL_OS_TARGET STREQUAL "mqx" OR ETCPAL_OS_TARGET STREQUAL "ios" OR ETCPAL_OS_TARGET STREQUAL "macos")
    target_compile_definitions(etcpal_cpp_unit_tests PRIVATE DISABLE_QUEUE_TESTS)
  else()
    target_sources(etcpal_cpp_unit_tests PRIVATE test_queue.cpp)
  endif()
endif()

if(ETCPAL_HAVE_NETWORKING_SUPPORT)
  target_sources(etcpal_cpp_unit_tests PRIVATE
    test_inet.cpp
  )
endif()
