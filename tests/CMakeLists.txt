# Main test module.

set(ETCPAL_INCLUDE ${ETCPAL_ROOT}/include)
set(ETCPAL_SRC ${ETCPAL_ROOT}/src)
set(ETCPAL_TEST ${CMAKE_CURRENT_LIST_DIR})

option(ETCPAL_TEST_IPV6 "Test IPv6 socket functions in the EtcPal unit and integration tests" ON)
option(ETCPAL_TEST_BUILD_AS_LIBRARIES "Build the EtcPal unit and integration tests as libraries rather than executables" OFF)

function(etcpal_record_test target_name)
  if(ETCPAL_TEST_BUILD_AS_LIBRARIES)
    file(GENERATE OUTPUT "${CMAKE_BINARY_DIR}/tests/test_libraries/$<CONFIG>/${target_name}.txt" CONTENT "${target_name}")
  else()
    add_test(NAME ${target_name} COMMAND $<TARGET_FILE:${target_name}> -v)
    set_tests_properties(${target_name} PROPERTIES TIMEOUT 30)
    file(GENERATE OUTPUT "${CMAKE_BINARY_DIR}/tests/test_executables/$<CONFIG>/${target_name}.txt" CONTENT "$<TARGET_FILE:${target_name}>")
  endif()
endfunction()

if(WIN32)
  set(ETCPAL_TEST_COMPILE_OPTIONS /wd4210)
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_C_COMPILER_ID MATCHES "Clang")
  set(ETCPAL_TEST_COMPILE_OPTIONS -Wno-conversion)
else()
  set(ETCPAL_TEST_COMPILE_OPTIONS "")
endif()

# Set the platform-dependent compile definitions for the tests
if(ETCPAL_TEST_IPV6)
  list(APPEND ETCPAL_TEST_COMPILE_DEFINITIONS ETCPAL_TEST_IPV6)
endif()

if(APPLE)
  # Darwin has a low default setting for the maximum number of socket descriptors a process can
  # have open.
  list(APPEND ETCPAL_TEST_COMPILE_DEFINITIONS ETCPAL_BULK_POLL_TEST_NUM_SOCKETS=200)
endif()

if(NOT ETCPAL_HAVE_OS_SUPPORT)
  list(APPEND ETCPAL_TEST_COMPILE_DEFINITIONS ETCPAL_NO_OS_SUPPORT)
endif()

if(NOT ETCPAL_HAVE_NETWORKING_SUPPORT)
  list(APPEND ETCPAL_TEST_COMPILE_DEFINITIONS ETCPAL_NO_NETWORKING_SUPPORT)
endif()

# Add a "live" test, which links the entire EtcPal library, unmodified
# Start by setting up a custom EtcPal target
function(add_etcpal_test_library target_name config_h_directory)
  add_etcpal_library(target_name)

  target_compile_definitions(target_name PUBLIC ETCPAL_HAVE_CONFIG_H)
  target_include_directories(target_name PUBLIC
    ${config_h_directory}
  )

  target_sources(target_name PRIVATE ${ETCPAL_TEST}/config/assert_verify.c)
  target_link_libraries(target_name PUBLIC ThrowTheSwitch::Unity)
endfunction()

add_etcpal_test_library(LiveTestEtcPal ${ETCPAL_TEST}/config)

# Add a "custom" test, which doesn't link the EtcPal library - EtcPal sources must then be selectively
# added to the target, or a custom library must be linked.
function(etcpal_add_custom_test target_name language)
  if(ETCPAL_TEST_BUILD_AS_LIBRARIES)
    add_library(${target_name} ${ARGN})
    install(TARGETS ${target_name} ARCHIVE DESTINATION lib)
  else()
    if(${language} STREQUAL C)
      add_executable(${target_name} ${ARGN} ${ETCPAL_TEST}/entrypoint/main_default.c)
    elseif(${language} STREQUAL CXX)
      add_executable(${target_name} ${ARGN} ${ETCPAL_TEST}/entrypoint/main_default.cpp)
    endif()
  endif()

  etcpal_record_test(${target_name})
  set_target_properties(${target_name} PROPERTIES FOLDER tests)

  if(${language} STREQUAL CXX)
    set_target_properties(${target_name} PROPERTIES CXX_STANDARD 11)
  endif()

  target_compile_options(${target_name} PRIVATE ${ETCPAL_TEST_COMPILE_OPTIONS})
  target_compile_definitions(${target_name} PRIVATE ${ETCPAL_TEST_COMPILE_DEFINITIONS})
  target_link_libraries(${target_name} PUBLIC meekrosoft::fff)
endfunction()

# Then a standard test which links the EtcPal library.
function(etcpal_add_live_test target_name language)
  etcpal_add_custom_test(target_name language ${ARGN})
  target_link_libraries(${target_name} PUBLIC LiveTestEtcPal)
endfunction()

add_subdirectory(unit)
add_subdirectory(integration)

if(WIN32)
  add_subdirectory(windows)
endif()
