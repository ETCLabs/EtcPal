# lwpa unit tests.
# Most of these are "live" tests which use system calls of the underlying OS to ensure consistent
# behavior across platforms. This is intentionally contrary to normal unit testing philosophy,
# which emphasizes abstraction and separation of dependencies; in this case, what we are testing
# *is* the abstraction, so it must be done in a live-dependency environment and run on each target
# platform.

add_subdirectory(live)
# add_subdirectory(timer)

# Each test builds a different executable. CTest then combines them all and runs them as one
# session. This is the way the Unity test framework nudges you to do things, and it's valuable for
# separating symbols when testing C code.

#lwpa_add_live_test(test_common test_common.c)
#lwpa_add_live_test(test_inet test_inet.c)
#lwpa_add_live_test(test_lock test_lock.c)
#lwpa_add_live_test(test_log test_log.c)
#lwpa_add_live_test(test_mempool test_mempool.c)
#lwpa_add_live_test(test_netint test_netint.c)
#lwpa_add_live_test(test_pack test_pack.c)
#lwpa_add_live_test(test_rbtree test_rbtree.c)
#lwpa_add_live_test(test_socket test_socket.c)
#lwpa_add_live_test(test_thread test_thread.c)
#lwpa_add_live_test(test_timer test_timer.c)
#lwpa_add_live_test(test_uuid test_uuid.c)

# lwpa_add_custom_test(test_timer_wraparound
#   test_timer_wraparound.c
#   ${LWPA_SRC}/lwpa/timer.c
# )
# target_include_directories(test_timer_wraparound PRIVATE ${LWPA_INCLUDE})

