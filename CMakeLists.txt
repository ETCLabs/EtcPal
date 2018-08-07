cmake_minimum_required(VERSION 3.10)

########################## Global Project Attributes ##########################

project(lwpa VERSION 0.1.0.3 LANGUAGES C)

############################### Platform Support ##############################

# The set of supported platforms
set(VALID_LWPA_PLATFORMS
  windows
)

if (NOT DEFINED LWPA_PLATFORM)
  message(STATUS "LWPA_PLATFORM not supplied, assuming Windows by default...")
  set (LWPA_PLATFORM windows)
endif()

if (NOT ${LWPA_PLATFORM} IN_LIST VALID_LWPA_PLATFORMS)
  message (FATAL_ERROR "${LWPA_PLATFORM} is not a valid platform.")
endif()

###############################################################################
# Core lwpa library
###############################################################################

add_library(lwpa
# lwpa headers
  include/acn_prot.h
  include/estardm.h
  include/estardmnet.h
  include/lwpa_bool.h
  include/lwpa_cid.h
  include/lwpa_common.h
  include/lwpa_error.h
  include/lwpa_inet.h
  include/lwpa_int.h
  include/lwpa_log.h
  include/lwpa_mempool.h
  include/lwpa_netint.h
  include/lwpa_pack.h
  include/lwpa_pdu.h
  include/lwpa_rbtree.h
  include/lwpa_rootlayerpdu.h
  include/lwpa_socket.h
  include/lwpa_timer.h
  include/lwpa_uid.h
  include/${LWPA_PLATFORM}/lwpa_lock.h
  include/${LWPA_PLATFORM}/lwpa_thread.h
  include/${LWPA_PLATFORM}/lwpa_socket_plat.h
# lwpa sources
  src/lwpa_cid.c
  src/lwpa_error.c
  src/lwpa_log.c
  src/lwpa_mempool.c
  src/lwpa_pdu.c
  src/lwpa_rbtree.c
  src/lwpa_rootlayerpdu.c
  src/lwpa_uid.c
  src/md5.h
  src/md5c.c
  src/${LWPA_PLATFORM}/lwpa_lock.c
  src/${LWPA_PLATFORM}/lwpa_netint.c
  src/${LWPA_PLATFORM}/lwpa_socket.c
  src/${LWPA_PLATFORM}/lwpa_thread.c
  src/${LWPA_PLATFORM}/lwpa_timer.c
)

if (MINGW)
  target_compile_definitions(lwpa PUBLIC -D_WIN32_WINNT=0x0501)
endif()

# lwpa public include directories and library dependencies

target_include_directories(lwpa PUBLIC ${LWPA_INCLUDE_DIR} ${LWPA_INCLUDE_DIR}/${LWPA_PLATFORM})
if (WIN32)
  target_link_libraries(lwpa PUBLIC winmm ws2_32 Iphlpapi)
endif()

set_target_properties(lwpa PROPERTIES DEBUG_POSTFIX d)

################################### testing ###################################

enable_testing()
include(GoogleTest)

# Get and build the Googletest library at configure time: https://crascit.com/2015/07/25/cmake-gtest/
include(${CMAKE_CURRENT_LIST_DIR}/cmake/get_googletest.cmake)
get_googletest()

set(TEST_SRC_DIR test/unit/windows/msvc2015)

add_executable(test_lwpa
# lwpa unit test sources
  ${TEST_SRC_DIR}/main.cpp
  ${TEST_SRC_DIR}/test_cid.cpp
  ${TEST_SRC_DIR}/test_log.cpp
  ${TEST_SRC_DIR}/test_mempool.cpp
  ${TEST_SRC_DIR}/test_mutex.cpp
  ${TEST_SRC_DIR}/test_netint.cpp
  ${TEST_SRC_DIR}/test_pack.cpp
  ${TEST_SRC_DIR}/test_rbtree.cpp
  ${TEST_SRC_DIR}/test_rwlock.cpp
  ${TEST_SRC_DIR}/test_signal.cpp
  ${TEST_SRC_DIR}/test_socket.cpp
  ${TEST_SRC_DIR}/test_thread.cpp
  ${TEST_SRC_DIR}/test_timer.cpp
)

target_link_libraries(test_lwpa lwpa gtest)

gtest_discover_tests(test_lwpa)