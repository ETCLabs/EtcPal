# lwpa integration tests
# These are "live" tests which use multiple parts of the lwpa library. Most of what is tested here
# are the locking, threading and synchronization functions.

add_executable(lwpa_integration_tests
  mutex_integration_test.c
  rwlock_integration_test.c
  signal_integration_test.c
  socket_integration_test.c
  test_main.c

  entrypoint/${LWPA_TEST_ENTRYPOINT}
)
set_target_properties(lwpa_integration_tests PROPERTIES FOLDER tests)
target_compile_options(lwpa_integration_tests PRIVATE ${LWPA_TEST_COMPILE_OPTIONS})
target_compile_definitions(lwpa_integration_tests PRIVATE ${LWPA_TEST_COMPILE_DEFINITIONS})
target_link_libraries(lwpa_integration_tests PRIVATE lwpa ThrowTheSwitch::Unity)
lwpa_add_to_ctest(lwpa_integration_tests)