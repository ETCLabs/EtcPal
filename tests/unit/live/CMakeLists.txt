# The "live" lwpa tests, all built as one executable or library for now.

lwpa_add_live_test(lwpa_live_unit_tests
  test_common.c
  test_inet.c
  test_lock.c
  test_log.c
  test_main.c
  test_mempool.c
  test_netint.c
  test_pack.c
  test_rbtree.c
  test_socket.c
  test_thread.c
  test_timer.c
  test_uuid.c
)
