# FreeRTOS compilation support for EtcPal

# Since FreeRTOS is an OS microkernel rather than a full-featured OS, its include paths must be
# added as those of a traditional library rather than being available automatically as default
# settings of the compiler toolchain.
#
# The way this is implemented in EtcPal is by looking for a CMake target called FreeRTOS which has
# the header paths for FreeRTOS and the project's FreeRTOSConfig.h as PUBLIC or INTERFACE include
# directories.
#
# If your project does not use CMake natively, this can just be an interface library target, added
# like so:
#
# add_library(FreeRTOS INTERFACE)
# target_include_directories(FreeRTOS INTERFACE [freertos/include/paths/...])
#
# If you are consuming this library from a CMake project, the FreeRTOS target can be what actually
# builds the FreeRTOS sources as well. In either case, the target must be created before adding
# EtcPal's subdirectory.

if(NOT TARGET FreeRTOS)
  message(FATAL_ERROR "You must provide a target called FreeRTOS to allow EtcPal to find the FreeRTOS headers."
                      "See the comment in this file for more information.")
endif()

set(ETCPAL_OS_ADDITIONAL_SOURCES
  ${ETCPAL_ROOT}/include/os/freertos/etcpal/os_lock.h
  ${ETCPAL_ROOT}/include/os/freertos/etcpal/os_thread.h

  ${ETCPAL_ROOT}/src/os/freertos/etcpal/os_lock.c
  ${ETCPAL_ROOT}/src/os/freertos/etcpal/os_queue.c
  ${ETCPAL_ROOT}/src/os/freertos/etcpal/os_thread.c
  ${ETCPAL_ROOT}/src/os/freertos/etcpal/os_timer.c
  ${ETCPAL_ROOT}/src/os/freertos/etcpal/os_uuid.c
)
set(ETCPAL_OS_INCLUDE_DIR ${ETCPAL_ROOT}/include/os/freertos)
set(ETCPAL_OS_ADDITIONAL_LIBS FreeRTOS)
