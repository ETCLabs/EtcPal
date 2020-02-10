# Targeting FreeRTOS                                                          {#targeting_freertos}

FreeRTOS is a real-time OS microkernel used as an OS target by EtcPal. To use EtcPal with FreeRTOS,
you must set the `ETCPAL_OS_TARGET` variable to `FreeRTOS`.

```cmake
set(ETCPAL_OS_TARGET freertos CACHE STRING "The OS target for EtcPal")
```

Additionally, you must provide a target called FreeRTOS to help EtcPal find the FreeRTOS headers.
If you are not building FreeRTOS from CMake (e.g. if your project is not a CMake project), this can
simply be an [interface library](https://cmake.org/cmake/help/latest/command/add_library.html#interface-libraries):

```cmake
add_library(FreeRTOS INTERFACE)
target_include_directories(FreeRTOS INTERFACE [freertos/include/paths...])
```

If you are building FreeRTOS with CMake, make sure your FreeRTOs library target is called
`FreeRTOS` exactly and has its include paths set using `PUBLIC` or `INTERFACE`.
