cmake_minimum_required(VERSION 3.3)

########################## Global Project Attributes ##########################

project(lwpa VERSION 0.1.0)

if(PROJECT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  if (MSVC)
    add_compile_options(/W4)
  elseif("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    add_compile_options(-Wall -Wconversion)
    if("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
      add_compile_options(-Wno-sign-conversion)
    else()
      add_compile_options(-Wno-sign-compare)
    endif()
  endif()
endif()

# Main library

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src)

if(PROJECT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  set(OPTION_DEFAULTS ON)
else()
  set(OPTION_DEFAULTS OFF)
endif()

# Tests

option(LWPA_BUILD_TESTS "Build the lwpa unit tests" ${OPTION_DEFAULTS})
if(LWPA_BUILD_TESTS)
  add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/tests)
endif()
