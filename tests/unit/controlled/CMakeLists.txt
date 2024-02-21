# A custom test to feed the etcpal_timer module edge cases from getms() and make sure it handles
# them successfully.

etcpal_add_custom_test(etcpal_controlled_unit_tests CXX
  test_timer_controlled.cpp
  test_main.c

  ${ETCPAL_SRC}/etcpal/timer.c

  ${ETCPAL_TEST}/config/assert_verify.c
)
target_include_directories(etcpal_controlled_unit_tests PRIVATE
  ${ETCPAL_INCLUDE}
  ${ETCPAL_SRC}
  ${ETCPAL_OS_INCLUDE_DIR}
  ${ETCPAL_NET_INCLUDE_DIR}
)
target_compile_definitions(etcpal_controlled_unit_tests PUBLIC ETCPAL_HAVE_CONFIG_H)
target_include_directories(etcpal_controlled_unit_tests PUBLIC
  ${ETCPAL_TEST}/config
)
target_link_libraries(etcpal_controlled_unit_tests PRIVATE
  ${ETCPAL_OS_ADDITIONAL_LIBS}
  ${ETCPAL_NET_ADDITIONAL_LIBS}
)
target_link_libraries(etcpal_controlled_unit_tests PUBLIC ThrowTheSwitch::Unity)

if(ETCPAL_HAVE_NETWORKING_SUPPORT)
  target_sources(etcpal_controlled_unit_tests PRIVATE
    ${ETCPAL_SRC}/etcpal/inet.c
    ${ETCPAL_SRC}/etcpal/netint.c
    test_netint_controlled.c
  )
  if (ETCPAL_OS_TARGET STREQUAL "freertos")
    target_sources(etcpal_controlled_unit_tests PRIVATE
      ${ETCPAL_SRC}/os/freertos/etcpal/os_mutex.c
    )
  elseif (ETCPAL_OS_TARGET STREQUAL "windows")
    target_sources(etcpal_controlled_unit_tests PRIVATE
      ${ETCPAL_SRC}/os/windows/etcpal/os_mutex.c
    )
  endif()
endif()
