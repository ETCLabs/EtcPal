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
# Main lwpa interface library
###############################################################################

add_library(lwpa_with_deps INTERFACE)
target_link_libraries(lwpa_with_deps INTERFACE lwpa)
if (WIN32)
  target_link_libraries(lwpa_with_deps INTERFACE winmm ws2_32 Iphlpapi)
endif()

###############################################################################
# Core lwpa library, should not be used directly from a CMake environment
###############################################################################

set(LWPA_INCLUDE_DIR include)
set(LWPA_SRC_DIR src)

add_library(lwpa
# lwpa headers
  ${LWPA_INCLUDE_DIR}/acn_prot.h
  ${LWPA_INCLUDE_DIR}/estardm.h
  ${LWPA_INCLUDE_DIR}/estardmnet.h
  ${LWPA_INCLUDE_DIR}/lwpa_bool.h
  ${LWPA_INCLUDE_DIR}/lwpa_cid.h
  ${LWPA_INCLUDE_DIR}/lwpa_common.h
  ${LWPA_INCLUDE_DIR}/lwpa_error.h
  ${LWPA_INCLUDE_DIR}/lwpa_inet.h
  ${LWPA_INCLUDE_DIR}/lwpa_int.h
  ${LWPA_INCLUDE_DIR}/lwpa_log.h
  ${LWPA_INCLUDE_DIR}/lwpa_mempool.h
  ${LWPA_INCLUDE_DIR}/lwpa_netint.h
  ${LWPA_INCLUDE_DIR}/lwpa_pack.h
  ${LWPA_INCLUDE_DIR}/lwpa_pdu.h
  ${LWPA_INCLUDE_DIR}/lwpa_rbtree.h
  ${LWPA_INCLUDE_DIR}/lwpa_rootlayerpdu.h
  ${LWPA_INCLUDE_DIR}/lwpa_socket.h
  ${LWPA_INCLUDE_DIR}/lwpa_timer.h
  ${LWPA_INCLUDE_DIR}/lwpa_uid.h
  ${LWPA_INCLUDE_DIR}/${LWPA_PLATFORM}/lwpa_lock.h
  ${LWPA_INCLUDE_DIR}/${LWPA_PLATFORM}/lwpa_thread.h
  ${LWPA_INCLUDE_DIR}/${LWPA_PLATFORM}/lwpa_socket_plat.h
# lwpa sources
  ${LWPA_SRC_DIR}/lwpa_cid.c
  ${LWPA_SRC_DIR}/lwpa_error.c
  ${LWPA_SRC_DIR}/lwpa_log.c
  ${LWPA_SRC_DIR}/lwpa_mempool.c
  ${LWPA_SRC_DIR}/lwpa_pdu.c
  ${LWPA_SRC_DIR}/lwpa_rbtree.c
  ${LWPA_SRC_DIR}/lwpa_rootlayerpdu.c
  ${LWPA_SRC_DIR}/lwpa_uid.c
  ${LWPA_SRC_DIR}/md5.h
  ${LWPA_SRC_DIR}/md5c.c
  ${LWPA_SRC_DIR}/${LWPA_PLATFORM}/lwpa_lock.c
  ${LWPA_SRC_DIR}/${LWPA_PLATFORM}/lwpa_netint.c
  ${LWPA_SRC_DIR}/${LWPA_PLATFORM}/lwpa_socket.c
  ${LWPA_SRC_DIR}/${LWPA_PLATFORM}/lwpa_thread.c
  ${LWPA_SRC_DIR}/${LWPA_PLATFORM}/lwpa_timer.c
)

if (MINGW)
  add_definitions(-D_WIN32_WINNT=0x0501)
endif()

include_directories(${LWPA_INCLUDE_DIR} ${LWPA_INCLUDE_DIR}/${LWPA_PLATFORM})

################################### testing ###################################

enable_testing()
include(GoogleTest)

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

target_link_libraries(test_lwpa lwpa_with_deps gtest)

gtest_discover_tests(test_lwpa)
