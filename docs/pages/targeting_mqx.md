# Targeting MQX                                                                    {#targeting_mqx}

MQX is a flexible and configurable RTOS which includes a built-in network stack called RTCS. To use
EtcPal with MQX, set both the `ETCPAL_OS_TARGET` and `ETCPAL_NET_TARGET` variables to `mqx`.

```cmake
set(ETCPAL_OS_TARGET mqx CACHE STRING "The OS target for EtcPal")
set(ETCPAL_NET_TARGET mqx CACHE STRING "The network target for EtcPal")
```

Additionally, you must provide a target called MQX to help EtcPal find the MQX headers. In most
cases, an [interface library](https://cmake.org/cmake/help/latest/command/add_library.html#interface-libraries)
is sufficient:

```cmake
add_library(MQX INTERFACE)
target_include_directories(MQX INTERFACE [mqx/include/paths...])
```

The target must be called `MQX` exactly. The MQX include paths must include the BSP, PSP and RTCS
headers.
