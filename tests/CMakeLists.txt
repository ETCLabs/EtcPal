# Main test module.

set(LWPA_INCLUDE ${CMAKE_CURRENT_LIST_DIR}/../include)
set(LWPA_SRC ${CMAKE_CURRENT_LIST_DIR}/../src)

if(LWPA_TARGET_OS STREQUAL windows OR LWPA_TARGET_OS STREQUAL macos OR LWPA_TARGET_OS STREQUAL linux)
  message(STATUS "Adding GoogleTest dependency...")
  include(AddGoogleTest)
  add_subdirectory(unit)
endif()
