# The C++ EtcPal tests, all built as one executable or library for now.

etcpal_add_live_test(etcpal_cpp_unit_tests CXX
  test_error.cpp
  test_inet.cpp
  test_lock.cpp
  test_main.cpp
  test_thread.cpp
  test_uuid.cpp
)
