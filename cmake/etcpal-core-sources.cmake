set(ETCPAL_CORE_HEADERS
  ${ETCPAL_ROOT}/include/etcpal/acn_pdu.h
  ${ETCPAL_ROOT}/include/etcpal/acn_prot.h
  ${ETCPAL_ROOT}/include/etcpal/acn_rlp.h
  ${ETCPAL_ROOT}/include/etcpal/common.h
  ${ETCPAL_ROOT}/include/etcpal/error.h
  ${ETCPAL_ROOT}/include/etcpal/handle_manager.h
  ${ETCPAL_ROOT}/include/etcpal/log.h
  ${ETCPAL_ROOT}/include/etcpal/mempool.h
  ${ETCPAL_ROOT}/include/etcpal/pack.h
  ${ETCPAL_ROOT}/include/etcpal/pack64.h
  ${ETCPAL_ROOT}/include/etcpal/rbtree.h
  ${ETCPAL_ROOT}/include/etcpal/timer.h
  ${ETCPAL_ROOT}/include/etcpal/uuid.h
  ${ETCPAL_ROOT}/include/etcpal/version.h
  ${ETCPAL_ROOT}/include/etcpal/cpp/common.h
  ${ETCPAL_ROOT}/include/etcpal/cpp/error.h
  ${ETCPAL_ROOT}/include/etcpal/cpp/hash.h
  ${ETCPAL_ROOT}/include/etcpal/cpp/log.h
  ${ETCPAL_ROOT}/include/etcpal/cpp/opaque_id.h
  ${ETCPAL_ROOT}/include/etcpal/cpp/timer.h
  ${ETCPAL_ROOT}/include/etcpal/cpp/uuid.h
)

set(ETCPAL_CORE_SOURCES
  ${ETCPAL_ROOT}/src/etcpal/acn_pdu.c
  ${ETCPAL_ROOT}/src/etcpal/acn_rlp.c
  ${ETCPAL_ROOT}/src/etcpal/common.c
  ${ETCPAL_ROOT}/src/etcpal/error.c
  ${ETCPAL_ROOT}/src/etcpal/handle_manager.c
  ${ETCPAL_ROOT}/src/etcpal/log.c
  ${ETCPAL_ROOT}/src/etcpal/mempool.c
  ${ETCPAL_ROOT}/src/etcpal/pack.c
  ${ETCPAL_ROOT}/src/etcpal/rbtree.c
  ${ETCPAL_ROOT}/src/etcpal/timer.c
  ${ETCPAL_ROOT}/src/etcpal/uuid.c
)

if(ETCPAL_HAVE_OS_SUPPORT)
  set(ETCPAL_CORE_HEADERS ${ETCPAL_CORE_HEADERS}
    ${ETCPAL_ROOT}/include/etcpal/event_group.h
    ${ETCPAL_ROOT}/include/etcpal/mutex.h
    ${ETCPAL_ROOT}/include/etcpal/queue.h
    ${ETCPAL_ROOT}/include/etcpal/rwlock.h
    ${ETCPAL_ROOT}/include/etcpal/sem.h
    ${ETCPAL_ROOT}/include/etcpal/signal.h
    ${ETCPAL_ROOT}/include/etcpal/thread.h
    ${ETCPAL_ROOT}/include/etcpal/cpp/event_group.h
    ${ETCPAL_ROOT}/include/etcpal/cpp/mutex.h
    ${ETCPAL_ROOT}/include/etcpal/cpp/queue.h
    ${ETCPAL_ROOT}/include/etcpal/cpp/recursive_mutex.h
    ${ETCPAL_ROOT}/include/etcpal/cpp/rwlock.h
    ${ETCPAL_ROOT}/include/etcpal/cpp/sem.h
    ${ETCPAL_ROOT}/include/etcpal/cpp/signal.h
    ${ETCPAL_ROOT}/include/etcpal/cpp/thread.h
  )
endif()

if(ETCPAL_HAVE_NETWORKING_SUPPORT)
  set(ETCPAL_CORE_HEADERS ${ETCPAL_CORE_HEADERS}
    ${ETCPAL_ROOT}/include/etcpal/inet.h
    ${ETCPAL_ROOT}/include/etcpal/netint.h
    ${ETCPAL_ROOT}/include/etcpal/socket.h
    ${ETCPAL_ROOT}/include/etcpal/cpp/inet.h
    ${ETCPAL_ROOT}/include/etcpal/cpp/netint.h
  )

  set(ETCPAL_CORE_SOURCES ${ETCPAL_CORE_SOURCES}
    ${ETCPAL_ROOT}/src/etcpal/inet.c
    ${ETCPAL_ROOT}/src/etcpal/netint.c
  )
endif()
