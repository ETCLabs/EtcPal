# A custom test to make sure that the etcpal_timer module handles wraparound cases correctly.

etcpal_add_custom_test(test_timer_wraparound C
  test_timer_wraparound.c
  ${ETCPAL_SRC}/etcpal/timer.c
)
target_include_directories(test_timer_wraparound PRIVATE ${ETCPAL_INCLUDE})
