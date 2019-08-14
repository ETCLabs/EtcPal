if(NOT TARGET ThrowTheSwitch::Unity)
  add_library(unity
    src/unity.h
    src/unity_internals.h
    src/unity.c
    
    # The "test fixture" extension
    extras/fixture/src/unity_fixture.c
  )
  set_target_properties(unity PROPERTIES FOLDER tests)
  target_include_directories(unity PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/src
    ${CMAKE_CURRENT_LIST_DIR}/extras/fixture/src
  )
  add_library(ThrowTheSwitch::Unity ALIAS unity)
endif()
