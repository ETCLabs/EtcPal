
set(LWPA_OS_TARGET freertos CACHE STRING "Override from toolchain file" FORCE)
set(LWPA_NET_TARGET lwip CACHE STRING "Override from toolchain file" FORCE)

set(LWPA_TEST_BUILD_AS_LIBRARIES ON CACHE BOOL "Override from toolchain file" FORCE)

set(LWPA_BUILD_TESTS ON CACHE BOOL "Build the lwpa tests" FORCE)
set(LWPA_BUILD_EXAMPLES OFF CACHE BOOL "Build the lwpa examples" FORCE)
