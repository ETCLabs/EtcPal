
set(LWPA_OS_ADDITIONAL_SOURCES
  ${LWPA_ROOT}/src/os/windows/lwpa/os_error.h
  ${LWPA_ROOT}/src/os/windows/lwpa/os_error.c
)
set(LWPA_OS_ADDITIONAL_LIBS winmm ws2_32 Iphlpapi Rpcrt4)
