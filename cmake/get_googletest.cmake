function(get_googletest)
  # Download and unpack googletest at configure time
  configure_file(${CMAKE_SOURCE_DIR}/cmake/CMakeLists.txt.gtest googletest-download/CMakeLists.txt)

  execute_process(COMMAND "${CMAKE_COMMAND}" -G "${CMAKE_GENERATOR}" .
                  RESULT_VARIABLE result
                  WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/googletest-download" )
  if (result)
    message(FATAL_ERROR "CMake step for googletest failed: ${result}")
  endif()

  execute_process(COMMAND "${CMAKE_COMMAND}" --build .
                  RESULT_VARIABLE result
                  WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/googletest-download" )

  if (result)
    message(FATAL_ERROR "Build step for googletest failed: ${result}")
  endif()

  # Prevent GoogleTest from overriding our compiler/linker options
  # when building with Visual Studio
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

  # Add googletest directly to our build. This adds
  # the following targets: gtest, gtest_main, gmock
  # and gmock_main
  add_subdirectory("${CMAKE_BINARY_DIR}/googletest-src"
                   "${CMAKE_BINARY_DIR}/googletest-build")

endfunction(get_googletest)