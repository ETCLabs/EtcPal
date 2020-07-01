cmake_minimum_required(VERSION 3.3)

########################## Global Project Attributes ##########################

project(EtcPal VERSION 0.3.0)

# Sometimes the seems to be some weirdness with drive letter capitalization on Windows, so do a
# case-insensitive comparision
if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL Windows)
  string(TOLOWER ${PROJECT_SOURCE_DIR} PROJECT_SOURCE_DIR_LC)
  string(TOLOWER ${CMAKE_SOURCE_DIR} CMAKE_SOURCE_DIR_LC)
else()
  set(PROJECT_SOURCE_DIR_LC ${PROJECT_SOURCE_DIR})
  set(CMAKE_SOURCE_DIR_LC ${CMAKE_SOURCE_DIR})
endif()

# Set options based on whether this project is being built on its own, or added as a dependency to
# another project via add_subdirectory(). In the latter case, we don't override any global compile
# options, or build tests or examples by default.
if(PROJECT_SOURCE_DIR_LC STREQUAL CMAKE_SOURCE_DIR_LC)
  if (MSVC)
    add_compile_options(/W4 /permissive-)
  elseif("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    add_compile_options(-Wall)
    if("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
      add_compile_options(-Wno-sign-conversion)
    else()
      add_compile_options(-Wno-sign-compare)
    endif()
  endif()

  # Allow folder grouping in generated IDE projects
  set_property(GLOBAL PROPERTY USE_FOLDERS ON)

  # Add a target to run Clang Format
  file(GLOB_RECURSE FILES_TO_REFORMAT 
    examples/*.c
    examples/*.cpp
    examples/*.h
    include/*.h
    src/*.c
    src/*.cpp
    src/*.h
    tests/*.c
    tests/*.cpp
    tests/*.h
  )
  add_custom_target(reformat_all COMMAND clang-format -i ${FILES_TO_REFORMAT})
  set_target_properties(reformat_all PROPERTIES FOLDER tools)

  set(OPTION_DEFAULTS ON)
else()
  set(OPTION_DEFAULTS OFF)
endif()

set(ETCPAL_ROOT ${CMAKE_CURRENT_LIST_DIR})

option(ETCPAL_BUILD_MOCK_LIB "Build the EtcPalMock library" OFF)
option(ETCPAL_BUILD_TESTS "Build the EtcPal unit tests" OFF)
option(ETCPAL_BUILD_EXAMPLES "Build the EtcPal example apps" OFF)

option(ETCPAL_EXPLICITLY_DISABLE_EXCEPTIONS "Disable throwing of exceptions throughout the EtcPal C++ headers" OFF)

# Platform support
include(${PROJECT_SOURCE_DIR}/cmake/etcpal-os.cmake)

if(ETCPAL_BUILD_MOCK_LIB OR ETCPAL_BUILD_TESTS)
  add_subdirectory(external/fff)
endif()

################################# Main library ################################

add_subdirectory(src)

#################################### Tests ####################################

if(ETCPAL_BUILD_TESTS)
  add_subdirectory(external/unity)
  enable_testing()
  add_subdirectory(tests)
endif()

################################## Examples ###################################

if(ETCPAL_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()
