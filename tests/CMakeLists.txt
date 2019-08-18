# Main test module.

set(LWPA_INCLUDE ${CMAKE_CURRENT_LIST_DIR}/../include)
set(LWPA_SRC ${CMAKE_CURRENT_LIST_DIR}/../src)

option(LWPA_TEST_IPV6 "Test IPv6 socket functions in the lwpa unit and integration tests" ON)

if(WIN32 OR APPLE OR UNIX)
  set(LWPA_TEST_ENTRYPOINT main_default.c)
  function(lwpa_add_to_ctest target_name)
    add_test(NAME ${target_name} COMMAND $<TARGET_FILE:${target_name}> -v)
    set_tests_properties(${target_name} PROPERTIES TIMEOUT 30)
  endfunction()
endif()

if(WIN32)
  set(LWPA_TEST_COMPILE_OPTIONS /wd4210)
elseif(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set(LWPA_TEST_COMPILE_OPTIONS -Wno-format-security)
else()
  set(LWPA_TEST_COMPILE_OPTIONS "")
endif()

# Set the platform-dependent compile definitions for the tests 
if(LWPA_TEST_IPV6)
  list(APPEND LWPA_TEST_COMPILE_DEFINITIONS LWPA_TEST_IPV6)
endif()

if(APPLE)
  list(APPEND LWPA_TEST_COMPILE_DEFINITIONS LWPA_BULK_POLL_TEST_NUM_SOCKETS=200)
endif()

add_subdirectory(unit)
add_subdirectory(integration)
