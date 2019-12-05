# Linux compilation support for EtcPal
set(ETCPAL_OS_ADDITIONAL_SOURCES
  ${ETCPAL_ROOT}/src/os/linux/etcpal/os_error.h
  ${ETCPAL_ROOT}/src/os/linux/etcpal/os_error.c
)
set(ETCPAL_OS_ADDITIONAL_LIBS uuid pthread)
