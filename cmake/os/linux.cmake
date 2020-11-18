# Linux OS compilation support for EtcPal

set(ETCPAL_OS_ADDITIONAL_SOURCES
  ${ETCPAL_ROOT}/include/os/linux/etcpal/os_event_group.h
  ${ETCPAL_ROOT}/include/os/linux/etcpal/os_mutex.h
  ${ETCPAL_ROOT}/include/os/linux/etcpal/os_recursive_mutex.h
  ${ETCPAL_ROOT}/include/os/linux/etcpal/os_rwlock.h
  ${ETCPAL_ROOT}/include/os/linux/etcpal/os_sem.h
  ${ETCPAL_ROOT}/include/os/linux/etcpal/os_signal.h
  ${ETCPAL_ROOT}/include/os/linux/etcpal/os_thread.h

  ${ETCPAL_ROOT}/src/os/linux/etcpal/os_error.h
  ${ETCPAL_ROOT}/src/os/linux/etcpal/os_error.c
  ${ETCPAL_ROOT}/src/os/linux/etcpal/os_event_group.c
  ${ETCPAL_ROOT}/src/os/linux/etcpal/os_mutex.c
  ${ETCPAL_ROOT}/src/os/linux/etcpal/os_recursive_mutex.c
  ${ETCPAL_ROOT}/src/os/linux/etcpal/os_rwlock.c
  ${ETCPAL_ROOT}/src/os/linux/etcpal/os_sem.c
  ${ETCPAL_ROOT}/src/os/linux/etcpal/os_signal.c
  ${ETCPAL_ROOT}/src/os/linux/etcpal/os_thread.c
  ${ETCPAL_ROOT}/src/os/linux/etcpal/os_timer.c
  ${ETCPAL_ROOT}/src/os/linux/etcpal/os_uuid.c
)
set(ETCPAL_OS_INCLUDE_DIR ${ETCPAL_ROOT}/include/os/linux)
set(ETCPAL_OS_ADDITIONAL_LIBS uuid pthread)
