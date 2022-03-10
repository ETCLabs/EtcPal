# A custom test to feed the etcpal_timer module edge cases from getms() and make sure it handles
# them successfully.

etcpal_add_custom_test(etcpal_controlled_unit_tests CXX
  test_timer_controlled.cpp
  test_main.c

  ${ETCPAL_SRC}/etcpal/timer.c
)
target_include_directories(etcpal_controlled_unit_tests PRIVATE
  ${ETCPAL_INCLUDE}
  ${ETCPAL_SRC}
  ${ETCPAL_OS_INCLUDE_DIR}
  ${ETCPAL_NET_INCLUDE_DIR}
)
target_link_libraries(etcpal_controlled_unit_tests PRIVATE
  ${ETCPAL_OS_ADDITIONAL_LIBS}
  ${ETCPAL_NET_ADDITIONAL_LIBS}
)
