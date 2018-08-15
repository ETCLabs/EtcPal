cmake_minimum_required(VERSION 3.3)

########################## Global Project Attributes ##########################

project(lwpa VERSION 0.1.0.3)

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

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/test)
