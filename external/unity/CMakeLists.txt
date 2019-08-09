if(NOT TARGET ThrowTheSwitch::Unity)
  add_library(unity
    unity.h
    unity_internals.h
    unity.c
  )
  target_include_directories(unity PUBLIC ${CMAKE_CURRENT_LIST_DIR})
  add_library(ThrowTheSwitch::Unity ALIAS unity)
endif()
