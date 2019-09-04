# Windows compilation support for EtcPal
set(ETCPAL_OS_ADDITIONAL_SOURCES
  ${ETCPAL_ROOT}/src/os/windows/lwpa/os_error.h
  ${ETCPAL_ROOT}/src/os/windows/lwpa/os_error.c
)
set(ETCPAL_OS_ADDITIONAL_LIBS winmm ws2_32 Iphlpapi Rpcrt4)
