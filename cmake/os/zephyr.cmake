# Zephyr OS compilation support for EtcPal

set(ETCPAL_OS_ADDITIONAL_HEADERS
	${ETCPAL_ROOT}/include/os/zephyr/etcpal/os_event_group.h
	${ETCPAL_ROOT}/include/os/zephyr/etcpal/os_mutex.h
	${ETCPAL_ROOT}/include/os/zephyr/etcpal/os_queue.h
	${ETCPAL_ROOT}/include/os/zephyr/etcpal/os_recursive_mutex.h
	${ETCPAL_ROOT}/include/os/zephyr/etcpal/os_rwlock.h
	${ETCPAL_ROOT}/include/os/zephyr/etcpal/os_sem.h
	${ETCPAL_ROOT}/include/os/zephyr/etcpal/os_signal.h
	${ETCPAL_ROOT}/include/os/zephyr/etcpal/os_thread.h
	${ETCPAL_ROOT}/include/os/zephyr/etcpal/os_timer.h
)

set(ETCPAL_OS_ADDITIONAL_SOURCES
	${ETCPAL_ROOT}/src/os/zephyr/etcpal/os_error.h
	${ETCPAL_ROOT}/src/os/zephyr/etcpal/os_error.c
	${ETCPAL_ROOT}/include/os/zephyr/etcpal/os_event_group.c
	${ETCPAL_ROOT}/include/os/zephyr/etcpal/os_mutex.c
	${ETCPAL_ROOT}/include/os/zephyr/etcpal/os_queue.c
	${ETCPAL_ROOT}/include/os/zephyr/etcpal/os_recursive_mutex.c
	${ETCPAL_ROOT}/include/os/zephyr/etcpal/os_rwlock.c
	${ETCPAL_ROOT}/include/os/zephyr/etcpal/os_sem.c
	${ETCPAL_ROOT}/include/os/zephyr/etcpal/os_signal.c
	${ETCPAL_ROOT}/include/os/zephyr/etcpal/os_thread.c
	${ETCPAL_ROOT}/include/os/zephyr/etcpal/os_timer.c
	${ETCPAL_ROOT}/include/os/zephyr/etcpal/os_uuid.c

	${ETCPAL_ROOT}/include/os/zephyr/etcpal/etcpal_zephyr_common.h
	${ETCPAL_ROOT}/include/os/zephyr/etcpal/etcpal_zephyr_common.c
)
set(ETCPAL_OS_INCLUDE_DIR ${ETCPAL_ROOT}/include/os/zephyr)
set(ETCPAL_OS_ADDITIONAL_LIBS kernel)

add_compile_definitions(ETCPAL_OS_IS_ZEPHYR)