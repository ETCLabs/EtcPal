# Targeting lwIP                                                                  {#targeting_lwip}

lwIP is a lightweight embedded TCP/IP stack used as a network target by EtcPal. To use EtcPal with
lwIP, you must set the `ETCPAL_NET_TARGET` variable to `lwip`.

```cmake
set(ETCPAL_NET_TARGET lwip CACHE STRING "The network target for EtcPal")
```

Additionally, you must provide a target called lwIP to help EtcPal find the lwIP headers. If you
are not building lwIP from CMake (e.g. if your project is not a CMake project), this can simply be
an [interface library](https://cmake.org/cmake/help/latest/command/add_library.html#interface-libraries):

```cmake
add_library(lwIP INTERFACE)
target_include_directories(lwIP INTERFACE [lwip/include/paths...])
```

If you are building lwIP with CMake, make sure your lwIP library target is called `lwIP` exactly
and has its include paths set using `PUBLIC` or `INTERFACE`.
