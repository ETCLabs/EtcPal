# Zephyr OS compilation support for EtcPal

set(ETCPAL_OS_ADDITIONAL_SOURCES
	${ETCPAL_ROOT}/include/os/zephyr/etcpal/os_mutex.h
	${ETCPAL_ROOT}/include/os/zephyr/etcpal/os_queue.h

	${ETCPAL_ROOT}/src/os/zephyr/etcpal/os_mutex.c
	${ETCPAL_ROOT}/src/os/zephyr/etcpal/os_queue.c
	)
set(ETCPAL_OS_INCLUDE_DIR ${ETCPAL_ROOT}/include/os/zephyr)
