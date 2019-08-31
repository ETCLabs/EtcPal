# A custom test to make sure that the lwpa_timer module handles wraparound cases correctly.

lwpa_add_custom_test(test_timer_wraparound
  test_timer_wraparound.c
  ${LWPA_SRC}/lwpa/timer.c
)
target_include_directories(test_timer_wraparound PRIVATE ${LWPA_INCLUDE})