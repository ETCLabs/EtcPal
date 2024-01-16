# Linux networking compilation support for EtcPal

set(ETCPAL_NET_ADDITIONAL_HEADERS
  ${ETCPAL_ROOT}/include/os/zephyr/etcpal/os_inet.h
  ${ETCPAL_ROOT}/include/os/zephyr/etcpal/os_socket.h
  ${ETCPAL_ROOT}/include/os/zephyr/etcpal/os_error.h
)

set(ETCPAL_NET_ADDITIONAL_SOURCES
  ${ETCPAL_ROOT}/src/os/zephyr/etcpal/os_error.c
  ${ETCPAL_ROOT}/src/os/zephyr/etcpal/os_inet.c
  ${ETCPAL_ROOT}/src/os/zephyr/etcpal/os_netint.c
  ${ETCPAL_ROOT}/src/os/zephyr/etcpal/os_socket.c
)
set(ETCPAL_NET_INCLUDE_DIR ${ETCPAL_ROOT}/include/os/zephyr)
