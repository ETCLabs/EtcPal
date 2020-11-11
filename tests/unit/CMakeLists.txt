# EtcPal's unit testing distinguishes between "live" tests (the live and cpp subdirectories), which
# link the library unmodified and interact with the underlying OS functionality while running, and
# "controlled" tests (the controlled subdirectory) which removes some platform-specific sources to
# test the behavior of EtcPal functions in isolation.

add_subdirectory(live)
add_subdirectory(cpp)
add_subdirectory(controlled)
