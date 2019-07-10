# Test the lwpa_timer module against wraparound errors.
# This needs to be a separate executable because the lwpa_getms() symbol is stubbed at link time.

add_executable(test_lwpa_timer_wraparound
  test_timer_wraparound.cpp
  ${LWPA_SRC}/lwpa/timer.c
)
set_target_properties(test_lwpa_timer_wraparound PROPERTIES CXX_STANDARD 14)
target_include_directories(test_lwpa_timer_wraparound PRIVATE ${LWPA_INCLUDE})
target_link_libraries(test_lwpa_timer_wraparound PRIVATE gtest_main meekrosoft::fff)
gtest_add_tests(TARGET test_lwpa_timer_wraparound)
