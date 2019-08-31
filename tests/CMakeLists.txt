# Main test module.

set(LWPA_INCLUDE ${CMAKE_CURRENT_LIST_DIR}/../include)
set(LWPA_SRC ${CMAKE_CURRENT_LIST_DIR}/../src)
set(LWPA_TEST ${CMAKE_CURRENT_LIST_DIR})

option(LWPA_TEST_IPV6 "Test IPv6 socket functions in the lwpa unit and integration tests" ON)
option(LWPA_TEST_BUILD_AS_LIBRARIES "Build the lwpa unit and integration tests as libraries rather than executables" OFF)

if(WIN32 OR APPLE OR UNIX)
  set(LWPA_TEST_ENTRYPOINT main_default.c)
  function(lwpa_add_to_ctest target_name)
    add_test(NAME ${target_name} COMMAND $<TARGET_FILE:${target_name}> -v)
    set_tests_properties(${target_name} PROPERTIES TIMEOUT 30)
  endfunction()
endif()

if(WIN32)
  set(LWPA_TEST_COMPILE_OPTIONS /wd4210)
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_C_COMPILER_ID MATCHES "Clang")
  set(LWPA_TEST_COMPILE_OPTIONS -Wno-format-security -Wno-conversion)
else()
  set(LWPA_TEST_COMPILE_OPTIONS "")
endif()

# Set the platform-dependent compile definitions for the tests 
if(LWPA_TEST_IPV6)
  list(APPEND LWPA_TEST_COMPILE_DEFINITIONS LWPA_TEST_IPV6)
endif()

if(APPLE)
  # Darwin has a low default setting for the maximum number of socket descriptors a process can
  # have open.
  list(APPEND LWPA_TEST_COMPILE_DEFINITIONS LWPA_BULK_POLL_TEST_NUM_SOCKETS=200)
endif()

# Add a "live" test, which links the entire lwpa library, unmodified
function(lwpa_add_live_test target_name)
  if(LWPA_TEST_BUILD_AS_LIBRARIES)
    add_library(${target_name} ${ARGN})
    install(TARGETS ${target_name} ARCHIVE DESTINATION lib)
  else()
    add_executable(${target_name} ${ARGN} ${LWPA_TEST}/entrypoint/${LWPA_TEST_ENTRYPOINT})
    lwpa_add_to_ctest(${target_name})
  endif()
  set_target_properties(${target_name} PROPERTIES FOLDER tests)
  target_compile_options(${target_name} PRIVATE ${LWPA_TEST_COMPILE_OPTIONS})
  target_compile_definitions(${target_name} PRIVATE ${LWPA_TEST_COMPILE_DEFINITIONS})
  target_link_libraries(${target_name} PUBLIC lwpa ThrowTheSwitch::Unity meekrosoft::fff)
endfunction()

# Add a "custom" test, which doesn't link the lwpa library - lwpa sources must then be selectively
# added to the target.
function(lwpa_add_custom_test target_name)
  if(LWPA_TEST_BUILD_AS_LIBRARIES)
    add_library(${target_name} ${ARGN})
    install(TARGETS ${target_name} ARCHIVE DESTINATION lib)
  else()
    add_executable(${target_name} ${ARGN} ${LWPA_TEST}/entrypoint/${LWPA_TEST_ENTRYPOINT})
    lwpa_add_to_ctest(${target_name})
  endif()
  set_target_properties(${target_name} PROPERTIES FOLDER tests)
  target_compile_options(${target_name} PRIVATE ${LWPA_TEST_COMPILE_OPTIONS})
  target_compile_definitions(${target_name} PRIVATE ${LWPA_TEST_COMPILE_DEFINITIONS})
  target_link_libraries(${target_name} PUBLIC ThrowTheSwitch::Unity meekrosoft::fff)
endfunction()

add_subdirectory(unit)
add_subdirectory(integration)
