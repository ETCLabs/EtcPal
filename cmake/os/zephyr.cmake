# Zephyr OS compilation support for EtcPal

set(ETCPAL_OS_ADDITIONAL_SOURCES
	${ETCPAL_ROOT}/include/os/zephyr/etcpal/os_mutex.h
	${ETCPAL_ROOT}/include/os/zephyr/etcpal/os_queue.h
	${ETCPAL_ROOT}/include/os/zephyr/etcpal/os_thread.h

	${ETCPAL_ROOT}/src/os/zephyr/etcpal/os_mutex.c
	${ETCPAL_ROOT}/src/os/zephyr/etcpal/os_queue.c
	${ETCPAL_ROOT}/src/os/zephyr/etcpal/os_thread.c
	)
set(ETCPAL_OS_INCLUDE_DIR ${ETCPAL_ROOT}/include/os/zephyr)
set(ETCPAL_OS_ADDITIONAL_LIBS kernel)

add_compile_definitions(ETCPAL_OS_IS_ZEPHYR)