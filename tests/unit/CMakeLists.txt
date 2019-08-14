# lwpa unit tests.
# Most of these are "live" tests which use system calls of the underlying OS to ensure consistent
# behavior across platforms. This is intentionally contrary to normal unit testing philosophy,
# which emphasizes abstraction and separation of dependencies; in this case, what we are testing
# *is* the abstraction, so it must be done in a live-dependency environment and run on each target
# platform.

if(WIN32 OR APPLE OR UNIX)
  set(LWPA_TEST_ENTRYPOINT ${CMAKE_CURRENT_LIST_DIR}/entrypoint/main_default.c)
  function(lwpa_add_to_ctest target_name)
    add_test(NAME ${target_name} COMMAND $<TARGET_FILE:${target_name}> -v)
  endfunction()
endif()

if(WIN32)
  set(LWPA_TEST_COMPILE_OPTIONS /wd4210)
elseif(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set(LWPA_TEST_COMPILE_OPTIONS -Wno-format-security)
else()
  set(LWPA_TEST_COMPILE_OPTIONS "")
endif()

# Add a "live" test, which links the entire lwpa library, unmodified
function(lwpa_add_live_test target_name)
  add_executable(${target_name} ${ARGN} ${LWPA_TEST_ENTRYPOINT})
  set_target_properties(${target_name} PROPERTIES FOLDER tests)
  target_compile_options(${target_name} PRIVATE ${LWPA_TEST_COMPILE_OPTIONS})
  target_link_libraries(${target_name} PRIVATE lwpa ThrowTheSwitch::Unity meekrosoft::fff)
  lwpa_add_to_ctest(${target_name})
endfunction()

# Add a "custom" test, which doesn't link the lwpa library - lwpa sources must then be selectively
# added to the target.
function(lwpa_add_custom_test target_name)
  add_executable(${target_name} ${ARGN} ${LWPA_TEST_ENTRYPOINT})
  set_target_properties(${target_name} PROPERTIES FOLDER tests)
  target_compile_options(${target_name} PRIVATE ${LWPA_TEST_COMPILE_OPTIONS})
  target_link_libraries(${target_name} PRIVATE ThrowTheSwitch::Unity meekrosoft::fff)
  lwpa_add_to_ctest(${target_name})
endfunction()

# Each test builds a different executable. CTest then combines them all and runs them as one
# session. This is the way the Unity test framework nudges you to do things, and it's valuable for
# separating symbols when testing C code.

lwpa_add_live_test(test_common test_common.c)
lwpa_add_live_test(test_inet test_inet.c)
lwpa_add_live_test(test_log test_log.c)

lwpa_add_custom_test(test_timer_wraparound
  test_timer_wraparound.c
  ${LWPA_SRC}/lwpa/timer.c
)
target_include_directories(test_timer_wraparound PRIVATE ${LWPA_INCLUDE})

