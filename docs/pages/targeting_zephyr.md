# Targeting Zephyr                                                              {#targeting_zephyr}

Zephyr is a real-time OS kernel used as an OS target by EtcPal. To use EtcPal with Zephyr,
you must set the `ETCPAL_OS_TARGET` variable to `zephyr`.

```cmake
set(ETCPAL_OS_TARGET zephyr CACHE STRING "The OS target for EtcPal")
```

Building for Zephyr requires a properly set up Zephyr project which calls:

```cmake
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
```

prior to running the cmake for EtcPal.