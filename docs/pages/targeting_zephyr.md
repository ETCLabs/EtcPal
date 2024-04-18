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

Zephyr is highly configurable and is not compatible with EtcPal in its default configuration. Some changes are required in order to make them compatible:
1. EtcPal expects preemptive timeslicing meaning that threads of lower or equal
   priority should not be able to starve out higher or equal priority threads. However, by default, Zephyr does not
   implement timeslicing and only dedicates specific priorities to be preemptive. In order for threads to behave the
   way EtcPal expects, their priorities must be within this preemptive range and timeslicing must be enabled in
   KConfig.
   Read more about this here: [Zephyr Scheduling](https://docs.zephyrproject.org/latest/kernel/services/scheduling/index.html)
2. EtcPal threads and queues are dynamically allocated. Zephyr by default only allows for static allocation. To use EtcPal threads and queues,
   dynamic threads and a malloc implementation must both be enabled in KConfig.