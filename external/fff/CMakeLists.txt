if(NOT TARGET meekrosoft::fff)
  add_library(meekrosoft::fff INTERFACE IMPORTED GLOBAL)
  # target_include_directories(meekrosoft::fff INTERFACE ${CMAKE_CURRENT_LIST_DIR})
  # portable version for pre-3.11 CMake
  set_property(TARGET meekrosoft::fff APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_LIST_DIR})

  # FFF generates a lot of unreachable code warnings in Visual C++
  if(MSVC)
    target_compile_options(meekrosoft::fff INTERFACE /wd4702)
  endif()
endif()
