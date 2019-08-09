include(GoogleTest)

add_subdirectory(test_lwpa)
add_subdirectory(test_lwpa_timer_wraparound)

if(WIN32 OR APPLE OR UNIX)
  add_executable(test_lwpa entrypoint_non_embedded/main.c)
  target_link_libraries(test_lwpa PRIVATE lib_test_lwpa)
  add_test(test_lwpa test_lwpa)

  add_executable(test_lwpa_timer_wraparound entrypoint_non_embedded/main.c)
  target_link_libraries(test_lwpa_timer_wraparound PRIVATE lib_test_lwpa_timer_wraparound)
endif()
