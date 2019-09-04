# EtcPal integration tests
# These are "live" tests which use multiple parts of the EtcPal library. Most of what is tested
# here are the locking, threading and synchronization functions.

etcpal_add_live_test(etcpal_integration_tests
  mutex_integration_test.c
  rwlock_integration_test.c
  signal_integration_test.c
  socket_integration_test.c
  test_main.c
)

# The macOS hosted agents have some unknown restriction on multicast traffic which causes the
# multicast tests to fail.
if(APPLE AND ETCPAL_BUILDING_FOR_AZURE_PIPELINES_CI)
  target_compile_definitions(etcpal_integration_tests PRIVATE LWPA_TEST_DISABLE_MCAST_INTEGRATION_TESTS)
endif()
