# Windows OS compilation support for EtcPal

set(ETCPAL_OS_ADDITIONAL_SOURCES
  ${ETCPAL_ROOT}/include/os/windows/etcpal/os_lock.h
  ${ETCPAL_ROOT}/include/os/windows/etcpal/os_thread.h

  ${ETCPAL_ROOT}/src/os/windows/etcpal/os_error.h
  ${ETCPAL_ROOT}/src/os/windows/etcpal/os_error.c
  ${ETCPAL_ROOT}/src/os/windows/etcpal/os_lock.c
  ${ETCPAL_ROOT}/src/os/windows/etcpal/os_thread.c
  ${ETCPAL_ROOT}/src/os/windows/etcpal/os_timer.c
  ${ETCPAL_ROOT}/src/os/windows/etcpal/os_uuid.c
)
set(ETCPAL_OS_INCLUDE_DIR ${ETCPAL_ROOT}/include/os/windows)
set(ETCPAL_OS_ADDITIONAL_LIBS winmm Rpcrt4)
