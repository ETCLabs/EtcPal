############################ etcpal_netint example ############################

add_executable(netint_example
  netint_example.c
)
target_link_libraries(netint_example PRIVATE EtcPal)
set_target_properties(netint_example PROPERTIES FOLDER examples)
