set(ETCPAL_CORE_SOURCES

  # EtcPal headers
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

  # EtcPal sources
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
  set(ETCPAL_CORE_SOURCES ${ETCPAL_CORE_SOURCES}
    ${ETCPAL_ROOT}/include/etcpal/mutex.h
    ${ETCPAL_ROOT}/include/etcpal/queue.h
    ${ETCPAL_ROOT}/include/etcpal/rwlock.h
    ${ETCPAL_ROOT}/include/etcpal/sem.h
    ${ETCPAL_ROOT}/include/etcpal/signal.h
    ${ETCPAL_ROOT}/include/etcpal/thread.h
  )
endif()

if(ETCPAL_HAVE_NETWORKING_SUPPORT)
  set(ETCPAL_CORE_SOURCES ${ETCPAL_CORE_SOURCES}
    ${ETCPAL_ROOT}/include/etcpal/inet.h
    ${ETCPAL_ROOT}/include/etcpal/netint.h
    ${ETCPAL_ROOT}/include/etcpal/socket.h
    ${ETCPAL_ROOT}/src/etcpal/inet.c
    ${ETCPAL_ROOT}/src/etcpal/netint.c
  )
endif()
