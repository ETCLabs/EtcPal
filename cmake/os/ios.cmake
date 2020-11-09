# iOS OS compilation support for EtcPal

# For now, we are mostly using the macOS sources. This may change in the future.

set(ETCPAL_OS_ADDITIONAL_SOURCES
  ${ETCPAL_ROOT}/include/os/macos/etcpal/os_mutex.h
  ${ETCPAL_ROOT}/include/os/macos/etcpal/os_recursive_mutex.h
  ${ETCPAL_ROOT}/include/os/macos/etcpal/os_rwlock.h
  ${ETCPAL_ROOT}/include/os/macos/etcpal/os_sem.h
  ${ETCPAL_ROOT}/include/os/macos/etcpal/os_signal.h
  ${ETCPAL_ROOT}/include/os/macos/etcpal/os_thread.h

  ${ETCPAL_ROOT}/src/os/macos/etcpal/os_error.h
  ${ETCPAL_ROOT}/src/os/macos/etcpal/os_error.c
  ${ETCPAL_ROOT}/src/os/macos/etcpal/os_mutex.c
  ${ETCPAL_ROOT}/src/os/macos/etcpal/os_recursive_mutex.c
  ${ETCPAL_ROOT}/src/os/macos/etcpal/os_rwlock.c
  ${ETCPAL_ROOT}/src/os/macos/etcpal/os_sem.c
  ${ETCPAL_ROOT}/src/os/macos/etcpal/os_signal.c
  ${ETCPAL_ROOT}/src/os/macos/etcpal/os_thread.c
  ${ETCPAL_ROOT}/src/os/macos/etcpal/os_timer.c
  ${ETCPAL_ROOT}/src/os/macos/etcpal/os_uuid.c
)
set(ETCPAL_OS_INCLUDE_DIR ${ETCPAL_ROOT}/include/os/macos)
