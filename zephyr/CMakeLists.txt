# zephyr/CMakeLists.txt
if(CONFIG_ETCPAL)
  message("EtcPal OS is set to Zephyr")
  set(ETCPAL_OS_TARGET "zephyr" CACHE INTERNAL "EtcPal OS Target")
  add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/.. etcpal)
else()
  message("EtcPal Not enabled in Zephyr Kconfig")
endif()
