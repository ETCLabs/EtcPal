# lwpa integration tests
# These are "live" tests which use multiple parts of the lwpa library. Most of what is tested here
# are the locking, threading and synchronization functions.

set(LWPA_INTEGRATION_TEST_SOURCES
  mutex_integration_test.c
  rwlock_integration_test.c
  signal_integration_test.c
  socket_integration_test.c
  test_main.c
)

if(LWPA_TEST_BUILD_AS_LIBRARIES)
  add_library(lwpa_integration_tests
    ${LWPA_INTEGRATION_TEST_SOURCES}
  )
  install(TARGETS lwpa_integration_tests ARCHIVE DESTINATION lib)
else()
  add_executable(lwpa_integration_tests
    ${LWPA_INTEGRATION_TEST_SOURCES}
    entrypoint/${LWPA_TEST_ENTRYPOINT}
  )
  lwpa_add_to_ctest(lwpa_integration_tests)
endif()

set_target_properties(lwpa_integration_tests PROPERTIES FOLDER tests)
target_compile_options(lwpa_integration_tests PRIVATE ${LWPA_TEST_COMPILE_OPTIONS})
target_compile_definitions(lwpa_integration_tests PRIVATE ${LWPA_TEST_COMPILE_DEFINITIONS})
target_link_libraries(lwpa_integration_tests PUBLIC lwpa ThrowTheSwitch::Unity)

# The macOS hosted agents have some unknown restriction on multicast traffic which causes the
# multicast tests to fail.
if(APPLE AND LWPA_BUILDING_FOR_AZURE_PIPELINES_CI)
  target_compile_definitions(lwpa_integration_tests PRIVATE LWPA_TEST_DISABLE_MCAST_INTEGRATION_TESTS)
endif()
