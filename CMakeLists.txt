cmake_minimum_required(VERSION 3.3)

########################## Global Project Attributes ##########################

project(lwpa VERSION 0.2.0)

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

  list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/tools/cmake)
endif()

option(LWPA_BUILD_MOCK_LIB "Build the lwpa_mock library" OFF)
if(LWPA_BUILD_MOCK_LIB)
  add_subdirectory(external/fff)
endif()

# Main library

add_subdirectory(src)

if(PROJECT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  set(OPTION_DEFAULTS ON)
else()
  set(OPTION_DEFAULTS OFF)
endif()

# Tests

option(LWPA_BUILD_TESTS "Build the lwpa unit tests" ${OPTION_DEFAULTS})
if(LWPA_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()
