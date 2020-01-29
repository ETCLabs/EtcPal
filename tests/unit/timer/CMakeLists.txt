# A custom test to feed the etcpal_timer module edge cases from getms() and make sure it handles
# them successfully.

etcpal_add_custom_test(test_timer_controlled C
  test_timer_controlled.c
  ${ETCPAL_SRC}/etcpal/timer.c
)
target_include_directories(test_timer_controlled PRIVATE ${ETCPAL_INCLUDE})
