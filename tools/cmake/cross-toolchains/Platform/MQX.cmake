# This file contains the settings that apply to targeting MQX as both an OS and
# network target with lwpa.

set(LWPA_OS_TARGET mqx CACHE STRING "Override from toolchain file" FORCE)
set(LWPA_NET_TARGET mqx CACHE STRING "Override from toolchain file" FORCE)

# Build the unit and integration tests as static libraries which can be linked into an on-target
# test executable
set(LWPA_BUILD_TESTS ON CACHE BOOL "Override from toolchain file" FORCE)
set(LWPA_TEST_BUILD_AS_LIBRARIES ON CACHE BOOL "Override from toolchain file" FORCE)

# Alternatively, remove the above and uncomment this to simply turn the tests off
# set (LWPA_BUILD_TESTS OFF CACHE BOOL "Override from toolchain file" FORCE)

# The examples will not build properly from CMake on this platform.
set(LWPA_BUILD_EXAMPLES OFF CACHE BOOL "Override from toolchain file" FORCE)
