############################# lwpa_netint example #############################

add_executable(netint_example
  netint_example.c
)
target_link_libraries(netint_example PRIVATE lwpa)
