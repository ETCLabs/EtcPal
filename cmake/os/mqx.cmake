# MQX compilation support for EtcPal

# MQX's include paths must be added as those of a traditional library rather than being available
# automatically as default settings of the compiler toolchain.
#
# The way this is implemented in EtcPal is by looking for a CMake target called MQX which has the
# header paths for MQX as PUBLIC or INTERFACE include directories.
#
# If your project does not use CMake natively, this can just be an interface library target, added
# like so:
#
# add_library(MQX INTERFACE)
# target_include_directories(MQX INTERFACE [mqx/include/paths/...])
#
# If you are consuming this library from a CMake project, the MQX target can be what actually
# builds the MQX sources as well. In either case, the target must be created before adding EtcPal's
# subdirectory.

if(NOT TARGET MQX)
  message(FATAL_ERROR "You must provide a target called MQX to allow EtcPal to find the MQX headers."
                      "See the comment in this file for more information.")
endif()

set(ETCPAL_OS_ADDITIONAL_LIBS MQX)
