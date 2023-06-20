# Zephyr OS compilation support for EtcPal

set(ETCPAL_OS_ADDITIONAL_SOURCES
	${ETCPAL_ROOT}/include/os/zephyr/etcpal/os_error.h
#	${ETCPAL_ROOT}/include/os/zephyr/etcpal/os_inet.h
	${ETCPAL_ROOT}/include/os/zephyr/etcpal/os_mutex.h
	${ETCPAL_ROOT}/include/os/zephyr/etcpal/os_queue.h
	${ETCPAL_ROOT}/include/os/zephyr/etcpal/os_thread.h
	${ETCPAL_ROOT}/include/os/zephyr/etcpal/os_signal.h
	${ETCPAL_ROOT}/include/os/zephyr/etcpal/os_socket.h

	${ETCPAL_ROOT}/src/os/zephyr/etcpal/os_error.c
#	${ETCPAL_ROOT}/src/os/zephyr/etcpal/os_inet.c
	${ETCPAL_ROOT}/src/os/zephyr/etcpal/os_mutex.c
#	${ETCPAL_ROOT}/src/os/zephyr/etcpal/os_netint.c
	${ETCPAL_ROOT}/src/os/zephyr/etcpal/os_queue.c
	${ETCPAL_ROOT}/src/os/zephyr/etcpal/os_thread.c
	${ETCPAL_ROOT}/src/os/zephyr/etcpal/os_timer.c
	${ETCPAL_ROOT}/src/os/zephyr/etcpal/os_signal.c
#	${ETCPAL_ROOT}/src/os/zephyr/etcpal/os_socket.c
	)
set(ETCPAL_OS_INCLUDE_DIR ${ETCPAL_ROOT}/include/os/zephyr)
set(ETCPAL_OS_ADDITIONAL_LIBS kernel)
# set(ETCPAL_HAVE_NETWORKING_SUPPORT ON)

add_compile_definitions(ETCPAL_OS_IS_ZEPHYR)