cmake_minimum_required(VERSION 3.15)

########################## Global Project Attributes ##########################

if (DEFINED ZEPHYR_CURRENT_MODULE_DIR)
  message("${CONFIG_ETCPAL}")
  if (DEFINED CONFIG_ETCPAL)
    message("EtcPal OS is set to Zephyr")
    set(ETCPAL_OS_TARGET "zephyr" CACHE INTERNAL "EtcPal OS Target")
    set(ETCPAL_NET_TARGET "none" CACHE INTERNAL "EtcPal Net Target")
  else()
    message("EtcPal Not enabled in Zephyr Kconfig")
    return()
  endif()
endif()

project(EtcPal)

set(ETCPAL_ROOT ${CMAKE_CURRENT_LIST_DIR})
set(ETCPAL_CMAKE ${ETCPAL_ROOT}/cmake)

option(ETCPAL_BUILD_MOCK_LIB "Build the EtcPalMock library" OFF)
option(ETCPAL_BUILD_TESTS "Build the EtcPal unit tests" OFF)
option(ETCPAL_BUILD_EXAMPLES "Build the EtcPal example apps" OFF)
option(ETCPAL_INSTALL_PDBS "Include PDBs in EtcPal install target" ON)

option(ETCPAL_EXPLICITLY_DISABLE_EXCEPTIONS "Disable throwing of exceptions throughout the EtcPal C++ headers" OFF)

############################## Standalone Support #############################

include(${ETCPAL_CMAKE}/OSS.cmake)
determine_compile_environment()

set(QUALITY_GATES_ENABLED OFF)

if(NOT COMPILING_AS_OSS)
  option(ETCPAL_FORCE_QUALITY_GATES "Force enable quality gate setup even if not building standalone" OFF)

  get_directory_property(HAS_PARENT PARENT_DIRECTORY)

  if(HAS_PARENT)
    if(ETCPAL_FORCE_QUALITY_GATES)
      set(QUALITY_GATES_ENABLED ON)
    endif()
  else()
    set(QUALITY_GATES_ENABLED ON)
  endif()
endif()

if(QUALITY_GATES_ENABLED)
  include(${ETCPAL_CMAKE}/AddCMakeTools.cmake)
  include(${CMAKE_TOOLS_MODULES}/QualityGateHelpers.cmake)
  setup_standalone_compile()
  setup_clang_format()
  setup_clang_tidy()
  setup_address_sanitizer()
  setup_memory_sanitizer()
  setup_undefined_behavior_sanitizer()
  setup_thread_sanitizer()
endif()

# Platform support + core sources
include(${ETCPAL_CMAKE}/etcpal-os.cmake)
include(${ETCPAL_CMAKE}/etcpal-core-sources.cmake)

if(ETCPAL_BUILD_MOCK_LIB OR ETCPAL_BUILD_TESTS)
  add_subdirectory(external/fff)
endif()

################################# Main library ################################

add_subdirectory(src)

#################################### Tests ####################################

if(ETCPAL_BUILD_TESTS)
  add_subdirectory(external/unity)
  enable_testing()
  add_subdirectory(tests)
endif()

################################## Examples ###################################

if(ETCPAL_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()
