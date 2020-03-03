# Linux compilation support for EtcPal

set(ETCPAL_OS_ADDITIONAL_SOURCES
  ${ETCPAL_ROOT}/include/os/linux/etcpal/os_inet.h
  ${ETCPAL_ROOT}/include/os/linux/etcpal/os_lock.h
  ${ETCPAL_ROOT}/include/os/linux/etcpal/os_socket.h
  ${ETCPAL_ROOT}/include/os/linux/etcpal/os_thread.h

  ${ETCPAL_ROOT}/src/os/linux/etcpal/os_error.h
  ${ETCPAL_ROOT}/src/os/linux/etcpal/os_error.c
  ${ETCPAL_ROOT}/src/os/linux/etcpal/os_inet.c
  ${ETCPAL_ROOT}/src/os/linux/etcpal/os_lock.c
  ${ETCPAL_ROOT}/src/os/linux/etcpal/os_netint.c
  ${ETCPAL_ROOT}/src/os/linux/etcpal/os_socket.c
  ${ETCPAL_ROOT}/src/os/linux/etcpal/os_thread.c
  ${ETCPAL_ROOT}/src/os/linux/etcpal/os_timer.c
  ${ETCPAL_ROOT}/src/os/linux/etcpal/os_uuid.c
)
set(ETCPAL_OS_ADDITIONAL_INCLUDE_DIRS
  ${ETCPAL_ROOT}/include/os/linux
)
set(ETCPAL_OS_ADDITIONAL_LIBS uuid pthread)
