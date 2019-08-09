# Test the lwpa_timer module against wraparound errors.
# This needs to be a separate executable because the lwpa_getms() symbol is stubbed at link time.

add_library(lib_test_lwpa_timer_wraparound
  test_main.h
  test_main.c
  ${LWPA_SRC}/lwpa/timer.c
)
target_include_directories(lib_test_lwpa_timer_wraparound PUBLIC ${LWPA_INCLUDE} ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(lib_test_lwpa_timer_wraparound PUBLIC ThrowTheSwitch::Unity meekrosoft::fff)
# gtest_add_tests(TARGET test_lwpa_timer_wraparound)
