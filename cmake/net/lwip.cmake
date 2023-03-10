# lwIP compilation support for EtcPal

# Since lwIP is set up as a traditional library rather than a full-featured OS network stack, its
# include paths must be added as those of a traditional library rather than being available
# automatically as default settings of the compiler toolchain.
#
# The way this is implemented in EtcPal is by looking for a CMake target called lwIP which has the
# header paths for lwIP and the project's lwipopts.h as PUBLIC or INTERFACE include directories.
#
# If your project does not use CMake natively, this can just be an interface library target, added
# like so:
#
# add_library(lwIP INTERFACE)
# target_include_directories(lwIP INTERFACE [lwip/include/paths/...])
#
# If you are consuming this library from a CMake project, the lwIP target can be what actually
# builds the lwIP sources as well. In either case, the target must be created before adding
# EtcPal's subdirectory.

if(NOT TARGET lwIP)
  message(FATAL_ERROR "You must provide a target called lwIP to allow EtcPal to find the lwIP headers."
                      "See the comment in this file for more information.")
endif()

set(ETCPAL_NET_ADDITIONAL_HEADERS
  ${ETCPAL_ROOT}/include/os/lwip/etcpal/os_socket.h
  ${ETCPAL_ROOT}/include/os/lwip/etcpal/os_inet.h
)

set(ETCPAL_NET_ADDITIONAL_SOURCES
  ${ETCPAL_ROOT}/src/os/lwip/etcpal/os_error.h
  ${ETCPAL_ROOT}/src/os/lwip/etcpal/os_error.c
  ${ETCPAL_ROOT}/src/os/lwip/etcpal/os_inet.c
  ${ETCPAL_ROOT}/src/os/lwip/etcpal/os_netint.c
  ${ETCPAL_ROOT}/src/os/lwip/etcpal/os_socket.c
)
set(ETCPAL_NET_INCLUDE_DIR ${ETCPAL_ROOT}/include/os/lwip)
set(ETCPAL_NET_ADDITIONAL_LIBS lwIP)

set(ETCPAL_NET_ADDITIONAL_DEFINES ETCPAL_USING_LWIP_NETWORKING_SUPPORT)
